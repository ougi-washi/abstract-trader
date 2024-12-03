// abstract-trader https://github.com/ougi-washi/abstract-trader

#pragma once
#include "core/types.h"

static const c8* LOG_LEVEL_INFO =  "INFO"; 
static const c8* LOG_LEVEL_WARN =  "WARN";
static const c8* LOG_LEVEL_ERROR = "ERROR";

extern void log_message(const c8 *level, const c8 *file, const i32 line, const c8 *fmt, ...);

#define log_info(...) log_message(LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...) log_message(LOG_LEVEL_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_message(LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
