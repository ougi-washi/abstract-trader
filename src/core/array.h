// abstract-trader https://github.com/ougi-washi/abstract-trader

#pragma once

#include "types.h"

#define AT_DECLARE_ARRAY(_type, _size) \
    typedef struct { \
        _type data[_size]; \
        sz count; \
    } _type##_array;
#define AT_DECLARE_PTR_ARRAY(_type, _size) \
    typedef struct { \
        _type* data[_size]; \
        sz count; \
    } _type##_array;

#define AT_ARRAY_ADD(_array, _value) \
    (_array).data[(_array).count++] = _value;
#define AT_ARRAY_REMOVE(_array, _index) \
    for (sz i = _index; i < (_array).count - 1; i++) { (_array).data[i] = (_array).data[i + 1]; } \
    (_array).count--;
#define AT_ARRAY_REMOVE_LAST(_array) \
    (_array).count--;
#define AT_ARRAY_CLEAR(_array) \
    (_array).count = 0;
#define AT_ARRAY_GET(_array, _index) \
    (_array).data[_index];
#define AT_ARRAY_LAST(_array) \
    (_array).data[(_array).count - 1];
#define AT_ARRAY_FOREACH(_array, _value, _exec) \
    for (sz i = 0; i < (_array).count; i++) { \
        _value = (_array).data[i]; \
        _exec; }
#define AT_ARRAY_FOREACH_REVERSE(_array, _value, _exec) \
    for (sz i = (_array).count - 1; i >= 0; i--) { \
        _value = (_array).data[i]; \
        _exec; }
#define AT_ARRAY_SIZE(_array) \
    (_array).count;

#define AT_ARRAY_DEFAULT_SIZE 1048576

AT_DECLARE_PTR_ARRAY(void, AT_ARRAY_DEFAULT_SIZE);
AT_DECLARE_PTR_ARRAY(c8, AT_ARRAY_DEFAULT_SIZE);
AT_DECLARE_ARRAY(b8, AT_ARRAY_DEFAULT_SIZE);
AT_DECLARE_ARRAY(u8, AT_ARRAY_DEFAULT_SIZE);
AT_DECLARE_ARRAY(i8, AT_ARRAY_DEFAULT_SIZE);
AT_DECLARE_ARRAY(u16, AT_ARRAY_DEFAULT_SIZE);
AT_DECLARE_ARRAY(i16, AT_ARRAY_DEFAULT_SIZE);
AT_DECLARE_ARRAY(u32, AT_ARRAY_DEFAULT_SIZE);
AT_DECLARE_ARRAY(i32, AT_ARRAY_DEFAULT_SIZE);
AT_DECLARE_ARRAY(u64, AT_ARRAY_DEFAULT_SIZE);
AT_DECLARE_ARRAY(i64, AT_ARRAY_DEFAULT_SIZE);
AT_DECLARE_ARRAY(f32, AT_ARRAY_DEFAULT_SIZE);
AT_DECLARE_ARRAY(f64, AT_ARRAY_DEFAULT_SIZE);
AT_DECLARE_ARRAY(sz, AT_ARRAY_DEFAULT_SIZE);
