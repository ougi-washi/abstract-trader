// abstract-trader https://github.com/ougi-washi/abstract-trader

#pragma once
#include "core/types.h"

static const char* LOG_LEVEL_INFO =  "INFO"; 
static const char* LOG_LEVEL_WARN =  "WARN";
static const char* LOG_LEVEL_ERROR = "ERROR";

void log_message(const char *level, const char *file, const i32 line, const char *fmt, ...);

#define log_info(...) log_message(LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...) log_message(LOG_LEVEL_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_message(LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
