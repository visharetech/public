cmake_minimum_required (VERSION 2.8)

# Configure build type (Release when no one is selected)
if (NOT DEFINED CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
endif ()

project (autonet_example)

message("DEBUG: CMAKE_SYSTEM_PROCESSOR = ${CMAKE_SYSTEM_PROCESSOR}")
message("DEBUG: CMAKE_SYSTEM_NAME      = ${CMAKE_SYSTEM_NAME}")


if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-O3)
    add_compile_options(-g)
endif()


if (HLS_XMEM)
    message("Compile definition: HLS_XMEM")
	add_definitions(-DHLS_XMEM)
endif()

if (ASIM_CALL)
    message("Compile definition: ASIM_CALL")
	add_definitions(-DASIM_CALL)
endif()

if(CAPTURE_COSIM)
    message("Compile definition: Capture test data")
    add_definitions(-DCAPTURE_COSIM)
endif()

if (NOINLINE)
    message("Compile definition: no-inline")
    add_compile_options(-fno-inline)
endif()

if (GCOV)
    message("Compile definition: GCOV")
    add_compile_options(-g -fno-inline -fprofile-arcs -ftest-coverage)
endif()


if(NOT "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "riscv")
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

set(SOURCES_FILES
    hls.cpp
    xmem.cpp
    main.c
    tgcapture.cpp
    hls_apcall.cpp
)

if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "riscv")
    list (APPEND SOURCES_FILES riscv/riscv_sys.cpp)
endif()

if(WIN32 OR ("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "armv7l"))
    option (BUILD_SHARED_LIBS "enabled static library instead of shared" OFF)
else()
    option (BUILD_SHARED_LIBS "enabled static library instead of shared" OFF)
endif()
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_XKEYCHECK_H)
endif(MSVC)

if (FUNCLOG)
    foreach(file IN LISTS SOURCES_FILES)
        set_source_files_properties(${file} PROPERTIES COMPILE_OPTIONS "-finstrument-functions")
    endforeach()
endif()

if (NOINLINE)
    add_compile_options(-fno-inline)
endif()


if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "riscv")
    set(TARGET_EXEC_NAME example.elf)
else()
    #riscv: libcrt already include relevant pthread functions
    if(PTHREADS_FOUND)
        target_link_libraries(autonet_example pthread)
    endif()
    set(TARGET_EXEC_NAME example)
endif()

include_directories(.)

option(ENABLE_EXECUTABLE "Generate the test application" ON)

if(ENABLE_EXECUTABLE)

    set(LINK_LIBRARIES_LIST )
    
    if(MINGW)
        list(APPEND LINK_LIBRARIES_LIST -lwinmm)
    endif()

    if(GCOV)
        list(APPEND LINK_LIBRARIES_LIST gcov)
    endif()

    # Build executable
    add_executable(${TARGET_EXEC_NAME} ${SOURCES_FILES})
    # Link executable
    target_link_libraries(${TARGET_EXEC_NAME} ${LINK_LIBRARIES_LIST})
endif()


if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "riscv")
    add_custom_command(
        TARGET ${TARGET_EXEC_NAME} POST_BUILD
        COMMAND python3 gen_image.py  ${TARGET_EXEC_NAME} example.img -p riscv32-unknown-elf-objcopy -t riscv32-unknown-elf-
    )
    
    add_custom_command(
        TARGET ${TARGET_EXEC_NAME} POST_BUILD
        COMMAND riscv32-unknown-elf-objdump -d ${TARGET_EXEC_NAME} > example.S
        #disasm with source
        #COMMAND riscv32-unknown-elf-objdump -S -d ${TARGET_EXEC_NAME} > example.S
    )
endif()

