all: build1 run library build2 build3 build4
build1: a1
	gcc -Wall -ansi -c -g linkedList.c parser.c a1.c -Include
	gcc -o a1 a1.o linkedList.o parser.o

run: a1
	./a1 post.cc&

library:
	gcc -Wall -ansi -c -g stream.c -o stream.o
	ar cr libstream.a stream.o

build2:
	gcc -Wall -ansi -c -g addauthor.c
	gcc -o addauthor addauthor.o -L. -lstream 


build3:
	gcc -Wall -ansi -c -g post.c
	gcc -o post post.o -L. -lstream

build4:
	gcc -Wall -ansi -c -g parserphp.c main.c -Include
	gcc -o generate_html parserphp.o main.o

clean:
	rm *.o a1 addauthor post post.c *.a

.PHONY: all run a1 build1 library build2 build3 clean
