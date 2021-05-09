md build
cd build
cmake -G "Visual Studio 16 2019" -A x64 ..\ -Dgtest_force_shared_crt=on
PAUSE