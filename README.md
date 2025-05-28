# You need CUDA 12.2
https://developer.nvidia.com/cuda-12-2-0-download-archive?target_os=Linux&target_arch=x86_64

Do not forget to set the PATH on .bashrc <br>
export PATH=/usr/local/cuda/bin${PATH:+:${PATH}} <br>
export LD_LIBRARY_PATH=/usr/local/cuda/lib64${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}} <br>

# If you are on WSL you need a XServer to run the graphics interface.
Install and run it (you need it on the background) <br>
https://sourceforge.net/projects/xming/ <br>

# You need ROOT CERN
https://root.cern/install/#install-via-a-package-manager <br>

Do not forget to set the PATH on .bashrc <br>
export LD_LIBRARY_PATH=/snap/root-framework/936/usr/local/lib:$LD_LIBRARY_PATH <br>

(The Graphics Library will be optionally built, I need to study how to add compile options.) <br>

# Build Quantum-Abysmal 
mkdir _build <br>
cd _build <br>
cmake .. <br>
cmake -DINFO_LOGGING_ENABLED=ON -S . -B _build && cmake --build _build && cmake --install _build <br>
