#include <stdio.h>
#include <stdlib.h>
#include "xmem.h"

//edward 2023-0721: For hardware, xmem is pointed to xmem io bus.
// xmem_malloc function
xmem_t* xmem_malloc(void)
{
    xmem_t* allocated_memory = NULL;

#if __riscv && HLS_XMEM /*&& !TEST_COPY_XMEM*/
    allocated_memory = (xmem_t*)get_riscv_xmem_base();
#else
    allocated_memory = (xmem_t*)malloc(sizeof(xmem_t));

#if _rvTranslate
    asim_notify_xmem_start(allocated_memory); 
#endif

#endif
    return allocated_memory;
}

void xmem_read_dbg_section(){
#if HLS_XMEM
    void *xmem = get_riscv_xmem_base();

    volatile uint32_t* xmem_dbg_section = (volatile uint32_t*)((uintptr_t)xmem + 0xDB000);
    printf("=== XMEM DBG SECTION ===\n");
    printf("date: %u\n",        xmem_dbg_section[0]);
    printf("enum func: %u\n",   xmem_dbg_section[1]);
    printf("xmem chksum: %u\n", xmem_dbg_section[2]);
    printf("xmem size: %u\n",   xmem_dbg_section[3]);
    printf("hls chksum: %u\n",  xmem_dbg_section[4]);
    printf("hls size: %u\n",    xmem_dbg_section[5]);
#endif
}

template <typename T>
void xmem_element_rw(T & item, const char *s) {
    // Write the value to the standard output
    bool status = true;

    volatile T* item_ptr = (volatile T*)&item;

    *item_ptr = 0x5A5A5A5A;
    if (*item_ptr != ((T)0x5A5A5A5A)) {
        status = false;
    }
    *item_ptr = (T)0xA5A5A5A5;
    if (*item_ptr != ((T)0xA5A5A5A5)) {
        status = false;
    }
    *item_ptr = (T)0x00000000;
    if (*item_ptr != ((T)0x00000000)) {
        status = false;
    }
    *item_ptr = (T)0xFFFFFFFF;
    if (*item_ptr != ((T)0xFFFFFFFF)) {
        status = false;
    }

    if (status) {
        printf("xmem %s rw passed\n", s);
    } else {
        printf("xmem %s rw failed\n", s);
        //while(1){};
    }
}

template <typename T, size_t Size>
void xmem_element_rw(T (&array)[Size], const char *s) {
    bool status = true;
    for (auto & item :array) {
        // Write the value to the standard output
        volatile T* item_ptr = (volatile T*)&item;
        *item_ptr = (T)0x5A5A5A5A;
        if (*item_ptr != ((T)0x5A5A5A5A)) {
            status = false;
            break;
        }
        *item_ptr = (T)0xA5A5A5A5;
        if (*item_ptr != ((T)0xA5A5A5A5)) {
            status = false;
            break;
        }
        *item_ptr = (T)0x00000000;
        if (*item_ptr != ((T)0x00000000)) {
            status = false;
            break;
        }
        *item_ptr = (T)0xFFFFFFFF;
        if (*item_ptr != ((T)0xFFFFFFFF)) {
            status = false;
            break;
        }
    }
    if (status) {
        printf("xmem arr %s rw passed\n", s);
    } else {
        printf("xmem arr %s rw failed\n", s);
        //while(1){};
    }
}

template <typename T, size_t Rows, size_t Cols>
void xmem_element_rw(T (&array)[Rows][Cols], const char *s) {
    bool status = true;
    for (auto & row :array) {
        for (auto & item :row) {
            volatile T* item_ptr = (volatile T*)&item;
            // Write the value to the standard output
            *item_ptr = (T)0x5A5A5A5A;
            if (item != ((T)0x5A5A5A5A)) {
                status = false;
                break;
            }
            *item_ptr = (T)0xA5A5A5A5;
            if (item != ((T)0xA5A5A5A5)) {
                status = false;
                break;
            }
            *item_ptr = (T)0x00000000;
            if (item != ((T)0x00000000)) {
                status = false;
                break;
            }
            *item_ptr = (T)0xFFFFFFFF;
            if (item != ((T)0xFFFFFFFF)) {
                status = false;
                break;
            }
        }
    }
    if (status) {
        printf("xmem 2d arr %s rw passed\n", s);
    } else {
        printf("xmem 2d arr %s rw failed\n", s);
        //while(1){};
    }
}

// Specialization for vector_2d type
template <>
void xmem_element_rw<vector_2d>(vector_2d & item, const char *s) {
	char desc[80];
	snprintf(desc, sizeof(desc), "%s.x", s);
    xmem_element_rw(item.x, desc);

	snprintf(desc, sizeof(desc), "%s.y", s);
    xmem_element_rw(item.y, desc);
}

#define QUOTA_STR(x)	#x
#define xmem_element_verify(x)		xmem_element_rw(x, QUOTA_STR(x))

void xmem_rw_test(){
#if HLS_XMEM
    xmem_t *xmem_ptr = (xmem_t *)get_riscv_xmem_base();
    xmem_t &xmem = *xmem_ptr;

    xmem_element_verify( xmem.arr_s1   );
    xmem_element_verify( xmem.arr_s2   );
    xmem_element_verify( xmem.arr_d1   );
    
    xmem_element_verify( xmem.vec_s1   );
    xmem_element_verify( xmem.vec_s1   );
    xmem_element_verify( xmem.vec_d1   );
    
#endif
}