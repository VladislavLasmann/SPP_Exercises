## This job script run all jobScripts on the cluster
## with N ∈ {10,100,1000,10000,100000,1000000}

## first compile the programm
make

# N = 10
sbatch jobScript.sh 10

# N = 100
sbatch jobScript.sh 100

# N = 1000
sbatch jobScript.sh 1000

# N = 10000
sbatch jobScript.sh 10000

# N = 100000
sbatch jobScript.sh 100000

# N = 1000000
sbatch jobScript.sh 1000000

# N = 10000000
sbatch jobScript.sh 10000000
