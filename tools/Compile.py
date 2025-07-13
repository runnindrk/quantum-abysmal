# =================================================================================================
# Copyright (c) 2024, runnindrk
#
# This file is part of Quantum Abysmal.
#
# Quantum Abysmal is free software: you can redistribute it and/or modify it
# as needed, with the intent of making it freely available to everyone.
#
# This project is in its early stages and is provided without any warranties,
# expressed or implied, including but not limited to the warranties of
# merchantability, fitness for a particular purpose, or non-infringement.
#
# Use it at your own risk, and feel free to contribute as the project evolves!
# =================================================================================================

import os
import shutil
import json
import argparse
from jinja2 import Environment, FileSystemLoader, TemplateSyntaxError

# -------------------------------------------------------------------------------------------------
# Command line argument parsing

parser = argparse.ArgumentParser(description="Quantum Abysmal Codegen")
parser.add_argument(
    "--json", "-j",
    help="Path to the lattice model JSON file"
)
args = parser.parse_args()

# -------------------------------------------------------------------------------------------------
# Prepare codegen context from lattice JSON file

lattice_context = {}

if args.json:
    with open(args.json) as f:
        data = json.load(f)

    hoppings = data["lattice"]["hoppings"]

    # Determine dimensionality from the length of the first delta vector
    if hoppings:
        dim = len(hoppings[0]["delta"])
        lattice_context["dimension"] = dim
    else:
        raise ValueError("No hoppings defined in the JSON file to determine lattice dimensionality.")

    # Collect all unique orbitals
    all_orbitals = set()
    for hop in hoppings:
        all_orbitals.add(hop["from"])
        all_orbitals.add(hop["to"])

    # Sort and assign indices alphabetically
    orbital_map = {orb: idx for idx, orb in enumerate(sorted(all_orbitals))}

    # Prepare hoppings for C++
    cpp_hoppings = []
    for hop in hoppings:
        cpp_hoppings.append({
            "latticeHop": hop["delta"] + [0] * (3 - len(hop["delta"])),
            "orbitalHop": [orbital_map[hop["from"]], orbital_map[hop["to"]]],
            "hoppingStrength": hop["value"]
        })

    # Add Hermitian conjugates
    hermitian_hoppings = []
    for hop in cpp_hoppings:
        if hop["orbitalHop"][0] != hop["orbitalHop"][1] or any(hop["latticeHop"]):
            hermitian_hoppings.append({
                "latticeHop": [-x for x in hop["latticeHop"]],
                "orbitalHop": [hop["orbitalHop"][1], hop["orbitalHop"][0]],
                "hoppingStrength": hop["hoppingStrength"]
            })

    all_hoppings = cpp_hoppings + hermitian_hoppings
    for idx, hop in enumerate(all_hoppings):
        hop["macro_idx"] = idx

    lattice_context["hoppings"] = all_hoppings
    lattice_context["num_orbitals"] = len(orbital_map)

# -------------------------------------------------------------------------------------------------
# Template rendering

SRC_DIR = "src"
OUT_DIR = "generated"

def is_utf8_text_file(filepath):
    try:
        with open(filepath, "rb") as f:
            raw = f.read()
        raw.decode("utf-8")
        return True
    except UnicodeDecodeError:
        return False

def strip_jinja2_suffix(path):
    return path[:-7] if path.endswith(".jinja2") else path

def render_all_templates(src_dir, out_dir, context=None):
    if context is None:
        context = {}

    # Clean the output directory
    if os.path.exists(out_dir):
        shutil.rmtree(out_dir)
    os.makedirs(out_dir, exist_ok=True)

    env = Environment(loader=FileSystemLoader(src_dir), autoescape=False)

    for root, _, files in os.walk(src_dir):
        for filename in files:
            src_path = os.path.join(root, filename)
            rel_dir = os.path.relpath(root, src_dir)
            rel_file = os.path.join(rel_dir, filename) if rel_dir != "." else filename

            if not is_utf8_text_file(src_path):
                print(f"Skipping binary or non-UTF-8 file: {rel_file}")
                continue

            try:
                template = env.get_template(rel_file)
                rendered = template.render(context)
            except TemplateSyntaxError as e:
                print(f"Template syntax error in {rel_file}: {e}")
                continue
            except Exception as e:
                print(f"Failed to render {rel_file}: {e}")
                continue

            # Compute the output path and strip .jinja2 if present
            stripped_rel_file = strip_jinja2_suffix(rel_file)
            output_path = os.path.join(out_dir, stripped_rel_file)
            os.makedirs(os.path.dirname(output_path), exist_ok=True)

            with open(output_path, "w", encoding="utf-8") as f:
                f.write(rendered)
                print(f"Rendered: {rel_file} -> {output_path}")

# -------------------------------------------------------------------------------------------------
# Entry point

if __name__ == "__main__":
    default_context = {
        "project_name": "Quantum Abysmal",
        "version": "0.1.0",
        "author": "runnindrk",
    }

    # Merge JSON-derived lattice context into default template context
    full_context = {**default_context, **lattice_context}
    render_all_templates(SRC_DIR, OUT_DIR, full_context)
