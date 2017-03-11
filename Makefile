CC = gcc

binarysearcher: binarysearcher.o
	$(CC) -o binarysearcher binarysearcher.o

binarysearcher.o: binarysearcher.c
	$(CC) -c binarysearcher.c
clean:
	rm -f binarysearcher *.o