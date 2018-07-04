#!/bin/bash
set -Eeuxo pipefail
mkdir build
cd build
cmake -G "Ninja" ..
cmake --build .