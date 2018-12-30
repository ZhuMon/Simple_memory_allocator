
#include "hw_malloc.h"

size_t mmap_threshold = 32*1024;

void *hw_malloc(size_t bytes)
{
    if(heap == NULL) {
        heap = sbrk(0);
        heap = heap - 64*1024;

        for(int i = 0; i < 11; i++) {
            bin[i] = mmap(NULL, 24,PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, 0, 0);
            bin[i] -> next = bin[i];
            bin[i] -> prev = bin[i];
        }
    }

    if(bytes+24 > mmap_threshold) {
        //mmap


        struct mychunk *p, *now;
        p = mmap(NULL, bytes+24,PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, 0, 0);
        if (p == (void*)-1) {
            err_msg("mmap() failed\n");
            return NULL;
        }

        p -> size_and_flag.curr_chunk_size = bytes+24;
        p -> size_and_flag.allo_flag = 1;
        p -> size_and_flag.mmap_flag = 1;

        if(mmap_head == NULL) {
            mmap_head = mmap(NULL, bytes+24,PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, 0, 0);
            mmap_head -> next = p;
            mmap_head -> prev = p;
            mmap_head -> size_and_flag.mmap_flag = 1;
            p -> next = mmap_head;
            p -> prev = mmap_head;
            p -> size_and_flag.prev_chunk_size = 0;

        } else {
            now = mmap_head -> next;
            while(now != mmap_head) {
                if(now -> size_and_flag.curr_chunk_size > p -> size_and_flag.curr_chunk_size) {
                    break;
                }
                now = now -> next;
            }


            now -> prev -> next = p;
            p -> prev = now -> prev;
            now -> prev = p;
            p -> next = now;

            p -> size_and_flag.prev_chunk_size = p -> prev -> size_and_flag.curr_chunk_size;

        }



        return p+24;

    } else {
        //heap

        struct mychunk *new;
        int bin_num = alloc_bin(bytes+24);

        if(bin[bin_num] -> next = bin[bin_num]) {

        }


    }
    return NULL;
}

int alloc_bin(size_t bytes)
{
    if(bytes < 32) {
        return 0;
    } else if(bytes < 64) {
        return 1;
    } else if(bytes < 128) {
        return 2;
    } else if(bytes < 256) {
        return 3;
    } else if(bytes < 512) {
        return 4;
    } else if(bytes < 1024) {
        return 5;
    } else if(bytes < 1024*2) {
        return 6;
    } else if(bytes < 1024*4) {
        return 7;
    } else if(bytes < 1024*8) {
        return 8;
    } else if(bytes < 1024*16) {
        return 9;
    } else if(bytes < 1024*32) {
        return 10;
    }
}

int hw_free(void *mem)
{
    return 0;
}

void *get_start_sbrk(void)
{
    if(heap != NULL) {
        return heap;
    } else {
        return NULL;
    }
}
