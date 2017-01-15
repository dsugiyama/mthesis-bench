if [[ $1 == knl ]]; then
    membind="numactl --membind 1"
else
    membind=""
fi

echo "unit: ms"
echo ""

for impl in iomp gomp abt; do
    for t in 10 1000; do
        for n in 10 20 30 40 50 60; do
            ruby aggregate.rb 7 $membind ./eval_$impl $n $t
        done
        echo ""
    done
    echo ""
done

for impl in iomp gomp abt; do
    for t in 10 100 1000 10000; do
        ruby aggregate.rb 7 $membind ./eval_$impl 60 $t
    done
    echo ""
done
