CC := gcc

SRC_DIR := src
OBJ_DIR := objs
INC_DIR := include

TST_DIR := tests
TST_EXE := test_suite

DEBUG_FLAGS := -g3
COMPILE_FLAGS := -Wall -I $(INC_DIR)

SOURCES := $(wildcard src/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES)) #$(SOURCES:.c=.o)

main: $(OBJS)
	$(CC) $(COMPILE_FLAGS) $^ -o $@

.PHONY: debug
debug: COMPILE_FLAGS += $(DEBUG_FLAGS)
debug: clean main

#compile test suite and everything but the main program.
tests: $(TST_DIR)/test_suite.c $(filter-out $(OBJ_DIR)/main.o ,$(OBJS))
	$(CC) $(COMPILE_FLAGS) $^ -o $(TST_EXE)

#Build objects using pattern
objs/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) -c $(COMPILE_FLAGS) $(DEBUG_FLAGS) $< -o $@

./objs:
	mkdir -p $(OBJ_DIR)

.PHONY: clean
clean:
	rm -f $(OBJ_DIR)/*.o