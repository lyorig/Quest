cmake -S .. -B ../build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug -DHalcyon_BUILD_TESTS=OFF -DHalcyon_BUILD_EXAMPLES=OFF
cmake --build ../build -j --config Debug