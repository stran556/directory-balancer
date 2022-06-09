//Sean Tran
//CS 4348
//Assignment #1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

//fd[0] = read
//fd[1] = write

int main(int argc, char* argv[]) {

	char txt[100];
	char command[100];

	//Verify initial discrepancies between d1 and d2
	system("diff d1 d2");

	//Clear both files
	system(": > d1.txt");
	system(": > d2.txt");

	//Create pipes
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
	//Once pipes are created, then fork so "children are aware"
	int pid = fork();
	if (pid == -1) {
		return 0;
	}

	
	//child AKA child 2
	if (pid == 0) {

		//assign child process to directory 2
		char* directory = "d2"; 
		
		//add file names to list and print
		sprintf(txt, "%s.txt", directory);
		sprintf(command, "ls %s >> %s.txt", directory, directory);
		system(command); 
		sprintf(command, "cat %s.txt", directory);
		system(command);

		//printf("List: %s\n", txt);
		if (write(p2[1], txt, 7) < 0){ //write to pipe p2
			return 1;
		}
		if (read(p1[0], txt, 7) < 0) { //read from pipe p1
			return 1;
		}
		//close(p2[1]);
		printf("List from child 1 received by child 2: %s\n", txt);
		
	}

	//main AKA child 1
	else {
		//assign main process to directory 1
		char* directory = "d1"; 
		
		//add file names to list and print
		sprintf(txt, "%s.txt", directory);
		sprintf(command, "ls %s >> %s", directory, txt);
		system(command); 
		sprintf(command, "cat %s.txt", directory);
		system(command);
		
		//printf("List: %s\n", txt);
		if (write(p1[1], txt, 7) < 0){ //write to pipe p1
			return 1;
		}
		if (read(p2[0], txt, 7) < 0) { //read from pipe p2
			return 1;
		}
		printf("List from child 2 received by child 1: %s\n", txt);
		//close(p1[1]);
		
		//srand(time(NULL));
		//wait(NULL);
	}
	char cmd[100];
	sprintf(cmd, "wc -l < %s", txt);
	system(cmd);
	return 0;
}