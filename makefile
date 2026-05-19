CC := gcc

SRC_DIR := src
OBJ_DIR := objs
INC_DIR := include

DEBUG_FLAGS := -g3
COMPILE_FLAGS := -Wall -I $(INC_DIR)

SOURCES := $(wildcard src/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES)) #$(SOURCES:.c=.o)

main: $(OBJS)
	$(CC) $(COMPILE_FLAGS) $^ -o $@

.PHONY: debug
debug: COMPILE_FLAGS += $(DEBUG_FLAGS)
debug: clean main

#Build objects using pattern
objs/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) -c $(COMPILE_FLAGS) $< -o $@

./objs:
	mkdir -p $(OBJ_DIR)

.PHONY: clean
clean:
	rm -f $(OBJ_DIR)/*.o