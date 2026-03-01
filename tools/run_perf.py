#!/usr/bin/env python3
from __future__ import annotations

import argparse
import csv
import math
import os
import shlex
import subprocess
import sys
from collections import defaultdict
from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class CMakeConfig:
    build_dir: Path
    config: str
    generator: str | None
    parallel: int | None


def _format_cmd(cmd: list[str]) -> str:
    if os.name == "nt":
        return subprocess.list2cmdline(cmd)
    return " ".join(shlex.quote(c) for c in cmd)


def _run(cmd: list[str], *, cwd: Path) -> None:
    print(f"+ {_format_cmd(cmd)}")
    subprocess.run(cmd, cwd=str(cwd))


def cmake_configure(root: Path, cfg: CMakeConfig, *, extra_args: list[str]) -> None:
    cmd = [
        "cmake",
        "-S",
        str(root),
        "-B",
        str(cfg.build_dir),
        "-DASCII_AFO_BUILD_BENCHMARKS=ON",
        *extra_args,
    ]
    if cfg.generator:
        cmd.insert(1, cfg.generator)
        cmd.insert(1, "-G")
    _run(cmd, cwd=root)


def cmake_build(root: Path, cfg: CMakeConfig, *, targets: list[str] | None = None) -> None:
    cmd = ["cmake", "--build", str(cfg.build_dir), "--config", cfg.config]
    if cfg.parallel:
        cmd += ["--parallel", str(cfg.parallel)]
    if targets:
        cmd += ["--target", *targets]
    _run(cmd, cwd=root)


def ctest(root: Path, cfg: CMakeConfig) -> None:
    cmd = ["ctest", "--test-dir", str(cfg.build_dir), "-C", cfg.config, "--output-on-failure"]
    _run(cmd, cwd=root)


def _exe_name(stem: str) -> str:
    return f"{stem}.exe" if os.name == "nt" else stem


def find_benchmark_exe(cfg: CMakeConfig) -> Path:
    candidates = [
        cfg.build_dir / cfg.config / _exe_name("benchmark_ascii"),
        cfg.build_dir / _exe_name("benchmark_ascii"),
        cfg.build_dir / "bin" / _exe_name("benchmark_ascii"),
    ]
    for p in candidates:
        if p.is_file():
            return p

    # Fall back to a shallow search (keeps things predictable/fast).
    name = _exe_name("benchmark_ascii")
    for p in cfg.build_dir.rglob(name):
        if p.is_file():
            return p
    raise FileNotFoundError(f"could not find benchmark executable {name!r} under {cfg.build_dir}")


def run_benchmarks(root: Path, cfg: CMakeConfig) -> Path:
    exe = find_benchmark_exe(cfg)
    out_csv = cfg.build_dir / "benchmark_results.csv"
    cmd = [str(exe), f"--benchmark_out={out_csv}", "--benchmark_out_format=csv"]
    _run(cmd, cwd=root)
    return out_csv


def clean_benchmark_csv(in_path: Path, out_path: Path) -> Path:
    # Google Benchmark's CSV output can include environment/preamble lines; keep only the CSV table.
    lines_out: list[str] = []
    for line in in_path.read_text(encoding="utf-8", errors="replace").splitlines():
        if line.startswith("name,") or line.startswith('"'):
            lines_out.append(line)
    out_path.write_text("\n".join(lines_out) + ("\n" if lines_out else ""), encoding="utf-8")
    return out_path


def _parse_bench_name(name: str) -> tuple[str, str, int]:
    # Example names:
    #  ascii_find_first_not_of/any/16
    #  ranges/find_if_not/fnptr/any/16
    parts = name.split("/")
    if parts and parts[0] == "ascii_find_first_not_of" and len(parts) == 3:
        return "ascii_find_first_not_of", parts[1], int(parts[2])
    if parts and parts[0] == "ranges" and len(parts) == 5 and parts[1] == "find_if_not":
        return f"ranges/find_if_not/{parts[2]}", parts[3], int(parts[4])
    raise ValueError(f"unknown benchmark name format: {name!r}")


def generate_horizontal(clean_csv: Path, out_horizontal: Path, out_sorted: Path) -> tuple[Path, Path]:
    impl_order = [
        "ascii_find_first_not_of",
        "ranges/find_if_not/fnptr",
        "ranges/find_if_not/lambda",
        "ranges/find_if_not/naive",
    ]

    # data[(cls, len)][impl] = {'cpu_time_ns': float, 'bytes_per_second': float}
    data: dict[tuple[str, int], dict[str, dict[str, float]]] = defaultdict(dict)

    with clean_csv.open(newline="") as f:
        reader = csv.DictReader(f)
        for row in reader:
            name = row["name"].strip().strip('"')
            impl, cls, length = _parse_bench_name(name)
            if impl not in impl_order:
                continue

            cpu_time = float(row["cpu_time"])
            unit = row.get("time_unit", "ns")
            if unit == "ns":
                cpu_time_ns = cpu_time
            elif unit == "us":
                cpu_time_ns = cpu_time * 1e3
            elif unit == "ms":
                cpu_time_ns = cpu_time * 1e6
            elif unit == "s":
                cpu_time_ns = cpu_time * 1e9
            else:
                raise ValueError(f"unknown time unit {unit!r} for {name!r}")

            bps_raw = (row.get("bytes_per_second") or "").strip()
            bytes_per_second = float(bps_raw) if bps_raw else math.nan

            data[(cls, length)][impl] = {"cpu_time_ns": cpu_time_ns, "bytes_per_second": bytes_per_second}

    def ratio(a: float, b: float) -> float:
        if math.isnan(a) or math.isnan(b) or b == 0.0:
            return math.nan
        return a / b

    rows_out: list[dict[str, object]] = []
    for (cls, length), impls in sorted(data.items(), key=lambda x: (x[0][0], x[0][1])):
        times = {impl: impls.get(impl, {}).get("cpu_time_ns", math.nan) for impl in impl_order}
        valid = [(impl, t) for impl, t in times.items() if not math.isnan(t)]
        if not valid:
            continue
        best_impl, best_time = min(valid, key=lambda kv: kv[1])
        slowest_impl, slowest_time = max(valid, key=lambda kv: kv[1])
        spread = ratio(slowest_time, best_time)

        ascii_t = times["ascii_find_first_not_of"]

        ascii_bps = impls.get("ascii_find_first_not_of", {}).get("bytes_per_second", math.nan)

        rows_out.append(
            {
                "cls": cls,
                "len": length,
                "best": best_impl,
                "ascii_vs_best": ratio(ascii_t, best_time),
                "ascii_bytes_per_second": ascii_bps,
                # Not part of the output, but kept for sorting in the "sorted" CSV.
                "_spread_slowest_vs_best": spread,
                "_slowest_impl": slowest_impl,
                "_best_cpu_time_ns": best_time,
                "_slowest_cpu_time_ns": slowest_time,
            }
        )

    fieldnames = [
        "cls",
        "len",
        "best",
        "ascii_vs_best",
        "ascii_bytes_per_second",
    ]

    with out_horizontal.open("w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=fieldnames, extrasaction="ignore")
        w.writeheader()
        for r in rows_out:
            w.writerow(r)

    rows_sorted = sorted(
        rows_out,
        key=lambda r: (-(float(r["_spread_slowest_vs_best"]) or 0.0), str(r["cls"]), int(r["len"])),
    )
    with out_sorted.open("w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=fieldnames, extrasaction="ignore")
        w.writeheader()
        for r in rows_sorted:
            w.writerow(r)

    return out_horizontal, out_sorted


def main(argv: list[str]) -> int:
    parser = argparse.ArgumentParser(
        description="Configure/build with CMake, run tests and benchmarks, and generate horizontal benchmark CSVs."
    )
    parser.add_argument("--build-dir", default="build", help="CMake build directory (default: build)")
    parser.add_argument("--config", default="Release", help="Build configuration (default: Release)")
    parser.add_argument("--generator", default=None, help="CMake generator name (passed as -G)")
    parser.add_argument(
        "--parallel",
        type=int,
        default=None,
        help="Parallel build jobs (passed as --parallel N); omit to let CMake decide",
    )
    parser.add_argument(
        "--cmake-arg",
        action="append",
        default=[],
        help="Extra argument to pass to CMake configure (repeatable), e.g. --cmake-arg=-A --cmake-arg=x64",
    )
    parser.add_argument("--skip-configure", action="store_true", help="Skip CMake configure")
    parser.add_argument("--skip-build", action="store_true", help="Skip build")
    parser.add_argument("--skip-test", action="store_true", help="Skip tests")
    parser.add_argument("--skip-bench", action="store_true", help="Skip running benchmarks")
    args = parser.parse_args(argv)

    root = Path(__file__).resolve().parent.parent
    cfg = CMakeConfig(
        build_dir=(root / args.build_dir).resolve(),
        config=args.config,
        generator=args.generator,
        parallel=args.parallel,
    )

    if not args.skip_configure:
        cmake_configure(root, cfg, extra_args=args.cmake_arg)

    if not args.skip_build:
        cmake_build(root, cfg)

    if not args.skip_test:
        ctest(root, cfg)

    out_csv = cfg.build_dir / "benchmark_results.csv"
    if not args.skip_bench:
        # Ensure the benchmark target exists/built; keeps errors clearer if benchmarks were accidentally disabled.
        cmake_build(root, cfg, targets=["benchmark_ascii"])
        out_csv = run_benchmarks(root, cfg)

    clean_csv = cfg.build_dir / "benchmark_results_clean.csv"
    clean_csv = clean_benchmark_csv(out_csv, clean_csv)

    out_horizontal = cfg.build_dir / "benchmark_horizontal.csv"
    out_sorted = cfg.build_dir / "benchmark_spread_sorted.csv"
    out_horizontal, out_sorted = generate_horizontal(clean_csv, out_horizontal, out_sorted)

    print(f"Wrote {clean_csv}")
    print(f"Wrote {out_horizontal}")
    print(f"Wrote {out_sorted}")
    return 0


if __name__ == "__main__":
    main(sys.argv[1:])
