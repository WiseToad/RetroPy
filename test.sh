#!/bin/bash

PROJECT_DIR="$(dirname "$0")"
retroarch --libretro "${PROJECT_DIR}/build/release/retropy_libretro.so" "${PROJECT_DIR}/python/sample.py"
