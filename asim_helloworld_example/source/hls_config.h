#ifndef _HLS_CONFIG_H_
#define _HLS_CONFIG_H_

#ifndef CAPTURE_COSIM
    #define CAPTURE_COSIM   0
#endif

#if (__riscv && (HLS_XMEM || ASIM_CALL))
    #define HLS_DECLARE(func_name)  func_name##_impl
    #define IMPL(func_name)         func_name##_impl
    #define CAPTURE_(func_name)     func_name##_capture
    #define APCALL(func_name)       func_name
#elif (CAPTURE_COSIM)
    #define HLS_DECLARE(func_name)  func_name
    #define IMPL(func_name)         func_name##_impl
    #define CAPTURE_(func_name)     func_name
    #define APCALL(func_name)       func_name##_apcall
#else
    #define HLS_DECLARE(func_name)  func_name
    #define IMPL(func_name)         func_name
    #define CAPTURE_(func_name)     func_name##_capture
    #define APCALL(func_name)       func_name##_apcall
#endif

//INTF is for asim native call interface
#define INTF(func_name)             func_name##INTF

#endif