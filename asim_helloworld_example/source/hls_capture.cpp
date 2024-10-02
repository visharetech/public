//####################################
// Append auto generated functions to the end of hls.cpp
#include <mutex>
#include "tgcapture.h"

template<typename T> T& ASSIGN_REF(T* ptr, const char *func_name) {
    if(ptr == NULL) {
        static T zero;
        static unsigned int i;
        if (i < 10) {
            std::cout << "warning: " <<  i << ':' << func_name << ": pointer is NULL\n";
            ++i;
        }
        return zero;
    } else {
        return *ptr;
    }
}

template<typename T> const T& ASSIGN_REF(const T* ptr, const char *func_name) {
    if(ptr == NULL) {
        static T zero;
        static unsigned int i;
        if (i < 10) {
            std::cout << "warning: " <<  i << ':' << func_name << ": const pointer is NULL\n";
            ++i;
        }
        return zero;
    } else {
        return *ptr;
    }
}

static const unsigned int MAX_CAPTURE_COUNT = 10000;
static const unsigned int MAX_CAPTURE_INTERVAL = 1;

//==============================================================
// Save cabac data bin
#define CABAC_LOG_START(filename)   static FILE *fbin = NULL;                   \
                                    static bool fbin_opened = false;            \
                                    if (!fbin_opened)   {                       \
                                        fbin = fopen(filename, "wb");           \
                                        if (fbin == NULL) {                     \
                                            printf("cannot open file to write: %s", filename);  \
                                            exit(-1);                           \
                                        }                                       \
                                        fbin_opened = true;                     \
                                    }                                           \
                                    g_fbin = fbin;


#define CABAC_LOG_END()             if (counter == max_counter) {  \
                                        fclose(fbin);                           \
                                        fbin = NULL;                            \
                                    }                                           \
                                    g_fbin = NULL;


static FILE* g_fbin = NULL;

void decodeBin_log(int ctx, int bin)
{
    if (g_fbin != NULL)
    {
        //fputc(ctx & 0xff, outF);
        //fputc((ctx >> 8) & 0xff, outF);
        fputc(bin, g_fbin);
    }
}

//==============================================================

#ifdef __cplusplus
extern "C" {
#endif

void CAPTURE_(xor_diff_type)(uint32_t * xor_val32_ptr,uint16_t xor_val16,uint8_t xor_val8){
#if 1
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);

    // define temporary variables to hold pointers
    uint32_t & xor_val32 = ASSIGN_REF(xor_val32_ptr, __FUNCTION__);


    tgOpen("xor_diff_type_output.bin", xor_val32, xor_val16, xor_val8);

    tgCaptureBeforeCall(xor_val32,xor_val16,xor_val8);

    // call the function with the initial parameters
    IMPL(xor_diff_type)(xor_val32_ptr, xor_val16, xor_val8);

    tgCaptureAfterCall(xor_val32);

    tgClose();
#else
    IMPL(xor_diff_type)(xor_val32_ptr, xor_val16, xor_val8);
#endif
}

void CAPTURE_(assign_array_complete)(int arr_complete[5],int base){
#if 1
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);

    // define temporary variables to hold pointers


    tgOpen("assign_array_complete_output.bin", arr_complete, base);

    tgCaptureBeforeCall(arr_complete,5,base);

    // call the function with the initial parameters
    IMPL(assign_array_complete)(arr_complete, base);

    tgCaptureAfterCall(arr_complete,5);

    tgClose();
#else
    IMPL(assign_array_complete)(arr_complete, base);
#endif
}

void CAPTURE_(array_xor)(int arr_d1[10],int arr_s1[10],int arr_s2[10],int count){
#if 1
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);

    // define temporary variables to hold pointers


    tgOpen("array_xor_output.bin", arr_d1, arr_s1, arr_s2, count);

    tgCaptureBeforeCall(arr_d1,10,arr_s1,10,arr_s2,10,count);

    // call the function with the initial parameters
    IMPL(array_xor)(arr_d1, arr_s1, arr_s2, count);

    tgCaptureAfterCall(arr_d1,10,arr_s1,10,arr_s2,10);

    tgClose();
#else
    IMPL(array_xor)(arr_d1, arr_s1, arr_s2, count);
#endif
}

void CAPTURE_(vector_add)(vector_2d * vec_d1_ptr,const vector_2d * vec_s1_ptr,const vector_2d * vec_s2_ptr){
#if 1
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);

    // define temporary variables to hold pointers
    vector_2d & vec_d1 = ASSIGN_REF(vec_d1_ptr, __FUNCTION__);
    const vector_2d & vec_s1 = ASSIGN_REF(vec_s1_ptr, __FUNCTION__);
    const vector_2d & vec_s2 = ASSIGN_REF(vec_s2_ptr, __FUNCTION__);


    tgOpen("vector_add_output.bin", vec_d1, vec_s1, vec_s2);

    tgCaptureBeforeCall(vec_d1,vec_s1,vec_s2);

    // call the function with the initial parameters
    IMPL(vector_add)(vec_d1_ptr, vec_s1_ptr, vec_s2_ptr);

    tgCaptureAfterCall(vec_d1,vec_s1,vec_s2);

    tgClose();
#else
    IMPL(vector_add)(vec_d1_ptr, vec_s1_ptr, vec_s2_ptr);
#endif
}




#ifdef __cplusplus
}
#endif