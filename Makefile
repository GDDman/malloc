#Josh Liu ID: 260612384
# gcc
CC=gcc 

CFLAGS= -W -Wformat

OBJ=memory.o test.o

memory: $(OBJ) 					
	$(CC) -o $@ $(OBJ) $(CFLAGS)

memory.o: memory.c memory.h
	$(CC) -c memory.c $(CFLAGS)

test.o: test.c memory.h
	$(CC) -c test.c $(CFLAGS)

clean:
	rm *.o memory
