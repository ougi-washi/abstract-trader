// abstract-trader https://github.com/ougi-washi/abstract-trader
  
#include "core/log.h"
#include "core/render.h"

const at_tick ticks_sample[] = {
    {0, 142.0, 100.0}, {1, 144.0, 100.0}, {2, 145.5, 100.0}, {3, 143.0, 100.0}, {4, 147.0, 100.0}, {5, 150.0, 100.0},
    {6, 149.0, 100.0}, {7, 152.0, 100.0}, {8, 148.0, 100.0}, {9, 146.0, 100.0}, {10, 144.5, 100.0}, {11, 147.5, 100.0},
    {12, 135.0, 100.0}, {13, 137.0, 100.0}, {14, 139.5, 100.0}, {15, 136.5, 100.0}, {16, 140.0, 100.0}, {17, 142.5, 100.0},
    {18, 130.0, 100.0}, {19, 132.0, 100.0}, {20, 134.0, 100.0}, {21, 133.5, 100.0}, {22, 136.0, 100.0}, {23, 138.0, 100.0},
    {24, 125.0, 100.0}, {25, 123.0, 100.0}, {26, 126.5, 100.0}, {27, 127.0, 100.0}, {28, 129.0, 100.0}, {29, 130.5, 100.0},
    {30, 140.0, 100.0}, {31, 141.5, 100.0}, {32, 144.0, 100.0}, {33, 145.0, 100.0}, {34, 142.5, 100.0}, {35, 141.0, 100.0},
    {36, 155.0, 100.0}, {37, 157.0, 100.0}, {38, 159.0, 100.0}, {39, 160.0, 100.0}, {40, 161.5, 100.0}, {41, 162.0, 100.0},
    {42, 150.0, 100.0}, {43, 148.5, 100.0}, {44, 147.0, 100.0}, {45, 146.0, 100.0}, {46, 149.5, 100.0}, {47, 151.0, 100.0},
    {48, 168.0, 100.0}, {49, 169.0, 100.0}, {50, 170.0, 100.0}, {51, 173.0, 100.0}, {52, 175.0, 100.0}, {53, 174.0, 100.0},
    {54, 180.0, 100.0}, {55, 178.0, 100.0}, {56, 181.0, 100.0}, {57, 182.5, 100.0}, {58, 185.0, 100.0}, {59, 186.0, 100.0},
    {60, 175.5, 100.0}, {61, 172.0, 100.0}, {62, 173.5, 100.0}, {63, 176.0, 100.0}, {64, 179.0, 100.0}, {65, 181.5, 100.0},
    {66, 166.0, 100.0}, {67, 164.5, 100.0}, {68, 167.0, 100.0}, {69, 170.0, 100.0}, {70, 168.5, 100.0}, {71, 169.5, 100.0},
    {72, 154.0, 100.0}, {73, 156.0, 100.0}, {74, 155.5, 100.0}, {75, 153.0, 100.0}, {76, 157.0, 100.0}, {77, 159.0, 100.0},
    {78, 140.0, 100.0}, {79, 138.0, 100.0}, {80, 141.0, 100.0}, {81, 144.5, 100.0}, {82, 146.5, 100.0}, {83, 149.0, 100.0},
    {84, 132.5, 100.0}, {85, 134.0, 100.0}, {86, 136.5, 100.0}, {87, 139.0, 100.0}, {88, 141.5, 100.0}, {89, 144.0, 100.0},
    {90, 120.0, 100.0}, {91, 118.0, 100.0}, {92, 121.0, 100.0}, {93, 123.5, 100.0}, {94, 125.0, 100.0}, {95, 126.5, 100.0},
    {96, 110.0, 100.0}, {97, 111.5, 100.0}, {98, 112.0, 100.0}, {99, 113.5, 100.0}, {100, 115.0, 100.0}, {101, 118.5, 100.0},
    {102, 100.0, 100.0}, {103, 102.0, 100.0}, {104, 103.5, 100.0}, {105, 105.0, 100.0}, {106, 107.0, 100.0}, {107, 109.0, 100.0},
};

const sz ticks_count = sizeof(ticks_sample) / sizeof(at_tick);

void on_start_strategy(at_instance *instance) {
    log_info("Starting strategy %s", instance->strategy->name);
}

static b8 trigger_once = 0;
void on_tick_strategy(at_instance *instance, at_tick *tick) {
    log_info("Tick: %f", tick->price);
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

// i32 main(i32 argc, c8 **argv) {
//     at_symbol symbol = {0};
//     at_account account = {0};
//     at_strategy strategy = {0};
//     at_instance instance = {0};
//     at_init_symbol(&symbol, "AAPL", "NASDAQ", "USD", 0);
//     at_init_account(&account, 1000.0);
//     at_init_strategy(&strategy, "Test Strategy", on_start_strategy, on_tick_strategy, (u32[]){3, 5, 15}, 3); // use 3, 5, 15 candles (from ticks) for strategy
//     at_init_instance(&instance, &strategy, &symbol, &account, 0.0, 0.0, 1.0);

//     at_start_instance(&instance);
//     for (sz i = 0; i < ticks_count; i++) {
//         at_tick tick = ticks_sample[i];
//         at_tick_instance(&instance, &tick);
//     }

//     u32 candle_count = 0;
//     at_candle *candles = at_get_candles(&symbol, 5, &candle_count);

//     // Render candles
//     at_render render = {0};
//     at_init_render(&render);
//     at_render_object object = {0};
//     at_candles_to_render_object(candles, candle_count, &object);
//     at_add_render_object(&render, &object);
//     while (at_should_loop_render(&render)) {
//         at_draw_render(&render);
//     }
//     free(candles);
//     at_free_symbol(&symbol);
//     at_free_account(&account);
//     at_free_strategy(&strategy);
//     at_free_instance(&instance);
//     at_free_render(&render);

//     return 0;
// }

i32 main (i32 argc, c8 **argv) {
    at_backtest backtest = {0};
    at_init_backtest(&backtest, "strategies/test_strategy.json", on_start_strategy, on_tick_strategy);
    at_start_backtest(&backtest);
    at_save_backtest_results(&backtest, "results/test_strategy.json");

    at_render render = {0};
    at_init_render(&render);
    at_render_object object = {0};
    
    //logs
    AT_ARRAY_FOREACH_PTR(backtest.instance->strategy->candles, at_candle_chunk, candles, {
        log_info("Period: %u, Candles count: %zu", candles->period, AT_ARRAY_SIZE(candles->candles));
        AT_ARRAY_FOREACH_PTR(candles->candles, at_candle, candle, {
            log_info("Candle: %f %f %f %f %f", candle->open, candle->high, candle->low, candle->close, candle->volume);
        });
    });

    at_candle_chunk* candles = AT_ARRAY_GET_PTR(backtest.instance->strategy->candles, 0);
    at_candles_to_render_object(&candles->candles, &object);
    at_add_render_object(&render, &object);
    while (at_should_loop_render(&render)) {
        at_draw_render(&render);
    }
    at_free_render(&render);
    at_free_backtest(&backtest);
    return 0;
}