#ifndef HLS_LONG_TAIL_H
#define HLS_LONG_TAIL_H

//#include "hls_common.h"
#include "../riscv/syscall.h"
#include "../riscv/io_map.h"
#ifndef _MSC_VER
#define nop() asm volatile("nop")
#else
#define nop()
#endif

#if ASIM_CALL
    #define ASIM_HLS_HANDLER()      asm volatile ("ecall");
#else
    #define ASIM_HLS_HANDLER()
#endif

//Use HLS function arbiter (NOT FOR ASIM)
#if ASIM_CALL
    #define HLS_FUNCTION_ARB  0
#else
    #define HLS_FUNCTION_ARB  0  //1
#endif

//New ap control command (SET_AP_CTRL)
#define NEW_AP_CTRL_CMD   1

//XMEM adress base
#define DEC_XMEM_BASE     (LONGTAIL_IO + (XMEM_ACCESS << 20))
#define DEC_DMA_BASE      (LONGTAIL_IO + (DMA_ACCESS << 20))

//RISCV Command
#define XMEM_ACCESS       0    //HLS xmem access
#define SET_AP_START      1    //Set HLS ap start and argument[0].
#define SET_AP_ARGUMENT   2    //Set HLS ap argument with selected vector index.
#define WAIT_AP_DONE      3    //Wait HLS ap done (blocking) and get return[0].
#define CHECK_AP_DONE     4    //Check value of HLS ap done (non-blocking).
#define GET_AP_RETRUN     5    //Get HLS return with selected vector index.
#define DMA_ACCESS		  6    //DMA access
#define SET_AP_START_NB   7    //Set HLS ap start and argument[0] without return.
#define SET_AP_CTRL       8    //Signle command for SET_AP_START, SET_AP_START_NB, SET_AP_ARGUMENT and WAIT_AP_DONE
#define GET_HLS_CYCLE1    9    //HLS profiling: cycle between ap_start & ap_done.
#define GET_HLS_CYCLE2    10   //HLS profiling: cycle between ap_start & ap_done but no cache miss.

//AP control command (SET_AP_CTRL)
#define AP_CTRL_START     0    //Same as SET_AP_START.
#define AP_CTRL_START_NB  1    //Same as SET_AP_START_NB.
#define AP_CTRL_ARGUMENT  2    //Same as SET_AP_ARGUMENT.
#define AP_CTRL_DONE      3    //Same as WAIT_AP_DONE.

static ALWAYS_INLINE void longtail_wr(int command, int idx, int value)
{
    volatile unsigned int *longtail_io = (volatile unsigned int *)((LONGTAIL_IO) + (command << 20) + (idx << 2));
    *longtail_io = value;
}
static ALWAYS_INLINE uint32_t longtail_rd(int command, int idx)
{
    volatile unsigned int *longtail_io = (volatile unsigned int *)((LONGTAIL_IO) + (command << 20) + (idx << 2));
    return *longtail_io;
}

static ALWAYS_INLINE void longtail_wr2(int command, int hls, int arg, int value)
{
#if _MSC_VER
    volatile unsigned int *longtail_io = (volatile unsigned int *)((LONGTAIL_IO) + (SET_AP_CTRL << 20) + (command << 2) + (arg << 4) + (hls << 7));
    *longtail_io = value;
#else
    uint32_t adr = (LONGTAIL_IO) + (SET_AP_CTRL << 20) + (command << 2) + (arg << 4) + (hls << 7);
    asm volatile ("sw   %0, %2(%1)" ::"r"(value),"r"(adr & (~0x7ff)),"i"(adr & 0x7ff));
#endif
}
static ALWAYS_INLINE uint32_t longtail_rd2(int command, int hls)
{
#if _MSC_VER
    volatile unsigned int *longtail_io = (volatile unsigned int *)((LONGTAIL_IO) + (SET_AP_CTRL << 20) + (command << 2) + (hls << 7));
    return *longtail_io;
#else
    uint32_t ret;
    uint32_t adr = (LONGTAIL_IO) + (SET_AP_CTRL << 20) + (command << 2) + (hls << 7);
    asm volatile ("lw   %0, %2(%1)" :"=r"(ret):"r"(adr & (~0x7ff)),"i"(adr & 0x7ff));
    return ret;
#endif

}
//edward 2024-08-19: hardware cycle counter to profile longtail HLS
static ALWAYS_INLINE uint32_t longtail_cycle1()
{
#if _MSC_VER
    volatile unsigned int *longtail_io = (volatile unsigned int *)((LONGTAIL_IO) + (GET_HLS_CYCLE1));
    return *longtail_io;
#else
    uint32_t ret;
    uint32_t adr = (LONGTAIL_IO) + (GET_HLS_CYCLE1 << 20);
    asm volatile ("lw   %0, %2(%1)" :"=r"(ret):"r"(adr & (~0x7ff)),"i"(adr & 0x7ff));
    return ret;
#endif
}
//edward 2024-08-19: hardware cycle counter (excluding cache miss) to profile longtail HLS
static ALWAYS_INLINE uint32_t longtail_cycle2()
{
#if _MSC_VER
    volatile unsigned int *longtail_io = (volatile unsigned int *)((LONGTAIL_IO) + (GET_HLS_CYCLE2));
    return *longtail_io;
#else
    uint32_t ret;
    uint32_t adr = (LONGTAIL_IO) + (GET_HLS_CYCLE2 << 20);
    asm volatile ("lw   %0, %2(%1)" :"=r"(ret):"r"(adr & (~0x7ff)),"i"(adr & 0x7ff));
    return ret;
#endif
}

//AP Command macro
#if NEW_AP_CTRL_CMD 
  #define AP_ARG(idx)         longtail_wr2(AP_CTRL_ARGUMENT, hls_id, idx, arg##idx)
  #define AP_START()          longtail_wr2(AP_CTRL_START, hls_id, 0, 0)
  #define AP_START_NB()       longtail_wr2(AP_CTRL_START_NB, hls_id, 0, 0)
  #define AP_START_ARG()      longtail_wr2(AP_CTRL_START, hls_id, 0, arg0)
  #define AP_START_ARG_NB()   longtail_wr2(AP_CTRL_START_NB, hls_id, 0, arg0)
  #define AP_RETURN()         longtail_rd2(AP_CTRL_DONE, hls_id)
#else
  #define AP_ARG(idx)         longtail_wr(SET_AP_ARGUMENT, idx, arg##idx)
  #define AP_START()          longtail_wr(SET_AP_START, hls_id, 0)
  #define AP_START_NB()       longtail_wr(SET_AP_START_NB, hls_id, 0)
  #define AP_START_ARG()      longtail_wr(SET_AP_START, hls_id, arg0)
  #define AP_START_ARG_NB()   longtail_wr(SET_AP_START_NB, hls_id, arg0)
  #define AP_RETURN()         longtail_rd(WAIT_AP_DONE, 0)
#endif


//ap_call          : Start and wait done.
//ap_call_nb       : Start without blocking. Need call ap_ret() to pop return fifo later.
//ap_call_nb_noret : start with fire and forget. No need call ap_ret().

#if HLS_FUNCTION_ARB
#include "hls_dataflow.h"
#define CHILD    (hls_id + 2)  //CHILD 0 is RISCV; CHILD 1 is dataflow
//Request child control
static ALWAYS_INLINE void call_itf_ctrl(int child, int argc, int reqRet)
{
    if (reqRet)
    {
        if (argc == 0)      asm volatile ("slti x0, x16, %0" ::"i"(child + (1 << 10)));
        else if (argc == 1) asm volatile ("slti x0, x17, %0" ::"i"(child + (1 << 10)));
        else if (argc == 2) asm volatile ("slti x0, x18, %0" ::"i"(child + (1 << 10)));
        else if (argc == 3) asm volatile ("slti x0, x19, %0" ::"i"(child + (1 << 10)));
        else if (argc == 4) asm volatile ("slti x0, x20, %0" ::"i"(child + (1 << 10)));
        else if (argc == 5) asm volatile ("slti x0, x21, %0" ::"i"(child + (1 << 10)));
        else if (argc == 6) asm volatile ("slti x0, x22, %0" ::"i"(child + (1 << 10)));
        else if (argc == 7) asm volatile ("slti x0, x23, %0" ::"i"(child + (1 << 10)));
        else if (argc == 8) asm volatile ("slti x0, x24, %0" ::"i"(child + (1 << 10)));
    }
    else
    {
        if (argc == 0)      asm volatile ("slti x0, x0, %0" ::"i"(child + (1 << 10)));
        else if (argc == 1) asm volatile ("slti x0, x1, %0" ::"i"(child + (1 << 10)));
        else if (argc == 2) asm volatile ("slti x0, x2, %0" ::"i"(child + (1 << 10)));
        else if (argc == 3) asm volatile ("slti x0, x3, %0" ::"i"(child + (1 << 10)));
        else if (argc == 4) asm volatile ("slti x0, x4, %0" ::"i"(child + (1 << 10)));
        else if (argc == 5) asm volatile ("slti x0, x5, %0" ::"i"(child + (1 << 10)));
        else if (argc == 6) asm volatile ("slti x0, x6, %0" ::"i"(child + (1 << 10)));
        else if (argc == 7) asm volatile ("slti x0, x7, %0" ::"i"(child + (1 << 10)));
        else if (argc == 8) asm volatile ("slti x0, x8, %0" ::"i"(child + (1 << 10)));
    }
}
//Get return valid from child (timeout version)
static ALWAYS_INLINE int call_itf_return_timeout(int child, uint32_t timeout_cyc)
{    
    int vld, dat;
    uint32_t cyc = cycle32();
    do {
        uint32_t d = cycle32() - cyc;
        if (d > timeout_cyc) {
            exit(1);
        }
        asm volatile ("csrrw %0, %1, x0" :"=r"(vld) :"i"(CSR_CALL_VLD));
    } while (vld == 0);
    asm volatile ("csrrw %0, %1, x0" :"=r"(dat) :"i"(CSR_CALL_DAT));
    return dat;
}
//Get return valid from child
static ALWAYS_INLINE int call_itf_return(int child)
{    
    int vld, dat;
    do {
        asm volatile ("csrrw %0, %1, x0" :"=r"(vld) :"i"(CSR_CALL_VLD));
    } while (vld == 0);
    asm volatile ("csrrw %0, %1, x0" :"=r"(dat) :"i"(CSR_CALL_DAT));
    return dat;
}
#endif

//Return
static ALWAYS_INLINE int ap_ret(int hls_id)
{
#if HLS_FUNCTION_ARB
    return call_itf_return(CHILD);
#else
    return AP_RETURN();
#endif
}

//No argument HLS function call
static ALWAYS_INLINE int ap_call_0(int hls_id)
{
#if HLS_FUNCTION_ARB
    call_itf_ctrl(CHILD, 0, 1);
    hls_func0();
    return call_itf_return(CHILD);
#else
    AP_START();
    ASIM_HLS_HANDLER();
    //nop();
    return AP_RETURN();
#endif
}
static ALWAYS_INLINE void ap_call_nb_0(int hls_id)
{
#if HLS_FUNCTION_ARB
    call_itf_ctrl(CHILD, 0, 1);
    hls_func0();
#else
    AP_START();
#endif
}
static ALWAYS_INLINE void ap_call_nb_noret_0(int hls_id)
{
#if HLS_FUNCTION_ARB
    call_itf_ctrl(CHILD, 0, 0);
    hls_func0();
#else
    AP_START_NB();
#endif
}

//1 argument HLS function call
static ALWAYS_INLINE int ap_call_1(int hls_id, int arg0)
{
#if HLS_FUNCTION_ARB
    call_itf_ctrl(CHILD, 1, 1);
    hls_func1(arg0);
    return call_itf_return(CHILD);
#else
    AP_START_ARG();
    ASIM_HLS_HANDLER();
    //nop();
    return AP_RETURN();
#endif
}
static ALWAYS_INLINE void ap_call_nb_1(int hls_id, int arg0)
{
#if HLS_FUNCTION_ARB
    call_itf_ctrl(CHILD, 1, 1);
    hls_func1(arg0);
#else
    AP_START_ARG();
#endif
}
static ALWAYS_INLINE void ap_call_nb_noret_1(int hls_id, int arg0)
{
#if HLS_FUNCTION_ARB
    call_itf_ctrl(CHILD, 1, 0);
    hls_func1(arg0);
#else
    AP_START_ARG_NB();
#endif
}

//2 arguments HLS function call
static ALWAYS_INLINE int ap_call_2(int hls_id, int arg0, int arg1)
{
#if HLS_FUNCTION_ARB
    call_itf_ctrl(CHILD, 2, 1);
    hls_func2(arg0, arg1);
    return call_itf_return(CHILD);
#else
    AP_ARG(1);
    AP_START_ARG();
    ASIM_HLS_HANDLER();
    //nop();
    return AP_RETURN();
#endif
}
static ALWAYS_INLINE void ap_call_nb_2(int hls_id, int arg0, int arg1)
{
#if HLS_FUNCTION_ARB
    call_itf_ctrl(CHILD, 2, 1);
    hls_func2(arg0, arg1);
#else
    AP_ARG(1);
    AP_START_ARG();
#endif
}
static ALWAYS_INLINE void ap_call_nb_noret_2(int hls_id, int arg0, int arg1)
{
#if HLS_FUNCTION_ARB
    call_itf_ctrl(CHILD, 2, 0);
    hls_func2(arg0, arg1);
#else
    AP_ARG(1);
    AP_START_ARG_NB();
#endif
}

//3 arguments HLS function call
static ALWAYS_INLINE int ap_call_3(int hls_id, int arg0, int arg1, int arg2)
{
#if HLS_FUNCTION_ARB
    call_itf_ctrl(CHILD, 3, 1);
    hls_func3(arg0, arg1, arg2);
    return call_itf_return(CHILD);
#else
    AP_ARG(1);
    AP_ARG(2);
    AP_START_ARG();
    ASIM_HLS_HANDLER();
    //nop();
    return AP_RETURN();
#endif
}
static ALWAYS_INLINE void ap_call_nb_3(int hls_id, int arg0, int arg1, int arg2)
{
#if HLS_FUNCTION_ARB
    call_itf_ctrl(CHILD, 3, 1);
    hls_func3(arg0, arg1, arg2);
#else
    AP_ARG(1);
    AP_ARG(2);
    AP_START_ARG();
#endif
}
static ALWAYS_INLINE void ap_call_nb_noret_3(int hls_id, int arg0, int arg1, int arg2)
{
#if HLS_FUNCTION_ARB
    call_itf_ctrl(CHILD, 3, 0);
    hls_func3(arg0, arg1, arg2);
#else
    AP_ARG(1);
    AP_ARG(2);
    AP_START_ARG_NB();
#endif    
}

//4 arguments HLS function call
static ALWAYS_INLINE int ap_call_4(int hls_id, int arg0, int arg1, int arg2, int arg3)
{
#if HLS_FUNCTION_ARB
    call_itf_ctrl(CHILD, 4, 1);
    hls_func4(arg0, arg1, arg2, arg3);
    return call_itf_return(CHILD);
#else
    AP_ARG(1);
    AP_ARG(2);
    AP_ARG(3);
    AP_START_ARG();
    ASIM_HLS_HANDLER();
    //nop();
    return AP_RETURN();
#endif
}
static ALWAYS_INLINE void ap_call_nb_4(int hls_id, int arg0, int arg1, int arg2, int arg3)
{
#if HLS_FUNCTION_ARB
    call_itf_ctrl(CHILD, 4, 1);
    hls_func4(arg0, arg1, arg2, arg3);
#else
    AP_ARG(1);
    AP_ARG(2);
    AP_ARG(3);
    AP_START_ARG();
#endif
}
static ALWAYS_INLINE void ap_call_nb_noret_4(int hls_id, int arg0, int arg1, int arg2, int arg3)
{
#if HLS_FUNCTION_ARB
    call_itf_ctrl(CHILD, 4, 0);
    hls_func4(arg0, arg1, arg2, arg3);
#else
    AP_ARG(1);
    AP_ARG(2);
    AP_ARG(3);
    AP_START_ARG_NB();
#endif
}

//5 arguments HLS function call
static ALWAYS_INLINE int ap_call_5(int hls_id, int arg0, int arg1, int arg2, int arg3,
                            int arg4)
{
#if HLS_FUNCTION_ARB
    call_itf_ctrl(CHILD, 5, 1);
    hls_func5(arg0, arg1, arg2, arg3, arg4);
    return call_itf_return(CHILD);
#else
    AP_ARG(1);
    AP_ARG(2);
    AP_ARG(3);
    AP_ARG(4);
    AP_START_ARG();
    ASIM_HLS_HANDLER();
    //nop();
    return AP_RETURN();
#endif
}
static ALWAYS_INLINE void ap_call_nb_5(int hls_id, int arg0, int arg1, int arg2, int arg3,
                                    int arg4)
{
#if HLS_FUNCTION_ARB
    call_itf_ctrl(CHILD, 5, 1);
    hls_func5(arg0, arg1, arg2, arg3, arg4);
#else
    AP_ARG(1);
    AP_ARG(2);
    AP_ARG(3);
    AP_ARG(4);
    AP_START_ARG();
#endif
}
static ALWAYS_INLINE void ap_call_nb_noret_5(int hls_id, int arg0, int arg1, int arg2, int arg3,
                                    int arg4)
{
#if HLS_FUNCTION_ARB
    call_itf_ctrl(CHILD, 5, 0);
    hls_func5(arg0, arg1, arg2, arg3, arg4);
#else
    AP_ARG(1);
    AP_ARG(2);
    AP_ARG(3);
    AP_ARG(4);
    AP_START_ARG_NB();
#endif
}

//6 arguments
static ALWAYS_INLINE int ap_call_6(int hls_id, int arg0, int arg1, int arg2, int arg3,
                            int arg4, int arg5)
{
#if HLS_FUNCTION_ARB
    call_itf_ctrl(CHILD, 6, 1);
    hls_func6(arg0, arg1, arg2, arg3, arg4, arg5);
    return call_itf_return(CHILD);
#else
    AP_ARG(1);
    AP_ARG(2);
    AP_ARG(3);
    AP_ARG(4);
    AP_ARG(5);
    AP_START_ARG();
    ASIM_HLS_HANDLER();
    //nop();
    return AP_RETURN();
#endif
}
static ALWAYS_INLINE void ap_call_nb_6(int hls_id, int arg0, int arg1, int arg2, int arg3,
                                    int arg4, int arg5)
{
#if HLS_FUNCTION_ARB
    call_itf_ctrl(CHILD, 6, 1);
    hls_func6(arg0, arg1, arg2, arg3, arg4, arg5);
#else
    AP_ARG(1);
    AP_ARG(2);
    AP_ARG(3);
    AP_ARG(4);
    AP_ARG(5);
    AP_START_ARG();
#endif
}
static ALWAYS_INLINE void ap_call_nb_noret_6(int hls_id, int arg0, int arg1, int arg2, int arg3,
                                    int arg4, int arg5)
{
#if HLS_FUNCTION_ARB
    call_itf_ctrl(CHILD, 6, 0);
    hls_func6(arg0, arg1, arg2, arg3, arg4, arg5);
#else
    AP_ARG(1);
    AP_ARG(2);
    AP_ARG(3);
    AP_ARG(4);
    AP_ARG(5);
    AP_START_ARG_NB();
#endif
}

//7 arguments HLS function call
static ALWAYS_INLINE int ap_call_7(int hls_id, int arg0, int arg1, int arg2, int arg3,
                            int arg4, int arg5, int arg6)
{
#if HLS_FUNCTION_ARB
    call_itf_ctrl(CHILD, 7, 1);
    hls_func7(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
    return call_itf_return(CHILD);
#else
    AP_ARG(1);
    AP_ARG(2);
    AP_ARG(3);
    AP_ARG(4);
    AP_ARG(5);
    AP_ARG(6);
    AP_START_ARG();
    ASIM_HLS_HANDLER();
    //nop();
    return AP_RETURN();
#endif
}
static ALWAYS_INLINE void ap_call_nb_7(int hls_id, int arg0, int arg1, int arg2, int arg3,
                                    int arg4, int arg5, int arg6)
{
#if HLS_FUNCTION_ARB
    call_itf_ctrl(CHILD, 7, 1);
    hls_func7(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
#else
    AP_ARG(1);
    AP_ARG(2);
    AP_ARG(3);
    AP_ARG(4);
    AP_ARG(5);
    AP_ARG(6);
    AP_START_ARG();
#endif
}
static ALWAYS_INLINE void ap_call_nb_noret_7(int hls_id, int arg0, int arg1, int arg2, int arg3,
                                    int arg4, int arg5, int arg6)
{
#if HLS_FUNCTION_ARB
    call_itf_ctrl(CHILD, 7, 0);
    hls_func7(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
#else    
    AP_ARG(1);
    AP_ARG(2);
    AP_ARG(3);
    AP_ARG(4);
    AP_ARG(5);
    AP_ARG(6);
    AP_START_ARG_NB();
#endif
}

//8 arguments HLS function call
static ALWAYS_INLINE int ap_call_8(int hls_id, int arg0, int arg1, int arg2, int arg3,
                            int arg4, int arg5, int arg6, int arg7)
{
#if HLS_FUNCTION_ARB
    call_itf_ctrl(CHILD, 8, 1);
    hls_func8(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    return call_itf_return(CHILD);
#else
    AP_ARG(1);
    AP_ARG(2);
    AP_ARG(3);
    AP_ARG(4);
    AP_ARG(5);
    AP_ARG(6);
    AP_ARG(7);
    AP_START_ARG();
    ASIM_HLS_HANDLER();
    //nop();
    return AP_RETURN();
#endif
}
static ALWAYS_INLINE void ap_call_nb_8(int hls_id, int arg0, int arg1, int arg2, int arg3,
                                    int arg4, int arg5, int arg6, int arg7)
{
#if HLS_FUNCTION_ARB
    call_itf_ctrl(CHILD, 8, 1);
    hls_func8(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
#else
    AP_ARG(1);
    AP_ARG(2);
    AP_ARG(3);
    AP_ARG(4);
    AP_ARG(5);
    AP_ARG(6);
    AP_ARG(7);
    AP_START_ARG();
#endif
}
static ALWAYS_INLINE void ap_call_nb_noret_8(int hls_id, int arg0, int arg1, int arg2, int arg3,
                                    int arg4, int arg5, int arg6, int arg7)
{
#if HLS_FUNCTION_ARB
    call_itf_ctrl(CHILD, 8, 0);
    hls_func8(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
#else
    AP_ARG(1);
    AP_ARG(2);
    AP_ARG(3);
    AP_ARG(4);
    AP_ARG(5);
    AP_ARG(6);
    AP_ARG(7);
    AP_START_ARG_NB();
#endif
}

static ALWAYS_INLINE void * get_riscv_xmem_base(void)
{
    return (void*)(LONGTAIL_IO + (XMEM_ACCESS << 20));
}

#endif