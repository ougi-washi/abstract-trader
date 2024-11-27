// abstract-trader https://github.com/ougi-washi/abstract-trader
#pragma once

#include "core/types.h"

typedef u32 at_id;

typedef struct at_tick {
    f64 open;
    f64 high;
    f64 low;
    f64 close;
    f64 volume;
    f64 adj_close;
} at_tick;

typedef struct at_symbol {
    c8* name;
    c8* exchange;
    c8* currency;
    sz tick_count;
    struct at_tick *ticks;
} at_symbol;

typedef struct at_account {
    at_id id;
    f64 balance;
    f64 equity;
    f64 margin;
    f64 free_margin;
    f64 margin_level;
} at_account;

typedef struct at_trade {
    at_id id;
    at_id account_id;
    c8* symbol;
    u32 volume;
    f64 open_price;
    f64 close_price;
    f64 swap;
    f64 profit;
    f64 commission;
    u32 open_time;
    u32 close_time;
} at_trade;

typedef struct at_order {
    at_id id;
    at_id account_id;
    c8* symbol;
    u32 volume;
    f64 price;
    u32 time;
} at_order;

extern at_id at_new_id();

extern void at_init_symbol(at_symbol *symbol, c8 *name, c8 *exchange, c8 *currency, sz tick_count);
extern void at_free_symbol(at_symbol *symbol);

extern void at_init_account(at_account *account, f64 balance);
extern void at_free_account(at_account *account);

extern void at_init_trade(at_trade *trade, at_id account_id, c8 *symbol, u32 volume, f64 open_price, u32 open_time);
extern void at_free_trade(at_trade *trade);

extern void at_init_order(at_order *order, at_id account_id, c8 *symbol, u32 volume, f64 price, u32 time);
extern void at_add_order(at_account *account, at_order *order);
extern void at_remove_order(at_account *account, at_order *order);
extern void at_update_order(at_account *account, at_order *order, f64 price);
extern void at_close_order(at_account *account, at_order *order, f64 price);
extern void at_cancel_order(at_account *account, at_order *order);
extern void at_free_order(at_order *order);