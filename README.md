# Required packages

# -------------------------------------------------------------------------------------------------
# If WSL/Linux build

# CUDA Toolkit 
https://developer.nvidia.com/cuda-downloads

# OpenMP

# HDF5

# Google Test
sudo apt install libgtest-dev

# -------------------------------------------------------------------------------------------------
# If Apple build

# OpenMP
brew install libomp

# HDF5 
brew install hdf5

# Google Test
brew install googletest

# -------------------------------------------------------------------------------------------------
# Build Quantum-Abysmal

sudo cmake -DINFO_LOGGING_ENABLED=ON -S . -B _build && sudo cmake --build _build && sudo cmake --install _build <br>
sudo cmake -DINFO_LOGGING_ENABLED=ON -DCMAKE_CUDA_ARCHITECTURES=86 -DCMAKE_CUDA_COMPILER=/usr/local/cuda/bin/nvcc -S . -B _build && sudo cmake --build _build && sudo cmake --install _build <br>