#!/bin/bash
#SBATCH -J mat-multiply
#SBATCH -e log.err.%j
#SBATCH -o log.out.%j
#SBATCH -n 16                  # Number of procs
#SBATCH -c 1				   # 1 thread per process
#SBATCH --mem-per-cpu=1750     # Main memory in MByte for each cpu core
#SBATCH --nodes=1
#SBATCH -t 00:10:00            # Hours and minutes, or '#SBATCH -t 10' - only minutes
#SBATCH --exclusive
#SBATCH --account=kurs00015
#SBATCH --partition=kurs00015
#SBATCH --reservation=kurs00015
# -------------------------------
# Afterwards you write your own commands, e.g.

echo "Start timestamp: `date`"

nranks=16
log_file="log"
def_iters=10
warmup_iters=1
base_input=256
inp_iters=6
inp_line="POINTS"

# Timing should be initialized each time before the iterations start
rm -f .cpu_freq

rm -f input.res

j=1
while [ $j -le $inp_iters ]
do
	input=`echo "${base_input} * ${j}" | bc -l`
	inp_line="${inp_line} $input"
	
	echo "Starting input ${input} -- `date`"
	
	i=1
	iters=$warmup_iters             # Warmup iters
	while [ $i -le $iters ]
	do
		srun -n $nranks --cpu-freq=HighM1-HighM1 ./task1 $input > /dev/null

		i=$(( i + 1 ))
	done

	data_line="DATA"
	i=1
	iters=$def_iters                # Normal iters
	while [ $i -le $iters ]
	do
		srun -n $nranks --cpu-freq=HighM1-HighM1 ./task1 $input > log_${j}_${i}
		exect=`cat "log_${j}_${i}" | grep 'Elapsed' | awk '{print $4}'`
		data_line="${data_line} $exect"
		
		i=$(( i + 1 ))
	done
	echo ${data_line} >> input.tmp
	
	rm -f log_*
	j=$(( j + 1 ))
	
	echo "Finished input ${input} -- `date`"
done

# Create the Extra-P input file
echo "X n" > input.res
echo "$inp_line" >> input.res
echo "REGION reg" >> input.res
echo "METRIC exect" >> input.res
cat input.tmp >> input.res
rm -f input.tmp

echo "End timestamp: `date`"
