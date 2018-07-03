#!/bin/bash
set -Eeuxo pipefail
ci/10-build_win.sh
ci/20-run_tests_win.sh