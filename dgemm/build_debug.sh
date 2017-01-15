CLANG="clang -std=gnu11 -g3 -O0"

$CLANG test_abt.c dgemm_abt.c ../clock.c ompc_serial_spawn.c -o dgemm_abt_serial_spawn \
    -I/Users/daisuke/inst/argobots/include -L/Users/daisuke/inst/argobots/lib \
    -labt -lpthread
