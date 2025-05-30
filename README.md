# Quantum-Abysmal

## Required Packages

### Building on WSL/Linux

- **CUDA Toolkit**  
  https://developer.nvidia.com/cuda-downloads

- **OpenMP**  
  Usually comes with GCC. Ensure GCC is installed:
  ```bash  
  `sudo apt install build-essential`
  ```

- **HDF5**  
  ```bash
  sudo apt install libhdf5-dev
  ```

- **Google Test**  
  ```bash
  sudo apt install libgtest-dev
  ```

- **ROOT (via binaries)**  
  ```bash
  wget https://root.cern/download/root_v6.30.06.Linux-ubuntu22.04-x86_64-gcc11.4.tar.gz
  tar -xzf root_v6.30.06.Linux-ubuntu22.04-x86_64-gcc11.4.tar.gz
  mv root_v6.30.06.Linux* $HOME/root
  source $HOME/root/bin/thisroot.sh
  ```

> **Note:** CMake may prompt you to install additional dependencies. Follow its suggestions.

---

### Building on macOS

- **OpenMP**  
  ```bash
  brew install libomp
  ```

- **HDF5**  
  ```bash
  brew install hdf5
  ```

- **Google Test**  
  ```bash
  brew install googletest
  ```

- **ROOT**

---

## Building Quantum-Abysmal

### On WSL/Linux

```bash
sudo cmake -DINFO_LOGGING_ENABLED=ON -DCMAKE_PREFIX_PATH=$HOME/root -S . -B _build
sudo cmake --build _build
sudo cmake --install _build
```
