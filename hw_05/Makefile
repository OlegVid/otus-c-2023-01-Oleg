TARGET = hw_05
CC = gcc
CFLAGS =  -Wall -Wextra -Wpedantic -std=c11  -rdynamic -D_POSIX_C_SOURCE=199309L -pthread 
LDFLAGS= -L./ll_log/ -lllog -static

FILES = main.c 

all:
	$(CC) $(CFLAGS) $(FILES) $(LDFLAGS) -o $(TARGET)

clean:
	rm *.gch
