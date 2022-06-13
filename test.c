#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

int main(int argc, char* argv[]) {

    char command[1000];
    char out[1000];
    FILE *fpipe;
    char c = 0;
    char *filecnt = command;

    for(int i = 0; i < 10; i++){  
        c = 0;  
		sprintf(command, "echo %d", i);

		if (0 == (fpipe = (FILE*)popen(filecnt, "r"))) {
			exit(EXIT_FAILURE);
        } 
		while (fread(&c, sizeof c, 1, fpipe)) {
			//printf("%c", c);
			sprintf(out, "%s%c", out, c);
		}
    }
    printf("%s", out);
    return 0;
}