CC=       	gcc
CFLAGS= 	-g -gdwarf-2 -std=gnu99 -Wall -lmavalloc -pedantic -ansi
CLFLAGS= 	-g -gdwarf-2 -std=gnu99 -Wall -pedantic -ansi
LDFLAGS=
LIBRARIES=      lib/libmavalloc.a

all:   unit_test benchmark1 benchmark2 benchmark3 benchmark4 benchmark5

benchmark1: benchmark1.o libmavalloc.a
	gcc -o benchmark1 benchmark1.o -L. -lmavalloc -g

benchmark2: benchmark2.o libmavalloc.a
	gcc -o benchmark2 benchmark2.o -L. -lmavalloc -g

benchmark3: benchmark3.o libmavalloc.a
	gcc -o benchmark3 benchmark3.o -L. -lmavalloc -g

benchmark4: benchmark4.o libmavalloc.a
	gcc -o benchmark4 benchmark4.o -L. -lmavalloc -g

benchmark5: benchmark5.o libmavalloc.a
	gcc -o benchmark5 benchmark5.o -L. -lmavalloc -g

unit_test: main.o libmavalloc.a
	gcc -o unit_test main.o -L. -lmavalloc -g

main.o: main.c
	gcc  -c  -Wall -Wno-self-assign -Wno-nonnull main.c -g 

benchmark1.o: benchmark1.c
	gcc  -c -Wall benchmark1.c -g

benchmark2.o: benchmark2.c
	gcc  -c -Wall benchmark2.c -g

benchmark3.o: benchmark3.c
	gcc  -c -Wall benchmark3.c -g

benchmark4.o: benchmark4.c
	gcc  -c -Wall benchmark4.c -g

benchmark5.o: benchmark5.c
	gcc  -c -Wall benchmark5.c -g

mavalloc.o: mavalloc.c
	gcc  -c  -Wall mavalloc.c -g

libmavalloc.a: mavalloc.o
	ar rcs libmavalloc.a mavalloc.o

clean:
	rm -f *.o *.a unit_test main benchmark1 benchmark2 benchmark3 benchmark4 benchmark5

.PHONY: all clean
