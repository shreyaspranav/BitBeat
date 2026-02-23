export ARM_GCC=/usr/bin/arm-none-eabi-gcc
export ARM_GXX=/usr/bin/arm-none-eabi-g++

cmake -DCMAKE_C_COMPILER:FILEPATH=$ARM_GCC -DCMAKE_CXX_COMPILER:FILEPATH=$ARM_GXX -S . -B build/
bear -- cmake --build build/
