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
    } _type##_ptr_array;

#define AT_ARRAY_INIT(_array) (_array).count = 0; memset((_array).data, 0, sizeof((_array).data))
#define AT_ARRAY_ADD(_array, _value) (_array).data[(_array).count++] = _value
#define AT_ARRAY_ADD_EMPTY(_array) (_array).count++
#define AT_ARRAY_ADD_ARRAY(_array, _other) for (sz i = 0; i < (_other).count; i++) { AT_ARRAY_ADD(_array, (_other).data[i]); }
#define AT_ARRAY_REMOVE(_array, _index) for (sz i = _index; i < (_array).count - 1; i++) { (_array).data[i] = (_array).data[i + 1]; } (_array).count--
#define AT_ARRAY_REMOVE_LAST(_array) (_array).count--
#define AT_ARRAY_CLEAR(_array) (_array).count = 0
#define AT_ARRAY_GET(_array, _index) (_array).data[_index]
#define AT_ARRAY_GET_PTR(_array, _index) &(_array).data[_index]
#define AT_ARRAY_LAST(_array) (_array).data[(_array).count - 1]
#define AT_ARRAY_LAST_PTR(_array) &(_array).data[(_array).count - 1]
#define AT_ARRAY_FIRST(_array) (_array).data[0]
#define AT_ARRAY_FIRST_PTR(_array) &(_array).data[0]
#define AT_ARRAY_SIZE(_array) (_array).count
#define AT_ARRAY_RESIZE(_array, _size) (_array).count = _size
#define AT_ARRAY_SORT(array, type, compare_func) do { if ((array).count > 1) { qsort((array).data, (array).count, sizeof(type), compare_func); } } while (0)
#define AT_ARRAY_FOREACH(_array, _type, _value, _exec) for (sz i = 0; i < (_array).count; i++) { _type _value = (_array).data[i]; _exec; }
#define AT_ARRAY_FOREACH_PTR(_array, _type, _value, _exec) for (sz i = 0; i < (_array).count; i++) { _type* _value = &(_array).data[i]; _exec; }
#define AT_ARRAY_FOREACH_CONST_PTR(_array, _type, _value, _exec) for (sz i = 0; i < (_array).count; i++) { const _type* _value = &(_array).data[i]; _exec; }
#define AT_ARRAY_FOREACH_REVERSE(_array, _type, _value, _exec) for (sz i = (_array).count - 1; i >= 0; i--) { _type _value = (_array).data[i]; _exec; }
#define AT_ARRAY_FOREACH_PTR_REVERSE(_array, _type, _value, _exec) for (sz i = (_array).count - 1; i >= 0; i--) { _type* _value = &(_array).data[i]; _exec; }
#define AT_ARRAY_REMOVE_WITH_PREDICATE(_array, _type, _value, _predicate) for (sz j = 0; j < (_array).count; j++) { _type _value = (_array).data[j]; if (_predicate) { AT_ARRAY_REMOVE(_array, j); break; } }
#define AT_ARRAY_REMOVE_WITH_PTR_PREDICATE(_array, _type, _value, _predicate) for (sz j = 0; j < (_array).count; j++) { _type* _value = &(_array).data[j]; if (_predicate) { AT_ARRAY_REMOVE(_array, j); break; } }

#define AT_ARRAY_DEFAULT_SIZE 4096

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


#define AT_DECLARE_DYNAMIC_ARRAY(_type) \
    typedef struct { \
        _type* data; \
        sz count; \
        sz capacity; \
    } _type##_dynamic_array;

#define AT_DYNAMIC_ARRAY_INIT(_array) (_array).count = 0; (_array).capacity = 0; (_array).data = NULL
#define AT_DYNAMIC_ARRAY_ADD(_array, _value) if ((_array).count == (_array).capacity) { (_array).capacity = (_array).capacity == 0 ? 1 : (_array).capacity * 2; (_array).data = realloc((_array).data, sizeof((_array).data[0]) * (_array).capacity); } (_array).data[(_array).count++] = _value
#define AT_DYNAMIC_ARRAY_REMOVE(_array, _index) for (sz i = _index; i < (_array).count - 1; i++) { (_array).data[i] = (_array).data[i + 1]; } (_array).count--
#define AT_DYNAMIC_ARRAY_REMOVE_LAST(_array) (_array).count--
#define AT_DYNAMIC_ARRAY_CLEAR(_array) (_array).count = 0
#define AT_DYNAMIC_ARRAY_GET(_array, _index) (_array).data[_index]
#define AT_DYNAMIC_ARRAY_GET_PTR(_array, _index) &(_array).data[_index]
#define AT_DYNAMIC_ARRAY_LAST(_array) (_array).data[(_array).count - 1]
#define AT_DYNAMIC_ARRAY_SIZE(_array) (_array).count
#define AT_DYNAMIC_ARRAY_FOREACH(_array, _type, _value, _exec) for (sz i = 0; i < (_array).count; i++) { _type _value = (_array).data[i]; _exec; }
#define AT_DYNAMIC_ARRAY_FOREACH_PTR(_array, _type, _value, _exec) for (sz i = 0; i < (_array).count; i++) { _type* _value = &(_array).data[i]; _exec; }
#define AT_DYNAMIC_ARRAY_FOREACH_REVERSE(_array, _type, _value, _exec) for (sz i = (_array).count - 1; i >= 0; i--) { _type _value = (_array).data[i]; _exec; }
#define AT_DYNAMIC_ARRAY_FOREACH_PTR_REVERSE(_array, _type, _value, _exec) for (sz i = (_array).count - 1; i >= 0; i--) { _type* _value = &(_array).data[i]; _exec; }
#define AT_DYNAMIC_ARRAY_REMOVE_WITH_PREDICATE(_array, _type, _value, _predicate) for (sz j = 0; j < (_array).count; j++) { _type _value = (_array).data[j]; if (_predicate) { AT_DYNAMIC_ARRAY_REMOVE(_array, j); break; } }
#define AT_DYNAMIC_ARRAY_REMOVE_WITH_PTR_PREDICATE(_array, _type, _value, _predicate) for (sz j = 0; j < (_array).count; j++) { _type* _value = &(_array).data[j]; if (_predicate) { AT_DYNAMIC_ARRAY_REMOVE(_array, j); break; } }

AT_DECLARE_DYNAMIC_ARRAY(void);
AT_DECLARE_DYNAMIC_ARRAY(c8);
AT_DECLARE_DYNAMIC_ARRAY(b8);
AT_DECLARE_DYNAMIC_ARRAY(u8);
AT_DECLARE_DYNAMIC_ARRAY(i8);
AT_DECLARE_DYNAMIC_ARRAY(u16);
AT_DECLARE_DYNAMIC_ARRAY(i16);
AT_DECLARE_DYNAMIC_ARRAY(u32);
AT_DECLARE_DYNAMIC_ARRAY(i32);
AT_DECLARE_DYNAMIC_ARRAY(u64);
AT_DECLARE_DYNAMIC_ARRAY(i64);
AT_DECLARE_DYNAMIC_ARRAY(f32);
AT_DECLARE_DYNAMIC_ARRAY(f64);
AT_DECLARE_DYNAMIC_ARRAY(sz);
