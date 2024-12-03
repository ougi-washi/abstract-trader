// abstract-trader https://github.com/ougi-washi/abstract-trader
#pragma once

#include "types.h"

// HEAVY WIP, not recommended for production use

typedef enum { AT_JSON_STRING, AT_JSON_NUMBER, AT_JSON_OBJECT, AT_JSON_ARRAY, AT_JSON_BOOL, AT_JSON_NULL } at_json_type;
typedef struct at_json {
    at_json_type type;
    c8 *key;
    union {
        c8 *string;
        f64 number;
        struct at_json **object; // Object or Array
        i32 boolean;
    };
    sz size; // Object/Array size
} at_json;

extern at_json *at_json_parse(const c8 *json);
extern void at_json_free(at_json *value);
extern void at_json_serialize(at_json *value, c8 *buffer, i32 *pos);
extern const c8 *at_json_get_string(at_json *value, const c8 *key);
extern i32 at_json_get_int(at_json *value, const c8 *key);
extern f32 at_json_get_float(at_json *value, const c8 *key);
extern b8 at_json_get_bool(at_json *value, const c8 *key);
extern at_json *at_json_get_object(at_json *value, const char *key);
extern at_json *at_json_get_array(at_json *value, const c8 *key);
extern at_json *at_json_get_array_item(at_json *array, i32 index);
extern sz at_json_get_array_size(at_json *array);
extern sz at_json_get_i32_array(at_json *value, const c8 *key, i32 *out_array, sz max_size);
extern sz at_json_get_u32_array(at_json *value, const c8 *key, u32 *out_array, sz max_size);
extern sz at_json_get_string_array(at_json *value, const c8 *key, c8 **out_array, sz max_size);