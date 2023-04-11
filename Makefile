# makefile for Virtual Memeory Unit (MMU)
#
# usage: make mmu 

CC=gcc
CFLAGS=-Wall

clean:
	rm -rf *.o
	rm -rf mmu
	rm -rf output256.csv
	rm -rf output128.csv
	
mmu: 
	$(CC) $(CFLAGS) -o mmu mmu.c