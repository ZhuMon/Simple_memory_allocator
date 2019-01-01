
#include "hw_malloc.h"

size_t mmap_threshold = 32*1024;
struct mychunk *mmap_head = NULL;
struct mychunk *bin[11];
void *heap;
int b_bin; //biggest free bin

void *hw_malloc(size_t bytes)
{
    if(heap == NULL) {
        heap = sbrk(0);
        brk(heap+64*1024);
        //printf("heap: %p\n", heap);

        for(int i = 0; i < 11; i++) {
            bin[i] = mmap(NULL, 24,PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, 0, 0);
            bin[i] -> next = bin[i];
            bin[i] -> prev = bin[i];
            bin[i] -> size_and_flag.prev_chunk_size = power(2,5+i);
            bin[i] -> size_and_flag.curr_chunk_size = power(2,5+i);
        }
        struct mychunk *chunk_32k_1, *chunk_32k_2;

        chunk_32k_1 = heap;
        //printf("32k_1: %p\n", chunk_32k_1);
        bin[10] -> next = chunk_32k_1;
        chunk_32k_1 -> prev = bin[10];
        chunk_32k_1 -> size_and_flag.curr_chunk_size = 32*1024;
        chunk_32k_1 -> size_and_flag.prev_chunk_size = 0;
        chunk_32k_1 -> size_and_flag.allo_flag = 0;
        chunk_32k_1 -> size_and_flag.mmap_flag = 0;

        chunk_32k_2 = heap + 32*1024;
        //printf("32k_2: %p\n", chunk_32k_2);

        chunk_32k_1 -> next = chunk_32k_2;
        chunk_32k_2 -> prev = chunk_32k_1;
        chunk_32k_2 -> size_and_flag.curr_chunk_size = 32*1024;
        chunk_32k_2 -> size_and_flag.prev_chunk_size = 32*1024;
        chunk_32k_2 -> size_and_flag.allo_flag = 0;
        chunk_32k_2 -> size_and_flag.mmap_flag = 0;

        chunk_32k_2 -> next = bin[10];
        bin[10] -> prev = chunk_32k_2;

        b_bin = 10;
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

        void *tmp = p;
        return tmp+24;

    } else {
        //heap

        struct mychunk *new;
        int bin_index = alloc_bin(bytes+24);

        new = split(bin_index);
        //printf("new : %p\n", new);
        new -> size_and_flag.allo_flag = 1;
        new -> size_and_flag.mmap_flag = 0;

        void *tmp = new;
        return tmp + 24;
    }
    return NULL;
}

void* split(int bin_index)
{
    struct mychunk *now;
    if(bin[bin_index] -> next == bin[bin_index]) {
        //split larger bin
        if(bin[bin_index + 1] -> next != bin[bin_index+1]) {
            now = bin[bin_index+1] -> next;
            while(now != bin[bin_index+1]) {
                if(now -> size_and_flag.allo_flag == 0) {
                    break;
                }
                now = now ->  next;
            }
            if(now == bin[bin_index+1]) {
                return split(bin_index+1);
            } else {
                now -> prev -> next = now -> next;
                now -> next -> prev = now -> prev;

                bin[bin_index] -> prev -> next = now;
                now -> prev = bin[bin_index] -> prev;
                bin[bin_index] -> prev = now;
                now -> next = bin[bin_index];

                now -> size_and_flag.prev_chunk_size = now -> prev->size_and_flag.curr_chunk_size;
                now -> size_and_flag.curr_chunk_size = power(2, bin_index+5);

                void *tmp = now;
                now = tmp + power(2, bin_index+5);

                bin[bin_index] -> prev -> next = now;
                now -> prev = bin[bin_index] -> prev;
                bin[bin_index] -> prev = now;
                now -> next = bin[bin_index];

                now -> size_and_flag.prev_chunk_size = now -> prev->size_and_flag.curr_chunk_size;
                now -> size_and_flag.curr_chunk_size = power(2, bin_index+5);

                return tmp;
            }
        } else {
            //printf("1.bin: %d", bin_index);
            now = split(bin_index+1);

            now -> prev -> next = now -> next;
            now -> next -> prev = now -> prev;

            bin[bin_index] -> prev -> next = now;
            now -> prev = bin[bin_index] -> prev;
            bin[bin_index] -> prev = now;
            now -> next = bin[bin_index];

            now -> size_and_flag.prev_chunk_size = now -> prev->size_and_flag.curr_chunk_size;
            now -> size_and_flag.curr_chunk_size = power(2, bin_index+5);

            void *tmp = now;
            now = tmp + power(2, bin_index+5);

            bin[bin_index] -> prev -> next = now;
            now -> prev = bin[bin_index] -> prev;
            bin[bin_index] -> prev = now;
            now -> next = bin[bin_index];

            now -> size_and_flag.prev_chunk_size = now -> prev->size_and_flag.curr_chunk_size;
            now -> size_and_flag.curr_chunk_size = power(2, bin_index+5);

            return tmp;
        }
    } else if(bin[bin_index] -> next != bin[bin_index] ) {
        now = bin[bin_index] -> next;
        while(now != bin[bin_index]) {
            if(now -> size_and_flag.allo_flag == 0) {
                break;
            }
            now = now -> next;
        }
        if(now == bin[bin_index]) {
            //split larger bin
            //printf("2.bin: %d\n", bin_index);

            now = split(bin_index+1);

            now -> prev -> next = now -> next;
            now -> next -> prev = now -> prev;

            bin[bin_index] -> prev -> next = now;
            now -> prev = bin[bin_index] -> prev;
            bin[bin_index] -> prev = now;
            now -> next = bin[bin_index];

            now -> size_and_flag.prev_chunk_size = now -> prev->size_and_flag.curr_chunk_size;
            now -> size_and_flag.curr_chunk_size = power(2, bin_index+5);

            void *tmp = now;
            now = tmp + power(2, bin_index+5);

            bin[bin_index] -> prev -> next = now;
            now -> prev = bin[bin_index] -> prev;
            bin[bin_index] -> prev = now;
            now -> next = bin[bin_index];

            now -> size_and_flag.prev_chunk_size = now -> prev->size_and_flag.curr_chunk_size;
            now -> size_and_flag.curr_chunk_size = power(2, bin_index+5);

            return tmp;
        } else {
            //printf("now : %p\n", now);
            //printf("3.bin: %d\n", bin_index);
            return now;
        }
    }
    return NULL;
}

int power(int a, int b)
{
    int tmp = 1;
    for(int i = 0; i < b; i++) {
        tmp *= a;
    }
    return tmp;
}

int alloc_bin(size_t bytes)
{
    if(bytes <= 32) {
        return 0;
    } else if(bytes <= 64) {
        return 1;
    } else if(bytes <= 128) {
        return 2;
    } else if(bytes <= 256) {
        return 3;
    } else if(bytes <= 512) {
        return 4;
    } else if(bytes <= 1024) {
        return 5;
    } else if(bytes <= 1024*2) {
        return 6;
    } else if(bytes <= 1024*4) {
        return 7;
    } else if(bytes <= 1024*8) {
        return 8;
    } else if(bytes <= 1024*16) {
        return 9;
    } else if(bytes <= 1024*32) {
        return 10;
    }
    return -1;
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

void print_bin(int n)
{
    struct mychunk *tmp;
    void *a;
    tmp = bin[n] -> next;
    while(tmp != bin[n]) {
        if(tmp -> size_and_flag.allo_flag == 0) {
            a = tmp;
            printf("0x%012lx--------%d\n", a - get_start_sbrk(), tmp->size_and_flag.curr_chunk_size);
        }
        tmp = tmp -> next;
    }
}