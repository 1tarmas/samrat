1. Install prerequisites:
   - CMake 3.12+
   - OpenSSL 1.1.1+
   - Qt5 (for GUI)

2. Configure:
```bash
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019"  # Windows
cmake .. -DCMAKE_BUILD_TYPE=Release  # Linux/macOS