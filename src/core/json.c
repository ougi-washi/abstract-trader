// abstract-trader https://github.com/ougi-washi/abstract-trader

#include "json.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

static c8 *json_strdup(const c8 *src) {
    c8 *dest = malloc(strlen(src) + 1);
    if (dest) strcpy(dest, src);
    return dest;
}

static const c8 *skip_whitespace(const c8 *str) {
    while (isspace(*str)) str++;
    return str;
}

static const c8 *parse_string(const c8 *json, c8 **out) {
    json++; // Skip the opening quote
    const c8 *start = json;
    while (*json && *json != '\"') json++;
    *out = json_strdup(start);
    (*out)[json - start] = '\0';
    return (*json == '\"') ? json + 1 : NULL; // Skip the closing quote
}

static const c8 *parse_number(const c8 *json, f64 *out) {
    c8 *end;
    *out = strtod(json, &end);
    return end;
}

static const c8 *parse_value(const c8 *json, at_json **out) {
    json = skip_whitespace(json);
    if (*json == '\"') { // String
        *out = malloc(sizeof(at_json));
        (*out)->type = AT_JSON_STRING;
        json = parse_string(json, &((*out)->string));
    } else if (isdigit(*json) || *json == '-') { // Number
        *out = malloc(sizeof(at_json));
        (*out)->type = AT_JSON_NUMBER;
        json = parse_number(json, &((*out)->number));
    } else if (*json == '{') { // Object
        *out = malloc(sizeof(at_json));
        (*out)->type = AT_JSON_OBJECT;
        (*out)->size = 0;
        (*out)->object = NULL;
        json++;
        while (*json && *json != '}') {
            json = skip_whitespace(json);
            c8 *key;
            json = parse_string(json, &key);
            json = skip_whitespace(json);
            if (*json++ != ':') break; // Skip ':'
            at_json *value;
            json = parse_value(json, &value);
            value->key = key;

            (*out)->object = realloc((*out)->object, (++(*out)->size) * sizeof(at_json *));
            (*out)->object[(*out)->size - 1] = value;
            json = skip_whitespace(json);
            if (*json == ',') json++; // Skip ','
        }
        json++; // Skip '}'
    } else if (*json == '[') { // Array
        *out = malloc(sizeof(at_json));
        (*out)->type = AT_JSON_ARRAY;
        (*out)->size = 0;
        (*out)->object = NULL;
        json++;
        while (*json && *json != ']') {
            at_json *value;
            json = parse_value(json, &value);
            (*out)->object = realloc((*out)->object, (++(*out)->size) * sizeof(at_json *));
            (*out)->object[(*out)->size - 1] = value;
            json = skip_whitespace(json);
            if (*json == ',') json++; // Skip ','
        }
        json++; // Skip ']'
    } else if (!strncmp(json, "true", 4)) { // Boolean true
        *out = malloc(sizeof(at_json));
        (*out)->type = AT_JSON_BOOL;
        (*out)->boolean = 1;
        json += 4;
    } else if (!strncmp(json, "false", 5)) { // Boolean false
        *out = malloc(sizeof(at_json));
        (*out)->type = AT_JSON_BOOL;
        (*out)->boolean = 0;
        json += 5;
    } else if (!strncmp(json, "null", 4)) { // Null
        *out = malloc(sizeof(at_json));
        (*out)->type = AT_JSON_NULL;
        json += 4;
    }
    return json;
}

void at_json_serialize(at_json *value, c8 *buffer, i32 *pos) {
    if (!value) return;
    switch (value->type) {
        case AT_JSON_STRING:
            *pos += sprintf(buffer + *pos, "\"%s\"", value->string);
            break;
        case AT_JSON_NUMBER:
            *pos += sprintf(buffer + *pos, "%g", value->number);
            break;
        case AT_JSON_OBJECT:
            *pos += sprintf(buffer + *pos, "{");
            for (i32 i = 0; i < value->size; i++) {
                if (i > 0) *pos += sprintf(buffer + *pos, ",");
                *pos += sprintf(buffer + *pos, "\"%s\":", value->object[i]->key);
                at_json_serialize(value->object[i], buffer, pos);
            }
            *pos += sprintf(buffer + *pos, "}");
            break;
        case AT_JSON_ARRAY:
            *pos += sprintf(buffer + *pos, "[");
            for (i32 i = 0; i < value->size; i++) {
                if (i > 0) *pos += sprintf(buffer + *pos, ",");
                at_json_serialize(value->object[i], buffer, pos);
            }
            *pos += sprintf(buffer + *pos, "]");
            break;
        case AT_JSON_BOOL:
            *pos += sprintf(buffer + *pos, value->boolean ? "true" : "false");
            break;
        case AT_JSON_NULL:
            *pos += sprintf(buffer + *pos, "null");
            break;
    }
}

void at_json_free(at_json *value) {
    if (!value) return;
    if (value->type == AT_JSON_STRING) free(value->string);
    else if (value->type == AT_JSON_OBJECT || value->type == AT_JSON_ARRAY) {
        for (i32 i = 0; i < value->size; i++) {
            at_json_free(value->object[i]);
        }
        free(value->object);
    }
    if (value->key) free(value->key);
    free(value);
}

const c8 *at_json_get_string(at_json *value, const c8 *key) {
    assert(value && key);
    if (value->type != AT_JSON_OBJECT) return NULL;
    for (i32 i = 0; i < value->size; i++) {
        if (strcmp(value->object[i]->key, key) == 0 && value->object[i]->type == AT_JSON_STRING) {
            return value->object[i]->string;
        }
    }
    return NULL;
}

i32 at_json_get_int(at_json *value, const c8 *key) {
    assert(value && key);
    if (value->type != AT_JSON_OBJECT) return 0;
    for (i32 i = 0; i < value->size; i++) {
        if (strcmp(value->object[i]->key, key) == 0 && value->object[i]->type == AT_JSON_NUMBER) {
            return (i32)value->object[i]->number;
        }
    }
    return 0;
}

f32 at_json_get_float(at_json *value, const c8 *key) {
    assert(value && key);
    if (value->type != AT_JSON_OBJECT) return 0.0f;
    for (i32 i = 0; i < value->size; i++) {
        if (strcmp(value->object[i]->key, key) == 0 && value->object[i]->type == AT_JSON_NUMBER) {
            return (f32)value->object[i]->number;
        }
    }
    return 0.0f;
}

b8 at_json_get_bool(at_json *value, const c8 *key) {
    assert(value && key);
    if (value->type != AT_JSON_OBJECT) return 0;
    for (i32 i = 0; i < value->size; i++) {
        if (strcmp(value->object[i]->key, key) == 0 && value->object[i]->type == AT_JSON_BOOL) {
            return value->object[i]->boolean;
        }
    }
    return 0;
}

at_json *at_json_get_object(at_json *value, const char *key) {
    assert(value && key);
    if (value->type != AT_JSON_OBJECT) return NULL;
    for (int i = 0; i < value->size; i++) {
        if (strcmp(value->object[i]->key, key) == 0 && value->object[i]->type == AT_JSON_OBJECT) {
            return value->object[i];
        }
    }
    return NULL;
}

at_json *at_json_parse(const c8 *json) {
    at_json *root = NULL;
    parse_value(skip_whitespace(json), &root);
    return root;
}

at_json *at_json_get_array(at_json *value, const c8 *key) {
    assert(value && key);
    if (value->type != AT_JSON_OBJECT) return NULL;
    for (i32 i = 0; i < value->size; i++) {
        if (strcmp(value->object[i]->key, key) == 0 && value->object[i]->type == AT_JSON_ARRAY) {
            return value->object[i];
        }
    }
    return NULL;
}

at_json *at_json_get_array_item(at_json *array, i32 index) {
    assert(array && index >= 0 && index < array->size);
    if (array->type != AT_JSON_ARRAY || index < 0 || index >= array->size) return NULL;
    return array->object[index];
}

sz at_json_get_array_size(at_json *array){
    assert(array);
    if (array->type != AT_JSON_ARRAY) return 0;
    return array->size;
}

sz at_json_get_i32_array(at_json *value, const c8 *key, i32 *out_array, sz max_size) {
    at_json *array = at_json_get_array(value, key);
    if (!array || array->type != AT_JSON_ARRAY) return 0;

    sz count = 0;
    for (sz i = 0; i < array->size && count < max_size; i++) {
        at_json *item = at_json_get_array_item(array, i);
        if (item && item->type == AT_JSON_NUMBER) {
            out_array[count++] = (i32)item->number;
        }
    }
    return count;
}

sz at_json_get_u32_array(at_json *value, const c8 *key, u32 *out_array, sz max_size) {
    at_json *array = at_json_get_array(value, key);
    if (!array || array->type != AT_JSON_ARRAY) return 0;

    sz count = 0;
    for (sz i = 0; i < array->size && count < max_size; i++) {
        at_json *item = at_json_get_array_item(array, i);
        if (item && item->type == AT_JSON_NUMBER) {
            out_array[count++] = (u32)item->number;
        }
    }
    return count;
}

sz at_json_get_string_array(at_json *value, const c8 *key, c8 **out_array, sz max_size) {
    at_json *array = at_json_get_array(value, key);
    if (!array || array->type != AT_JSON_ARRAY) return 0;

    sz count = 0;
    for (sz i = 0; i < array->size && count < max_size; i++) {
        at_json *item = at_json_get_array_item(array, i);
        if (item && item->type == AT_JSON_STRING) {
            out_array[count++] = json_strdup(item->string); // Duplicate the string to avoid overwriting
        }
    }
    return count;
}

b8 at_json_get_bool_array(at_json *value, const c8 *key, b8 *out_array, sz max_size) {
    at_json *array = at_json_get_array(value, key);
    if (!array || array->type != AT_JSON_ARRAY) return 0;

    sz count = 0;
    for (sz i = 0; i < array->size && count < max_size; i++) {
        at_json *item = at_json_get_array_item(array, i);
        if (item && item->type == AT_JSON_BOOL) {
            out_array[count++] = item->boolean;
        }
    }
    return count;
}

b8 at_json_get_object_array(at_json *value, const c8 *key, at_json **out_array, sz max_size){
    at_json *array = at_json_get_array(value, key);
    if (!array || array->type != AT_JSON_ARRAY) return 0;

    sz count = 0;
    for (sz i = 0; i < array->size && count < max_size; i++) {
        at_json *item = at_json_get_array_item(array, i);
        if (item && item->type == AT_JSON_OBJECT) {
            out_array[count++] = item;
        }
    }
    return count;
}

void serialize_string(const c8 *key, const c8 *value, c8 *buffer, i32 *pos){
    if (value) {
        *pos += sprintf(buffer + *pos, "\"%s\":\"%s\"", key, value);
    }
}

void serialize_number(const c8 *key, f64 value, c8 *buffer, i32 *pos){
    *pos += sprintf(buffer + *pos, "\"%s\":%g", key, value);
}

void serialize_int(const c8 *key, i32 value, c8 *buffer, i32 *pos){
    *pos += sprintf(buffer + *pos, "\"%s\":%d", key, value);
}

void serialize_array_start(const c8 *key, c8 *buffer, i32 *pos){
    *pos += sprintf(buffer + *pos, "\"%s\":[", key);
}

void serialize_array_end(c8 *buffer, i32 *pos){
    *pos += sprintf(buffer + *pos, "]");
}

void serialize_object_start(const c8 *key, c8 *buffer, i32 *pos){
    if (key) {
        *pos += sprintf(buffer + *pos, "\"%s\":{", key);
    } else {
        *pos += sprintf(buffer + *pos, "{");
    }
}

void serialize_object_end(c8 *buffer, i32 *pos){
    *pos += sprintf(buffer + *pos, "}");
}

void serialize_comma(c8 *buffer, i32 *pos){
    *pos += sprintf(buffer + *pos, ",");
}
