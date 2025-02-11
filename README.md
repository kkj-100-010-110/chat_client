# chat_client

configure : cmake -Bbuild . -DCMAKE_BUILD_TYPE=Debug
build     : cmake --build build --config Debug
run       : ./build/CHAT