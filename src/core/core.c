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

i32 compare_ticks_by_timestamp(const void* a, const void* b) {
    const at_tick* tick_a = (const at_tick*)a;
    const at_tick* tick_b = (const at_tick*)b;

    if (tick_a->timestamp < tick_b->timestamp) return -1; // Ascending order
    if (tick_a->timestamp > tick_b->timestamp) return 1;
    return 0;
}

at_candle* at_get_candles(at_symbol* symbol, u32 period, u32* out_candle_count) {
    assert(symbol && period > 0 && out_candle_count);
    if (AT_ARRAY_SIZE(symbol->ticks) < period) {
        *out_candle_count = 0;
        log_error("Not enough ticks to create candles");
        return NULL; 
    }

    u32 candle_count = AT_ARRAY_SIZE(symbol->ticks) / period;
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
        candle.open = AT_ARRAY_GET(symbol->ticks, start_idx).price;
        candle.high = candle.open;
        candle.low = candle.open;
        candle.close = AT_ARRAY_GET(symbol->ticks, end_idx - 1).price;
        candle.volume = 0;
        candle.adj_close = 0;

        for (u32 j = start_idx; j < end_idx; j++) {
            const f64 price = AT_ARRAY_GET(symbol->ticks, j).price;
            candle.volume += AT_ARRAY_GET(symbol->ticks, j).volume;
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

void at_add_tick_to_candles(at_candle_chunk *candles, at_tick *tick){
    assert(candles && tick);
    AT_ARRAY_ADD(candles->extra_ticks, *tick);
    at_update_candles(candles);
}

void at_add_ticks_to_candles(at_candle_chunk *candles, at_tick_array* ticks) {
    assert(candles && ticks);
    AT_ARRAY_ADD_ARRAY(candles->extra_ticks, *ticks);
    at_update_candles(candles);
}

void at_update_candles(at_candle_chunk *candles){
    assert(candles);
    
    at_candle_array* candles_array = &candles->candles;
    const u32 candle_period = candles->period;
    const sz tick_count = AT_ARRAY_SIZE(candles->extra_ticks);

    if (tick_count == 0) {
        return;
    }

    AT_ARRAY_SORT(candles->extra_ticks, at_tick, compare_ticks_by_timestamp);

    at_tick* first_tick = AT_ARRAY_GET_PTR(candles->extra_ticks, 0);
    u64 start_time = first_tick->timestamp - (first_tick->timestamp % candle_period);
    // Iterate over ticks and assign them to candles
    for (sz i = 0; i < tick_count; ++i) {
        at_tick* tick = AT_ARRAY_GET_PTR(candles->extra_ticks, i);
        u64 candle_start_time = tick->timestamp - (tick->timestamp % candle_period);

        // Create candles if needed
        while (AT_ARRAY_SIZE(*candles_array) == 0 || candle_start_time > start_time) {
            at_candle new_candle = {0};
            new_candle.timestamp = start_time;
            AT_ARRAY_ADD(*candles_array, new_candle);
            start_time += candle_period;
        }

        at_candle* current_candle = AT_ARRAY_LAST_PTR(*candles_array);
        if (current_candle->open == 0) {
            current_candle->open = tick->price;
            current_candle->high = tick->price;
            current_candle->low = tick->price;
        }
        current_candle->close = tick->price;
        current_candle->volume += tick->volume;
        current_candle->high = tick->price > current_candle->high ? tick->price : current_candle->high;
        current_candle->low = tick->price < current_candle->low ? tick->price : current_candle->low;
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
    serialize_int("tick_count", AT_ARRAY_SIZE(symbol->ticks), buffer, pos);
    // ticks serialization if necessary
    serialize_object_end(buffer, pos);
}

void at_free_symbol(at_symbol* symbol){
    assert(symbol);
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
    strcpy(position->symbol_name, symbol);
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
    serialize_string("symbol", position->symbol_name, buffer, pos);
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

void at_init_strategy(at_strategy *strategy, const c8 *name, on_start_callback on_start, on_tick_callback on_tick, u32_array* candles_periods){
    assert(strategy && name && candles_periods); // at least one period
    strategy->id = at_new_id();
    strcpy(strategy->name, name);
    strategy->on_start = on_start;
    strategy->on_tick = on_tick;
    AT_ARRAY_INIT(strategy->candles);
    AT_ARRAY_FOREACH(*candles_periods, u32, period,{
        AT_ARRAY_ADD(strategy->candles, (at_candle_chunk){0});
        AT_ARRAY_LAST(strategy->candles).period = period;
    });
}

void at_set_strategy_on_start(at_strategy *strategy, on_start_callback on_start){
    assert(strategy);
    strategy->on_start = on_start;
}

void at_set_strategy_on_tick(at_strategy *strategy, on_tick_callback on_tick){
    assert(strategy);
    strategy->on_tick = on_tick;
}

void at_update_strategy_single_tick(at_strategy *strategy, at_tick* tick){
    assert(strategy && tick);

    AT_ARRAY_FOREACH_PTR(strategy->candles, at_candle_chunk, candles, {
        at_add_tick_to_candles(candles, tick);
    });
}

void at_update_strategy_multi_ticks(at_strategy *strategy, at_tick_array *ticks){
    assert(strategy && ticks);
    if (AT_ARRAY_SIZE(*ticks) == 0){
        log_error("No ticks to update strategy");
        return;
    }
    AT_ARRAY_FOREACH_PTR(strategy->candles, at_candle_chunk, candles, {
        at_add_ticks_to_candles(candles, ticks);
    });
}

void at_serialize_strategy(const at_strategy *strategy, c8 *buffer, i32 *pos)
{
    serialize_object_start(NULL, buffer, pos);
    serialize_number("id", strategy->id, buffer, pos);
    serialize_comma(buffer, pos);
    serialize_string("name", strategy->name, buffer, pos);
    serialize_comma(buffer, pos);
    serialize_array_start("candles_periods", buffer, pos);
    AT_ARRAY_FOREACH(strategy->candles, at_candle_chunk, candles, {
        if (i > 0) serialize_comma(buffer, pos);
        *pos += sprintf(buffer + *pos, "%u", candles.period);
    });
    serialize_array_end(buffer, pos);
    serialize_object_end(buffer, pos);
}

void at_free_strategy(at_strategy* strategy){
    assert(strategy);
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
    AT_ARRAY_INIT(instance->open_positions);
    AT_ARRAY_INIT(instance->closed_positions);
}

void at_free_instance(at_instance *instance)
{
    assert(instance);
}

void at_add_position(at_instance* instance, at_position* position){
    assert(instance && position);
    AT_ARRAY_ADD(instance->open_positions, *position);
    instance->account->equity -= position->volume * position->open_price;
    instance->account->balance -= position->volume * position->open_price;
    instance->account->margin += position->volume * position->open_price;
    instance->account->free_margin = instance->account->balance - instance->account->margin;
    instance->account->margin_level = instance->account->margin > 0 ? instance->account->equity / instance->account->margin : 0;

    log_info("Opened: position %d, symbol %s, volume %d, direction %d, open price %f, take profit %f, stop loss %f", position->id, position->symbol_name, position->volume, position->direction, position->open_price, position->take_profit_price, position->stop_loss_price);
}

void at_close_position(at_instance *instance, at_position *position, f64 close_price){
    assert(instance && position);

    at_id p_id = position->id; // store ID so we can remove it from open positions
    b8 removed = false;
    AT_ARRAY_FOREACH_PTR(instance->open_positions, at_position, p, {
        if (p->id == position->id) {
            instance->account->equity += position->volume * close_price;
            instance->account->balance += position->volume * close_price;
            instance->account->margin -= position->volume * position->open_price;
            instance->account->free_margin = instance->account->balance - instance->account->margin;
            instance->account->margin_level = instance->account->margin > 0 ? instance->account->equity / instance->account->margin : 0;
            p->close_price = close_price;
            p->swap = 0;
            p->profit = position->volume * (close_price - position->open_price);
            p->close_time = time(NULL);
            AT_ARRAY_ADD(instance->closed_positions, *p);
            removed = true;
            log_info("Closed: position %d, symbol %s, volume %d, direction %d, open price %f, close price %f, profit %f", position->id, position->symbol_name, position->volume, position->direction, position->open_price, close_price, p->profit);
        }
    });
    if (removed){
        AT_ARRAY_REMOVE_WITH_PTR_PREDICATE(instance->open_positions, at_position, p, p->id == p_id);
    }
    else {
        log_error("Failed to close position");
    }
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
    at_update_strategy_single_tick(instance->strategy, tick);
    if (instance->strategy->on_tick){
        instance->strategy->on_tick(instance, tick);
    }

    AT_ARRAY_FOREACH_PTR(instance->open_positions, at_position, position, {
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
    });
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
    AT_ARRAY_FOREACH_CONST_PTR(instance->open_positions, at_position, position, {
        if (i > 0) serialize_comma(buffer, pos);
        at_serialize_position(position, buffer, pos);
    });
    serialize_array_end(buffer, pos);
    serialize_comma(buffer, pos);
    serialize_array_start("closed_positions", buffer, pos);
    AT_ARRAY_FOREACH_CONST_PTR(instance->closed_positions, at_position, position, {
        if (i > 0) serialize_comma(buffer, pos);
        at_serialize_position(position, buffer, pos);
    });
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
        u32 timestamp = at_json_get_int(tick, "timestamp");
        f64 price = at_json_get_float(tick, "price");
        i32 volume = at_json_get_int(tick, "volume");
        at_tick t = {timestamp, price, volume};
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

    u32_array candles_periods = {0};
    sz found_periods_count = at_json_get_u32_array(json, "candles_periods", &candles_periods);
    if (found_periods_count == 0){
        log_error("Failed to get candles periods from JSON");
        return false;
    }
    at_init_strategy(strategy, name, NULL, NULL, &candles_periods);
    return true;
}

// TODO change all the checks into a macro that access the variable name and prints the error message
b8 at_get_instance_from_json(at_instance *instance, at_json *json){
    assert(instance && json);
    at_symbol* symbol = (at_symbol*)malloc(sizeof(at_symbol)); 
    at_account* account = (at_account*)malloc(sizeof(at_account));
    at_strategy* strategy = (at_strategy*)malloc(sizeof(at_strategy));
    assert(symbol && account && strategy);
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
    at_init_instance(instance, strategy, symbol, account, 0.0, 0.0, 1.0);
    return true;
}

void at_init_backtest(at_backtest *backtest, c8 *path, on_start_callback on_start, on_tick_callback on_tick){
    assert(backtest && path);
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

    AT_ARRAY_FOREACH_PTR(symbol->ticks, at_tick, tick, {
        at_tick_instance(backtest->instance, tick, false);
    });
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
}

