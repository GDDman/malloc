// Josh Liu ID: 260612384
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include "memory.h"

int globalpolicy = 1;
char* startheap;
char* endheap;

int first = 1;
int totalFree = 0;
int allocated = 0;
int totalData = 0;
	
char *my_malloc_error = "error: could not allocate memory";

int getTotalBytes() { return (totalData + allocated + totalFree); }

int sizeOfBlock(void* p) { return *((int*)(p + 1)); }

char getAddress(void* p) { return *((char*) p); }

int sizeOfHeap() { return ((int)(endheap-startheap)); }

// set the characterics of the blocks (pointer, size, free)
void updateBlock(void* p, int size, char c) {
	
	void *pointer = p;
	*((char*)pointer) = c;
	pointer += 1;
	*((int*)pointer) = size;
	pointer += (size + 4);
	*((int*)pointer) = size;
	pointer += 4;
	*((char*)pointer) = c;
}

// get the block after
char getNextBlock(void* p) {

	void *nextBlock = p;
	nextBlock = p + sizeOfBlock(p) + 10;
	if (nextBlock == endheap) return -1;
	return *((char*)(nextBlock));
}

// get the preceding block
char getPrevBlock(void* p) {

	if (p == startheap) return -1;
	return *((char*)(p - 1));
}

void *prevBlock(void* p) {

	void *prevBlock = p;
	if (p == startheap) return NULL;
	int size = *((int*)(p - 5));
	prevBlock = p - size - 10;
	return prevBlock;
}

void *nextBlock(void* p) {
	
	void *nextBlock = p;
	nextBlock = p + sizeOfBlock(p) + 10;
	if (nextBlock == endheap || p == endheap) return NULL;
	return nextBlock;
}

// fuse with previous and next block
void fuseBothWays(void* p) {

	int prevsize = sizeOfBlock(prevBlock(p));
	int size = sizeOfBlock(p);
	int nextsize = sizeOfBlock(nextBlock(p));
	
	updateBlock(prevBlock(p), prevsize + size + nextsize + 20, getAddress(nextBlock(p)));
	totalFree += (size + 20);
	totalData -= 20;
	allocated -= size;
}

// fuse only with next
void fuseNext(void* p) {

	int size = sizeOfBlock(p);
	int nextsize = sizeOfBlock(nextBlock(p));
	updateBlock(p, size + nextsize + 10, getAddress(nextBlock(p)));
	totalFree += (size + 10);
	totalData -= 10;
	allocated -= nextsize;
}

void fusePrev(void* p) {

	int prevsize = sizeOfBlock(prevBlock(p));
	int size = sizeOfBlock(p);
	updateBlock(prevBlock(p), prevsize + size + 10, getAddress(prevBlock(p)));
	totalFree += (size + 10); 
	totalData -= 10;
	allocated -= prevsize;
}

// extends heap and places 5120 free buffer
void* extendHeap(int size) {

	void *prevh = sbrk(size + 5140);
	
	if(prevh == -1) {
		printf("%s\n", my_malloc_error);
		return -1;
	}

	endheap += (size + 5140);
	printf("Extending heap... before: %d   after: %d\n", (int)((int*)prevh), (int)endheap);
	updateBlock(prevh ,size, 1);
	updateBlock(prevh + size + 10, 5120, 0);
	totalData += 20;
	allocated += size;
	totalFree += 5120;
	return prevh;
}

// finds first free working block
void* findFirstBlock(int size, void* p) {
	
	char* pointer = startheap;	
	if (p != NULL) pointer = p;

	while (pointer <= endheap) {
		
		if (getAddress(pointer) == 0) {
			// free
			if (sizeOfBlock(pointer) + 10 >= size + 30) {

				// splitting
				int newSize = sizeOfBlock(pointer) - size;		
				updateBlock(pointer, size, 1);
				void *newBlock = nextBlock(pointer);
				updateBlock(newBlock, newSize - 10, 0);
				
				totalData += 10;
				totalFree -= (size + 10);
				allocated += size;
				return pointer;
			}
			else if (sizeOfBlock(pointer) >= size){

				updateBlock(pointer, sizeOfBlock(pointer), 0);
				totalFree -= sizeOfBlock(pointer);
				allocated += sizeOfBlock(pointer);
				return pointer;
			}
		}
		pointer = nextBlock(pointer);
		if (pointer == NULL) break;
	}
	return extendHeap(size);	
}

// finds smallest free block that works
void* findBestFit(int size) {
	
	void *block = NULL;

	void *pointer = startheap;
	void *best = NULL;
	int bsize = 0;

	while (pointer <= endheap) {

		if (getAddress(pointer) == 0) {

			if (sizeOfBlock(pointer) >= size || sizeOfBlock(pointer) + 10 >= size + 30){

				if (bsize == 0 | sizeOfBlock(pointer) < bsize) {
					bsize = sizeOfBlock(pointer);
					best = pointer;
				}
			}
		}
		pointer = nextBlock(pointer);
		if (pointer == NULL) break;
	}
	if (best == NULL) {
		block = extendHeap(size);
	}
	else {
		block = findFirstBlock(size, best);
	}
	return block;
}

// functions for getting only the data of the block 
void* crop(void* block) {
	return (void*)((int)block + sizeof(char) + sizeof(int));
}

void* pad(void* block) {
	return (void*)((int)block - sizeof(int) - sizeof(char));
}

void *my_malloc(int size) {
	
	void* block = NULL;
	
	if (first) {
		printf("starting heap...\n");
		startheap = (char*)sbrk(size + 5140);
		if (startheap == -1) {
			printf("%s\n", my_malloc_error);
			return -1;
		}
		endheap = startheap + size + 5140;
		first = 0;
		updateBlock(startheap, size, 1);
		updateBlock(startheap + size + 10, 5120, 0);
		totalData += 20;
		allocated += size;
		totalFree += 5120;
		return crop((void*)startheap);
	}
	if (globalpolicy == 1){
		block = findFirstBlock(size, NULL);
	}
	else {
		block = findBestFit(size);
	}
	if (block == NULL) printf("Error.\n");
	return crop(block);
}

void my_free(void *p) {

	p = pad(p);
	if (p == NULL) return;

	if((getNextBlock(p) != -1) && (getPrevBlock(p) != -1) && !getNextBlock(p) && !getPrevBlock(p)) {
		fuseBothWays(p);
	}
	else if ((getNextBlock(p) != -1) && !getNextBlock(p)) {
		fuseNext(p);
	}
	else if ((getPrevBlock(p) != -1) && !getPrevBlock(p)) {
		fusePrev(p);
	} 
	else {
		updateBlock(p, sizeOfBlock(p), 0);
		totalFree += sizeOfBlock(p); 
		allocated -= sizeOfBlock(p);
	}

	char topBlock = *(endheap-sizeof(char));
	int sizeTopBlock = *((int*)(endheap - sizeof(char) - sizeof(int)));

	// checking if it's less than 128 kb
	if(!topBlock && sizeTopBlock >= 131072){
		int over = sizeTopBlock - 131072;
		if(over < 0) over = 0;
		void *prevh = sbrk(-(20480 + over));
	}
}

// 0 for best-fit, 1 for first-fit
void my_mallopt(int p) {
	if (p != 0 && p != 1) {
		printf("Must be 0 (best) or 1 (first).\n");
		return;
	}
	globalpolicy = p;
	printf("Global policy set to %d.\n", p);
}

int getFreeStreak() {

	void *pointer = startheap;
	int best = 0;
	while (pointer <= endheap) {
		if(!getAddress(pointer)) {
			if(sizeOfBlock(pointer) > best) best = sizeOfBlock(pointer);
		}
		pointer = nextBlock(pointer);
		if (pointer == NULL) break;
	}
	if (best == 0) printf("No free space\n");
	return best;
}

void my_mallinfo() {

	if (allocated < 0) allocated = 0;
	printf("Bytes Allocated: %d\nBytes Free: %d\nLargest Free Space: %d\n", allocated, totalFree, getFreeStreak());
}

