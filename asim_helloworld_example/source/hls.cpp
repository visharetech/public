#include "hls.h"
#include "xmem.h"
#include "hls_config.h"

#ifdef __cplusplus
extern "C" {
#endif

//Below function demonstrate different data type are supported
void IMPL(xor_diff_type)(uint32_t *xor_val32, uint16_t xor_val16, uint8_t xor_val8){
    *xor_val32 = xor_val16 ^ xor_val8;
}

// Below function demonstrate it can identify complete partition
void IMPL(assign_array_complete)(int arr_complete[5], int base){
#pragma HLS array_partition variable=arr_complete type=complete
    for (int i=0; i<5; i++){
        arr_complete[i] = base + i;
    }
}

//Below function demonstrate it can support array in xmem
// but the count is not declared in xmem, it will be passed by APCALL argument
void IMPL(array_xor)(int arr_d1[10], int arr_s1[10], int arr_s2[10], int count){
    constexpr size_t MAX_ARR_SIZE = 10;

    for (int i=0; i<MAX_ARR_SIZE && i < count; i++){
        arr_d1[i] = arr_s1[i] ^ arr_s2[i];
    }
}

//Below function demonstrate it can support struct in xmem
void IMPL(vector_add)(vector_2d *vec_d1, const vector_2d *vec_s1, const vector_2d *vec_s2){
#pragma HLS disaggregate variable=vec_s1
#pragma HLS disaggregate variable=vec_s2
#pragma HLS disaggregate variable=vec_d1

    vec_d1->x = vec_s1->x + vec_s2->x;
    vec_d1->y = vec_s1->y + vec_s2->y;
}

#ifdef __cplusplus
}
#endif

#if CAPTURE_COSIM
	#include "hls_capture.cpp"
#endif
