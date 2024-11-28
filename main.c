// abstract-trader https://github.com/ougi-washi/abstract-trader
  
#include "core/types.h"
#include "core/log.h"
#include "core/render.h"

at_tick ticks_sample[] = {
    {100.0, 100.0}, {101.0, 100.0}, {102.0, 100.0}, {103.0, 100.0}, {104.0, 100.0}, {105.0, 100.0}, 
    {106.0, 100.0}, {107.0, 100.0}, {108.0, 100.0}, {109.0, 100.0}, {110.0, 100.0}, {111.0, 100.0}, 
    {112.0, 100.0}, {113.0, 100.0}, {114.0, 100.0}, {115.0, 100.0}, {116.0, 100.0}, {117.0, 100.0}, 
    {124.0, 100.0}, {125.0, 100.0}, {126.0, 100.0}, {127.0, 100.0}, {128.0, 100.0}, {129.0, 100.0}, 
    {142.0, 100.0}, {143.0, 100.0}, {144.0, 100.0}, {145.0, 100.0}, {146.0, 100.0}, {147.0, 100.0}, 
    {136.0, 100.0}, {137.0, 100.0}, {138.0, 100.0}, {139.0, 100.0}, {140.0, 100.0}, {141.0, 100.0}, 
    {130.0, 100.0}, {131.0, 100.0}, {132.0, 100.0}, {133.0, 100.0}, {134.0, 100.0}, {135.0, 100.0}, 
    {118.0, 100.0}, {119.0, 100.0}, {120.0, 100.0}, {121.0, 100.0}, {122.0, 100.0}, {123.0, 100.0},
    {148.0, 100.0}, {149.0, 100.0}, {150.0, 100.0}, {151.0, 100.0}, {152.0, 100.0}, {153.0, 100.0}, 
    {160.0, 100.0}, {161.0, 100.0}, {162.0, 100.0}, {163.0, 100.0}, {164.0, 100.0}, {165.0, 100.0}, 
    {178.0, 100.0}, {179.0, 100.0}, {180.0, 100.0}, {181.0, 100.0}, {182.0, 100.0}, {183.0, 100.0}, 
    {154.0, 100.0}, {155.0, 100.0}, {156.0, 100.0}, {157.0, 100.0}, {158.0, 100.0}, {159.0, 100.0}, 
    {166.0, 100.0}, {167.0, 100.0}, {168.0, 100.0}, {169.0, 100.0}, {170.0, 100.0}, {171.0, 100.0}, 
    {172.0, 100.0}, {173.0, 100.0}, {174.0, 100.0}, {175.0, 100.0}, {176.0, 100.0}, {177.0, 100.0}, 
    {184.0, 100.0}, {185.0, 100.0}, {186.0, 100.0}, {187.0, 100.0}, {188.0, 100.0}, {189.0, 100.0}
};

i32 main(i32 argc, c8 **argv) {

    at_render render = {0};
    at_init_render(&render);
    at_symbol symbol = {0};
    at_init_symbol(&symbol, "AAPL", "NASDAQ", "USD", 0);
    sz ticks_count = sizeof(ticks_sample) / sizeof(at_tick);
    at_add_ticks(&symbol, ticks_sample, ticks_count);

    u32 candle_count = 0;
    at_candle* candles = at_get_candles(&symbol, 3, &candle_count); // 3 seconds candles (if ticks are in seconds)
    if (candles) {
        for (u32 i = 0; i < candle_count; i++) {
            printf("Candle %u: Open=%.2f, High=%.2f, Low=%.2f, Close=%.2f, Volume=%.2f\n",
                i, candles[i].open, candles[i].high, candles[i].low,
                candles[i].close, candles[i].volume);
        }
    }

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