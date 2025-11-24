import os
import sys

import matplotlib.pyplot as plt
import numpy as np


STEPPER_NAMES = {"exp_euler", "impl_euler", "impr_euler", "crank_nicolson"}
STEPPER_DIRS = {
    "exp_euler": "ExplicitEuler",
    "impl_euler": "ImplicitEuler",
    "impr_euler": "ImprovedEuler",
    "crank_nicolson": "CrankNicolson",
}
PREFIX = "mass_spring_"
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
BUILD_DIR = os.path.abspath(os.path.join(SCRIPT_DIR, "..", "build"))


def usage():
    print("Usage: python plotmassspring.py <mass_spring_*.txt>")
    sys.exit(1)


def parse_metadata(filepath):
    basename = os.path.basename(filepath)
    name, ext = os.path.splitext(basename)
    if ext.lower() != ".txt":
        raise ValueError("Input file must use .txt extension")
    if not name.startswith(PREFIX):
        raise ValueError("Input file must start with 'mass_spring_'")

    remainder = name[len(PREFIX):]
    stepper = None
    suffix = ""
    for candidate in STEPPER_NAMES:
        if remainder == candidate or remainder.startswith(candidate + "_"):
            stepper = candidate
            suffix = remainder[len(candidate):]
            break
    if stepper is None:
        raise ValueError("Filename does not encode a supported stepper")

    suffix = suffix.lstrip("_")
    tokens = [tok for tok in suffix.split("_") if tok] if suffix else []

    tend_factor = None
    n_factor = None
    has_nomod = False
    for token in tokens:
        if token == "nomod":
            has_nomod = True
        elif token.endswith("tend"):
            value = token[:-4]
            tend_factor = value
        elif token.endswith("steps"):
            value = token[:-5]
            n_factor = value

    modifiers = "no modification"
    modifier_parts = []
    if tend_factor is not None:
        modifier_parts.append(f"{tend_factor}*tend")
    if n_factor is not None:
        modifier_parts.append(f"{n_factor}*steps")
    if modifier_parts:
        modifiers = ", ".join(modifier_parts)
    elif has_nomod or not tokens:
        modifiers = "no modification"

    pretty_stepper = stepper.replace("_", " ").title()

    suffix_for_name = suffix if suffix else stepper

    return {
        "stepper": stepper,
        "pretty_stepper": pretty_stepper,
        "modifiers": modifiers,
        "suffix": suffix,
        "suffix_for_name": suffix_for_name,
        "output_dir": STEPPER_DIRS[stepper],
    }


def main():
    if len(sys.argv) != 2:
        usage()

    requested_path = sys.argv[1]
    file_path = requested_path
    if not os.path.isfile(file_path):
        candidate = os.path.join(BUILD_DIR, os.path.basename(requested_path))
        if os.path.isfile(candidate):
            file_path = candidate
        else:
            print(f"Input file '{requested_path}' not found (also checked {candidate})")
            sys.exit(1)

    try:
        meta = parse_metadata(file_path)
    except ValueError as err:
        print(err)
        sys.exit(1)

    output_dir = os.path.join(SCRIPT_DIR, meta["output_dir"])
    os.makedirs(output_dir, exist_ok=True)

    try:
        data = np.loadtxt(file_path, usecols=(0, 1, 2))
    except OSError as err:
        print(f"Failed to read '{file_path}': {err}")
        sys.exit(1)

    time = data[:, 0]
    position = data[:, 1]
    velocity = data[:, 2]

    time_fig, time_ax = plt.subplots()
    time_ax.plot(time, position, label="position")
    time_ax.plot(time, velocity, label="velocity")
    time_ax.set_xlabel("time")
    time_ax.set_ylabel("value")
    time_ax.set_title(
        f"Mass-Spring System Time Evolution, {meta['pretty_stepper']} ({meta['modifiers']})"
    )
    time_ax.legend()
    time_ax.grid(True)

    suffix_tag = meta["suffix_for_name"]

    time_output = os.path.join(output_dir, f"mass_spring_time_evolution_{suffix_tag}.png")
    time_fig.savefig(time_output, bbox_inches="tight")
    plt.close(time_fig)

    phase_fig, phase_ax = plt.subplots()
    phase_ax.plot(position, velocity)
    phase_ax.set_xlabel("position")
    phase_ax.set_ylabel("velocity")
    phase_ax.set_title(
        f"Mass-Spring System Phase Plot, {meta['pretty_stepper']} ({meta['modifiers']})"
    )
    phase_ax.grid(True)

    phase_output = os.path.join(output_dir, f"mass_spring_phase_{suffix_tag}.png")
    phase_fig.savefig(phase_output, bbox_inches="tight")
    plt.close(phase_fig)

    print(f"Saved time evolution plot to {time_output}")
    print(f"Saved phase plot to {phase_output}")


if __name__ == "__main__":
    main()
