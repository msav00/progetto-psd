DEBUG_FLAGS := -g3
COMPILE_FLAGS := -Wall

SOURCES := menu.c report.c main.c
OBJS := $(SOURCES:.c=.o)

main: $(OBJS)
	gcc $(COMPILE_FLAGS) $^ -o $@

.PHONY: debug
debug: COMPILE_FLAGS := $(DEBUG_FLAGS)
debug: clean main

#Build objects using pattern
%.o: %.c
	gcc -c $(COMPILE_FLAGS) $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJS)