#include "lib/hw_malloc.h"
#include "hw4_mm_test.h"

int main(int argc, char *argv[])
{
    while(1) {
        char command[20] = {};
        char tmp_char;
        printf("\e[1;32mmyshell]e[0m$ ");

        scanf("%s", command);
        if(strcmp(command, "alloc") == 0) {

        } else if( strcmp(command, "free") == 0) {

        } else if( strcmp(command, "print") == 0) {

        }
    }
    return 0;
}
