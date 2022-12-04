mkdir build
cd build

cmake ..
cmake --build . --config Release

cd ..
copy .\build\Release\main.bin.exe .\

