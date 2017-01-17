if [[ $1 == knl ]]; then
    ICC="icc -std=gnu11 -O3 -xMIC-AVX512"
    GCC="gcc -std=gnu11 -O3 -mavx512f -mavx512cd -mavx512er -mavx512pf"
    OMPC_MAIN="/home/sugiyama/inst/omni-compiler_cilk/opt/lib/ompc_main.o"
    OMPC_FLAGS="-L/home/sugiyama/inst/omni-compiler_cilk/opt/lib \
                -L/home/sugiyama/inst/argobots/opt/lib \
                -L/home/sugiyama/inst/hwloc-1.11.4/lib \
                -I/home/sugiyama/inst/argobots/opt/include \
                -lompc -labt -lhwloc -lpthread -qopenmp"
else
    ICC="icc -std=gnu11 -O3 -xHOST"
    GCC="gcc -std=gnu11 -O3 -march=native"
    OMPC_MAIN="/home/sugiyama/inst/omni-compiler_cilk/opt/lib/ompc_main.o"
    OMPC_FLAGS="-L/home/sugiyama/inst/omni-compiler_cilk/opt/lib \
                -L/home/sugiyama/inst/argobots/opt/lib \
                -L/home/sugiyama/inst/hwloc-1.11.2/lib \
                -I/home/sugiyama/inst/argobots/opt/include \
                -lompc -labt -lhwloc -lpthread -qopenmp"
fi

$ICC -c daxpy_omp.c -o daxpy_iomp_serial.o
$ICC -c daxpy_omp.c -o daxpy_iomp.o -qopenmp
$GCC -c daxpy_omp.c -o daxpy_gomp_serial.o
$GCC -c daxpy_omp.c -o daxpy_gomp.o -fopenmp
$ICC -c daxpy_abt.c -o daxpy_abt.o

$ICC -ipo  test_omp.c dgemm_omp.c daxpy_iomp_serial.o ../clock.c -o dgemm_lib_iomp_serial -qopenmp -DLIB_DAXPY
$ICC -ipo  test_omp.c dgemm_omp.c daxpy_iomp.o        ../clock.c -o dgemm_lib_iomp        -qopenmp -DLIB_DAXPY
$GCC -flto test_omp.c dgemm_omp.c daxpy_gomp_serial.o ../clock.c -o dgemm_lib_gomp_serial -fopenmp -DLIB_DAXPY
$GCC -flto test_omp.c dgemm_omp.c daxpy_gomp.o        ../clock.c -o dgemm_lib_gomp        -fopenmp -DLIB_DAXPY
$ICC -ipo  test_abt.c dgemm_abt.c daxpy_abt.o         ../clock.c $OMPC_MAIN $OMPC_FLAGS -o dgemm_lib_abt -DLIB_DAXPY
