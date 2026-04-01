CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -Iinclude
TARGET = groupX_manager
SRC = src/main.c src/group29_manager.c
BONUS_TARGET = group29_bonus
BONUS_SRC = group29_bonus.c
ADDRESSES_FILE ?= addresses.txt
BACKING_STORE_FILE ?= BACKING_STORE.bin

all: $(TARGET)

bonus: $(BONUS_TARGET)

bonus-run: $(BONUS_TARGET)
	./$(BONUS_TARGET) $(BACKING_STORE_FILE) < $(ADDRESSES_FILE)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

$(BONUS_TARGET): $(BONUS_SRC)
	$(CC) -std=c11 -Wall -Wextra -pedantic $(BONUS_SRC) -o $(BONUS_TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(TARGET).exe $(BONUS_TARGET) $(BONUS_TARGET).exe

.PHONY: all bonus bonus-run run clean
