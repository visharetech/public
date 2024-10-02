#ifndef _HLS_H_
#define _HLS_H_


#if __VITIS_HLS__
    #include <ap_int.h>
#endif

#include <stdint.h>
#include <stddef.h>
#include "hls_config.h"
#include "xmem.h"
#include "common_with_hls.h"


#ifdef __cplusplus
extern "C" {
#endif

void HLS_DECLARE(xor_diff_type)(uint32_t *xor_val32, uint16_t xor_val16, uint8_t xor_val8);
void HLS_DECLARE(assign_array_complete)(int arr_complete[5], int base);
void HLS_DECLARE(array_xor)(int arr_d1[10], int arr_s1[10], int arr_s2[10], int count);
void HLS_DECLARE(vector_add)(vector_2d *vec_d1, const vector_2d *vec_s1, const vector_2d *vec_s2);


#ifdef __cplusplus
}
#endif

#endif
