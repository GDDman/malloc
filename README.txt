Josh Liu ID: 260612384

Compile with the MakeFile provided

Run with ./memory

mallopt:
Code for First-Fit is 1. Code for Best-Fit is 0

Program break is reduced if the top free block is more than 128 kb 

Whenever the heap is extended, a free portion of 5120 bytes is created on the top of the heap

Whenever free memory (in the middle of the heap) is re-allocated, the new memory is given a 10 byte
buffer after it (to prevent errors). This should be accounted for in the testing file

The test file does some allocations and gives the expected answer after each mallinfo is called
The data seems to match for each case that I tried. It should be clear how to test your own cases from
the info in the test file.

