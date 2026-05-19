CC := gcc

DEBUG_FLAGS := -g3
COMPILE_FLAGS := -Wall -I include

SOURCES := $(wildcard src/*.c)
OBJS := $(patsubst ./src/%.c,./objs/%.o,$(SOURCES)) #$(SOURCES:.c=.o)

main: $(OBJS)
	$(CC) $(COMPILE_FLAGS) $^ -o $@

.PHONY: debug
debug: COMPILE_FLAGS += $(DEBUG_FLAGS)
debug: clean main

#Build objects using pattern
objs/%.o: %.c | ./objs
	$(CC) -c $(COMPILE_FLAGS) $< -o $@

./objs:
	mkdir -p ./objs

.PHONY: clean
clean:
	rm -f ./objs/*.o