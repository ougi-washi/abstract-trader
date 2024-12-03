// abstract-trader https://github.com/ougi-washi/abstract-trader

#include "fs.h"
#include <stdlib.h>
#include <string.h>

c8* at_read_file(const c8* path){
    FILE* file = fopen(path, "r");
    if (!file){
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    sz size = ftell(file);
    fseek(file, 0, SEEK_SET);
    c8* buffer = (c8* )malloc(size + 1);
    if (!buffer){
        fclose(file);
        return NULL;
    }
    fread(buffer, 1, size, file);
    buffer[size] = '\0';
    fclose(file);
    return buffer;
}

void at_write_file(const c8 *path, const c8 *data){
    FILE* file = fopen(path, "w");
    if (!file){
        return;
    }
    fwrite(data, 1, strlen(data), file);
    fclose(file);
}
