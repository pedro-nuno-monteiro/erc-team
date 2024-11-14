call: call_circular_queue_dynamic.o circular_queue_dynamic.o
	gcc -o call call_circular_queue_dynamic.o circular_queue_dynamic.o

call_circular_queue_dynamic.o: call_circular_queue_dynamic.c circular_queue_dynamic.h
	gcc -c call_circular_queue_dynamic.c

circular_queue_dynamic.o: circular_queue_dynamic.c circular_queue_dynamic.h
	gcc -c circular_queue_dynamic.c

clear:
	del *.o call.exe
