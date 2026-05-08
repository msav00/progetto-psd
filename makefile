main: main.o report.o
	gcc -Wall -W main.o report.o -o main

debug: main.o report.o
	gcc -Wall -W -g3 main.o report.o -o main

main.o: main.c
	gcc -g3 -c main.c

report.o: report.c
	gcc -g3 -c report.c

clean:
	rm main.o report.o