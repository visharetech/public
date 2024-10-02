#!/bin/bash
# Run this from within a bash shell
#rm ../../source/config.h
#rm ../../source/config.asm
cmake -G "Unix Makefiles" $@ ../../source && make
