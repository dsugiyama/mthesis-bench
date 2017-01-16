if [[ $1 == knl ]]; then
    ICC="icc -std=gnu11 -O3 -ipo  -xMIC-AVX512"
    GCC="gcc -std=gnu11 -O3 -flto -mavx512f -mavx512cd -mavx512er -mavx512pf"
    OMPC_MAIN="/home/sugiyama/inst/omni-compiler_cilk/opt/lib/ompc_main.o"
    OMPC_FLAGS="-L/home/sugiyama/inst/omni-compiler_cilk/opt/lib \
                -L/home/sugiyama/inst/argobots/opt/lib \
                -L/home/sugiyama/inst/hwloc-1.11.4/lib \
                -lompc -labt -lhwloc -lpthread -qopenmp"
else
    ICC="icc -std=gnu11 -O3 -ipo  -xHOST"
    GCC="gcc -std=gnu11 -O3 -flto -march=native"
    OMPC_MAIN="/home/sugiyama/inst/omni-compiler_cilk/opt/lib/ompc_main.o"
    OMPC_FLAGS="-L/home/sugiyama/inst/omni-compiler_cilk/opt/lib \
                -L/home/sugiyama/inst/argobots/opt/lib \
                -L/home/sugiyama/inst/hwloc-1.11.2/lib \
                -lompc -labt -lhwloc -lpthread -qopenmp"
fi

$ICC test_abt.c  ../delay.c ../clock.c $OMPC_MAIN $OMPC_FLAGS -o test_abt
$ICC test_abt.c  ../delay.c ../clock.c ../ompc_serial_spawn.c -o test_abt_master \
    -I/home/sugiyama/inst/argobots/opt/include -L/home/sugiyama/inst/argobots/opt/lib \
    -labt -lpthread
$ICC test_cilk.c ../delay.c ../clock.c -o test_cilk
