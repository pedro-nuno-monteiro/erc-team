fila1s: fila1s.o lcgrand.o fila_circular.o
	gcc -o fila1s fila1s.o lcgrand.o fila_circular.o -lm

fila1s.o: fila1s.c lcgrand.h fila_circular.h
	gcc -c fila1s.c     

lcgrand.o: lcgrand.c lcgrand.h
	gcc -c lcgrand.c

fila_circular.o: fila_circular.c fila_circular.h
	gcc -c fila_circular.c

clean: 
	rm *.o fila1s
	
