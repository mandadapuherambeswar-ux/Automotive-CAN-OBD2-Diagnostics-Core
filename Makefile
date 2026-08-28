CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude
SRC = $(wildcard src/*.c)
TARGET = obd_diag_core

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
