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

	char txt[100];
	//Verify initial discrepancies between d1 and d2
	system("diff d1 d2");

	//Clear both files
	system(": > d1.txt");
	system(": > d2.txt");

	int p1[2]; //main -> child
	int p2[2]; // child -> main

	//Confirm successful pipe initialization
	if (pipe(p1) == -1) {
		printf("Error.");
		return 1;
	}
	if (pipe(p2) == -1) {
		printf("Error.");
		return 1;
	}
	int pid = fork();
	if (pid == -1) {
		return 0;
	}

	char command[100];
	//child
	if (pid == 0) { //child, so use pipe p2
		char* directory = "d2"; //assign child process to directory 2
		
		//text file name
		
		
		//add file names to list
		sprintf(command, "ls %s >> %s.txt", directory, directory);
		system(command); 

		//test print
		sprintf(command, "cat %s.txt", directory);
		system(command);
	}

	//main 
	else { //main, so use pipe p1
		char* directory = "d1"; //assign main process to directory 1
		sprintf(txt, "%s.txt", directory);

		//text file name
		sprintf(txt, "%s.txt", directory);

		//add file names to list
		sprintf(command, "ls %s >> %s", directory, txt);
		system(command); 

		//test print
		sprintf(command, "cat %s.txt", directory);
		system(command);

		//write(p1[1], str)
		
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