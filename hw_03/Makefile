SRCS =  $(wildcard mht_src/*.c)
TESTS = $(wildcard mht_tests/*.c)
OBJS  = $(subst mht_src/,build/,${SRCS:.c=.o})
TOBJS  = $(subst mht_tests/,build/,${TESTS:.c=.o})
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -O2
TARGET = hw_03

     all: test_suite validate $(TARGET) clean

     build/%.o: %.c build
	$(CC) $(CFLAGS) -c -o $@ $<

     build/%.o: mht_src/%.c build
	$(CC) $(CFLAGS) -c -o $@ $<

     build/%.o: mht_tests/%.c build
	$(CC) $(CFLAGS) -c -o $@ $<

     $(TARGET): ${OBJS} build/main.o
	$(CC) -o $@ ${OBJS} build/main.o

     test_suite:${TOBJS} ${OBJS} build/tests.o
	$(CC) -o $@ ${TOBJS} ${OBJS} build/tests.o
     build:
	mkdir build

     validate:
	./test_suite v

     clean:
	rm -rf ./build 
	rm test_suite
