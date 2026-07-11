#!/usr/bin/env python3
# Copyright 2012 The Chromium Authors
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

'''Bootstrapping for GRIT.
'''


import os
import sys

import grit.grit_runner

# Only apply once even if imported multiple times
_PATCH_APPLIED = False

def _read_gn_args_from_grit_argv(argv):
    root_gen_dir = None
    for i, arg in enumerate(argv):
        if arg == "-E" and i + 1 < len(argv):
            key, _, value = argv[i + 1].partition("=")
            if key == "root_gen_dir":
                root_gen_dir = value
                break
    build_dir = os.path.dirname(os.path.abspath(root_gen_dir)) if root_gen_dir else os.getcwd()

    repo_root = os.path.normpath(os.path.join(os.path.dirname(__file__), "..", ".."))
    sys.path.insert(0, os.path.join(repo_root, "build"))
    import gn_helpers
    return gn_helpers.ReadArgsGN(build_dir)

def init_custom_browser():
    global _PATCH_APPLIED
    if _PATCH_APPLIED:
        return
    gn_args = _read_gn_args_from_grit_argv(sys.argv)
    branding_name = gn_args.get("branding_path_component", "")
    should_apply_patches = gn_args.get("enable_custom_browser", False)
    if should_apply_patches:
      should_apply_patches =  branding_name != "chromium" and branding_name != ""
    if not should_apply_patches:
        _PATCH_APPLIED = True
        return

    # Calculate the absolute path to custom (4 levels up)
    script_dir = os.path.dirname(os.path.abspath(__file__))
    custom_module_dir = os.path.normpath(os.path.join(script_dir, "..", "..", "custom_browser","resources", "grit"))

    if custom_module_dir not in sys.path:
        sys.path.insert(0, custom_module_dir)
    try:
        from grit_patch import apply_patches
        apply_patches(custom_module_dir, branding_name)
        _PATCH_APPLIED = True
    except Exception as e:
        print(f"[Init] Failed to apply custom patches: {e}", file=sys.stderr)
# Run patch setup
init_custom_browser()
if __name__ == '__main__':
  ret = grit.grit_runner.Main(sys.argv[1:])
  # Use os._exit() instead of sys.exit() to skip the Python garbage
  # collector teardown at script exit, which takes 1.4s for the 100MB AST.
  # As os._exit() stops the process immediately without calling cleanup handlers,
  # we must explicitly flush stdout and stderr to avoid losing any output.
  # Note: Since os._exit() bypasses Python's normal file cleanup, we must ensure
  # that all other file objects are properly closed inside grit_runner.Main().
  # See https://docs.python.org/3.14/library/os.html#os._exit
  sys.stdout.flush()
  sys.stderr.flush()
  os._exit(ret or 0)
