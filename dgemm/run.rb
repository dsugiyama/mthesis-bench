require 'optparse'

L = M = N = 4096
workloads_to_run = []
num_threads = []
num_threads_inner = 32
niter = "1"
membind = ""
iomp_proc_bind = "OMP_PROC_BIND=true"
max_es = 48

parser = OptionParser.new
parser.on('-m', '--matsize=L,M,N', 'Matrix size (C[L][N] = A[L][M] * B[M][N])') do |v|
    L, M, N = v.split(',')
end
parser.on('-w', '--workloads=LIST', 'Workloads to run (comma-separated list)') do |v|
    workloads_to_run = v.split(',')
end
parser.on('-n', '--num-threads=LIST', 'Number of threads (comma-separated list)') do |v|
    num_threads = v.split(',').map(&:to_i)
end
parser.on('--num-threads-inner', 'Degree of inner level parallelism') do |v|
  num_threads_inner = v.to_i
end

parser.on('-i', '--num-iterations=NUM', 'Number of iterations') do |v|
    niter = v
end
parser.on('--knl', 'Whether executed on KNL') do
    membind = "numactl --membind 1"
    max_es = 256
end
parser.on_tail('-h', '--help', 'Show this message') do
    puts parser
    exit
end
parser.parse!(ARGV)

puts 'unit: sec'
puts "number of threads: #{num_threads.join(' ')}"
puts

def run(command, num_iterations, sleep)
  sleep_cmd = sleep ? "sleep 3;" : ""
  iter_cmd = "for i in $(seq #{num_iterations}); do #{command}; #{sleep_cmd} done"
  settings = "export LD_LIBRARY_PATH=$HOME/inst/argobots/opt/lib:$LD_LIBRARY_PATH;"
  result = `#{settings} #{iter_cmd}`
    .scan(/([0-9\.]+) sec/)
    .flatten
    .map(&:to_f)
    .min
  puts result
end

workloads = {
  'serial' => lambda do
    run "#{membind} ./dgemm_serial #{L} #{M} #{N}", niter, false
  end,

  'iomp-cyclic' => lambda do
    num_threads.each do |n|
      run "OMP_NUM_THREADS=#{n} #{iomp_proc_bind} #{membind} ./dgemm_iomp_cyclic #{L} #{M} #{N}", niter, (n >= 64)
    end
  end,

  'iomp-block' => lambda do
    num_threads.each do |n|
      run "OMP_NUM_THREADS=#{n} #{iomp_proc_bind} #{membind} ./dgemm_iomp_block  #{L} #{M} #{N}", niter, (n >= 64)
    end
  end,

  'gomp-cyclic' => lambda do
    num_threads.each do |n|
      run "OMP_NUM_THREADS=#{n} OMP_PROC_BIND=true #{membind} ./dgemm_gomp_cyclic #{L} #{M} #{N}", niter, (n >= 64)
    end
  end,

  'gomp-block' => lambda do
    num_threads.each do |n|
      run "OMP_NUM_THREADS=#{n} OMP_PROC_BIND=true #{membind} ./dgemm_gomp_block  #{L} #{M} #{N}", niter, (n >= 64)
    end
  end,

  'abt-cyclic' => lambda do
    num_threads.each do |n|
      run "OMPC_NUM_PROCS=#{n} #{membind} ./dgemm_abt_cyclic #{L} #{M} #{N}", niter, (n >= 64)
    end
  end,

  'abt-block' => lambda do
    num_threads.each do |n|
      run "OMPC_NUM_PROCS=#{n} #{membind} ./dgemm_abt_block  #{L} #{M} #{N}", niter, (n >= 64)
    end
  end,

  'abt-serial-spawn' => lambda do
    num_threads.each do |n|
      run "LD_LIBRARY_PATH=/home/sugiyama/inst/argobots_master/opt/lib:$LD_LIBRARY_PATH " +
          "OMPC_NUM_PROCS=#{n} #{membind} ./dgemm_abt_serial_spawn #{L} #{M} #{N}", niter, (n >= 64)
    end
  end,

  'iomp-lib' => lambda do
    num_threads.each do |n|
      run "OMP_NUM_THREADS=#{n},#{num_threads_inner} OMP_NESTED=true #{iomp_proc_bind} #{membind} ./dgemm_lib_iomp #{L} #{M} #{N}", niter, (n >= 64)
    end
  end,

  'iomp-lib-nonest' => lambda do
    num_threads.each do |n|
      run "OMP_NUM_THREADS=#{n} #{iomp_proc_bind} #{membind} ./dgemm_lib_iomp_serial #{L} #{M} #{N}", niter, (n >= 64)
    end
  end,

  'gomp-lib' => lambda do
    num_threads.each do |n|
      run "OMP_NUM_THREADS=#{n},#{num_threads_inner} OMP_NESTED=true OMP_PROC_BIND=true #{membind} ./dgemm_lib_gomp #{L} #{M} #{N}", niter, (n >= 64)
    end
  end,

  'gomp-lib-nonest' => lambda do
    num_threads.each do |n|
      run "OMP_NUM_THREADS=#{n} OMP_PROC_BIND=true #{membind} ./dgemm_lib_gomp_serial #{L} #{M} #{N}", niter, (n >= 64)
    end
  end,

  'abt-lib' => lambda do
    num_threads.each do |n|
      run "OMPC_NUM_PROCS=#{max_es} NULTS_OUTER=#{n} NULTS_INNER=#{num_threads_inner} #{membind} ./dgemm_lib_abt #{L} #{M} #{N}", niter, (n >= 64)
    end
  end,
}

if workloads_to_run.include?("all")
  workloads.each do |name, workload|
    puts name
    workload.call
    puts
  end
else
  workloads_to_run.each do |name|
    puts name
    workloads[name].call
    puts
  end
end
