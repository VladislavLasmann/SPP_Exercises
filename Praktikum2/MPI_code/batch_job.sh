#!/bin/bash
#SBATCH -J mat-mult
#SBATCH -e log.err.%j
#SBATCH -o log.out.%j
#SBATCH -n 16                  # Number of procs
#SBATCH -c 1		       # 1 thread per process
#SBATCH --mem-per-cpu=1750     # Main memory in MByte for each cpu core
#SBATCH --nodes=1
#SBATCH -t 00:05:00            # Hours and minutes, or '#SBATCH -t 10' - only minutes
#SBATCH --exclusive
#SBATCH --account=kurs00015
#SBATCH --partition=kurs00015
#SBATCH --reservation=kurs00015

# -------------------------------
# Afterwards you write your own commands, e.g.

echo "Start: `date`"

mpirun -n 16 ./task1 2048

echo "End: `date`"
