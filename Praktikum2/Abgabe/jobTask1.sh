#!/bin/bash
#SBATCH -J Task1
#SBATCH --mail-type=END
#SBATCH -e /home/kurse/kurs00015/vt63popi/Praktikum2/Abgabe/logTask1.err.%j
#SBATCH -o /home/kurse/kurs00015/vt63popi/Praktikum2/Abgabe/logTask1.out.%j
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
cd /home/kurse/kurs00015/vt63popi/Praktikum2/Abgabe
module load openmpi/gcc
make

echo "Start: `date`"

mpirun -n 16 ./task1 2048

echo "End: `date`"