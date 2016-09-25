// Josh Liu ID: 260612384
// Headers for methods

void *my_malloc(int size); 
void my_free(void *p);
void my_mallopt(int p);
void my_mallinfo();

int getTotalBytes();
int sizeOfBlock(void* p);
char getAddress(void* p);
int sizeOfHeap();
void updateBlock(void* p, int size, char c);
char getNextBlock(void* p);
char getPrevBlock(void* p);
void *prevBlock(void* p);
void *nextBlock(void* p);
void fuseBothWays(void* p);
void fuseNext(void* p);
void fusePrev(void* p);
void* extendHeap(int size);
void* findFirstBlock(int size, void* p);
void* findBestFit(int size);
void* crop(void* block);
void* pad(void* block);

