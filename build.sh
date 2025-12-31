mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
cp -r ../resources ./