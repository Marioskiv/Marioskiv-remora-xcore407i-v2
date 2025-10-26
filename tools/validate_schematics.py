#!/usr/bin/env python3
import json
import os
import sys
from glob import glob
from typing import Any, Dict, List, Tuple


def validate_topology(data: Dict[str, Any]) -> Tuple[List[str], List[str]]:
    """Return (errors, warnings) for a given schematic JSON object."""
    errors: List[str] = []
    warnings: List[str] = []

    # Required: at least one of these top-level keys
    if not any(k in data for k in ("device", "subsystem")):
        warnings.append("missing 'device' or 'subsystem' top-level key")

    # Validate pins arrays (if present)
    for pins_key in ("pins", "pins_1_to_150", "mcu_power_pins"):
        if pins_key in data:
            pins = data.get(pins_key)
            if not isinstance(pins, list):
                errors.append(f"'{pins_key}' must be a list")
            else:
                for i, p in enumerate(pins):
                    if not isinstance(p, dict):
                        errors.append(f"{pins_key}[{i}] must be an object")
                        continue
                    # Heuristic checks for typical fields
                    if not any(x in p for x in ("name", "num", "pin", "header_pin")):
                        warnings.append(f"{pins_key}[{i}] missing typical pin identifier (name/num/pin/header_pin)")

    # Validate components (if present)
    if "components" in data:
        comps = data["components"]
        if not isinstance(comps, list):
            errors.append("'components' must be a list")
        else:
            for i, c in enumerate(comps):
                if not isinstance(c, dict):
                    errors.append(f"components[{i}] must be an object")
                    continue
                if "refdes" not in c:
                    warnings.append(f"components[{i}] missing 'refdes'")
                if "value" not in c:
                    warnings.append(f"components[{i}] missing 'value'")

    # Validate nets (if present)
    if "nets" in data:
        nets = data["nets"]
        if not isinstance(nets, list):
            errors.append("'nets' must be a list")
        else:
            for i, n in enumerate(nets):
                if not isinstance(n, dict):
                    errors.append(f"nets[{i}] must be an object")
                    continue
                if "name" not in n:
                    warnings.append(f"nets[{i}] missing 'name'")
                if not any(k in n for k in ("nodes", "from", "to")):
                    warnings.append(f"nets[{i}] missing 'nodes' (or from/to)")

    return errors, warnings


def main() -> int:
    repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
    default_dir = os.path.join(repo_root, "resources", "schematic xcore407i")
    target_dir = os.path.abspath(sys.argv[1]) if len(sys.argv) > 1 else default_dir

    if not os.path.isdir(target_dir):
        print(f"ERROR: target directory not found: {target_dir}")
        return 2

    files = sorted(glob(os.path.join(target_dir, "*.json")))
    if not files:
        print(f"No .json files found under: {target_dir}")
        return 0

    total = 0
    ok = 0
    error_count = 0
    warning_count = 0

    print(f"Validating {len(files)} files in: {target_dir}\n")

    for fp in files:
        total += 1
        name = os.path.basename(fp)
        try:
            with open(fp, "r", encoding="utf-8") as f:
                data = json.load(f)
        except Exception as e:
            error_count += 1
            print(f"[ERROR] {name}: invalid JSON -> {e}")
            continue

        errors, warnings = validate_topology(data)
        if errors:
            error_count += 1
            print(f"[FAIL ] {name}")
            for msg in errors:
                print(f"  - error: {msg}")
            for msg in warnings:
                warning_count += 1
                print(f"  - warn : {msg}")
        else:
            ok += 1
            status = "OK" if not warnings else "OK (with warnings)"
            print(f"[OK   ] {name} -> {status}")
            for msg in warnings:
                warning_count += 1
                print(f"  - warn : {msg}")

    print("\nSummary:")
    print(f"  Total   : {total}")
    print(f"  OK      : {ok}")
    print(f"  Errors  : {error_count}")
    print(f"  Warnings: {warning_count}")

    # Non-zero exit if any errors
    return 1 if error_count > 0 else 0


if __name__ == "__main__":
    sys.exit(main())
