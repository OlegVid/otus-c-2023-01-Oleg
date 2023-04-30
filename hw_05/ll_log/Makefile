TARGET = littlelog.o
CC = gcc
CFLAGS =  -Wall -Wextra -Wpedantic -std=c11 -rdynamic -D_POSIX_C_SOURCE=199309L -pthread -c

all:
	gcc ${CFLAGS} littlelog.c -o littlelog.o
	ar cr libllog.a littlelog.o
	rm littlelog.o

