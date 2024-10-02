#ifndef _TG_COMMON_H_
#define _TG_COMMON_H_

#include <cstdint>
#include <type_traits>
#include "common_with_hls.h"

typedef uint8_t datatype_t;


static constexpr uint8_t UNKNWON_TYPE       = 0;
static constexpr uint8_t IS_NATIVE_TYPE     = 1 << 0;
static constexpr uint8_t IS_ARRAY_TYPE      = 1 << 1;
static constexpr uint8_t IS_POINTER_TYPE    = 1 << 2;
static constexpr uint8_t IS_STRUCT_TYPE     = 1 << 3;

template<typename T>
static datatype_t get_datatype(const T& data) {
    datatype_t datatype = UNKNWON_TYPE;
    if (std::is_class<T>::value) {
        datatype |= IS_STRUCT_TYPE;
    }
    if (std::is_pointer<T>::value) {
        datatype |= IS_POINTER_TYPE;
    }
    if (std::is_array<T>::value) {
        datatype |= IS_ARRAY_TYPE;
    }
    if (std::is_fundamental<T>::value){
        datatype |= IS_NATIVE_TYPE;
    }

    return datatype;
}


#endif