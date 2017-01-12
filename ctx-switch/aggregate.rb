niter = ARGV[0]
cmd = ARGV[1..-1].join(' ')

iter_cmd = "for i in $(seq #{niter}); do #{cmd}; done"
result = `#{iter_cmd}`.scan(/[0-9\.]+ sec/).map(&:to_f).min
puts "#{result * 1000} ms"
