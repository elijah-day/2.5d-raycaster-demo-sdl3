cmake CMakeLists.txt -B build
cmake --build build
cp -r data build/data
cd build
./main
cd ..
