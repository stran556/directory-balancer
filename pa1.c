#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
	int id = fork();
	printf("Weve got a job to do\n");

	if (id == 0) {
		printf("Child Process\n");
	} else {
	printf("Parent Process\n");
	}
	return 0;
}