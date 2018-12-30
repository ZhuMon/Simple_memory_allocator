#ifndef HW_MALLOC_H
#define HW_MALLOC_H
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>

#define err_msg(msg) printf("\e[1;31merror: \e[0m");\
   printf(msg);\
   printf("\n")


struct chunk_info_t {
    unsigned prev_chunk_size:31;
    unsigned curr_chunk_size:31;
    unsigned allo_flag:1;
    unsigned mmap_flag:1;
};

struct mychunk {
    struct mychunk *next,*prev;
    struct chunk_info_t size_and_flag;
};

struct mychunk *mmap_head = NULL, *bin[11];
void *heap;

void *hw_malloc(size_t bytes);
int hw_free(void *mem);
void *get_start_sbrk(void);

#endif
