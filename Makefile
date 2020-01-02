CFLAGS = -g -Wall -Werror -std=c99

.PHONY: all clean

SRC    := $(wildcard *.c)
TARGET := $(basename $(SRC))

all: $(TARGET)

clean:
	rm -f $(TARGET)
