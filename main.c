// abstract-trader https://github.com/ougi-washi/abstract-trader
  
#include "core/types.h"
#include "core/log.h"
#include "core/core.h"

at_tick ticks_sample[] = {
    {100.0, 100.0}, {101.0, 100.0}, {102.0, 100.0}, {103.0, 100.0}, {104.0, 100.0}, {105.0, 100.0}, 
    {106.0, 100.0}, {107.0, 100.0}, {108.0, 100.0}, {109.0, 100.0}, {110.0, 100.0}, {111.0, 100.0}, 
    {112.0, 100.0}, {113.0, 100.0}, {114.0, 100.0}, {115.0, 100.0}, {116.0, 100.0}, {117.0, 100.0}, 
    {118.0, 100.0}, {119.0, 100.0}, {120.0, 100.0}, {121.0, 100.0}, {122.0, 100.0}, {123.0, 100.0},
    {124.0, 100.0}, {125.0, 100.0}, {126.0, 100.0}, {127.0, 100.0}, {128.0, 100.0}, {129.0, 100.0}, 
    {130.0, 100.0}, {131.0, 100.0}, {132.0, 100.0}, {133.0, 100.0}, {134.0, 100.0}, {135.0, 100.0}, 
    {136.0, 100.0}, {137.0, 100.0}, {138.0, 100.0}, {139.0, 100.0}, {140.0, 100.0}, {141.0, 100.0}, 
    {142.0, 100.0}, {143.0, 100.0}, {144.0, 100.0}
};

i32 main(i32 argc, c8 **argv) {
    at_symbol symbol = {0};
    at_init_symbol(&symbol, "AAPL", "NASDAQ", "USD", 1000);    
    at_add_ticks(&symbol, ticks_sample, sizeof(ticks_sample) / sizeof(at_tick));
    u32 candle_count = 0;
    at_candle *candles = at_get_candles(&symbol, 5, candle_count);
    if (candles){
        for (u32 i = 0; i < symbol.tick_count / 5; i++){
            log_info("Candle %d: Open: %.2f, Close: %.2f", i, candles[i].open, candles[i].close);
        }
        free(candles);
    }
    at_free_symbol(&symbol);
    return 0;
}