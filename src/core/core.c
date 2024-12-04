#include "core.h"
#include "log.h"
#include "fs.h"
#include "json.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdatomic.h>
#include <time.h>

#define AT_CANDLES_PERIODS_MAX 20
#define AT_SRIALIZE_BUFFER_SIZE 16384 

at_id at_new_id(){
    static _Atomic at_id id = 0;
    return id++;
}

void at_init_symbol(at_symbol* symbol, const c8* name, const c8* exchange, const c8* currency, sz tick_count){
    assert(symbol && name && exchange && currency);
    strcpy(symbol->name, name);
    strcpy(symbol->exchange, exchange);
    strcpy(symbol->currency, currency);
}

void at_add_tick(at_symbol* symbol, at_tick* tick){
    assert(symbol);
    AT_ARRAY_ADD(symbol->ticks, *tick);
}

void at_add_ticks(at_symbol* symbol, at_tick* ticks, sz count) {
    assert(symbol && ticks && count > 0);
    for (sz i = 0; i < count; i++) {
        at_add_tick(symbol, &ticks[i]);
    }
}

at_tick* at_get_tick(at_symbol* symbol, u32 index){
    assert(symbol);
    if (index >= AT_ARRAY_SIZE(symbol->ticks)){
        log_error("Index out of bounds");
        return NULL;
    }
    return &AT_ARRAY_GET(symbol->ticks, index);
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
    if (new_candle_count == candle_count) {
        return;
    }
    if (candles->candles){
        candles->candles = (at_candle*)realloc(candles->candles, sizeof(at_candle) * new_candle_count);
    }
    else {
        candles->candles = (at_candle*)malloc(sizeof(at_candle) * new_candle_count);
    }
    candles->count = new_candle_count;
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

void at_serialize_symbol(const at_symbol *symbol, c8 *buffer, i32 *pos){
    serialize_object_start("symbol", buffer, pos);
    serialize_string("name", symbol->name, buffer, pos);
    serialize_comma(buffer, pos);
    serialize_string("exchange", symbol->exchange, buffer, pos);
    serialize_comma(buffer, pos);
    serialize_string("currency", symbol->currency, buffer, pos);
    serialize_comma(buffer, pos);
    serialize_int("tick_count", symbol->tick_count, buffer, pos);
    // ticks serialization if necessary
    serialize_object_end(buffer, pos);
}

void at_update_candles(at_candles *candles, at_tick *ticks, u32 ticks_count){
    assert(candles && ticks && ticks_count > 0);

}

void at_free_symbol(at_symbol* symbol){
    assert(symbol);
    free(symbol->name);
    free(symbol->exchange);
    free(symbol->currency);
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

void at_serialize_account(const at_account *account, c8 *buffer, i32 *pos){
    serialize_object_start("account", buffer, pos);
    serialize_number("id", account->id, buffer, pos);
    serialize_comma(buffer, pos);
    serialize_number("balance", account->balance, buffer, pos);
    serialize_comma(buffer, pos);
    serialize_number("equity", account->equity, buffer, pos);
    serialize_comma(buffer, pos);
    serialize_number("margin", account->margin, buffer, pos);
    serialize_comma(buffer, pos);
    serialize_number("free_margin", account->free_margin, buffer, pos);
    serialize_comma(buffer, pos);
    serialize_number("margin_level", account->margin_level, buffer, pos);
    serialize_object_end(buffer, pos);
}

void at_free_account(at_account* account){
    // Nothing to free
}

void at_init_position(at_position* position, c8 *symbol, u32 volume, i8 direction, f64 open_price, f64 commission, f64 take_profit_price, f64 stop_loss_price){
    assert(position);
    position->id = at_new_id();
    position->symbol = symbol;
    position->volume = volume;
    position->direction = direction;
    position->open_price = open_price;
    position->commission = commission;
    position->take_profit_price = take_profit_price;
    position->stop_loss_price = stop_loss_price;
    position->close_price = 0;
    position->swap = 0;
    position->profit = 0;
    position->open_time = time(NULL);
    position->close_time = 0;
}

void at_serialize_position(const at_position *position, c8 *buffer, i32 *pos){
    serialize_object_start(NULL, buffer, pos);
    serialize_string("symbol", position->symbol, buffer, pos);
    serialize_comma(buffer, pos);
    serialize_int("volume", position->volume, buffer, pos);
    serialize_comma(buffer, pos);
    serialize_string("direction", AT_DIRECTION_STRING(position->direction), buffer, pos);
    serialize_comma(buffer, pos);
    serialize_number("take_profit_price", position->take_profit_price, buffer, pos);
    serialize_comma(buffer, pos);
    serialize_number("stop_loss_price", position->stop_loss_price, buffer, pos);
    serialize_comma(buffer, pos);
    serialize_number("commission", position->commission, buffer, pos);
    serialize_comma(buffer, pos);
    serialize_number("open_price", position->open_price, buffer, pos);
    serialize_comma(buffer, pos);
    serialize_number("close_price", position->close_price, buffer, pos);
    serialize_comma(buffer, pos);
    serialize_number("swap", position->swap, buffer, pos);
    serialize_comma(buffer, pos);
    serialize_number("profit", position->profit, buffer, pos);
    serialize_comma(buffer, pos);
    serialize_int("open_time", position->open_time, buffer, pos);
    serialize_comma(buffer, pos);
    serialize_int("close_time", position->close_time, buffer, pos);
    serialize_object_end(buffer, pos);
}

void at_free_position(at_position *position){
    // Nothing to free    
}

void at_init_strategy(at_strategy *strategy, const c8 *name, on_start_callback on_start, on_tick_callback on_tick, u32 *candles_periods, sz candles_periods_count){
    assert(strategy && name && candles_periods && candles_periods_count > 0); // at least one period
    strategy->id = at_new_id();
    strategy->name = (c8*)malloc(strlen(name) + 1);
    strcpy(strategy->name, name);
    strategy->on_start = on_start;
    strategy->on_tick = on_tick;
    strategy->candles_periods = candles_periods;
    strategy->candles_periods_count = candles_periods_count;
    strategy->cached_candles = NULL;
    strategy->cached_candles_count = 0;
}

void at_set_strategy_on_start(at_strategy *strategy, on_start_callback on_start){
    assert(strategy);
    strategy->on_start = on_start;
}

void at_set_strategy_on_tick(at_strategy *strategy, on_tick_callback on_tick){
    assert(strategy);
    strategy->on_tick = on_tick;
}

void at_update_strategy(at_strategy *strategy, at_tick *tick){
    assert(strategy && tick);
    strategy->cached_ticks.count++;
    strategy->cached_ticks.ticks = (at_tick*)realloc(strategy->cached_ticks.ticks, sizeof(at_tick) * strategy->cached_ticks.count);
    assert(strategy->cached_ticks.ticks);
    strategy->cached_ticks.ticks[strategy->cached_ticks.count - 1] =* tick;
    for (sz i = 0; i < strategy->candles_periods_count; i++){
        u32 period = strategy->candles_periods[i];
        if (strategy->cached_ticks.count % period == 0){
            continue;
        }
        if (strategy->cached_candles_count > 0){
            for (sz j = 0; j < strategy->cached_candles_count; j++){
                at_candles* candles = &strategy->cached_candles[j];
                if (candles->period == period){
                    at_add_ticks_to_candles(candles, strategy->cached_ticks.ticks, period);
                }
            }
        }
        else {
            strategy->cached_candles_count++;
            strategy->cached_candles = (at_candles*)malloc(sizeof(at_candles) * strategy->cached_candles_count);
            strategy->cached_candles->candles = NULL;
            strategy->cached_candles->count = 0;
            strategy->cached_candles->period = period;
            assert(strategy->cached_candles);
            at_add_ticks_to_candles(strategy->cached_candles, strategy->cached_ticks.ticks, period);
            assert(strategy->cached_candles);
        }
    }
}

void at_serialize_strategy(const at_strategy *strategy, c8 *buffer, i32 *pos){
    serialize_object_start(NULL, buffer, pos);
    serialize_number("id", strategy->id, buffer, pos);
    serialize_comma(buffer, pos);
    serialize_string("name", strategy->name, buffer, pos);
    serialize_comma(buffer, pos);
    serialize_array_start("candles_periods", buffer, pos);
    for (sz i = 0; i < strategy->candles_periods_count; i++) {
        if (i > 0) serialize_comma(buffer, pos);
        *pos += sprintf(buffer + *pos, "%u", strategy->candles_periods[i]);
    }
    serialize_array_end(buffer, pos);
    serialize_object_end(buffer, pos);
}

void at_free_strategy(at_strategy* strategy){
    assert(strategy);
    free(strategy->name);
    for (sz i = 0; i < strategy->cached_candles_count; i++){
        at_candles* candles = &strategy->cached_candles[i];
        if (candles->candles){
            free(candles->candles);
        }
    }
    free(strategy->cached_candles);
    free(strategy->cached_ticks.ticks);
}

void at_init_instance(at_instance *instance, at_strategy *strategy, at_symbol *symbol, at_account *account, f32 commission, f32 swap, f32 leverage){
    assert(instance && strategy && symbol && account);
    instance->id = at_new_id();
    instance->strategy = strategy;
    instance->symbol = symbol;
    instance->account = account;
    instance->commission = commission;
    instance->swap = swap;
    instance->leverage = leverage;
    instance->open_positions = NULL;
    instance->open_positions_count = 0;
    instance->closed_positions = NULL;
    instance->closed_positions_count = 0;
}

void at_free_instance(at_instance *instance)
{
    assert(instance);
    free(instance->open_positions);
    free(instance->closed_positions);
}

void at_add_position(at_instance* instance, at_position* position){
    assert(instance && position);
    instance->open_positions_count++;
    instance->open_positions = (at_position*)realloc(instance->open_positions, sizeof(at_position) * instance->open_positions_count);
    instance->open_positions[instance->open_positions_count - 1] =* position;
    instance->account->equity -= position->volume * position->open_price;
    instance->account->balance -= position->volume * position->open_price;
    instance->account->margin += position->volume * position->open_price;
    instance->account->free_margin = instance->account->balance - instance->account->margin;
    instance->account->margin_level = instance->account->margin > 0 ? instance->account->equity / instance->account->margin : 0;

    log_info("Opened: position %d, symbol %s, volume %d, direction %d, open price %f, take profit %f, stop loss %f", position->id, position->symbol, position->volume, position->direction, position->open_price, position->take_profit_price, position->stop_loss_price);
}

void at_close_position(at_instance *instance, at_position *position, f64 close_price){
    assert(instance && position);
    for (sz i = 0; i < instance->open_positions_count; i++){
        if (instance->open_positions[i].id == position->id){
            instance->account->equity += position->volume * close_price;
            instance->account->balance += position->volume * close_price;
            instance->account->margin -= position->volume * position->open_price;
            instance->account->free_margin = instance->account->balance - instance->account->margin;
            instance->account->margin_level = instance->account->margin > 0 ? instance->account->equity / instance->account->margin : 0;
            instance->open_positions[i].close_price = close_price;
            instance->open_positions[i].swap = 0;
            instance->open_positions[i].profit = position->volume * (close_price - position->open_price);
            instance->open_positions[i].close_time = time(NULL);
            log_info("Closed: position %d, symbol %s, volume %d, direction %d, open price %f, close price %f, profit %f", position->id, position->symbol, position->volume, position->direction, position->open_price, close_price, instance->open_positions[i].profit);
            // add to closed positions
            instance->closed_positions_count++;
            instance->closed_positions = (at_position*)realloc(instance->closed_positions, sizeof(at_position) * instance->closed_positions_count);
            instance->closed_positions[instance->closed_positions_count - 1] =* position;
            // remove from open positions
            instance->open_positions_count--;
            if (instance->open_positions_count > 0){
                instance->open_positions = (at_position*)realloc(instance->open_positions, sizeof(at_position) * instance->open_positions_count);
                assert(instance->open_positions);
            }
            else {
                free(instance->open_positions);
                instance->open_positions = NULL;
            }
            return;
        }
    }
    log_error("Failed to close position");
}

void at_start_instance(at_instance *instance){
    assert(instance);
    if (instance->strategy->on_start){
        instance->strategy->on_start(instance);
    }
}

void at_tick_instance(at_instance* instance, at_tick* tick, const b8 add_tick){
    assert(instance && tick);
    if (add_tick){
        at_add_tick(instance->symbol, tick);
    }
    at_update_strategy(instance->strategy, tick);
    if (instance->strategy->on_tick){
        instance->strategy->on_tick(instance, tick);
    }
    for (sz i = 0; i < instance->open_positions_count; i++){
        at_position* position = &instance->open_positions[i];
        if (position->take_profit_price > 0 && position->direction == AT_DIRECTION_LONG && tick->price >= position->take_profit_price){
            at_close_position(instance, position, position->take_profit_price);
        }
        else if (position->stop_loss_price > 0 && position->direction == AT_DIRECTION_LONG && tick->price <= position->stop_loss_price){
            at_close_position(instance, position, position->stop_loss_price);
        }
        else if (position->take_profit_price > 0 && position->direction == AT_DIRECTION_SHORT && tick->price <= position->take_profit_price){
            at_close_position(instance, position, position->take_profit_price);
        }
        else if (position->stop_loss_price > 0 && position->direction == AT_DIRECTION_SHORT && tick->price >= position->stop_loss_price){
            at_close_position(instance, position, position->stop_loss_price);
        }
    }
}

void at_serialize_instance(const at_instance *instance, c8 *buffer, i32 *pos){
    serialize_object_start("instance", buffer, pos);
    serialize_number("id", instance->id, buffer, pos);
    serialize_comma(buffer, pos);
    serialize_string("strategy", instance->strategy->name, buffer, pos); // Assuming strategy has a name
    serialize_comma(buffer, pos);
    at_serialize_symbol(instance->symbol, buffer, pos);
    serialize_comma(buffer, pos);
    at_serialize_account(instance->account, buffer, pos);
    serialize_comma(buffer, pos);
    serialize_number("commission", instance->commission, buffer, pos);
    serialize_comma(buffer, pos);
    serialize_number("swap", instance->swap, buffer, pos);
    serialize_comma(buffer, pos);
    serialize_number("leverage", instance->leverage, buffer, pos);
    serialize_comma(buffer, pos);
    serialize_array_start("open_positions", buffer, pos);
    for (u32 i = 0; i < instance->open_positions_count; i++) {
        if (i > 0) serialize_comma(buffer, pos);
        at_serialize_position(&instance->open_positions[i], buffer, pos);
    }
    serialize_array_end(buffer, pos);
    serialize_comma(buffer, pos);
    serialize_array_start("closed_positions", buffer, pos);
    for (u32 i = 0; i < instance->closed_positions_count; i++) {
        if (i > 0) serialize_comma(buffer, pos);
        at_serialize_position(&instance->closed_positions[i], buffer, pos);
    }
    serialize_array_end(buffer, pos);
    serialize_object_end(buffer, pos);
}

b8 at_get_symbol_from_json(at_symbol *symbol, at_json *json){
    assert(symbol && json);
    const c8* name = at_json_get_string(json, "name");
    if (!name){
        log_error("Failed to get symbol name from JSON");
        return false;
    }
    const c8* exchange = at_json_get_string(json, "exchange");
    if (!exchange){
        log_error("Failed to get symbol exchange from JSON");
        return false;
    }
    const c8* currency = at_json_get_string(json, "currency");
    if (!currency){
        log_error("Failed to get symbol currency from JSON");
        return false;
    }
    at_json* ticks = at_json_get_array(json, "ticks");
    if (!ticks){
        log_error("Failed to get symbol ticks from JSON");
        return false;
    }
    sz tick_count = at_json_get_array_size(ticks);
    if (tick_count == 0){
        log_error("No ticks found in JSON");
        return false;
    }
    at_init_symbol(symbol, name, exchange, currency, 0);
    for (sz i = 0; i < tick_count; i++){
        at_json* tick = at_json_get_array_item(ticks, i);
        f64 price = at_json_get_float(tick, "price");
        i32 volume = at_json_get_int(tick, "volume");
        at_tick t = {price, volume};
        at_add_tick(symbol, &t);
    }
    return true;
}

b8 at_get_account_from_json(at_account *account, at_json *json){
    assert(account && json);
    f64 balance = at_json_get_int(json, "balance");
    if (balance == 0){
        log_error("Failed to get account balance from JSON");
        return false;
    }
    at_init_account(account, balance);
    return true;
}

b8 at_get_strategy_from_json(at_strategy *strategy, at_json *json){
    assert(strategy && json);
    const c8* name = at_json_get_string(json, "name");
    if (!name){
        log_error("Failed to get strategy name from JSON");
        return false;
    }
    u32* candles_periods = (u32*)malloc(sizeof(u32) * AT_CANDLES_PERIODS_MAX);
    sz found_periods_count = at_json_get_u32_array(json, "candles_periods", candles_periods, AT_CANDLES_PERIODS_MAX);
    if (found_periods_count == 0){
        log_error("Failed to get candles periods from JSON");
        return false;
    }
    candles_periods = (u32*)realloc(candles_periods, sizeof(u32) * found_periods_count);
    at_init_strategy(strategy, name, NULL, NULL, candles_periods, found_periods_count);
    return true;
}

// TODO change all the checks into a macro that access the variable name and prints the error message
b8 at_get_instance_from_json(at_instance *instance, at_json *json){
    assert(instance && json);
    at_symbol* symbol = (at_symbol*)malloc(sizeof(at_symbol)); 
    at_account* account = (at_account*)malloc(sizeof(at_account));
    at_strategy* strategy = (at_strategy*)malloc(sizeof(at_strategy));
    at_json* symbol_json = at_json_get_object(json, "symbol");
    if (!at_get_symbol_from_json(symbol, symbol_json)){
        log_error("Failed to get symbol from JSON");
        return false;
    }
    at_json* account_json = at_json_get_object(json, "account");
    if (!at_get_account_from_json(account, account_json)){
        log_error("Failed to get account from JSON");
        return false;
    }
    at_json* strategy_json = at_json_get_object(json, "strategy");
    if (!at_get_strategy_from_json(strategy, strategy_json)){
        log_error("Failed to get strategy from JSON");
        return false;
    }
    at_get_symbol_from_json(symbol, symbol_json);
    if (!symbol){
        log_error("Failed to get symbol from JSON");
        return false;
    }
    at_get_account_from_json(account, account_json);
    if (!account){
        log_error("Failed to get account from JSON");
        return false;
    }
    at_get_strategy_from_json(strategy, strategy_json);
    if (!strategy){
        log_error("Failed to get strategy from JSON");
        return false;
    }
    at_init_instance(instance, strategy, symbol, account, 0.0, 0.0, 1.0);
    return true;
}

void at_init_backtest(at_backtest *backtest, c8 *path, on_start_callback on_start, on_tick_callback on_tick){
    assert(backtest && path);
    
    backtest->path = malloc(strlen(path) + 1);
    strcpy(backtest->path, path);

    c8* file_content = at_read_file(path);
    if (!file_content){
        log_error("Failed to read file %s", path);
        return;
    }
    at_json* json_root = at_json_parse(file_content);
    if (!json_root){
        log_error("Failed to parse JSON file %s", path);
        free(file_content);
        return;
    }
    if (backtest->instance){
        at_free_instance(backtest->instance);
        free(backtest->instance);
    }
    backtest->instance = (at_instance*)malloc(sizeof(at_instance));
    at_get_instance_from_json(backtest->instance, json_root);
    backtest->render_period = (u32)at_json_get_int(json_root, "render_period");
    at_set_strategy_on_start(backtest->instance->strategy, on_start);
    at_set_strategy_on_tick(backtest->instance->strategy, on_tick);
}

void at_start_backtest(at_backtest *backtest){
    assert(backtest);
    at_start_instance(backtest->instance);
    at_symbol* symbol = backtest->instance->symbol;
    u32 tick_count = symbol->tick_count;
    for (u32 i = 0; i < tick_count; i++){
        at_tick tick = *at_get_tick(symbol, i);
        at_tick_instance(backtest->instance, &tick, false);
    }
}

void at_save_backtest_results(at_backtest *backtest, c8 *path){
    assert(backtest);
    log_info("Saving backtest results to %s", path);
    i32 pos = 0;
    c8 *buffer = (c8*)malloc(AT_SRIALIZE_BUFFER_SIZE);
    at_serialize_backtest(backtest, buffer, &pos);
    if (at_write_file(path, buffer)){
        log_info("Backtest results saved to %s", path);
    }
    else {
        log_error("Failed to save backtest results to %s", path);
    }
    free(buffer);
}

void at_serialize_backtest(const at_backtest *backtest, c8 *buffer, i32 *pos){
    serialize_object_start(NULL, buffer, pos);

    // Serialize the file path
    serialize_string("path", backtest->path, buffer, pos);
    serialize_comma(buffer, pos);

    // Serialize the instance
    at_serialize_instance(backtest->instance, buffer, pos); // Reuse the instance serialization function
    serialize_comma(buffer, pos);

    // Serialize the render period
    serialize_int("render_period", backtest->render_period, buffer, pos);

    serialize_object_end(buffer, pos);
}

void at_free_backtest(at_backtest *backtest){
    assert(backtest && backtest->instance);
    at_free_symbol(backtest->instance->symbol);
    at_free_account(backtest->instance->account);
    at_free_strategy(backtest->instance->strategy);
    at_free_instance(backtest->instance);
    free(backtest->instance);
    free(backtest->path);
}

