SRCS = main.c, srxl2.c
CC = gcc
CFLAGS = -Wall
TARGET = test.out
OBJS = $(SRCS:.c)

%.o : $.c
	$(CC) $(CFLAGS) -c $<

build :
	$(CC) $(CFLAGS) -c srxl2.c -o srxl2.o
	$(CC) $(CFLAGS) -c main.c -o main.o
	$(CC) main.o srxl2.o -o main.out

clean :
	rm -vrf *.o *.out
