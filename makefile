DEBUG_FLAGS := -g3
COMPILE_FLAGS := -Wall

SOURCES := main.c report.c hash.c menu.c menu_impl.c
OBJS := $(patsubst %.c,./objs/%.o,$(SOURCES)) #$(SOURCES:.c=.o)

main: $(OBJS)
	gcc $(COMPILE_FLAGS) $^ -o $@

.PHONY: debug
debug: COMPILE_FLAGS := $(DEBUG_FLAGS)
debug: clean main

#Build objects using pattern
objs/%.o: %.c | ./objs
	gcc -c $(COMPILE_FLAGS) $< -o $@

./objs:
	mkdir -p ./objs

.PHONY: clean
clean:
	rm -f ./objs/*.o