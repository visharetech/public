#include <memory>
#include "hls_config.h"
#include "hls.h"
#include "tgload.h"
#if __VITIS_HLS__
    #include <ap_int.h>
#endif

#ifndef TBCONFIG_ALL
    #define TBCONFIG_ALL                             0
#endif

#ifndef TBCONFIG_XOR_DIFF_TYPE
    #define TBCONFIG_XOR_DIFF_TYPE                   0
#endif

#ifndef TBCONFIG_ASSIGN_ARRAY_COMPLETE
    #define TBCONFIG_ASSIGN_ARRAY_COMPLETE           0
#endif

#ifndef TBCONFIG_ARRAY_XOR
    #define TBCONFIG_ARRAY_XOR                       0
#endif

#ifndef TBCONFIG_VECTOR_ADD
    #define TBCONFIG_VECTOR_ADD                      0
#endif



template<typename TSRC, typename TDST>
void array_to_ap_uint(TSRC *src, size_t src_count, TDST &dst){
    size_t idx = 0;
    for(size_t i=0; i<src_count; i++){
        for (size_t j=0; j<32; j++) {
            if (idx < dst.length()) {
                uint8_t value = (src[i] >> j) & 1;
                dst[idx] = value;
                ++idx;
            }
        }
    }
}

template<typename TSRC, typename TDST>
void ap_uint_to_array(TSRC &src, TDST *dst, size_t dst_count){
    size_t idx = 0;
    for(size_t i=0; i<dst_count; i++){
        dst[i] = 0;
        for (size_t j=0; j<32; j++) {
            if (idx < src.length()) {
                uint8_t value = src[idx];
                if (value) {
                    dst[i] |= (1 << j);
                }
                ++idx;
            }
        }
    }
}

#if (DCACHE_SIZE >= 0x1FFFFFFF)
    #error "DCACHE_SIZE is too large for cosimulation, please minimize the DACHE_SIZE for cosimulation and revert back the DCACHE_SIZE to 0x1FFFFFFF after cosim"
#endif

bool test_xor_diff_type(){
#if (TBCONFIG_XOR_DIFF_TYPE || TBCONFIG_ALL)
    printf("Test xor_diff_type\n");
    // define input variables 
    uint16_t xor_val16;
    uint8_t xor_val8;

    // define arrays

    // define temp variables
    uint32_t xor_val32;


    // start loading 
    tgLoad("xor_diff_type_output.bin")

    int total_count = tgCount();
    if (total_count == 0) {
    	printf("error: cannot determine the count\n");
        return false;
    }
    for (int iterator=0; iterator<total_count; iterator++){
        tgPop(xor_val32,xor_val16,xor_val8);
        // call the function
        xor_diff_type(&xor_val32,xor_val16,xor_val8);

        tgCheck(xor_val32);
    }

    printf("Passed after %d\n", total_count);
    return true;
#else
    printf("Skip xor_diff_type\n");
    return true;
#endif
}

bool test_assign_array_complete(){
#if (TBCONFIG_ASSIGN_ARRAY_COMPLETE || TBCONFIG_ALL)
    printf("Test assign_array_complete\n");
    // define input variables 
    int base;

    // define arrays
    int arr_complete [5];

    // define temp variables


    // start loading 
    tgLoad("assign_array_complete_output.bin")

    int total_count = tgCount();
    if (total_count == 0) {
    	printf("error: cannot determine the count\n");
        return false;
    }
    for (int iterator=0; iterator<total_count; iterator++){
        tgPop(arr_complete,base);
        // call the function
        assign_array_complete(arr_complete,base);

        tgCheck(arr_complete);
    }

    printf("Passed after %d\n", total_count);
    return true;
#else
    printf("Skip assign_array_complete\n");
    return true;
#endif
}

bool test_array_xor(){
#if (TBCONFIG_ARRAY_XOR || TBCONFIG_ALL)
    printf("Test array_xor\n");
    // define input variables 
    int count;

    // define arrays
    int arr_d1 [10];
    int arr_s1 [10];
    int arr_s2 [10];

    // define temp variables


    // start loading 
    tgLoad("array_xor_output.bin")

    int total_count = tgCount();
    if (total_count == 0) {
    	printf("error: cannot determine the count\n");
        return false;
    }
    for (int iterator=0; iterator<total_count; iterator++){
        tgPop(arr_d1,arr_s1,arr_s2,count);
        // call the function
        array_xor(arr_d1,arr_s1,arr_s2,count);

        tgCheck(arr_d1,arr_s1,arr_s2);
    }

    printf("Passed after %d\n", total_count);
    return true;
#else
    printf("Skip array_xor\n");
    return true;
#endif
}

bool test_vector_add(){
#if (TBCONFIG_VECTOR_ADD || TBCONFIG_ALL)
    printf("Test vector_add\n");
    // define input variables 

    // define arrays

    // define temp variables
    vector_2d vec_d1;
    vector_2d vec_s1;
    vector_2d vec_s2;


    // start loading 
    tgLoad("vector_add_output.bin")

    int total_count = tgCount();
    if (total_count == 0) {
    	printf("error: cannot determine the count\n");
        return false;
    }
    for (int iterator=0; iterator<total_count; iterator++){
        tgPop(vec_d1,vec_s1,vec_s2);
        // call the function
        vector_add(&vec_d1,&vec_s1,&vec_s2);

        tgCheck(vec_d1,vec_s1,vec_s2);
    }

    printf("Passed after %d\n", total_count);
    return true;
#else
    printf("Skip vector_add\n");
    return true;
#endif
}



int main(){
    test_xor_diff_type();
    test_assign_array_complete();
    test_array_xor();
    test_vector_add();

    return 0;
}