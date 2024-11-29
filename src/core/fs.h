// abstract-trader https://github.com/ougi-washi/abstract-trader
#pragma once

#include "types.h"

extern c8* at_read_file(const char* path);
extern void at_write_file(const char* path, const c8* data);