#!/bin/bash
# Run this from within a bash shell
#rm ../../source/config.h
#rm ../../source/config.asm
cmake -DCMAKE_TOOLCHAIN_FILE=crosscompile.cmake -G "Unix Makefiles" $@ ../../source && make

#Press C to configure, Press G to generate and exit
