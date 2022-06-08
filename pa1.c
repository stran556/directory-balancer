#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
	
	
	char* directory;

	int p1[2];
	if (pipe(p1) == -1) {
		return 1;
	}
	int pid = fork();
	if (pid == -1) {
		return 2;
	}
	if (pid == 0) {
		//child
		strcpy(directory, "ls d2");
		int x;
		if (read(p1[0], &x, sizeof(x)) == -1) { //read data from parent process
			return 3; 
		}
		printf("Received %d\n", x);
		x *= 4;
		if (write(p1[1], &x, sizeof(x)) == -1) { 
			return 4;  
		}
		printf("Wrote %d\n", x);
	} 
	else {
		//Parent
		strcpy(directory, "ls d1");
		srand(time(NULL));
		int y = rand() % 10;
		if (write(p1[1], &y, sizeof(y)) == -1) {
			return 5;
		}
		printf("Wrote %d\n", y);
		if (read(p1[0], &y, sizeof(y)) == -1) {
			return 6;
		}
		printf("Result is %d\n", y);
		wait(NULL);
	}
	system(directory);

	return 0;
}