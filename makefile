all:	gestore daddy figlio tv
gestore: gestore.o lista.o header.h common.o
	gcc -o gestore lista.o gestore.o common.o
daddy:	daddy.o header.h common.o
	gcc -o daddy daddy.o common.o
figlio: figlio.o header.h common.o rle_reader.o
	gcc -o figlio figlio.o common.o rle_reader.o
tv:	tv.o header.h common.o 
	gcc -o tv tv.o common.o -lncurses 
rle_reader: rle_reader.o header.h common.o
	gcc -o rle_reader rle_reader.o common.o
daddy.o: daddy.c
	gcc -c daddy.c
gestore.o: gestore.c
	gcc -c gestore.c
lista.o: lista.c
	gcc -c lista.c
test.o: test.c 
	gcc -c test.c
figlio.o: figlio.c
	gcc -c figlio.c
common.o: common.c
	gcc -c common.c
tv.o: tv.c
	gcc -c tv.c -lncurses
rle_reader.o: rle_reader.c
	gcc -c rle_reader.c
clean:
	rm *.o; 
