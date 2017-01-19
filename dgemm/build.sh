if [[ $1 == knl ]]; then
    ICC="icc -std=gnu11 -O3 -ipo  -xMIC-AVX512"
    GCC="gcc -std=gnu11 -O3 -flto -mavx512f -mavx512cd -mavx512er -mavx512pf"
    OMPC_MAIN="/home/sugiyama/inst/omni-compiler_cilk/opt/lib/ompc_main.o"
    OMPC_FLAGS="-L/home/sugiyama/inst/omni-compiler_cilk/opt/lib \
                -L/home/sugiyama/inst/argobots/opt/lib \
                -L/home/sugiyama/inst/hwloc-1.11.4/lib \
                -I/home/sugiyama/inst/argobots/opt/include \
                -lompc -labt -lhwloc -lpthread -qopenmp"
else
    ICC="icc -std=gnu11 -O3 -ipo  -xHOST"
    GCC="gcc -std=gnu11 -O3 -flto -march=native"
    OMPC_MAIN="/home/sugiyama/inst/omni-compiler_cilk/opt/lib/ompc_main.o"
    OMPC_FLAGS="-L/home/sugiyama/inst/omni-compiler_cilk/opt/lib \
                -L/home/sugiyama/inst/argobots/opt/lib \
                -L/home/sugiyama/inst/hwloc-1.11.2/lib \
                -I/home/sugiyama/inst/argobots/opt/include \
                -lompc -labt -lhwloc -lpthread -qopenmp"
fi

$ICC test_omp.c dgemm_omp.c ../clock.c -o dgemm_serial
$ICC test_omp.c dgemm_omp.c ../clock.c -o dgemm_iomp_block  -qopenmp
$ICC test_omp.c dgemm_omp.c ../clock.c -o dgemm_iomp_cyclic -qopenmp -DSCHED_CYCLIC
$GCC test_omp.c dgemm_omp.c ../clock.c -o dgemm_gomp_block  -fopenmp
$GCC test_omp.c dgemm_omp.c ../clock.c -o dgemm_gomp_cyclic -fopenmp -DSCHED_CYCLIC
$ICC test_abt.c dgemm_abt.c ../clock.c $OMPC_MAIN $OMPC_FLAGS -o dgemm_abt_block
$ICC test_abt.c dgemm_abt.c ../clock.c $OMPC_MAIN $OMPC_FLAGS -o dgemm_abt_cyclic -DSCHED_CYCLIC

$ICC test_abt.c dgemm_abt.c ../clock.c ../ompc_serial_spawn.c -o dgemm_abt_serial_spawn \
    -I/home/sugiyama/inst/argobots/opt/include -L/home/sugiyama/inst/argobots/opt/lib \
    -labt -lpthread

$ICC test_abt.c dgemm_abt.c ../clock.c ../ompc_pthreads.c -o dgemm_abt_pthreads -lpthread -DUSE_PTHREADS
