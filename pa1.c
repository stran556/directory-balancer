#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
//fd[0] = input, write
//fd[1] = output, read
int main(int argc, char* argv[]) {
	
	//Clear both files
	system(": > d1.txt");
	system(": > d2.txt");

	int p1[2]; //fd[0], fd[1] main -> child
	int p2[2]; //fd[0], fd[1] child -> main
	if (pipe(p1) == -1) {
		return 1;
	}
	int pid = fork();
	if (pid == -1) {
		return 0;
	}
	if (pid == 0) {
		//child
		system("ls d2 >> d2.txt"); //child process to directory 2
		system("cat d2.txt");
		
	} 
	else {
		//main
		system("ls d1 >> d1.txt"); //main process to directory 1
		system("cat d1.txt");
		//srand(time(NULL));
		/*
		int y = rand() % 10;
		if (write(p1[1], &y, sizeof(y)) == -1) {
			return 5;
		}
		printf("Wrote %d\n", y);
		if (read(p1[0], &y, sizeof(y)) == -1) {
			return 6;
		}
		printf("Result is %d\n", y);
		*/
		//wait(NULL);
	}
	
	

	return 0;
}