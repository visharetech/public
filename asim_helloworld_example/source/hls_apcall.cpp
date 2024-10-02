#if __riscv

#include <stdio.h>
#include <string.h>
#include "hls_apcall.h"

#ifdef __cplusplus 
extern "C" {
#endif 

#if _DEBUG
    #define DEBUG_PRINTF(...)       printf(__VA_ARGS__);
#else
    #define DEBUG_PRINTF(...)
#endif


#if ASIM_CALL

static dcache_info_t dcache_info[MAX_CORE_NUM];

void /*__attribute__ ((interrupt ("supervisor")))*/ asim_hls_handler(){

    int ret = 0;

    xmem_t *xmem = (xmem_t*)asim_xmem_at();
    enum long_tail_id hls_id = (enum long_tail_id)asim_get_apcall_hls_id();
    
    switch(hls_id){
    case enum_xor_diff_type:
    {
        DEBUG_PRINTF("APCALL HLS function: xor_diff_type\n");
        uint32_t * xor_val32 = &xmem->xor_val32; //xmem pointer
        uint16_t xor_val16 = xmem->xor_val16;  //xmem input
        uint8_t xor_val8 = xmem->xor_val8;  //xmem input
        IMPL(xor_diff_type)(xor_val32, xor_val16, xor_val8);
        break;
    }
    case enum_assign_array_complete:
    {
        DEBUG_PRINTF("APCALL HLS function: assign_array_complete\n");
        int *arr_complete = xmem->arr_complete;  //xmem array
        int base = (int)asim_get_apcall_arg0();
        IMPL(assign_array_complete)(arr_complete, base);
        break;
    }
    case enum_array_xor:
    {
        DEBUG_PRINTF("APCALL HLS function: array_xor\n");
        int *arr_d1 = xmem->arr_d1;  //xmem array
        int *arr_s1 = xmem->arr_s1;  //xmem array
        int *arr_s2 = xmem->arr_s2;  //xmem array
        int count = (int)asim_get_apcall_arg0();
        IMPL(array_xor)(arr_d1, arr_s1, arr_s2, count);
        break;
    }
    case enum_vector_add:
    {
        DEBUG_PRINTF("APCALL HLS function: vector_add\n");
        vector_2d * vec_d1 = &xmem->vec_d1; //xmem pointer
        const vector_2d * vec_s1 = &xmem->vec_s1; //xmem pointer
        const vector_2d * vec_s2 = &xmem->vec_s2; //xmem pointer
        IMPL(vector_add)(vec_d1, vec_s1, vec_s2);
        break;
    }

        default:
            printf("Undefined HLS func ID\n");
    }

    asim_set_apreturn(ret);
}

#endif

#if APCALL_PROFILE

apcall_profile_t apcall_profile[HLS_THREAD][ENUM_FUNC_NUM_ALIGN];

const char *hls_func_name[] = { "xor_diff_type",
    "assign_array_complete",
    "array_xor",
    "vector_add" };

void apcall_profile_init(){
    memset(apcall_profile, 0, sizeof(apcall_profile));
    for (int i=0; i<ENUM_FUNC_NUM; i++){
        for (int j=0; j<HLS_THREAD; j++){
            apcall_profile[j][i].min = 1000000;
//edward 2024-08-16: new hardware cycle counter for longtail hls profiling
#if NEW_HLS_PROF_CNT
            apcall_profile[j][i].min_no_dc = 1000000;
#endif
        }
    }
}

void apcall_profile_report(){
    printf("write apcall profile\n");
    FILE *f = fopen("apcall_profile.csv", "w");
    if (!f) {
        printf("cannot open apcall_profile.csv\n");
        return;
    }
//edward 2024-08-16: new hardware cycle counter for longtail hls profiling
#if NEW_HLS_PROF_CNT
    fprintf(f, "hls_func_name,count,acc_cycle,min,max,avg,acc_cycle_no_dc,min_no_dc,max_no_dc,avg_no_dc\n");
#else
    fprintf(f, "hls_func_name,count,acc_cycle,min,max,avg\n");
#endif
    for(int i=0; i<ENUM_FUNC_NUM; i++){
        uint32_t count = 0;
        uint32_t acc_cycle = 0;
        uint32_t min = 1000000;
        uint32_t max = 0;
        uint32_t avg = 0;
        uint32_t acc_cycle_no_dc = 0;
        uint32_t min_no_dc = 1000000;
        uint32_t max_no_dc = 0;
        uint32_t avg_no_dc = 0;
        
        for (int j=0; j<HLS_THREAD; j++){
            count += apcall_profile[j][i].count;
            acc_cycle += apcall_profile[j][i].acc_cycle;
            if (apcall_profile[j][i].max > max)
                max = apcall_profile[j][i].max;
            if (apcall_profile[j][i].min < min)
                min = apcall_profile[j][i].min;
//edward 2024-08-16: new hardware cycle counter for longtail hls profiling
#if NEW_HLS_PROF_CNT
            acc_cycle_no_dc += apcall_profile[j][i].acc_cycle_no_dc;
            if (apcall_profile[j][i].max_no_dc > max_no_dc)
                max_no_dc = apcall_profile[j][i].max_no_dc;
            if (apcall_profile[j][i].min_no_dc < min_no_dc)
                min_no_dc = apcall_profile[j][i].min_no_dc;
#endif
        }
        if (count != 0) {
            avg = acc_cycle / count;
            avg_no_dc = acc_cycle_no_dc / count;
        }
        else {
            min = 0;
            min_no_dc = 0;
        }
//edward 2024-08-16: new hardware cycle counter for longtail hls profiling
#if NEW_HLS_PROF_CNT
        fprintf(f, "\"%s\", %u, %u, %u, %u, %u, %u, %u, %u, %u\n", hls_func_name[i], count, acc_cycle, min, max, avg, acc_cycle_no_dc, min_no_dc, max_no_dc, avg_no_dc);
#else
        fprintf(f, "\"%s\", %u, %u, %u, %u, %u\n", hls_func_name[i], count, acc_cycle, min, max, avg);
#endif
    }
    fclose(f);
}

#else

void apcall_profile_init(){
}

void apcall_profile_report(){
}

#endif

#ifdef __cplusplus 
}
#endif

#endif //__riscv
