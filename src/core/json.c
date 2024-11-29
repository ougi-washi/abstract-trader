// abstract-trader https://github.com/ougi-washi/abstract-trader

#include "json.h"
#include <stdlib.h>
#include <string.h>

at_json at_parse_json(const c8* data){
    at_json json;
    json.data = (c8* )malloc(strlen(data) + 1);
    if (!json.data){
        json.size = 0;
        return json;
    }
    strcpy(json.data, data);
    json.size = strlen(data);
    return json;
}

void at_free_json(at_json* json){
    for (sz i = 0; i < json->size; i++){
        json->data[i] = '\0';
    }
    free(json->data);
    json->size = 0;
}

c8* at_json_get_string(at_json* json, const c8* key){
    for (sz i = 0; i < json->size; i++){
        if (json->data[i] == key[0]){
            b8 found = 1;
            for (sz j = 1; j < strlen(key); j++){
                if (json->data[i + j] != key[j]){
                    found = 0;
                    break;
                }
            }
            if (found){
                i += strlen(key) + 3;
                c8* value = (c8* )malloc(1);
                value[0] = '\0';
                while (json->data[i] != '"'){
                    value = (c8* )realloc(value, strlen(value) + 2);
                    value[strlen(value)] = json->data[i];
                    value[strlen(value) + 1] = '\0';
                    i++;
                }
                return value;
            }
        }
    }
    return "";
}

f64 at_json_get_number(at_json* json, const c8* key){
    for (sz i = 0; i < json->size; i++){
        if (json->data[i] == key[0]){
            b8 found = 1;
            for (sz j = 1; j < strlen(key); j++){
                if (json->data[i + j] != key[j]){
                    found = 0;
                    break;
                }
            }
            if (found){
                i += strlen(key) + 3;
                c8* value = (c8* )malloc(1);
                value[0] = '\0';
                while (json->data[i] != '"'){
                    value = (c8* )realloc(value, strlen(value) + 2);
                    value[strlen(value)] = json->data[i];
                    value[strlen(value) + 1] = '\0';
                    i++;
                }
                return atof(value);
            }
        }
    }
    return 0;
}

i32 at_json_get_integer(at_json* json, const c8* key){
    for (sz i = 0; i < json->size; i++){
        if (json->data[i] == key[0]){
            b8 found = 1;
            for (sz j = 1; j < strlen(key); j++){
                if (json->data[i + j] != key[j]){
                    found = 0;
                    break;
                }
            }
            if (found){
                i += strlen(key) + 3;
                c8* value = (c8* )malloc(1);
                value[0] = '\0';
                while (json->data[i] != '"'){
                    value = (c8* )realloc(value, strlen(value) + 2);
                    value[strlen(value)] = json->data[i];
                    value[strlen(value) + 1] = '\0';
                    i++;
                }
                return atoi(value);
            }
        }
    }
    return 0;
}

at_json at_json_get_object(at_json* json, const c8* key){
    for (sz i = 0; i < json->size; i++){
        if (json->data[i] == key[0]){
            b8 found = 1;
            for (sz j = 1; j < strlen(key); j++){
                if (json->data[i + j] != key[j]){
                    found = 0;
                    break;
                }
            }
            if (found){
                i += strlen(key) + 3;
                c8* value = (c8* )malloc(1);
                value[0] = '\0';
                while (json->data[i] != '}'){
                    value = (c8* )realloc(value, strlen(value) + 2);
                    value[strlen(value)] = json->data[i];
                    value[strlen(value) + 1] = '\0';
                    i++;
                }
                return at_parse_json(value);
            }
        }
    }
    return at_parse_json("");
}

at_json at_json_get_array(at_json* json, const c8* key){
    for (sz i = 0; i < json->size; i++){
        if (json->data[i] == key[0]){
            b8 found = 1;
            for (sz j = 1; j < strlen(key); j++){
                if (json->data[i + j] != key[j]){
                    found = 0;
                    break;
                }
            }
            if (found){
                i += strlen(key) + 3;
                c8* value = (c8* )malloc(1);
                value[0] = '\0';
                while (json->data[i] != ']'){
                    value = (c8* )realloc(value, strlen(value) + 2);
                    value[strlen(value)] = json->data[i];
                    value[strlen(value) + 1] = '\0';
                    i++;
                }
                return at_parse_json(value);
            }
        }
    }
    return at_parse_json("");
}

sz at_json_get_array_size(at_json* json){
    for (sz i = 0; i < json->size; i++){
        if (json->data[i] == '['){
            sz size = 0;
            while (json->data[i] != ']'){
                if (json->data[i] == ','){
                    size++;
                }
                i++;
            }
            return size;
        }
    }
    return 0;
}

at_json at_json_get_array_item(at_json* json, sz index){
    for (sz i = 0; i < json->size; i++){
        if (json->data[i] == '['){
            sz size = 0;
            while (json->data[i] != ']'){
                if (json->data[i] == ','){
                    size++;
                }
                if (size == index){
                    c8* value = (c8* )malloc(1);
                    value[0] = '\0';
                    while (json->data[i] != ','){
                        value = (c8* )realloc(value, strlen(value) + 2);
                        value[strlen(value)] = json->data[i];
                        value[strlen(value) + 1] = '\0';
                        i++;
                    }
                    return at_parse_json(value);
                }
                i++;
            }
        }
    }
    return at_parse_json("");
}

b8 at_json_is_valid(at_json* json){
    for (sz i = 0; i < json->size; i++){
        if (json->data[i] == '{' || json->data[i] == '['){
            return 1;
        }
    }
    return 0;
}

void at_json_print(at_json* json){
    for (sz i = 0; i < json->size; i++){
        printf("%c", json->data[i]);
    }
}

c8* at_json_to_string(at_json* json){
    return json->data;
}

void at_json_set_string(at_json* json, const c8* key, const c8* value){
    for (sz i = 0; i < json->size; i++){
        if (json->data[i] == key[0]){
            b8 found = 1;
            for (sz j = 1; j < strlen(key); j++){
                if (json->data[i + j] != key[j]){
                    found = 0;
                    break;
                }
            }
            if (found){
                i += strlen(key) + 3;
                while (json->data[i] != '"'){
                    i++;
                }
                i++;
                sz start = i;
                while (json->data[i] != '"'){
                    i++;
                }
                sz end = i;
                for (sz j = start; j < end; j++){
                    json->data[j] = '\0';
                }
                for (sz j = 0; j < strlen(value); j++){
                    json->data[start + j] = value[j];
                }
                return;
            }
        }
    }
}

void at_json_set_number(at_json* json, const c8* key, f64 value){
    for (sz i = 0; i < json->size; i++){
        if (json->data[i] == key[0]){
            b8 found = 1;
            for (sz j = 1; j < strlen(key); j++){
                if (json->data[i + j] != key[j]){
                    found = 0;
                    break;
                }
            }
            if (found){
                i += strlen(key) + 3;
                while (json->data[i] != '"'){
                    i++;
                }
                i++;
                sz start = i;
                while (json->data[i] != '"'){
                    i++;
                }
                sz end = i;
                for (sz j = start; j < end; j++){
                    json->data[j] = '\0';
                }
                c8* str = (c8* )malloc(32);
                sprintf(str, "%f", value);
                for (sz j = 0; j < strlen(str); j++){
                    json->data[start + j] = str[j];
                }
                return;
            }
        }
    }
}

void at_json_set_integer(at_json* json, const c8* key, i32 value){
    for (sz i = 0; i < json->size; i++){
        if (json->data[i] == key[0]){
            b8 found = 1;
            for (sz j = 1; j < strlen(key); j++){
                if (json->data[i + j] != key[j]){
                    found = 0;
                    break;
                }
            }
            if (found){
                i += strlen(key) + 3;
                while (json->data[i] != '"'){
                    i++;
                }
                i++;
                sz start = i;
                while (json->data[i] != '"'){
                    i++;
                }
                sz end = i;
                for (sz j = start; j < end; j++){
                    json->data[j] = '\0';
                }
                c8* str = (c8* )malloc(32);
                sprintf(str, "%d", value);
                for (sz j = 0; j < strlen(str); j++){
                    json->data[start + j] = str[j];
                }
                return;
            }
        }
    }
}

void at_json_set_object(at_json* json, const c8* key, at_json* value){
    for (sz i = 0; i < json->size; i++){
        if (json->data[i] == key[0]){
            b8 found = 1;
            for (sz j = 1; j < strlen(key); j++){
                if (json->data[i + j] != key[j]){
                    found = 0;
                    break;
                }
            }
            if (found){
                i += strlen(key) + 3;
                while (json->data[i] != '{'){
                    i++;
                }
                i++;
                sz start = i;
                while (json->data[i] != '}'){
                    i++;
                }
                sz end = i;
                for (sz j = start; j < end; j++){
                    json->data[j] = '\0';
                }
                for (sz j = 0; j < value->size; j++){
                    json->data[start + j] = value->data[j];
                }
                return;
            }
        }
    }
}

void at_json_set_array(at_json* json, const c8* key, at_json* value){
    for (sz i = 0; i < json->size; i++){
        if (json->data[i] == key[0]){
            b8 found = 1;
            for (sz j = 1; j < strlen(key); j++){
                if (json->data[i + j] != key[j]){
                    found = 0;
                    break;
                }
            }
            if (found){
                i += strlen(key) + 3;
                while (json->data[i] != '['){
                    i++;
                }
                i++;
                sz start = i;
                while (json->data[i] != ']'){
                    i++;
                }
                sz end = i;
                for (sz j = start; j < end; j++){
                    json->data[j] = '\0';
                }
                for (sz j = 0; j < value->size; j++){
                    json->data[start + j] = value->data[j];
                }
                return;
            }
        }
    }
}

void at_json_set_array_item(at_json* json, sz index, at_json* value){
    for (sz i = 0; i < json->size; i++){
        if (json->data[i] == '['){
            sz size = 0;
            while (json->data[i] != ']'){
                if (json->data[i] == ','){
                    size++;
                }
                i++;
            }
            if (index > size){
                return;
            }
            i = 0;
            while (json->data[i] != '['){
                i++;
            }
            i++;
            sz start = i;
            while (json->data[i] != ']'){
                if (size == index){
                    break;
                }
                if (json->data[i] == ','){
                    size--;
                }
                i++;
            }
            i++;
            sz end = i;
            for (sz j = start; j < end; j++){
                json->data[j] = '\0';
            }
            for (sz j = 0; j < value->size; j++){
                json->data[start + j] = value->data[j];
            }
            return;
        }
    }
}

void at_json_remove(at_json* json, const c8* key){
    for (sz i = 0; i < json->size; i++){
        if (json->data[i] == key[0]){
            b8 found = 1;
            for (sz j = 1; j < strlen(key); j++){
                if (json->data[i + j] != key[j]){
                    found = 0;
                    break;
                }
            }
            if (found){
                i += strlen(key) + 3;
                while (json->data[i] != ':'){
                    i++;
                }
                i++;
                while (json->data[i] != ',' && json->data[i] != '}'){
                    for (sz j = i; j < json->size; j++){
                        json->data[j] = json->data[j + 1];
                    }
                }
                return;
            }
        }
    }
}

void at_json_remove_array_item(at_json* json, sz index){
    for (sz i = 0; i < json->size; i++){
        if (json->data[i] == '['){
            sz size = 0;
            while (json->data[i] != ']'){
                if (json->data[i] == ','){
                    size++;
                }
                i++;
            }
            if (index > size){
                return;
            }
            i = 0;
            while (json->data[i] != '['){
                i++;
            }
            i++;
            sz start = i;
            while (json->data[i] != ']'){
                if (size == index){
                    break;
                }
                if (json->data[i] == ','){
                    size--;
                }
                i++;
            }
            i++;
            sz end = i;
            for (sz j = start; j < end; j++){
                json->data[j] = '\0';
            }
            for (sz j = end; j < json->size; j++){
                json->data[j] = json->data[j + 1];
            }
            return;
        }
    }
}

void at_json_clear(at_json* json){
    for (sz i = 0; i < json->size; i++){
        json->data[i] = '\0';
    }
    json->size = 0;
}

b8 at_json_has_key(at_json* json, const c8* key){
    for (sz i = 0; i < json->size; i++){
        if (json->data[i] == key[0]){
            b8 found = 1;
            for (sz j = 1; j < strlen(key); j++){
                if (json->data[i + j] != key[j]){
                    found = 0;
                    break;
                }
            }
            if (found){
                return 1;
            }
        }
    }
}

b8 at_json_has_index(at_json* json, sz index){
    for (sz i = 0; i < json->size; i++){
        if (json->data[i] == '['){
            sz size = 0;
            while (json->data[i] != ']'){
                if (json->data[i] == ','){
                    size++;
                }
                i++;
            }
            if (index > size){
                return 0;
            }
            return 1;
        }
    }
}

b8 at_json_is_string(at_json* json, const c8* key){
    for (sz i = 0; i < json->size; i++){
        if (json->data[i] == key[0]){
            b8 found = 1;
            for (sz j = 1; j < strlen(key); j++){
                if (json->data[i + j] != key[j]){
                    found = 0;
                    break;
                }
            }
            if (found){
                i += strlen(key) + 3;
                while (json->data[i] != '"'){
                    i++;
                }
                return 1;
            }
        }
    }
    return 0;
}

b8 at_json_is_number(at_json* json, const c8* key){
    for (sz i = 0; i < json->size; i++){
        if (json->data[i] == key[0]){
            b8 found = 1;
            for (sz j = 1; j < strlen(key); j++){
                if (json->data[i + j] != key[j]){
                    found = 0;
                    break;
                }
            }
            if (found){
                i += strlen(key) + 3;
                while (json->data[i] != '"'){
                    i++;
                }
                return 1;
            }
        }
    }
}

b8 at_json_is_integer(at_json* json, const c8* key){
    for (sz i = 0; i < json->size; i++){
        if (json->data[i] == key[0]){
            b8 found = 1;
            for (sz j = 1; j < strlen(key); j++){
                if (json->data[i + j] != key[j]){
                    found = 0;
                    break;
                }
            }
            if (found){
                i += strlen(key) + 3;
                while (json->data[i] != '"'){
                    i++;
                }
                return 1;
            }
        }
    }
    return 0;
}

b8 at_json_is_object(at_json* json, const c8* key){
    for (sz i = 0; i < json->size; i++){
        if (json->data[i] == key[0]){
            b8 found = 1;
            for (sz j = 1; j < strlen(key); j++){
                if (json->data[i + j] != key[j]){
                    found = 0;
                    break;
                }
            }
            if (found){
                return 1;
            }
        }
    }
    return 0;
}

b8 at_json_is_array(at_json* json, const c8* key){
    for (sz i = 0; i < json->size; i++){
        if (json->data[i] == key[0]){
            b8 found = 1;
            for (sz j = 1; j < strlen(key); j++){
                if (json->data[i + j] != key[j]){
                    found = 0;
                    break;
                }
            }
            if (found){
                return 1;
            }
        }
    }
    return 0;
}

b8 at_json_is_null(at_json* json, const c8* key){
    for (sz i = 0; i < json->size; i++){
        if (json->data[i] == key[0]){
            b8 found = 1;
            for (sz j = 1; j < strlen(key); j++){
                if (json->data[i + j] != key[j]){
                    found = 0;
                    break;
                }
            }
            if (found){
                i += strlen(key) + 3;
                if (json->data[i] == 'n' && json->data[i + 1] == 'u' && json->data[i + 2] == 'l' && json->data[i + 3] == 'l'){
                    return 1;
                }
            }
        }
    }
    return 0;
}

b8 at_json_is_true(at_json* json, const c8* key){
    for (sz i = 0; i < json->size; i++){
        if (json->data[i] == key[0]){
            b8 found = 1;
            for (sz j = 1; j < strlen(key); j++){
                if (json->data[i + j] != key[j]){
                    found = 0;
                    break;
                }
            }
            if (found){
                i += strlen(key) + 3;
                if (json->data[i] == 't' && json->data[i + 1] == 'r' && json->data[i + 2] == 'u' && json->data[i + 3] == 'e'){
                    return 1;
                }
            }
        }
    }
    return 0;
}

b8 at_json_is_false(at_json* json, const c8* key){
    for (sz i = 0; i < json->size; i++){
        if (json->data[i] == key[0]){
            b8 found = 1;
            for (sz j = 1; j < strlen(key); j++){
                if (json->data[i + j] != key[j]){
                    found = 0;
                    break;
                }
            }
            if (found){
                i += strlen(key) + 3;
                if (json->data[i] == 'f' && json->data[i + 1] == 'a' && json->data[i + 2] == 'l' && json->data[i + 3] == 's' && json->data[i + 4] == 'e'){
                    return 1;
                }
            }
        }
    }
    return 0;
}

b8 at_json_is_empty(at_json* json){
    for (sz i = 0; i < json->size; i++){
        if (json->data[i] != ' ' && json->data[i] != '\n' && json->data[i] != '\t'){
            return 0;
        }
    }
}

b8 at_json_is_equal(at_json* json1, at_json* json2){
    return strcmp(json1->data, json2->data) == 0;
}

at_json at_json_copy(at_json* json){
    return at_parse_json(json->data);
}

void at_json_merge(at_json* json1, at_json* json2){
    json1->data = (c8* )realloc(json1->data, json1->size + json2->size + 1);
}

void at_json_merge_recursive(at_json* json1, at_json* json2){
    for (sz i = 0; i < json2->size; i++){
        if (json2->data[i] == '{' || json2->data[i] == '['){
            at_json json = at_parse_json(json2->data + i);
            at_json_merge_recursive(json1, &json);
        }
    }
    at_json_merge(json1, json2);
}

