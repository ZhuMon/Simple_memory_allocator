#include "lib/hw_malloc.h"
#include "hw4_mm_test.h"

int main(int argc, char *argv[])
{

    while(1) {
        char command[20] = {};
        char tmp_char;
        //printf("\e[1;32mmyshell\e[0m$ ");

        if(scanf("%s", command) != 1) {
            //err_msg("error input\n");
            break;
        }
        if(strcmp(command, "alloc") == 0) {
            size_t bytes;
            void *alloc;
            if(scanf("%c", &tmp_char) != 1) {
                err_msg("error input\n");
            }
            if(scanf("%ld", &bytes) != 1) {
                err_msg("error input\n");
            }

            alloc = hw_malloc(bytes);
            //printf("alloc: %p\n", alloc);
            //printf("sbrk: %p\n", get_start_sbrk());
            if((long int)alloc > (long int)get_start_sbrk() + 64*1024) {
                printf("%012p\n", alloc);
            } else {
                printf("0x%012lx\n", alloc - get_start_sbrk());
            }
        } else if( strcmp(command, "free") == 0) {
            void *addr;
            int err;

            if(scanf("%c", &tmp_char) != 1) {
                err_msg("error input\n");
            }
            if(scanf("%p", &addr) != 1) {
                err_msg("error input\n");
            }
            err = hw_free(addr);
            if(err == 1) {
                printf("success\n");
            } else if(err == 0) {
                printf("fail\n");
            }

        } else if( strcmp(command, "print") == 0) {
            int bin_i;
            if(scanf("%c", &tmp_char) != 1) {
                err_msg("error input\n");
            }

            if(scanf("bin[%d]", &bin_i) != 1) {
                if(scanf("%s", command) != 1) {
                    err_msg("error input\n");
                } else {
                    if(strcmp(command, "mmap_alloc_list") == 0) {
                        print_mmap();
                    }
                }
            } else {
                print_bin(bin_i);
            }
        }
    }
    return 0;
}
