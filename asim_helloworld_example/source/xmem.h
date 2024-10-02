#ifndef _XMEM_H_
#define _XMEM_H_

#include <stdint.h>
#include "common_with_hls.h"
#if __riscv
    #include "riscv/hls_long_tail.h"
#endif

typedef struct {
    int x   _ALIGN;
    int y   _ALIGN;
}vector_2d;


typedef struct xmem_t{
    uint8_t  xor_val8       _ALIGN;
    uint16_t xor_val16      _ALIGN;
    uint32_t xor_val32      _ALIGN;
    
    int arr_complete[5]     _ALIGN;
    
    int arr_s1[10]          _ALIGN;
    int arr_s2[10]          _ALIGN;
    int arr_d1[10]          _ALIGN;

    vector_2d vec_s1        _ALIGN;
    vector_2d vec_s2        _ALIGN;
    vector_2d vec_d1        _ALIGN;
}xmem_t;

#ifdef __cplusplus
extern "C" {
#endif

xmem_t* xmem_malloc(void);
void xmem_read_dbg_section(void);
void xmem_rw_test(void);

#ifdef __cplusplus
}
#endif



#ifndef ARRAY_SIZE
    #define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))
#endif

// Use volatile in xmem here to prevent mismatch load / store instruction being used for memory copy
// e.g. Prevent uint8_t lumaCache[] copy by lw / sw instruction
#ifdef __cplusplus

template <typename T>
static void inline xmem_assign(T *dst, T value)
{
	volatile T *dst_ptr = static_cast<volatile T *>(dst);
	*dst_ptr = value;
}

template <typename T>
static void inline xmem_copy(T *dst, const T *src, size_t num)
{
	for (size_t i = 0; i < num; i++)
	{
		volatile T *dst_ptr = static_cast<volatile T *>(dst);
		dst_ptr[i] = src[i];
	}
}

template <typename T>
static void inline xmem_set(T *dst, T value, size_t num)
{
	for (size_t i = 0; i < num; i++)
	{
		volatile T *dst_ptr = static_cast<volatile T *>(dst);
		dst_ptr[i] = value;
	}
}

#endif

// C version of function template xmem_assign
#define XMEM_ASSIGN(TYPE, DST, VALUE)                    \
	do                                                   \
	{                                                    \
		volatile TYPE *dst_ptr = (volatile TYPE *)(DST); \
		*dst_ptr = VALUE;                                \
	} while (0);

// C version of function template xmem_copy
#define XMEM_COPY(TYPE, DST, SRC, COUNT)                     \
	do                                                       \
	{                                                        \
		volatile TYPE *dst_ptr = (volatile TYPE *)(DST); 	 \
		volatile TYPE *src_ptr = (volatile TYPE *)(SRC); 	 \
		for (size_t i = 0; i < COUNT; i++)                   \
		{                                                    \
			dst_ptr[i] = src_ptr[i];                         \
		}                                                    \
	} while (0);

// C version of function template xmem_set
#define XMEM_SET(TYPE, DST, VAL, COUNT)                      \
	do                                                       \
	{                                                        \
		volatile TYPE *dst_ptr = (volatile TYPE *)(DST); 	 \
		for (size_t i = 0; i < COUNT; i++)                   \
		{                                                    \
			dst_ptr[i] = VAL;                                \
		}                                                    \
	} while (0);

#endif
