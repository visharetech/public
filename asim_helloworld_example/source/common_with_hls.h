#ifndef COMMON_WITH_HLS_H
#define COMMON_WITH_HLS_H

#include <stdint.h>
#include <stdbool.h>
#ifndef WIN32
#define _ALIGN                  __attribute__((aligned(4)))
#else
#define _ALIGN 
#endif

#define TEST_MV_POC
#define MRG_MAX_NUM_CANDS     5
#define MAX_REFS 16
#define STRUCT_REMOVE_PRED_FLAG      //remove the pred_flag in struct MvField 


#if __VITIS_HLS__        //also add __riscv here in order to let reorder xmem python script run properly
    typedef int32_t dcache_ptr_t;
#else
    typedef intptr_t dcache_ptr_t;
#endif


//=== function arbiter begin
//TODO: 1. Need to clarify FUNC_ARBITER_ARG_NUM
//TODO: 2. Need to clarify FUNC_ARBITER_FIFO_DEPTH
//TODO: 3. Need to clarify cmd_ptr value in call_child_wait_ret because cmd_ptr is increment in ST_SEND_CMD, the ST_WAIT_RES will use incremental value that it never use 0. 

#define FUNC_ARBITER_ARG_NUM         8      //9
#define FUNC_ARBITER_FIFO_DEPTH      1

typedef struct {
    int32_t id;
    int32_t param[FUNC_ARBITER_ARG_NUM];
}child_cmd_t;

typedef enum st_enum{
    ST_SEND_CMD,
    ST_WAIT_RES,
    ST_DONE
} st_t;

//=== function arbiter end

enum SAOType {
    SAO_NOT_APPLIED = 0,
    SAO_BAND,
    SAO_EDGE,
    SAO_APPLIED
};

enum SAOEOClass {
    SAO_EO_HORIZ = 0,
    SAO_EO_VERT,
    SAO_EO_135D,
    SAO_EO_45D,
};

#ifndef __VITIS_HLS__
enum SyntaxElement {
    SAO_MERGE_FLAG = 0,
    SAO_TYPE_IDX,
    SAO_EO_CLASS,
    SAO_BAND_POSITION,
    SAO_OFFSET_ABS,
    SAO_OFFSET_SIGN,
    END_OF_SLICE_FLAG,
    SPLIT_CODING_UNIT_FLAG,
    CU_TRANSQUANT_BYPASS_FLAG,
    SKIP_FLAG,
    CU_QP_DELTA,
    PRED_MODE_FLAG,
    PART_MODE,
    PCM_FLAG,
    PREV_INTRA_LUMA_PRED_FLAG,
    MPM_IDX,
    REM_INTRA_LUMA_PRED_MODE,
    INTRA_CHROMA_PRED_MODE,
    MERGE_FLAG,
    MERGE_IDX,
    INTER_PRED_IDC,
    REF_IDX_L0,
    REF_IDX_L1,
    ABS_MVD_GREATER0_FLAG,
    ABS_MVD_GREATER1_FLAG,
    ABS_MVD_MINUS2,
    MVD_SIGN_FLAG,
    MVP_LX_FLAG,
    NO_RESIDUAL_DATA_FLAG,
    SPLIT_TRANSFORM_FLAG,
    CBF_LUMA,
    CBF_CB_CR,
    TRANSFORM_SKIP_FLAG,
    EXPLICIT_RDPCM_FLAG,
    EXPLICIT_RDPCM_DIR_FLAG,
    LAST_SIGNIFICANT_COEFF_X_PREFIX,
    LAST_SIGNIFICANT_COEFF_Y_PREFIX,
    LAST_SIGNIFICANT_COEFF_X_SUFFIX,
    LAST_SIGNIFICANT_COEFF_Y_SUFFIX,
    SIGNIFICANT_COEFF_GROUP_FLAG,
    SIGNIFICANT_COEFF_FLAG,
    COEFF_ABS_LEVEL_GREATER1_FLAG,
    COEFF_ABS_LEVEL_GREATER2_FLAG,
    COEFF_ABS_LEVEL_REMAINING,
    COEFF_SIGN_FLAG,
    LOG2_RES_SCALE_ABS,
    RES_SCALE_SIGN_FLAG,
    CU_CHROMA_QP_OFFSET_FLAG,
    CU_CHROMA_QP_OFFSET_IDX,
#if COM16_C806_EMT
    EMT_CU_FLAG,
    EMT_TU_IDX,
#endif
};
#endif

typedef struct SAOParams {
    uint8_t offset_abs[3][4]  _ALIGN;   ///< sao_offset_abs
    uint8_t offset_sign[3][4]  _ALIGN;  ///< sao_offset_sign

    uint8_t band_position[3] _ALIGN;   ///< sao_band_position
#if __VITIS_HLS__
    uint8_t padding_1a;
#endif

    int16_t offset_val[3][5] _ALIGN;   ///<SaoOffsetVal
#if __VITIS_HLS__
    int16_t padding_2a;
#endif

    uint8_t eo_class[3] _ALIGN;        ///< sao_eo_class
#if __VITIS_HLS__
    uint8_t padding_3a;
#endif
    uint8_t type_idx[3] _ALIGN;        ///< sao_type_idx
#if __VITIS_HLS__
    uint8_t padding_4a;
#endif
    int sao_merge_left_flag _ALIGN;
    int sao_merge_up_flag _ALIGN;
} SAOParams;


enum SliceType {
    B_SLICE = 0,
    P_SLICE = 1,
    I_SLICE = 2,
};
#define EPEL_EXTRA_BEFORE           1
#define EPEL_EXTRA_AFTER            2
#define EPEL_EXTRA                  3
#define QPEL_EXTRA_BEFORE           3
#define QPEL_EXTRA_AFTER            4
#define QPEL_EXTRA                  7
#define ACTIVE_PU_UPSAMPLING        1
#define ACTIVE_BOTH_FRAME_AND_PU    0
#define EDGE_EMU_BUFFER_STRIDE      80
#define NUM_INTRA_MODE              35
#define NTAPS_LUMA                  8                               // Number of taps for luma
#define NTAPS_CHROMA                4                               // Number of taps for chroma
#define IF_INTERNAL_PREC            14                              // Number of bits for internal precision
#define IF_FILTER_PREC              6                               // Log2 of sum of filter taps
#define IF_INTERNAL_OFFS            (1 << (IF_INTERNAL_PREC - 1))   // Offset used internally

static const uint8_t l0_l1_cand_idx[12][2] = {
    { 0, 1, },
    { 1, 0, },
    { 0, 2, },
    { 2, 0, },
    { 1, 2, },
    { 2, 1, },
    { 0, 3, },
    { 3, 0, },
    { 1, 3, },
    { 3, 1, },
    { 2, 3, },
    { 3, 2, },
};
//edward 2024-06-28: calculate_transposed_value_hls
#define USE_CALCULATE_TRANSPOSED_VALUE_HLS 1

enum IntraPredMode {
    INTRA_PLANAR = 0,
    INTRA_DC,
    INTRA_ANGULAR_2,
    INTRA_ANGULAR_3,
    INTRA_ANGULAR_4,
    INTRA_ANGULAR_5,
    INTRA_ANGULAR_6,
    INTRA_ANGULAR_7,
    INTRA_ANGULAR_8,
    INTRA_ANGULAR_9,
    INTRA_ANGULAR_10,
    INTRA_ANGULAR_11,
    INTRA_ANGULAR_12,
    INTRA_ANGULAR_13,
    INTRA_ANGULAR_14,
    INTRA_ANGULAR_15,
    INTRA_ANGULAR_16,
    INTRA_ANGULAR_17,
    INTRA_ANGULAR_18,
    INTRA_ANGULAR_19,
    INTRA_ANGULAR_20,
    INTRA_ANGULAR_21,
    INTRA_ANGULAR_22,
    INTRA_ANGULAR_23,
    INTRA_ANGULAR_24,
    INTRA_ANGULAR_25,
    INTRA_ANGULAR_26,
    INTRA_ANGULAR_27,
    INTRA_ANGULAR_28,
    INTRA_ANGULAR_29,
    INTRA_ANGULAR_30,
    INTRA_ANGULAR_31,
    INTRA_ANGULAR_32,
    INTRA_ANGULAR_33,
    INTRA_ANGULAR_34,
};

enum ScanType {
    SCAN_DIAG = 0,
    SCAN_HORIZ,
    SCAN_VERT,
};

enum PartMode {
	PART_2Nx2N = 0,
	PART_2NxN = 1,
	PART_Nx2N = 2,
	PART_NxN = 3,
	PART_2NxnU = 4,
	PART_2NxnD = 5,
	PART_nLx2N = 6,
	PART_nRx2N = 7,
};

enum PredMode {
    MODE_INTER = 0,
    MODE_INTRA,
    MODE_SKIP,
};

enum PredFlag {
    PF_INTRA = 0,
    PF_L0,
    PF_L1,
    PF_BI,
};

enum LOG2_TRAFO_SIZE {
    LOG2_TRAFO_SIZE_2 = 2,
    LOG2_TRAFO_SIZE_3 = 3,
    LOG2_TRAFO_SIZE_4 = 4,
    LOG2_TRAFO_SIZE_5 = 5
};

enum INTRA_PRED_SIDE {
    INTRA_PRED_SIDE_1 = 1,
    INTRA_PRED_SIDE_2 = 2
};


//FIXED: Vitis HLS sometimes recognize wrong MvField struct size (Actual:20 vs Expected:32) in some HLS functions. Seems it doesn't support _ALIGN keyword well, manually added padding to workaround this issue.
typedef struct Mv {
    int16_t x   _ALIGN;  ///< horizontal component of motion vector
#if __VITIS_HLS__
    int16_t padding_0;
#endif

    int16_t y   _ALIGN;  ///< vertical component of motion vector
#if __VITIS_HLS__
    int16_t padding_1;
#endif
} Mv;

//FIXED: Vitis HLS sometimes recognize wrong MvField struct size (Actual:20 vs expected:32) in some HLS functions. Seems it doesn't support _ALIGN keyword well, manually added padding to workaround this issue.
typedef struct MvField {
    Mv mv[2]  _ALIGN;
#ifdef TEST_MV_POC
    int32_t poc[2]  _ALIGN;
#endif
    uint8_t ref_idx[2]  _ALIGN;
#if __VITIS_HLS__
    uint8_t padding_4a;
    uint8_t padding_4b;
#endif

#ifndef STRUCT_REMOVE_PRED_FLAG
    uint8_t pred_flag  _ALIGN;
#if __VITIS_HLS__
    uint8_t padding_5a;
    uint8_t padding_5b;
    uint8_t padding_5c;
#endif
#endif
} MvField;


//Convert struct of array Mv mv[2] to uint16_t mv0_x, mv0_y, mv1_x, mv1_y
typedef struct MvField_hls {
    int16_t mv_0_x  _ALIGN;
#if __VITIS_HLS__
    int16_t padding_0;
#endif
    int16_t mv_0_y  _ALIGN;
#if __VITIS_HLS__
    int16_t padding_1;
#endif
    int16_t mv_1_x  _ALIGN;
#if __VITIS_HLS__
    int16_t padding_2;
#endif
    int16_t mv_1_y  _ALIGN;
#if __VITIS_HLS__
    int16_t padding_3;
#endif

#ifdef TEST_MV_POC
    int32_t poc[2]  _ALIGN;
#endif

    uint8_t ref_idx[2]  _ALIGN;
#if __VITIS_HLS__
    uint8_t padding_4a;
    uint8_t padding_4b;
#endif

#ifndef STRUCT_REMOVE_PRED_FLAG
    uint8_t pred_flag  _ALIGN;
#if __VITIS_HLS__
    uint8_t padding_5a;
    uint8_t padding_5b;
    uint8_t padding_5c;
#endif
#endif
} MvField_hls;


typedef struct WeightValues
{
    int w       _ALIGN;
    int oo      _ALIGN; //o is special keyword
    int offset  _ALIGN;
    int shift   _ALIGN;
    int round   _ALIGN;
}WeightValues;

static inline bool uint32array_getbit(uint32_t *bitarray, unsigned int bit_idx) {
    unsigned int byte_idx = bit_idx >> 5;
    unsigned int offset = bit_idx & 31;
    return (bitarray[byte_idx] >> offset) & 1;
}

static inline void uint32array_setbit(uint32_t *bitarray, unsigned int bit_idx, unsigned char bit){
    unsigned int byte_idx = bit_idx >> 5;
    unsigned int offset = bit_idx & 31;

    if (bit){
        bitarray[byte_idx] |= (1 << offset);
    } else {
        bitarray[byte_idx] &= ~(1 << offset);
    }
}

#define FFABS(a) ((a) >= 0 ? (a) : (-(a)))

#define u32_setbit(src, idx)	(src[(idx) >> 5] |= 1 << ((idx) & 31))
#define u32_getbit(src, idx)	((src[(idx) >> 5] >> ((idx) & 31)) & 1)


#define u32b2_set(src, idx, value)			\
											\
		do {										\
			int b = ((idx) & 15) << 1;				\
			src[(idx) >> 4] &= ~(3 << b);			\
			src[(idx) >> 4] |= ((value & 3) << b);	\
		} while(0)
		
#define u32b2_get(src, idx)	((src[(idx) >> 4] >> (((idx) & 15) << 1)) & 3)

#endif