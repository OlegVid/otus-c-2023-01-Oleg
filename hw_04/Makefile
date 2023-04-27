TARGET = hw_04
CC = gcc
CFLAGS = -DCURL_STATICLIB -lcurl -Wall -Wextra -Wpedantic -std=c11 -O2 
LDFLAGS = -L/usr/local/ssl/lib
LIBS = -lcurl 

CFLAGS += $(shell pkg-config --cflags json-c)
LDFLAGS += $(shell pkg-config --libs json-c)

FILES = main.c 

all:
	$(CC) $(CFLAGS) $(FILES) $(LDFLAGS) $(LIBS)-o $(TARGET)
clean:
	rm *.gch
