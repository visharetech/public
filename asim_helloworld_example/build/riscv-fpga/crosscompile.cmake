# CMake toolchain file for cross compiling x265 for riscv arch
# This feature is only supported as experimental. Use with caution.
# Please report bugs on bitbucket
# Run cmake with: cmake -DCMAKE_TOOLCHAIN_FILE=crosscompile.cmake -G "Unix Makefiles" ../../source && ccmake ../../source

set(CROSS_COMPILE_RISCV 1)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
set(RISCV32 1)
set(RISCV64 0)

# HLS XMEM related functions
set(HLS_XMEM 0)

set(ASIM_CALL 0)

#no-inline option ?
#Can be configured by external argument: ./make-Makefiles.sh -DNOINLINE=1
if(NOT NOINLINE)
    set(NOINLINE 0)
endif()

# linker script
set(LINK_SCRIPT "../../source/riscv/script.ld")

#set(WRAP_C_FLAGS "-Wl,--wrap=__malloc_lock -Wl,--wrap=__malloc_unlock -Wl,--wrap=gettimeofday -Wl,--wrap=printf -Wl,--wrap=access")	


# specify the cross compiler
set(CMAKE_C_COMPILER "riscv32-unknown-elf-gcc")
set(CMAKE_CXX_COMPILER "riscv32-unknown-elf-g++")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WRAP_C_FLAGS} -DNO_ATOMICS -D_POSIX_THREADS -D_rvTranslate -march=rv32im -mabi=ilp32 -fdata-sections -ffunction-sections")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS}")

set(LINK_LIBRARIES "-L../../source/riscv -L./rvTranslate/libcrt -L. -Wl,--start-group -lstdc++ -lc -lcrt_tr -lgcc -lm -Wl,--end-group -Wl,--gc-sections")

set(CMAKE_LINKER "riscv32-unknown-elf-gcc -static -nostartfiles -T ${LINK_SCRIPT}  ${WRAP_C_FLAGS}")
set(CMAKE_C_LINK_EXECUTABLE "${CMAKE_LINKER} <OBJECTS> -Wl,-Map=<TARGET>.map -o <TARGET> ${LINK_LIBRARIES}")
set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_LINKER} <OBJECTS> -Wl,-Map=<TARGET>.map -o <TARGET> ${LINK_LIBRARIES}")
