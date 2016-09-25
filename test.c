// Josh Liu ID: 260612384

#include <stdio.h>
#include "memory.h"
#include <stdlib.h>

// Test file
int main() {

	// in first fit mode by default	
	puts("\nIn first-fit mode:");
 
	// saving 1 block of size 20000
	puts("\nallocating 20000 bytes...\n");
	void* a = my_malloc(20000);
	my_mallinfo();
	puts("\n");

	printf("20000, 5120, 5120\n\n");

	// saving 3 more blocks of 10000, 1000, and 44444
	puts("allocating 10000 bytes...");
	void* b = my_malloc(10000);
	puts("allocating 1000 bytes...");
	void* c = my_malloc(1000);
	puts("allocating 44444 bytes...");
	void* d = my_malloc(44444);
	
	puts("\n");
	my_mallinfo();
	puts("\n");

	printf("75444, 14350, 5120\n\n");

	// attempting to free the 10000 byte block and the 1000 byte block
	puts("Removing b and c (10000 and 1000 bytes)...");
	my_free(b);
	my_free(c);

	my_mallinfo();
	puts("\n");

	printf("46194, 25380, 20260\n\n");

	// copying a string to allocated memory and printing it out
	puts("Saving hello world to the 44444 byte memory, and printing the data from the pointer...");
	char *s = "hello world\n";
	printf("%s\n", s);

	printf("(Should say hello world)\n\n");

	// allocating one more block of memory (3000 bytes)
	puts("allocating 3000 bytes...\n");
	void *e = my_malloc(3000);
	my_mallinfo();
	puts("\n");

	printf("49194, 22370, 17250 (The +10 is from the middle allocation)\n\n");

	// free all
	printf("freeing everything...\n\n");
	my_free(a);
	my_free(d);
	my_free(e);

	my_mallinfo();
	puts("\n");

	printf("0, 89854, 89854\n\n");		

	// Setting to best fit-policy
	printf("Testing if the best-fit works...\n");
	my_mallopt(0);
	puts("\n");

	// filling up the free space
	puts("allocating 50000 bytes...\n");
	a = my_malloc(50000);
	puts("allocating 50000 bytes...\n");
	b = my_malloc(50000);
	my_mallinfo();
	puts("\n");

	printf("101000, 44964, 39844\n\n");

	printf("There should now be a large free space in the middle and a small free space in the end\n");
	printf("If we allocate 1000 bytes, then it should go at the end and the largest Free space should not change.\n\n");

	puts("allocating 1000 bytes...\n");
	c = my_malloc(1000);
	my_mallinfo();
	puts("\n");

	printf("101000, 43954, 39844\n\n");
	printf("The free bytes went down by 1000, but the largest free space did not. This is the desired behavior\n\n");


	// freeing everything
	printf("freeing everything...\n\n");
	my_free(a);
	my_free(b);
	my_free(c);

	my_mallinfo();
	puts("\n");

	// end of test
	return 0;
	
}
