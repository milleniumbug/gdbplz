#!/bin/bash
set -Eeuxo pipefail
ci/10-build_lin.sh
ci/20-run_tests_lin.sh