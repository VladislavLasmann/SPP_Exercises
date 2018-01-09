cmake_minimum_required(VERSION 3.9)
project(Praktikum2)

set(CMAKE_CXX_STANDARD 11)

add_executable(Praktikum2
	 kostja/MPI_code/calibrate.h
	 kostja/MPI_code/task1.c
	 kostja/MPI_code/task2.c
	 kostja/MPI_code/timing.h
	 MPI_code/calibrate.h
	 MPI_code/task1.c
	 MPI_code/task2.c
	 MPI_code/timing.h
	 Praktikum_MPI.pdf kostja/MPI_code/test1.c)
