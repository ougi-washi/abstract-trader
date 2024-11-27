// abstract-trader https://github.com/ougi-washi/abstract-trader
  
#include "core/types.h"
#include "core/log.h"
#include "core/core.h"

i32 main(i32 argc, c8 **argv) {
    at_symbol symbol = {0};
    at_init_symbol(&symbol, "AAPL", "NASDAQ", "USD", 10);
    at_free_symbol(&symbol);
    return 0;
}