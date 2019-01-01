#include "lib/hw_malloc.h"
#include "hw4_mm_test.h"

int main(int argc, char *argv[])
{

    while(1) {
        char command[20] = {};
        char tmp_char;
        printf("\e[1;32mmyshell\e[0m$ ");

        if(scanf("%s", command) != 1) {
            err_msg("error input\n");
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
            printf("0x%012lx\n", alloc - get_start_sbrk());
        } else if( strcmp(command, "free") == 0) {

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
