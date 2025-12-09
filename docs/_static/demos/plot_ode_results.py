import os
import sys

import matplotlib.pyplot as plt
import numpy as np


STEPPER_NAMES = {
    "exp_rk",
    "exp_euler",
    "impl_euler",
    "impr_euler",
    "crank_nicolson",
    "impl_rk_gauss_legendre",
    "impl_rk_gauss_radau",
}
STEPPER_DIRS = {
    "exp_rk": "ExplicitRK",
    "exp_euler": "ExplicitEuler",
    "impl_euler": "ImplicitEuler",
    "impr_euler": "ImprovedEuler",
    "crank_nicolson": "CrankNicolson",
    "impl_rk_gauss_legendre": "ImplicitRK_GaussLegendre",
    "impl_rk_gauss_radau": "ImplicitRK_GaussRadau",
}
SYSTEM_LABELS = {
    "mass_spring": "Mass-Spring System",
    "electric_network": "Electric Network",
}
SYSTEM_PREFIXES = sorted(SYSTEM_LABELS.keys(), key=len, reverse=True)
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
BUILD_DIR = os.path.abspath(os.path.join(SCRIPT_DIR, "..", "build"))
EXPLICIT_RK_PREFIX = "ExplicitRK"


def usage():
    print("Usage: python plot_ode_results.py <system_stepper_*.txt>")
    sys.exit(1)


def prettify_system(system):
    return SYSTEM_LABELS.get(system, system.replace("_", " ").title())


def camel_to_snake(name):
    result = []
    prev = ""
    for idx, ch in enumerate(name):
        if ch.isalnum():
            if ch.isupper():
                need_sep = False
                if idx > 0:
                    if prev.islower() or prev.isdigit():
                        need_sep = True
                    elif prev.isupper() and idx + 1 < len(name):
                        nxt = name[idx + 1]
                        if nxt.islower():
                            need_sep = True
                if need_sep and result and result[-1] != "_":
                    result.append("_")
                result.append(ch.lower())
            else:
                result.append(ch.lower())
        else:
            if result and result[-1] != "_":
                result.append("_")
        prev = ch
    snake = "".join(result).strip("_")
    return snake if snake else "custom"


def resolve_explicit_rk_dir(tableau_tag):
    if not tableau_tag:
        tableau_tag = "custom"
    candidates = []
    for entry in os.listdir(SCRIPT_DIR):
        if not entry.startswith(EXPLICIT_RK_PREFIX):
            continue
        remainder = entry[len(EXPLICIT_RK_PREFIX):]
        if remainder.startswith("_"):
            remainder = remainder[1:]
        candidate_tag = camel_to_snake(remainder)
        candidates.append((candidate_tag, entry))
        if candidate_tag == tableau_tag:
            return entry
    # fallback: synthesize CamelCase folder name from snake tokens
    parts = [part for part in tableau_tag.split("_") if part]
    camel = "".join(part.capitalize() for part in parts)
    if not camel:
        camel = "Custom"
    synthesized = f"{EXPLICIT_RK_PREFIX}_{camel}"
    return synthesized


def parse_metadata(filepath):
    basename = os.path.basename(filepath)
    name, ext = os.path.splitext(basename)
    if ext.lower() != ".txt":
        raise ValueError("Input file must use .txt extension")

    system = None
    remainder = ""
    for candidate in SYSTEM_PREFIXES:
        prefix = candidate + "_"
        if name.startswith(prefix):
            system = candidate
            remainder = name[len(prefix):]
            break
    if system is None or not remainder:
        raise ValueError(
            "Filename must start with a known system prefix like 'mass_spring_'"
        )

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
    stage_count = None
    tableau_tokens = []
    non_tableau_tokens = []
    for token in tokens:
        if token == "nomod":
            has_nomod = True
            non_tableau_tokens.append(token)
        elif token.startswith("s") and token[1:].isdigit():
            stage_count = token[1:]
            non_tableau_tokens.append(token)
        elif token.endswith("tend"):
            tend_factor = token[:-4]
            non_tableau_tokens.append(token)
        elif token.endswith("steps"):
            n_factor = token[:-5]
            non_tableau_tokens.append(token)
        else:
            tableau_tokens.append(token)

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
    tableau_tag = "_".join(tableau_tokens)
    if stepper == "exp_rk" and tableau_tag:
        pretty_stepper += f" [{tableau_tag.replace('_', ' ')}]"
    if stage_count:
        pretty_stepper += f" (s={stage_count})"
    suffix_for_name = suffix if suffix else "nomod"

    explicit_suffix_tokens = [tok for tok in non_tableau_tokens if tok]
    if not explicit_suffix_tokens:
        explicit_suffix_tokens = ["nomod"]
    explicit_suffix = "_".join(explicit_suffix_tokens)

    output_dir = STEPPER_DIRS.get(stepper, "plots")
    if stepper == "exp_rk":
        output_dir = resolve_explicit_rk_dir(tableau_tag)

    return {
        "system": system,
        "pretty_system": prettify_system(system),
        "stepper": stepper,
        "pretty_stepper": pretty_stepper,
        "modifiers": modifiers,
        "tableau_tag": tableau_tag,
        "suffix": suffix,
        "suffix_for_name": suffix_for_name,
        "explicit_suffix": explicit_suffix,
        "output_dir": output_dir,
    }


def resolve_input_path(requested_path):
    if os.path.isfile(requested_path):
        return requested_path
    candidate = os.path.join(BUILD_DIR, os.path.basename(requested_path))
    if os.path.isfile(candidate):
        return candidate
    raise FileNotFoundError(
        f"Input file '{requested_path}' not found (also checked {candidate})"
    )


def main():
    if len(sys.argv) != 2:
        usage()

    try:
        file_path = resolve_input_path(sys.argv[1])
    except FileNotFoundError as err:
        print(err)
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
        f"{meta['pretty_system']} Time Evolution, {meta['pretty_stepper']} ({meta['modifiers']})"
    )
    time_ax.legend()
    time_ax.grid(True)

    if meta["stepper"] == "exp_rk":
        suffix_tag = meta["explicit_suffix"]
    else:
        suffix_tag = meta["suffix_for_name"]

    time_filename = f"{meta['system']}_time_evolution"
    phase_filename = f"{meta['system']}_phase"
    if suffix_tag:
        time_filename += f"_{suffix_tag}"
        phase_filename += f"_{suffix_tag}"

    time_output = os.path.join(output_dir, f"{time_filename}.png")
    time_fig.savefig(time_output, bbox_inches="tight")
    plt.close(time_fig)

    phase_fig, phase_ax = plt.subplots()
    phase_ax.plot(position, velocity)
    phase_ax.set_xlabel("position")
    phase_ax.set_ylabel("velocity")
    phase_ax.set_title(
        f"{meta['pretty_system']} Phase Plot, {meta['pretty_stepper']} ({meta['modifiers']})"
    )
    phase_ax.grid(True)

    phase_output = os.path.join(output_dir, f"{phase_filename}.png")
    phase_fig.savefig(phase_output, bbox_inches="tight")
    plt.close(phase_fig)

    print(f"Saved time evolution plot to {time_output}")
    print(f"Saved phase plot to {phase_output}")


if __name__ == "__main__":
    main()
