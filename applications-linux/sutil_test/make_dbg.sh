mkdir -p build_dbg &&
cd build_dbg &&
cmake .. -DCMAKE_BUILD_TYPE=Debug &&
make -j8 &&
cp -rf sutil_test ../ &&
cd ..
