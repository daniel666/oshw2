setcolors: setcolors.o  hashtable.o
	gcc setcolors.o hashtable.o -o setcolors

setcolors.o: hashtable.h setcolors.c
	gcc -c setcolors.c

hashtable.o: hashtable.h hashtable.c
	gcc -c hashtable.c

testhash: testhash.c hashtable.c hashtable.h
	gcc -c testhash.c
	gcc -c hashtable.c
	gcc testhash.o hashtable.o -o testhash

