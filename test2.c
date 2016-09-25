#include <sys/types.h>
#include <unistd.h> 
#include <stdio.h>

#define align4(x) (((((x) -1) >>2) <<2)+4)

void *base = NULL;
size_t allocated = 0;
size_t deallocated = 0;

char *my_malloc_error;

// first fit = 0, best fit = 1
int policy = 0;

/* block struct */
struct block {

	size_t size;
	struct block *next;
	struct block *prev;
	int free;
	void *ptr;
	/* A pointer to the allocated block */
	char data [1];
};
typedef struct block *t_block;


void malloc_error(int error) {

	if (error == 1) my_malloc_error = "Error: Extension of heap failed.";
	if (error == 2) my_malloc_error = "Error: Initialization failed.";
	if (error == 3) my_malloc_error = "Error: Can't free when when no more blocks.";

	puts(my_malloc_error);	
}


void split_block (t_block b, size_t s){
	
	t_block new;
	new = (t_block )(b->data + s);
	new->size = b->size - s;
	new->next = b->next;
	new->prev = b;	
	new->free = 1;
	new->ptr = new ->data;
	b->size = s;
	b->next = new;
	if (new->next)
		new->next->prev = new;
}


t_block extend_heap(t_block last, size_t s) {
	
	long sb;
	t_block b;
	b = sbrk(0);
	sb = (long) sbrk(s);
	if (sb < 0) {
		malloc_error(1);
		return (NULL);
	}	
	b->size = s;
	b->next = NULL;
	b->prev = last;
	b->ptr = b->data;
	if (last)
		last->next = b;
	b->free = 0;
	return b;
}

t_block find_block(t_block *last, size_t size){
	
	t_block b = base;
	while (b && !(b->free && b->size >= size)) {
		*last = b;
		b = b->next;
	}
	return (b);
}

t_block find_best_block(t_block *last, size_t size) {

	t_block b = base;
	t_block best = NULL;
	while ((b = b->next) != NULL) {
		if (b->free && b->size >= size) {
			if (best == NULL | best->size > b->size) best = b;
			*last = b;
		}
	}
	return best;
}

t_block fusion(t_block b) {
	
	if (b->next && b->next->free){
		b->size += b->size + b->next->size;
		b->next = b->next->next;
		if (b->next)
			b->next->prev = b;
	}
	return (b);
}

size_t free_space() {
	
	t_block b = base;
	size_t freesize = 0;
	while (b) {
		if(b->free) freesize += (int) (b->size);
		b = b->next;
	}
	return freesize;
}

void * my_malloc(size_t size){
	
	allocated += size;
	t_block b, last;
	size_t s;
	s = align4(size);
	
	// first fit		
	if (base) {
		
		/* First find a block */
		last = base;
		if (policy == 0) b = find_block(&last, s);
		else b = find_best_block(&last, s);

		if (b) {
			/* can we split */
			if ((b->size - s) >= (b->size + 4)) split_block(b, s);
			b->free = 0;
		} 
		else {
			/* No fitting block , extend the heap */
			b = extend_heap(last, s);
			if (!b) {
				malloc_error(1);			
				return(NULL);
			}		
		}
	} 
	else {
		/* first time */
		b = extend_heap(NULL, s);
		base = b;	
		if (!b) {
			malloc_error(2);
			return NULL;
		}
	}
	return b;
}

int valid_addr (void *p) {
	
	if (base) {
		if (p > base && p < sbrk(0)) {
			puts("good");
			return 1;
		}
	}
	return 0;
}

void my_free(void *p) {
	
	t_block b;
	if (1) {
		
		b = (t_block) p;
		deallocated += (int) b->size;
		b->free = 1;
		/* fusion with previous if possible */
		if (b->prev && b->prev ->free) b = fusion(b->prev );
		/* then fusion with next */
		if (b->next) fusion(b);
		else {
			/* free the end of the heap */
			if (b->prev) b->prev->next = NULL;
			else {
				/* No more block !*/
				if (b != base) {
					malloc_error(3);
					base = NULL;
				}
			}			
			brk(b);
		}
	}		
}



void my_mallopt(int p){
	
	if (p == 0) {
		puts("First-fit policy set.\n");
		policy = 0;
	} 
	else if (p == 1) {
		puts("Best-fit policy set.\n");
		policy = 1;
	}
	else {
		puts("Error: argument not valid (must be 0 or 1).\n");
	}
}

void my_mallinfo() {

	//Bytes allocated
	char str[15];
	sprintf(str, "%d", (int) allocated);
	puts("Bytes allocated:");
	puts(str);
	//Bytes deallocated
	puts("Bytes deallocated:");
	sprintf(str, "%d", (int) deallocated);
	puts(str);
	//Bytes in use
	puts("Bytes in memory:");
	sprintf(str, "%d", (int) (allocated-deallocated));
	puts(str);
	//largest free space
	puts("Ammount of free space:");
	sprintf(str, "%d", (int) free_space());
	puts(str);
}


void main() {

	// in first fit mode by default	
	puts("\nIn first-fit mode:");

	// saving 1 block of size 20
	puts("\nallocating 20 bytes...\n");
	void* a = my_malloc(20);
	my_mallinfo();
	puts("\n");

	// saving 3 more blocks of 50, 100, and 44. block size 214, largest 100. 
	puts("allocating 52 bytes...");
	puts("allocating 100 bytes...");
	puts("allocating 44 bytes...\n");
	void* b = my_malloc(52);
	void* c = my_malloc(100);
	void* d = my_malloc(44);
	
	my_mallinfo();
	puts("\n");

	// attempting to free the 50 byte block, largest now 44
	puts("removing b and c (52 and 100 bytes)...");
	my_free(b);
	my_free(c);

	my_mallinfo();
	puts("\n");
mem
	// copying a string to allocated memory and printing it out
	puts("saving hello world to d (100 bytes), and printing the data of d out...");
	char *s = "hello world\n";
	t_block blk = d;
	strcpy(blk->data, s);
	puts(blk->data);

	// allocating one more block of memory (60 bytes)
	puts("allocating 30 bytes...\n");
	void *e = my_malloc(30);
	my_mallinfo();
	puts("\n");

	// Setting to best fit-policy
	my_mallopt(1);

	// saving 1 block of size 33
	puts("\nallocating 33 bytes...\n");
	void *f = my_malloc(33);
	my_mallinfo();
	puts("\n");
	
	my_free();


	
}


