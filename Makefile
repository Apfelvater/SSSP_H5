CFLAGS=-pthread -Wall -std=c11 -g


dock_procs:
	gcc $(CFLAGS) -o dock.o dock_process-functions.c
	
clean:
	rm -f dock.o
