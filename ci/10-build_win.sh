#!/bin/bash
set -Eeuxo pipefail
mkdir build
cd build
cmake -G "MSYS Makefiles" ..
cmake --build .