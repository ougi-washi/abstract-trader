// abstract-trader https://github.com/ougi-washi/abstract-trader
#pragma once

#include "core/types.h"
#include <stdlib.h>

typedef u32 at_id;

typedef struct at_tick {
    f64 price;
    f64 volume;
} at_tick;

typedef struct at_candle {
    f64 open;
    f64 high;
    f64 low;
    f64 close;
    f64 volume;
    f64 adj_close;
} at_candle;

typedef struct at_candles {
    u32 period;
    at_candle* candles;
    sz count;
} at_candles;

typedef struct at_symbol {
    c8* name;
    c8* exchange;
    c8* currency;
    sz tick_count;
    at_tick* ticks;
} at_symbol;

typedef struct at_account {
    at_id id;
    f64 balance;
    f64 equity;
    f64 margin;
    f64 free_margin;
    f64 margin_level;
} at_account;

#define AT_DIRECTION_LONG 1
#define AT_DIRECTION_SHORT -1
#define AT_DIRECTION_STRING(d) (d == AT_DIRECTION_LONG ? "LONG" : "SHORT")
typedef struct at_position {
    // configuration
    c8* symbol;
    u32 volume;
    i8 direction;
    f64 take_profit_price;
    f64 stop_loss_price;
    f64 commission;
    
    // runtime data
    at_id id;
    f64 open_price;
    f64 close_price;
    f64 swap;
    f64 profit;
    u32 open_time;
    u32 close_time;
} at_position;

typedef struct at_instance {
    // configuration
    at_id id;
    struct at_strategy* strategy;
    at_symbol* symbol;
    at_account* account;
    f32 commission;
    f32 swap;
    f32 leverage;

    // runtime data
    at_position* open_positions;
    u32 open_positions_count;
    at_position* closed_positions;
    u32 closed_positions_count;
} at_instance;

typedef void (*on_start_callback)(at_instance* instance);
typedef void (*on_tick_callback)(at_instance* instance, at_tick* tick);
typedef void (*on_candle_callback)(at_instance* instance, at_candle* candle);
typedef struct at_strategy {
    at_id id;
    c8* name;
    on_start_callback on_start;
    on_tick_callback on_tick;

    u32* candles_periods;
    sz candles_periods_count;
    at_candles* cached_candles;
    sz cached_candles_count;
} at_strategy;

extern at_id at_new_id();

extern void at_init_symbol(at_symbol* symbol, c8* name, c8* exchange, c8* currency, sz tick_count);
extern void at_add_tick(at_symbol* symbol, at_tick* tick);
extern void at_add_ticks(at_symbol* symbol, at_tick* ticks, sz count);
extern at_tick* at_get_tick(at_symbol* symbol, u32 index);
extern at_tick* at_get_last_tick(at_symbol* symbol);
extern at_candle* at_get_candles(at_symbol* symbol, u32 period, u32* out_candle_count); // if tick 1s = 1, 1m = 60, 1h = 3600, 1d = 86400
extern void at_add_ticks_to_candles(at_candles* candles, at_tick* ticks, u32 ticks_count);
extern i8 at_get_candle_direction(at_candle* candle);
extern void at_free_symbol(at_symbol* symbol);

extern void at_init_account(at_account* account, f64 balance);
extern void at_free_account(at_account* account);

extern void at_init_position(at_position* position, c8* symbol, u32 volume, i8 direction, f64 open_price, f64 commission, f64 take_profit_price, f64 stop_loss_price);
extern void at_free_position(at_position* position);

extern void at_init_strategy(at_strategy* strategy, c8* name, on_start_callback on_start, on_tick_callback on_tick, u32* candles_periods, sz candles_periods_count);
extern void at_update_strategy(at_strategy* strategy, at_instance* instance, at_tick* tick);
extern void at_free_strategy(at_strategy* strategy);

extern void at_init_instance(at_instance* instance, at_strategy* strategy, at_symbol* symbol, at_account* account, f32 commission, f32 swap, f32 leverage);
extern void at_free_instance(at_instance* instance);
extern void at_add_position(at_instance* instance, at_position* position);
extern void at_close_position(at_instance* instance, at_position* position, f64 close_price);
extern void at_start_instance(at_instance* instance);
extern void at_tick_instance(at_instance* instance, at_tick* tick);
