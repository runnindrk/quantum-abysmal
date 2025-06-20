#!/bin/bash

# Usage: ./Compile.sh --json path/to/model.json
if [ "$1" = "--model-json" ]; then
    shift
    JSON_INPUT="$1"
elif [ -n "$1" ]; then
    JSON_INPUT="$1"
else
    echo "Usage: $0 [--model-json] path/to/model.json"
    exit 1
fi

echo "==> Running code generation with $JSON_INPUT"
python3 tools/Codegen.py --json "$JSON_INPUT" || { echo "Codegen failed"; exit 1; }

echo "==> Building source code"
sudo cmake -DINFO_LOGGING_ENABLED=ON  -DCMAKE_CUDA_ARCHITECTURES=86 -DCMAKE_CUDA_COMPILER=/usr/local/cuda/bin/nvcc -DROOT_DIR=/snap/root-framework/current/usr/local/cmake -S . -B _build \
    && sudo cmake --build _build \
    && sudo cmake --install _build

echo "==> Done"