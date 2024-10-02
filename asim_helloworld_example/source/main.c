#include <stdio.h>
#include "xmem.h"
#include "hls.h"
#include "hls_apcall.h"

void test_xor_diff_type(xmem_t *xmem){
    for (int i=0; i<10000; i++) {
        XMEM_ASSIGN(int8_t, &xmem->xor_val8, i);        // i will be trimmed as 0-0xFF
        XMEM_ASSIGN(int16_t, &xmem->xor_val16, i+1);    // i will be trimmed to 0-0xFFFF

        xor_diff_type(&xmem->xor_val32, xmem->xor_val16, xmem->xor_val8);
    }
    printf("diff_type_add result: 0x%x\n", xmem->xor_val32);
}

void test_assign_arr_complete(xmem_t *xmem){
    for (int i=0; i<10000; i++) {
        assign_array_complete(xmem->arr_complete, i);
    }
    
    printf("assign_array_complete result: ");
    for (int i=0; i<sizeof(xmem->arr_complete)/sizeof(xmem->arr_complete[0]); i++){
        printf("%d ", xmem->arr_complete[i]);
    }
    printf("\n");
}

void test_array_xor(xmem_t *xmem){
    for (int i=0; i<10; i++) {
        xmem->arr_s1[i] = i;
        xmem->arr_s2[i] = i+3;
        xmem->arr_d1[i] = 0;
    }

    for (int i=0; i<10000; i++) {
        array_xor(xmem->arr_d1, xmem->arr_s1, xmem->arr_s2, 10);
    }

    printf("array_xor result: ");
    for (int i=0; i<10; i++){
        printf("%d ", xmem->arr_d1[i]);
    }
    printf("\n");
}

void test_vector_add(xmem_t *xmem){
    xmem->vec_s1.x = 1;
    xmem->vec_s1.y = 2;
    xmem->vec_s2.x = 3;
    xmem->vec_s2.y = 4;

    for (int i=0; i<10000; i++){
        vector_add(&xmem->vec_d1, &xmem->vec_s1, &xmem->vec_s2);
        xmem->vec_s1.x += xmem->vec_d1.x;
        xmem->vec_s1.y += xmem->vec_d1.y;

        xmem->vec_s2.x += xmem->vec_d1.x;
        xmem->vec_s2.y += xmem->vec_d1.y;
    }

    printf("vector_add result: %d %d\n", xmem->vec_d1.x, xmem->vec_d1.y);
}

int main(int argc, char **argv){
    printf("=== main entry ===\n");
    
#if ASIM_CALL
    printf("Enable ASIM_CALL\n");
#endif

#if CAPTURE_COSIM
    printf("Enable CAPTURE_COSIM\n");
#endif

#if _rvTranslate
    asim_startProfiling();
    asim_notify_xmem_size(sizeof(xmem_t));
#endif

#if __riscv
    riscv_init();
#endif

    //xmem_t *xmem = xmem_malloc();
    //
    //xmem_read_dbg_section();
    //xmem_rw_test();
    //
    //test_xor_diff_type(xmem);
    //test_assign_arr_complete(xmem);
    //test_array_xor(xmem);
    //test_vector_add(xmem);
    
    printf("helloworld\n");

    return 0;
}