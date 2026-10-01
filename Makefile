demo: main.o liste.o
	gcc -Wall -Wextra -Werror -std=c11 -g -o demo main.o liste.o

main.o: main.c liste.h
	gcc -Wall -Wextra -Werror -std=c11 -g -c main.c

liste.o: liste.c liste.h
	gcc -Wall -Wextra -Werror -std=c11 -g -c liste.c

clean:
	rm -f main.o liste.o demo demo.exe

.PHONY: clean
