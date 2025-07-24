#!/bin/bash

set -ex

meson test -v \
        -C _build \
        --timeout-multiplier 4 \
        "$@"

# Run only the flaky tests, so we can log the failures but without hard failing
meson test -v \
        -C _build \
        --timeout-multiplier 4 \
        "$@" --setup=unstable_tests --suite=failing --suite=flaky || true
