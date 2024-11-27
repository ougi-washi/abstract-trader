#include "core.h"
#include <stdlib.h>
#include <assert.h>

at_id at_new_id(){
    static at_id id = 0;
    return id++;
}

void at_init_symbol(at_symbol *symbol, c8 *name, c8 *exchange, c8 *currency, sz tick_count){
    assert(symbol && name && exchange && currency);
    symbol->name = name;
    symbol->exchange = exchange;
    symbol->currency = currency;
    symbol->tick_count = tick_count;
    if (tick_count > 0){
        symbol->ticks = (at_tick *)malloc(sizeof(at_tick) * tick_count);
    }
}

void at_add_tick(at_symbol *symbol, at_tick *tick){
    assert(symbol && tick);
    symbol->tick_count++;
    symbol->ticks = (at_tick *)realloc(symbol->ticks, sizeof(at_tick) * symbol->tick_count);
    symbol->ticks[symbol->tick_count - 1] = *tick;
}

at_tick* at_get_tick(at_symbol *symbol, u32 index){
    assert(symbol);
    if (index < symbol->tick_count){
        return &symbol->ticks[symbol->tick_count - index - 1];
    }
    return NULL;
}

at_tick* at_get_last_tick(at_symbol *symbol){
    return at_get_tick(symbol, 0);
}

at_candle* at_get_candles(at_symbol *symbol, u32 period){
    assert(symbol && period > 0);
    if (symbol->tick_count % period != 0){
        return NULL;
    }

    at_candle* candles = (at_candle *)malloc(sizeof(at_candle) * (symbol->tick_count / period));
    for (u32 i = 0; i < symbol->tick_count; i++){
        if (i % period == 0){
            at_candle candle = {0};
            candle.open = symbol->ticks[i].price;
            candle.close = symbol->ticks[i + period - 1].price;
            candle.volume = 0;
            candle.adj_close = 0;
            for (u32 j = i; j < i + period; j++){
                candle.volume += symbol->ticks[j].volume;
                candle.adj_close += symbol->ticks[j].price;
            }
            candle.adj_close /= period;
            candles[i / period] = candle;
        }
    }
    return candles;
}

void at_free_symbol(at_symbol *symbol){
    assert(symbol);
    free(symbol->ticks);
}

void at_init_account(at_account *account, f64 balance){
    assert(account);
    account->id = at_new_id();
    account->balance = balance;
    account->equity = balance;
    account->margin = 0;
    account->free_margin = balance;
    account->margin_level = 0;
}

void at_free_account(at_account *account){
    // Nothing to free
}

void at_init_trade(at_trade *trade, at_id account_id, c8 *symbol, u32 volume, f64 open_price, u32 open_time){
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

void at_free_trade(at_trade *trade){
    // Nothing to free
}

void at_init_order(at_order *order, at_id account_id, c8 *symbol, u32 volume, f64 price, u32 time){
    assert(order && symbol);
    order->id = at_new_id();
    order->account_id = account_id;
    order->symbol = symbol;
    order->volume = volume;
    order->price = price;
    order->time = time;
}

void at_add_order(at_account *account, at_order *order){
    assert(account && order);
    account->margin += order->volume * order->price;
    account->free_margin = account->balance - account->margin;
    account->margin_level = account->equity / account->margin;
}

void at_remove_order(at_account *account, at_order *order){
    assert(account && order);
    account->margin -= order->volume * order->price;
}

void at_update_order(at_account *account, at_order *order, f64 price){
    account->margin -= order->volume * order->price;
    account->margin += order->volume * price;
    account->free_margin = account->balance - account->margin;
    account->margin_level = account->equity / account->margin;
}

void at_close_order(at_account *account, at_order *order, f64 price){
    assert(account && order);
    account->margin -= order->volume * order->price;
    account->margin += order->volume * price;
    account->free_margin = account->balance - account->margin;
    account->margin_level = account->equity / account->margin;
    account->equity += order->volume * (price - order->price);
}

void at_cancel_order(at_account *account, at_order *order){
    assert(account && order);
    account->margin -= order->volume * order->price;
    account->free_margin = account->balance - account->margin;
    account->margin_level = account->equity / account->margin;
}

void at_free_order(at_order *order){
    // Nothing to free
}

void at_init_instance(at_instance *instance, at_strategy *strategy, at_symbol *symbol, at_account *account){
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

void at_free_instance(at_instance *instance){
    assert(instance);
    free(instance->trades);
    free(instance->orders);
}

void at_tick_instance(at_instance *instance, at_tick *tick){
    assert(instance && tick);
    instance->strategy->on_tick(instance, tick);
    instance->strategy->on_candle(instance, at_get_candles(instance->symbol, 5));

    for (u32 i = 0; i < instance->order_count; i++){
        at_order *order = &instance->orders[i];
        if (order->symbol == instance->symbol->name){
            if (order->price >= tick->price){
                at_close_order(instance->account, order, tick->price);
                at_remove_order(instance->account, order);
            }
        }
    }
}
