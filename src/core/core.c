#include "core.h"
#include "log.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdatomic.h>

at_id at_new_id(){
    static _Atomic at_id id = 0;
    return id++;
}

void at_init_symbol(at_symbol* symbol, c8* name, c8* exchange, c8* currency, sz tick_count){
    assert(symbol && name && exchange && currency);
    symbol->name = name;
    symbol->exchange = exchange;
    symbol->currency = currency;
    symbol->tick_count = tick_count;
    if (tick_count > 0) {
        symbol->ticks = (at_tick*)malloc(sizeof(at_tick) * tick_count);
        assert(symbol->ticks);
    }
    else {
        symbol->ticks = NULL;
    }
}

void at_add_tick(at_symbol* symbol, at_tick* tick){
    assert(symbol);
    symbol->tick_count++;
    symbol->ticks = (at_tick* )realloc(symbol->ticks, sizeof(at_tick)*  symbol->tick_count);
    assert(symbol->ticks);
    symbol->ticks[symbol->tick_count - 1] =* tick;
}

void at_add_ticks(at_symbol* symbol, at_tick* ticks, sz count) {
    assert(symbol && ticks && count > 0);
    u32 current_count = symbol->tick_count;
    symbol->tick_count += count;
    symbol->ticks = (at_tick*)realloc(symbol->ticks, sizeof(at_tick) * symbol->tick_count);
    assert(symbol->ticks);
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
        *out_candle_count = 0;
        return NULL; 
    }

    for (u32 i = 0; i < candle_count; i++) {
        u32 start_idx = i * period;
        u32 end_idx = start_idx + period;

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

        candle.adj_close /= period;
        candles[i] = candle;
    }

    *out_candle_count = candle_count;
    return candles;
}

void at_add_ticks_to_candles(at_candles *candles, at_tick *ticks, u32 ticks_count){
    assert(candles && ticks && ticks_count > 0);
    const u32 candle_period = candles->period;
    const u32 candle_count = candles->count;
    const u32 new_candle_count = candle_count + (ticks_count / candle_period);
    candles->candles = (at_candle*)realloc(candles->candles, sizeof(at_candle) * new_candle_count);
    assert(candles->candles);
    for (u32 i = 0; i < ticks_count; i++) {
        const u32 candle_idx = candle_count + (i / candle_period);
        at_candle* candle = &candles->candles[candle_idx];
        if (i % candle_period == 0) {
            candle->open = ticks[i].price;
            candle->high = ticks[i].price;
            candle->low = ticks[i].price;
            candle->volume = 0;
            candle->adj_close = 0;
        }
        candle->close = ticks[i].price;
        candle->volume += ticks[i].volume;
        candle->adj_close += ticks[i].price;
        if (ticks[i].price > candle->high) {
            candle->high = ticks[i].price;
        }
        if (ticks[i].price < candle->low) {
            candle->low = ticks[i].price;
        }
    }
}

i8 at_get_candle_direction(at_candle* candle){
    assert(candle);
    if (candle->close > candle->open){ return 1; } 
    else if (candle->close < candle->open){ return -1; }
    return 0;
}

void at_update_candles(at_candles *candles, at_tick *ticks, u32 ticks_count){
    assert(candles && ticks && ticks_count > 0);

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

void at_add_order(at_account* account, at_order* order) {
    assert(account && order);
    account->margin += order->volume * order->price;
    account->free_margin = account->balance - account->margin;
    if (account->margin != 0) {
        account->margin_level = account->equity / account->margin;
    } else {
        account->margin_level = 0;
    }
    log_info("Order added: %s %u %f", order->symbol, order->volume, order->price);
}

void at_update_order(at_account* account, at_order* order, f64 price) {
    assert(account && order);
    account->margin -= order->volume * order->price;
    account->margin += order->volume * price;
    if (account->margin < 0) {
        log_error("Margin cannot be negative");
        account->margin = 0;
    }
    account->free_margin = account->balance - account->margin;
    if (account->margin != 0) {
        account->margin_level = account->equity / account->margin;
    } else {
        account->margin_level = 0;
    }
}

void at_close_order(at_account* account, at_order* order, f64 price) {
    assert(account && order);
    account->margin -= order->volume * order->price;
    account->margin += order->volume * price;
    if (account->margin < 0) {
        log_error("Margin cannot be negative");
        account->margin = 0;
    }
    account->free_margin = account->balance - account->margin;
    if (account->margin != 0) {
        account->margin_level = account->equity / account->margin;
    } else {
        account->margin_level = 0;
    }
    account->equity += order->volume * (price - order->price);
    log_info("Order closed: %s %u %f", order->symbol, order->volume, price);
}

void at_cancel_order(at_account* account, at_order* order) {
    assert(account && order);
    account->margin -= order->volume * order->price;
    if (account->margin < 0) {
        log_error("Margin cannot be negative");
        account->margin = 0;
    }
    account->free_margin = account->balance - account->margin;
    if (account->margin != 0) {
        account->margin_level = account->equity / account->margin;
    } else {
        account->margin_level = 0;
    }
}

void at_free_order(at_order* order){
    // Nothing to free
}

void at_init_strategy(at_strategy *strategy, c8 *name, on_start_callback on_start, on_tick_callback on_tick, u32 *candles_periods, sz candles_periods_count){
    assert(strategy && name && candles_periods && candles_periods_count > 0); // at least one period
    strategy->id = at_new_id();
    strategy->name = name;
    strategy->on_start = on_start;
    strategy->on_tick = on_tick;
    strategy->candles_periods = candles_periods;
    strategy->candles_periods_count = candles_periods_count;
    strategy->cached_candles = (at_candles*)malloc(sizeof(at_candles));
    strategy->cached_candles_count = 1;
}

void at_update_strategy(at_strategy *strategy, at_instance *instance, at_tick *tick){
    assert(strategy && instance && tick);
    for (sz i = 0; i < strategy->candles_periods_count; i++){
        u32 period = strategy->candles_periods[i];
        if (strategy->cached_candles_count > 0){
            at_candles* candles = &strategy->cached_candles[i];
            if (candles->period == period){
                at_add_ticks_to_candles(candles, tick, 1);
            }
        }
        else {
            at_candles candles = {0};
            candles.period = period;
            candles.count = 1;
            candles.candles = (at_candle*)malloc(sizeof(at_candle));
            assert(candles.candles);
            at_add_ticks_to_candles(&candles, tick, 1);
            strategy->cached_candles_count++;
            strategy->cached_candles = (at_candles*)realloc(strategy->cached_candles, sizeof(at_candles) * strategy->cached_candles_count);
            assert(strategy->cached_candles);
            strategy->cached_candles[strategy->cached_candles_count - 1] = candles;
        }
    }
}

void at_free_strategy(at_strategy* strategy){
    assert(strategy);
    free(strategy->cached_candles);
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

void at_start_instance(at_instance *instance){
    assert(instance);
    if (instance->strategy->on_start){
        instance->strategy->on_start(instance);
    }
}

void at_tick_instance(at_instance* instance, at_tick* tick){
    assert(instance && tick);
    at_add_tick(instance->symbol, tick);
    at_update_strategy(instance->strategy, instance, tick);
    if (instance->strategy->on_tick){
        instance->strategy->on_tick(instance, tick);
    }
    for (u32 i = 0; i < instance->order_count; i++){
        at_order* order = &instance->orders[i];
        if (strcmp(order->symbol, instance->symbol->name) == 0){
            if (order->price >= tick->price){
                at_close_order(instance->account, order, tick->price);
            }
        }
    }
}
