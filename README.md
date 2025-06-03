# Quantum-Abysmal

## Required Packages

### Building on WSL/Linux

- **CUDA Toolkit**  
  https://developer.nvidia.com/cuda-downloads

- **OpenMP**  
  Usually comes with GCC. Ensure GCC is installed:
  ```bash  
  sudo apt install build-essential
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
  https://root.cern/install/
    
  This package may cause of lot of pain because of missing libraries or linkage. Please export the path.
  ```bash
  sudo snap install root-framework
  export LD_LIBRARY_PATH=/snap/root-framework/current/usr/local/lib:$LD_LIBRARY_PATH
  ```

> **Note:** CMake may prompt you to install additional dependencies. Follow its suggestions.
> **Note:** You may need an xServer, I recommend VcXsrv.

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

## Running Quantum-Abysmal Tests

```bash
export LD_LIBRARY_PATH=/snap/root-framework/current/usr/local/lib:$LD_LIBRARY_PATH
./_bin/Quantum-Abysmal-PoC-DoS
```