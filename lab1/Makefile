simula_fila1s: simula_fila1s.o fila1s.o lcgrand.o
	gcc -o simula_fila1s  simula_fila1s.o fila1s.o lcgrand.o  -lm

fila1s.o: fila1s.c fila1s.h lcgrand.h 
	gcc -c fila1s.c     

lcgrand.o: lcgrand.c lcgrand.h
	gcc -c lcgrand.c

simula_fila1s.o: lcgrand.h fila1s.h simula_fila1s.c
	gcc -c simula_fila1s.c
	
clean: 
	del *.o simula_fila1s
