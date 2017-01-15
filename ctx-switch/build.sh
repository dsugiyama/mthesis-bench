if [[ $1 == knl ]]; then
    ICC="icc -std=gnu11 -O3 -ipo  -xMIC-AVX512"
    GCC="gcc -std=gnu11 -O3 -flto -mavx512f -mavx512cd -mavx512er -mavx512pf"
    OMPC_FLAGS="/home/sugiyama/inst/omni-compiler_cilk/opt/lib/ompc_main.o \
                -L/home/sugiyama/inst/omni-compiler_cilk/opt/lib \
                -L/home/sugiyama/inst/argobots/opt/lib \
                -L/home/sugiyama/inst/hwloc-1.11.4/lib \
                -lompc -labt -lhwloc -lpthread -qopenmp"
else
    ICC="icc -std=gnu11 -O3 -ipo  -xHOST"
    GCC="gcc -std=gnu11 -O3 -flto -march=native"
    OMPC_FLAGS="/home/sugiyama/inst/omni-compiler_cilk/opt/lib/ompc_main.o \
                -L/home/sugiyama/inst/omni-compiler_cilk/opt/lib \
                -L/home/sugiyama/inst/argobots/opt/lib \
                -L/home/sugiyama/inst/hwloc-1.11.2/lib \
                -lompc -labt -lhwloc -lpthread -qopenmp"
fi

$ICC eval_omp.c ../delay.c ../clock.c -o eval_iomp -qopenmp
$GCC eval_omp.c ../delay.c ../clock.c -o eval_gomp -fopenmp
$ICC eval_abt.c ../delay.c ../clock.c $OMPC_FLAGS -o eval_abt
