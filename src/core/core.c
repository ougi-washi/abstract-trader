#include "core.h"
#include "log.h"
#include <stdlib.h>
#include <assert.h>

at_id at_new_id(){
    static at_id id = 0;
    return id++;
}

void at_init_symbol(at_symbol* symbol, c8* name, c8* exchange, c8* currency, sz tick_count){
    assert(symbol && name && exchange && currency);
    symbol->name = name;
    symbol->exchange = exchange;
    symbol->currency = currency;
    symbol->tick_count = tick_count;
    if (tick_count > 0){
        symbol->ticks = (at_tick* )malloc(sizeof(at_tick)*  tick_count);
    }
}

void at_add_tick(at_symbol* symbol, at_tick* tick){
    assert(symbol);
    symbol->tick_count++;
    symbol->ticks = (at_tick* )realloc(symbol->ticks, sizeof(at_tick)*  symbol->tick_count);
    symbol->ticks[symbol->tick_count - 1] =* tick;
}

void at_add_ticks(at_symbol* symbol, at_tick* ticks, sz count) {
    assert(symbol && ticks && count > 0);
    u32 current_count = symbol->tick_count;
    symbol->tick_count += count;
    symbol->ticks = (at_tick*)realloc(symbol->ticks, sizeof(at_tick) * symbol->tick_count);
    if (!symbol->ticks) {
        log_error("Failed to allocate memory for ticks");
        symbol->tick_count = 0;
        return;
    }
    for (u32 i = 0; i < count; i++) {
        symbol->ticks[current_count + i] = ticks[i];
    }
}


at_tick* at_get_tick(at_symbol* symbol, u32 index){
    assert(symbol);
    if (index < symbol->tick_count){
        return &symbol->ticks[symbol->tick_count - index - 1];
    }
    return NULL;
}

at_tick* at_get_last_tick(at_symbol* symbol){
    return at_get_tick(symbol, 0);
}

at_candle* at_get_candles(at_symbol* symbol, u32 period, u32* out_candle_count) {
    assert(symbol && period > 0 && out_candle_count);
    if (symbol->tick_count < period) {
        *out_candle_count = 0;
        log_error("Not enough ticks to create candles");
        return NULL; 
    }

    u32 candle_count = symbol->tick_count / period;
    at_candle* candles = (at_candle*)malloc(sizeof(at_candle) * candle_count);
    if (!candles) {
        log_error("Failed to allocate memory for candles");
        return NULL; 
    }

    for (u32 i = 0; i < candle_count; i++) {
        u32 start_idx = i * period;
        u32 end_idx = start_idx + period;  // End index is exclusive

        at_candle candle = {0};
        candle.open = symbol->ticks[start_idx].price;
        candle.high = candle.open;
        candle.low = candle.open;
        candle.close = symbol->ticks[end_idx - 1].price;
        candle.volume = 0;
        candle.adj_close = 0;

        for (u32 j = start_idx; j < end_idx; j++) {
            f64 price = symbol->ticks[j].price;
            candle.volume += symbol->ticks[j].volume;
            candle.adj_close += price;

            if (price > candle.high) {
                candle.high = price;
            }
            if (price < candle.low) {
                candle.low = price;
            }
        }

        candle.adj_close /= period; // Average price for the period
        candles[i] = candle;
    }

    *out_candle_count = candle_count;
    return candles;
}

i8 at_get_candle_direction(at_candle* candle){
    assert(candle);
    if (candle->close > candle->open){ return 1; } 
    else if (candle->close < candle->open){ return -1; }
    return 0;
}

void at_free_symbol(at_symbol* symbol){
    assert(symbol);
    free(symbol->ticks);
}

void at_init_account(at_account* account, f64 balance){
    assert(account);
    account->id = at_new_id();
    account->balance = balance;
    account->equity = balance;
    account->margin = 0;
    account->free_margin = balance;
    account->margin_level = 0;
}

void at_free_account(at_account* account){
    // Nothing to free
}

void at_init_trade(at_trade* trade, at_id account_id, c8* symbol, u32 volume, f64 open_price, u32 open_time){
    assert(trade && symbol);
    trade->id = at_new_id();
    trade->account_id = account_id;
    trade->symbol = symbol;
    trade->volume = volume;
    trade->open_price = open_price;
    trade->close_price = 0;
    trade->swap = 0;
    trade->profit = 0;
    trade->commission = 0;
    trade->open_time = open_time;
    trade->close_time = 0;
}

void at_free_trade(at_trade* trade){
    // Nothing to free
}

void at_init_order(at_order* order, at_id account_id, c8* symbol, u32 volume, f64 price, u32 time){
    assert(order && symbol);
    order->id = at_new_id();
    order->account_id = account_id;
    order->symbol = symbol;
    order->volume = volume;
    order->price = price;
    order->time = time;
}

void at_add_order(at_account* account, at_order* order){
    assert(account && order);
    account->margin += order->volume*  order->price;
    account->free_margin = account->balance - account->margin;
    account->margin_level = account->equity / account->margin;
}

void at_update_order(at_account* account, at_order* order, f64 price){
    account->margin -= order->volume*  order->price;
    account->margin += order->volume*  price;
    account->free_margin = account->balance - account->margin;
    account->margin_level = account->equity / account->margin;
}

void at_close_order(at_account* account, at_order* order, f64 price){
    assert(account && order);
    account->margin -= order->volume*  order->price;
    account->margin += order->volume*  price;
    account->free_margin = account->balance - account->margin;
    account->margin_level = account->equity / account->margin;
    account->equity += order->volume*  (price - order->price);
}

void at_cancel_order(at_account* account, at_order* order){
    assert(account && order);
    account->margin -= order->volume*  order->price;
    account->free_margin = account->balance - account->margin;
    account->margin_level = account->equity / account->margin;
}

void at_free_order(at_order* order){
    // Nothing to free
}

void at_init_strategy(at_strategy* strategy, c8* name, on_start_callback on_start, on_tick_callback on_tick, on_candle_callback on_candle){
    assert(strategy && name && on_start && on_tick && on_candle);
    strategy->id = at_new_id();
    strategy->name = name;
    strategy->on_start = on_start;
    strategy->on_tick = on_tick;
    strategy->on_candle = on_candle;
}

void at_free_strategy(at_strategy* strategy){
    // Nothing to free
}

void at_init_instance(at_instance* instance, at_strategy* strategy, at_symbol* symbol, at_account* account){
    assert(instance && strategy && symbol && account);
    instance->id = at_new_id();
    instance->strategy = strategy;
    instance->symbol = symbol;
    instance->account = account;
    instance->trades = NULL;
    instance->trade_count = 0;
    instance->orders = NULL;
    instance->order_count = 0;
}

void at_free_instance(at_instance* instance){
    assert(instance);
    free(instance->trades);
    free(instance->orders);
}

void at_add_trade(at_instance* instance, at_trade* trade){
    assert(instance && trade);
    instance->trade_count++;
    instance->trades = (at_trade* )realloc(instance->trades, sizeof(at_trade)*  instance->trade_count);
    instance->trades[instance->trade_count - 1] =* trade;
}

void at_tick_instance(at_instance* instance, at_tick* tick){
    assert(instance && tick);
    at_add_tick(instance->symbol, tick);
    instance->strategy->on_tick(instance, tick);
    u32 candle_count = 0;
    instance->strategy->on_candle(instance, at_get_candles(instance->symbol, 5, &candle_count));

    for (u32 i = 0; i < instance->order_count; i++){
        at_order* order = &instance->orders[i];
        if (order->symbol == instance->symbol->name){
            if (order->price >= tick->price){
                at_close_order(instance->account, order, tick->price);
            }
        }
    }
}
