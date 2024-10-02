#ifndef _HLS_APCALL_H_
#define _HLS_APCALL_H_

//#include <stdio.h>
//#include <iostream>
//#include <stdlib.h>
#include <stdint.h>
#include "hls.h"

#ifdef __cplusplus
extern "C" {
#endif

#if ASIM_CALL
    #define ASIM_SET_HEVC_CONTEXT() asim_set_hevc_context(HEVCCONTEXT_CALL_NoComma)
#else
    #define ASIM_SET_HEVC_CONTEXT()
#endif

#if __riscv && ASIM_CALL
void asim_hls_handler(void) __attribute__ ((interrupt ("supervisor")));
#endif

#if __riscv && (HLS_XMEM || ASIM_CALL)

//----- Longtail HLS with get cabac interface ----
#if HLS_GET_CABAC || HLS_CMDR || ASIM_CALL
    #define LONGTAIL_WITH_CABAC     1
#else
    #define LONGTAIL_WITH_CABAC     0
#endif

// Calculate the cycle count within apcall by mcycle32()  
#define APCALL_PROFILE      0

//----- Non-blocking ap call (for riscv fpga only) ----
#define NON_BLOCK_AP_CALL   0
#define HLS_SELECT          ((NON_BLOCK_AP_CALL)? 3 : 1)

//----- Hardware dma for memcpy/memset (for riscv fpga only) ----
#if ASIM_CALL
    #define HLS_MTDMA               0
#else
    #define HLS_MTDMA               1
#endif

//----- Select HLS run by hardware -----
#define riscv_xor_diff_type                                      1
#define riscv_assign_array_complete                              1
#define riscv_array_xor                                          1
#define riscv_vector_add                                         1

enum long_tail_id
{
    enum_xor_diff_type,
    enum_assign_array_complete,
    enum_array_xor,
    enum_vector_add
};

#define ENUM_FUNC_NUM       4
#define ENUM_FUNC_NUM_ALIGN (((ENUM_FUNC_NUM + 31) / 32) * 32) //aligned with 32-bytes
#define HLS_THREAD          8
#define NEW_HLS_PROF_CNT    0   //edward 2024-08-16: new hardware cycle counter for longtail hls profiling

#if APCALL_PROFILE
    typedef struct {
        uint32_t count;
        uint32_t acc_cycle;
        uint32_t min;
        uint32_t max;
        //edward 2024-08-16: cycle excluding cache miss
        #if NEW_HLS_PROF_CNT
        uint32_t acc_cycle_no_dc;
        uint32_t min_no_dc;
        uint32_t max_no_dc;
        #endif
    }apcall_profile_t;

    extern apcall_profile_t apcall_profile[HLS_THREAD][ENUM_FUNC_NUM_ALIGN];
    extern const char *hls_func_name[];
    
  //edward 2024-08-16: new hardware cycle counter for longtail hls profiling
  #if NEW_HLS_PROF_CNT

    #define APCALL_PROFILE_START()          
    #define APCALL_PROFILE_STOP(FUNC_ID)    uint32_t thd = mhartid();                                   \
                                            uint32_t diff = longtail_cycle1();                          \
                                            uint32_t diff_no_dc = longtail_cycle2();                    \
                                            if (diff > apcall_profile[thd][FUNC_ID].max)                \
                                                apcall_profile[thd][FUNC_ID].max = diff;                \
                                            if (diff < apcall_profile[thd][FUNC_ID].min)                \
                                                apcall_profile[thd][FUNC_ID].min = diff;                \
                                            apcall_profile[thd][FUNC_ID].acc_cycle += diff;             \
                                            if (diff_no_dc > apcall_profile[thd][FUNC_ID].max_no_dc)    \
                                                apcall_profile[thd][FUNC_ID].max_no_dc = diff_no_dc;    \
                                            if (diff_no_dc < apcall_profile[thd][FUNC_ID].min_no_dc)    \
                                                apcall_profile[thd][FUNC_ID].min_no_dc = diff_no_dc;    \
                                            apcall_profile[thd][FUNC_ID].acc_cycle_no_dc += diff_no_dc; \
                                            ++apcall_profile[thd][FUNC_ID].count;

  #else

    #define APCALL_PROFILE_START()          uint32_t cyc = mcycle32();
    #define APCALL_PROFILE_STOP(FUNC_ID)    uint32_t now = mcycle32();                          \
                                            uint32_t thd = mhartid();                           \
                                            uint32_t diff;                                      \
                                            if(now>cyc) {                                       \
                                                diff = now - cyc;                               \
                                            } else {                                            \
                                                diff = cyc - now;                               \
                                                printf("*** cycle overflow ***\n");             \
                                            }                                                   \
                                            if (diff > 100) {                                   \
                                                printf("*** apcall cycle > 100: %s %u ***\n", hls_func_name[FUNC_ID], diff);   \
                                            }                                                   \
                                            if (diff > apcall_profile[thd][FUNC_ID].max)        \
                                                apcall_profile[thd][FUNC_ID].max = diff;        \
                                            if (diff < apcall_profile[thd][FUNC_ID].min)        \
                                                apcall_profile[thd][FUNC_ID].min = diff;        \
                                            apcall_profile[thd][FUNC_ID].acc_cycle += diff;     \
                                            ++apcall_profile[thd][FUNC_ID].count;

  #endif
#else
    #define APCALL_PROFILE_START()
    #define APCALL_PROFILE_STOP(FUNC_ID)
#endif

void apcall_profile_init(void);
void apcall_profile_report(void);

#if DEBUG
    static void TRACE() {}

    template<typename T, typename ... Types>
    static void TRACE (T firstArg, Types ... args) {
        std::cout << firstArg << ' ';

        TRACE(args...);
    }
#else
    #define TRACE(...)
#endif

#ifndef ARRAY_SIZE
    #define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0])
#endif

#if 0
// memcpy may encounter error in xmem because the element may only support dedicated data width 
// use xmem_copy to clone the element to ensure the data width is exactly matched.
template <typename T>
void inline xmem_copy(T *dest, const T *src, size_t num){
    for (size_t i=0; i<num; i++) {
        dest[i] = src[i];
    }
}
#endif

// not use inline function for profilnig
#if APCALL_PROFILE
#define inline __attribute__((noinline))
#endif

static inline void APCALL(xor_diff_type)(uint32_t * xor_val32, uint16_t xor_val16, uint8_t xor_val8){
    //TRACE(xor_val32, xor_val16, xor_val8);
#if (riscv_xor_diff_type == 1u)  // apcall
    APCALL_PROFILE_START();
    asim_xmem_search_range(xor_val32);
    nop();
    ap_call_0(enum_xor_diff_type);
    APCALL_PROFILE_STOP(enum_xor_diff_type);
    //TRACE(xor_val32);
#elif (riscv_xor_diff_type == 2u)
    nop();
    ap_call_nb_0(enum_xor_diff_type);
#elif (riscv_xor_diff_type == 3u)
    nop();
    ap_call_nb_noret_0(enum_xor_diff_type);
#else
    IMPL(xor_diff_type)(xor_val32, xor_val16, xor_val8);
#endif
}
static inline void xor_diff_type_ret(void){
#if (riscv_xor_diff_type == 2u)
    ap_ret(enum_xor_diff_type);
#endif
}


static inline void APCALL(assign_array_complete)(int arr_complete[5], int base){
    //TRACE(arr_complete, base);
#if (riscv_assign_array_complete == 1u)  // apcall
    APCALL_PROFILE_START();
    asim_xmem_search_range(arr_complete);
    nop();
    ap_call_1(enum_assign_array_complete, base);
    APCALL_PROFILE_STOP(enum_assign_array_complete);
    //TRACE(arr_complete);
#elif (riscv_assign_array_complete == 2u)
    nop();
    ap_call_nb_1(enum_assign_array_complete, base);
#elif (riscv_assign_array_complete == 3u)
    nop();
    ap_call_nb_noret_1(enum_assign_array_complete, base);
#else
    IMPL(assign_array_complete)(arr_complete, base);
#endif
}
static inline void assign_array_complete_ret(void){
#if (riscv_assign_array_complete == 2u)
    ap_ret(enum_assign_array_complete);
#endif
}


static inline void APCALL(array_xor)(int arr_d1[10], int arr_s1[10], int arr_s2[10], int count){
    //TRACE(arr_d1, arr_s1, arr_s2, count);
#if (riscv_array_xor == 1u)  // apcall
    APCALL_PROFILE_START();
    asim_xmem_search_range(arr_d1);
    nop();
    ap_call_1(enum_array_xor, count);
    APCALL_PROFILE_STOP(enum_array_xor);
    //TRACE(arr_d1, arr_s1, arr_s2);
#elif (riscv_array_xor == 2u)
    nop();
    ap_call_nb_1(enum_array_xor, count);
#elif (riscv_array_xor == 3u)
    nop();
    ap_call_nb_noret_1(enum_array_xor, count);
#else
    IMPL(array_xor)(arr_d1, arr_s1, arr_s2, count);
#endif
}
static inline void array_xor_ret(void){
#if (riscv_array_xor == 2u)
    ap_ret(enum_array_xor);
#endif
}


static inline void APCALL(vector_add)(vector_2d * vec_d1, const vector_2d * vec_s1, const vector_2d * vec_s2){
    //TRACE(vec_d1, vec_s1, vec_s2);
#if (riscv_vector_add == 1u)  // apcall
    APCALL_PROFILE_START();
    asim_xmem_search_range(vec_d1);
    nop();
    ap_call_0(enum_vector_add);
    APCALL_PROFILE_STOP(enum_vector_add);
    //TRACE(vec_d1, vec_s1, vec_s2);
#elif (riscv_vector_add == 2u)
    nop();
    ap_call_nb_0(enum_vector_add);
#elif (riscv_vector_add == 3u)
    nop();
    ap_call_nb_noret_0(enum_vector_add);
#else
    IMPL(vector_add)(vec_d1, vec_s1, vec_s2);
#endif
}
static inline void vector_add_ret(void){
#if (riscv_vector_add == 2u)
    ap_ret(enum_vector_add);
#endif
}

#if APCALL_PROFILE
#undef inline
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif
