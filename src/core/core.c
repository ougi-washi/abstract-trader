#include "core.h"
#include <stdlib.h>

at_id at_new_id(){
    static at_id id = 0;
    return id++;
}

void at_init_symbol(at_symbol *symbol, c8 *name, c8 *exchange, c8 *currency, sz tick_count){
    symbol->name = name;
    symbol->exchange = exchange;
    symbol->currency = currency;
    symbol->tick_count = tick_count;
    symbol->ticks = (at_tick *)malloc(sizeof(at_tick) * tick_count);
}

void at_free_symbol(at_symbol *symbol)
{
    free(symbol->ticks);
}

void at_init_account(at_account *account, f64 balance)
{
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
    order->id = at_new_id();
    order->account_id = account_id;
    order->symbol = symbol;
    order->volume = volume;
    order->price = price;
    order->time = time;
}

void at_add_order(at_account *account, at_order *order){
    account->margin += order->volume * order->price;
    account->free_margin = account->balance - account->margin;
    account->margin_level = account->equity / account->margin;
}

void at_remove_order(at_account *account, at_order *order){
    account->margin -= order->volume * order->price;
    account->free_margin = account->balance - account->margin;
    account->margin_level = account->equity / account->margin;
}

void at_update_order(at_account *account, at_order *order, f64 price){
    account->margin -= order->volume * order->price;
    account->margin += order->volume * price;
    account->free_margin = account->balance - account->margin;
    account->margin_level = account->equity / account->margin;
}

void at_close_order(at_account *account, at_order *order, f64 price){
    account->margin -= order->volume * order->price;
    account->margin += order->volume * price;
    account->free_margin = account->balance - account->margin;
    account->margin_level = account->equity / account->margin;
    account->equity += order->volume * (price - order->price);
}

void at_cancel_order(at_account *account, at_order *order){
    account->margin -= order->volume * order->price;
    account->free_margin = account->balance - account->margin;
    account->margin_level = account->equity / account->margin;
}

void at_free_order(at_order *order){
    // Nothing to free
}
