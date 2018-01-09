CXX 	 = mpicxx
CC	 = mpicc
FLAGS    = -O2 -g -Wall -std=c99
LDFLAGS  = 
SRCS     = task1.c task2.c
OBJS     = $(SRCS:.c=.o)
EXEC	 = task1 task2

all: task1 task2


.c.o:
	$(CC) $(FLAGS) -c $< -o $@
	
	
$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) $(FLAGS) task1.o -o task1 -lm
	$(CC) $(LDFLAGS) $(FLAGS) task2.o -o task2 -lm

clean:
	rm -f *.o *~ $(EXEC)


# Dependencies
