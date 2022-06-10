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

//rcv = files of local directory
//snt = files received from other child
int main(int argc, char* argv[]) {

	char txt[100];
	char command[100];
	char main1[100];
	char main2[100];
	char rcv[100] = "";
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

		//assign d2.txt to process
		sprintf(main2, "%s.txt", directory);
		char* main = main2;

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

		printf("List from child 1 received by child 2: %s\n", txt); //should be d1.txt
		printf("File for directory: %s: %s\n", directory, main);

		//Retrieve d2 file as variable
		FILE *fpipe;
		sprintf(command, "grep txt %s", main);
		char *filecnt = command;
		char c = 0;

		if (0 == (fpipe = (FILE*)popen(filecnt, "r"))) {
			exit(EXIT_FAILURE);
		}

		while (fread(&c, sizeof c, 1, fpipe)) {
			//printf("%c", c);
			sprintf(rcv, "%s%c", rcv, c);
			
		}
		int i, count = 0;
		for(i = 0; rcv[i]; i++)  
		{
			if(rcv[i] == '\n')
			{
			count++;
			}
		}
		printf("Count for %s: %d\n", directory, count);



		printf("Files in d2:%s", rcv);
		char* snt2 = rcv;
		if (write(p2[1], rcv, strlen(rcv) + 1) < 0){ //write to pipe p2
			return 1;
		}
		if (read(p1[0], rcv, strlen(rcv) + 1) < 0) { //read from pipe p1
			return 1;
		}
		printf("Files in d1:%s", snt2);
		//printf("Files in %s:%s", main, s);
		pclose(fpipe);
		//exit(EXIT_SUCCESS);
//-----------------------
		
		

		
	}	
	//main AKA child 1
	else {
		//assign main process to directory 1
		char* directory = "d1"; 

		//assign d1.txt to process
		sprintf(main1, "%s.txt", directory);
		char* main = main1;

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
		printf("List from child 2 received by child 1: %s\n", txt); //should be d2.txt
		//close(p1[1]);
		
		//srand(time(NULL));
		//wait(NULL);

		printf("File for directory: %s: %s\n", directory, main);


		//Retrieve d1 file as variable
		FILE *fpipe2;
		sprintf(command, "grep txt %s", main);
		char *filecnt2 = command;
		char c2 = 0;

		if (0 == (fpipe2 = (FILE*)popen(filecnt2, "r"))) {
			exit(EXIT_FAILURE);
		}

		while (fread(&c2, sizeof c2, 1, fpipe2)) {
			//printf("%c", c);
			sprintf(rcv, "%s%c", rcv, c2);
			
		}

		int i, count = 0;
		for(i = 0; rcv[i]; i++)  
		{
			if(rcv[i] == '\n')
			{
			count++;
			}
		}
		printf("Count for %s: %d\n", directory, count);





		printf("Files in d1:%s", rcv);
		char* snt = rcv;
		if (write(p1[1], rcv, strlen(rcv) + 1) < 0){ //write to pipe p1
			return 1;
		}
		if (read(p2[0], rcv, strlen(rcv) + 1) < 0) { //read from pipe p2
			return 1;
		}
		printf("Files in d2:%s", snt);
		printf("Size: %lu\n", strlen(rcv));
		pclose(fpipe2);
		//exit(EXIT_SUCCESS);

		
	}
	//char cmd[100];
	//sprintf(cmd, "wc -l < %s", txt);
	//system(cmd); 
	printf("Process rcv: %d %s\n", pid, rcv);

	
	return 0;
	
}