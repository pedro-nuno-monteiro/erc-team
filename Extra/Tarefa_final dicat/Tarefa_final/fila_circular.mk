# Para  criar  testa_fila_circular screver na linha de comando
# make -f fila_circular.mk

testa_fila_circular: fila_circular.o  testa_fila_circular.o
	gcc -o testa_fila_circular  testa_fila_circular.o  fila_circular.o

testa_fila_circular.o: testa_fila_circular.c fila_circular.h
	gcc -c testa_fila_circular.c

fila_circular.o: fila_circular.c fila_circular.h
	gcc -c fila_circular.c

clean: 
	rm *.o fila_circular
