myfile:myfile.o
	gcc -o myfile myfile.o
myfile.o:myfile.s
	gcc -o myfile.o -c myfile.s
myfile.s:myfile.i 
	gcc -o myfile.s -S myfile.i
myfile.i:myfile.c
	gcc -o myfile.i -E myfile.c

.PHONY:clean
clean:
	rm -f myfile myfile.i myfile.s myfile.o 
