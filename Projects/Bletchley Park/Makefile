decrypt: main.o crypto.o clock.o schedule.o
	gcc main.o crypto.o clock.o schedule.o -lpthread -lcrypto -g -o decrypt --std=c99
    
main.o: main.c
	gcc -c main.c -o main.o
    
crypto.o: crypto.c
	gcc -c crypto.c -o crypto.o
    
clock.o: clock.c
	gcc -c clock.c -o clock.o
    
schedule.o: schedule.c
	gcc -c schedule.c -o schedule.o
   
clean:
	rm decrypt results/* 
