// abstract-trader https://github.com/ougi-washi/abstract-trader
#pragma once

#include "types.h"

// HEAVY WIP, not recommended for production use

typedef struct at_json {
    c8* data;
    sz size;
} at_json;

extern at_json at_parse_json(const c8* data);
extern void at_free_json(at_json* json);
extern c8* at_json_get_string(at_json* json, const c8* key);
extern f64 at_json_get_number(at_json* json, const c8* key);
extern i32 at_json_get_integer(at_json* json, const c8* key);
extern at_json at_json_get_object(at_json* json, const c8* key);
extern at_json at_json_get_array(at_json* json, const c8* key);
extern sz at_json_get_array_size(at_json* json);
extern at_json at_json_get_array_item(at_json* json, sz index);
extern b8 at_json_is_valid(at_json* json);
extern void at_json_print(at_json* json);
extern c8* at_json_to_string(at_json* json);
extern void at_json_set_string(at_json* json, const c8* key, const c8* value);
extern void at_json_set_number(at_json* json, const c8* key, f64 value);
extern void at_json_set_integer(at_json* json, const c8* key, i32 value);
extern void at_json_set_object(at_json* json, const c8* key, at_json* value);
extern void at_json_set_array(at_json* json, const c8* key, at_json* value);
extern void at_json_set_array_item(at_json* json, sz index, at_json* value);
extern void at_json_remove(at_json* json, const c8* key);
extern void at_json_remove_array_item(at_json* json, sz index);
extern void at_json_clear(at_json* json);
extern b8 at_json_has_key(at_json* json, const c8* key);
extern b8 at_json_has_index(at_json* json, sz index);
extern b8 at_json_is_string(at_json* json, const c8* key);
extern b8 at_json_is_number(at_json* json, const c8* key);
extern b8 at_json_is_integer(at_json* json, const c8* key);
extern b8 at_json_is_object(at_json* json, const c8* key);
extern b8 at_json_is_array(at_json* json, const c8* key);
extern b8 at_json_is_null(at_json* json, const c8* key);
extern b8 at_json_is_true(at_json* json, const c8* key);
extern b8 at_json_is_false(at_json* json, const c8* key);
extern b8 at_json_is_empty(at_json* json);
extern b8 at_json_is_equal(at_json* json1, at_json* json2);
extern at_json at_json_copy(at_json* json);
extern void at_json_merge(at_json* json1, at_json* json2);
extern void at_json_merge_recursive(at_json* json1, at_json* json2);

