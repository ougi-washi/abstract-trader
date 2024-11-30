// abstract-trader https://github.com/ougi-washi/abstract-trader
  
#include "core/types.h"
#include "core/log.h"
#include "core/render.h"

const at_tick ticks_sample[] = {
    {142.0, 100.0}, {144.0, 100.0}, {145.5, 100.0}, {143.0, 100.0}, {147.0, 100.0}, {150.0, 100.0},
    {149.0, 100.0}, {152.0, 100.0}, {148.0, 100.0}, {146.0, 100.0}, {144.5, 100.0}, {147.5, 100.0},
    {135.0, 100.0}, {137.0, 100.0}, {139.5, 100.0}, {136.5, 100.0}, {140.0, 100.0}, {142.5, 100.0},
    {130.0, 100.0}, {132.0, 100.0}, {134.0, 100.0}, {133.5, 100.0}, {136.0, 100.0}, {138.0, 100.0},
    {125.0, 100.0}, {123.0, 100.0}, {126.5, 100.0}, {127.0, 100.0}, {129.0, 100.0}, {130.5, 100.0},
    {140.0, 100.0}, {141.5, 100.0}, {144.0, 100.0}, {145.0, 100.0}, {142.5, 100.0}, {141.0, 100.0},
    {155.0, 100.0}, {157.0, 100.0}, {159.0, 100.0}, {160.0, 100.0}, {161.5, 100.0}, {162.0, 100.0},
    {150.0, 100.0}, {148.5, 100.0}, {147.0, 100.0}, {146.0, 100.0}, {149.5, 100.0}, {151.0, 100.0},
    {168.0, 100.0}, {169.0, 100.0}, {170.0, 100.0}, {173.0, 100.0}, {175.0, 100.0}, {174.0, 100.0},
    {180.0, 100.0}, {178.0, 100.0}, {181.0, 100.0}, {182.5, 100.0}, {185.0, 100.0}, {186.0, 100.0},
    {175.5, 100.0}, {172.0, 100.0}, {173.5, 100.0}, {176.0, 100.0}, {179.0, 100.0}, {181.5, 100.0},
    {166.0, 100.0}, {164.5, 100.0}, {167.0, 100.0}, {170.0, 100.0}, {168.5, 100.0}, {169.5, 100.0},
    {154.0, 100.0}, {156.0, 100.0}, {155.5, 100.0}, {153.0, 100.0}, {157.0, 100.0}, {159.0, 100.0},
    {140.0, 100.0}, {138.0, 100.0}, {141.0, 100.0}, {144.5, 100.0}, {146.5, 100.0}, {149.0, 100.0},
    {132.5, 100.0}, {134.0, 100.0}, {136.5, 100.0}, {139.0, 100.0}, {141.5, 100.0}, {144.0, 100.0},
    {120.0, 100.0}, {118.0, 100.0}, {121.0, 100.0}, {123.5, 100.0}, {125.0, 100.0}, {126.5, 100.0},
    {110.0, 100.0}, {111.5, 100.0}, {112.0, 100.0}, {113.5, 100.0}, {115.0, 100.0}, {118.5, 100.0},
    {100.0, 100.0}, {102.0, 100.0}, {103.5, 100.0}, {105.0, 100.0}, {107.0, 100.0}, {109.0, 100.0},
};
const sz ticks_count = sizeof(ticks_sample) / sizeof(at_tick);

void on_start_strategy(at_instance *instance) {
    log_info("Starting strategy %s", instance->strategy->name);
}

static b8 trigger_once = 0;
void on_tick_strategy(at_instance *instance, at_tick *tick) {
    if (tick->price > 150.0 && tick->price < 160.0 && !trigger_once) {
        at_position position1 = {0};
        at_init_position(&position1, "AAPL", 100, AT_DIRECTION_LONG, tick->price, 0.0, 160.0, 150.0);
        at_add_position(instance, &position1);
        trigger_once = 1;

        at_position position2 = {0};
        at_init_position(&position2, "AAPL", 100, AT_DIRECTION_SHORT, tick->price, 0.0, 160.0, 150.0);
        at_add_position(instance, &position2);

        at_position position3 = {0};
        at_init_position(&position3, "AAPL", 100, AT_DIRECTION_LONG, tick->price, 0.0, 160.0, 150.0);
        at_add_position(instance, &position3);
    }
}

i32 main(i32 argc, c8 **argv) {
    at_symbol symbol = {0};
    at_account account = {0};
    at_strategy strategy = {0};
    at_instance instance = {0};
    at_init_symbol(&symbol, "AAPL", "NASDAQ", "USD", 0);
    at_init_account(&account, 1000.0);
    at_init_strategy(&strategy, "Test Strategy", on_start_strategy, on_tick_strategy, (u32[]){3, 5, 15}, 3); // use 3, 5, 15 candles (from ticks) for strategy
    at_init_instance(&instance, &strategy, &symbol, &account, 0.0, 0.0, 1.0);

    at_start_instance(&instance);
    for (sz i = 0; i < ticks_count; i++) {
        at_tick tick = ticks_sample[i];
        at_tick_instance(&instance, &tick);
    }

    u32 candle_count = 0;
    at_candle *candles = at_get_candles(&symbol, 5, &candle_count);

    // Render candles
    at_render render = {0};
    at_init_render(&render);
    at_render_object object = {0};
    at_candles_to_render_object(candles, candle_count, &object);
    at_add_render_object(&render, &object);
    while (at_should_loop_render(&render)) {
        at_draw_render(&render);
    }
    free(candles);
    at_free_symbol(&symbol);
    return 0;
}