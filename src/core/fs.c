// abstract-trader https://github.com/ougi-washi/abstract-trader

#include "fs.h"
#include "log.h"
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <direct.h>
#define mkdir _mkdir
#endif

void ensure_directory(const c8* path) {
    c8 temp[256];
    c8* p = NULL;
    sz len;

    snprintf(temp, sizeof(temp), "%s", path);
    len = strlen(temp);

    // Remove the file name from the path
    if (temp[len - 1] != '/' && temp[len - 1] != '\\') {
        c8* last_sep = strrchr(temp, '/');
        if (!last_sep) last_sep = strrchr(temp, '\\');
        if (last_sep) *last_sep = '\0';
    }

    // Create directories one by one
    for (p = temp + 1; *p; ++p) {
        if (*p == '/' || *p == '\\') {
            *p = '\0';
            mkdir(temp, 0755); // Create intermediate directory
            *p = '/';
        }
    }
    mkdir(temp, 0755); // Create the final directory
}

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

b8 at_write_file(const c8 *path, const c8 *data){
    ensure_directory(path);
    FILE* file = fopen(path, "w");
    if (!file){
        log_error("Failed to open file %s", path);
        return false;
    }
    fwrite(data, 1, strlen(data), file);
    fclose(file);
    return true;
}
