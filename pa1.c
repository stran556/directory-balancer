//Sean Tran
//CS 4348.0U2
//Prof: Ravi Prakash
//Assignment #1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>


/*
 - Program begins by clearing file names from lists and building pipes. 
 - Once pipes are created successfully, parent forks. 
*/
int main(int argc, char* argv[]) {

	char txt[1000];
	char command[1000];
	char main1[1000];
	char main2[1000];
	char snt[1000];
	char missing_files[1000];
	char m1[1000];
	char m2[1000];
	char mf1[1000];
	char mf2[1000];
	int len;

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

	//===================================Begin Child 2===================================//

	/*
	 - Begin by assigning directory 2 to child 2
	 - Files present in directory 'd2' are added to list 'd2.txt' by name
	 - Child 2 sends list 'd2.txt' to child 1 via pipe, receiving 'd1.txt' via pipe in return
	 - Child 2 translates 'd2.txt' into a variable before sending it to child 1 via pipe, 
	 	receiving 'd1.txt' as a variable in return
	 - Separate file names in list into an array for parsing
	 - Identify files in directory 'd1' missing from directory 'd2' and create them in directory 'd2'
	*/

	if (pid == 0) {
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

		//Child sends list to other child
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
			sprintf(snt, "%s%c", snt, c);
			
		}
		
		int len2 = strlen(snt);
		int *rlen2 = malloc(sizeof *rlen2);
		*rlen2 = len2;
		printf("Size of OG %s: %d\n", directory, *rlen2);
		printf("Size of %s: %d\n", directory, len2);

		if (write(p2[1], &len2, sizeof(len2)) < 0){ //write to pipe p2
			return 1;
		}
		if (read(p1[0], &len2, sizeof(len2)) < 0) { //read from pipe p1
			return 1;
		}
		printf("Size of RCV %s: %d\n", directory, len2);

		char rcv[len2 + 1];

		//Pipe file list to other child
		printf("Files in d2:%s", snt);
		if (write(p2[1], snt, *rlen2 + 1) < 0){ //write to pipe p2
			return 1;
		}
		if (read(p1[0], rcv, len2 + 1) < 0) { //read from pipe p1
			return 1;
		}
		printf("Files in d1:%s", snt);
		//printf("Files in %s:%s", main, s);
		pclose(fpipe);
		//exit(EXIT_SUCCESS);	

		//SNT IS D2, RCV IS D1
		printf("RCV %s %s", directory, rcv);
		printf("SNT %s %s", directory, snt);

		//Retrieve number of files in list
		int i, d2count = 0;
		for(i = 0; snt[i]; i++) {
			if(snt[i] == '\n')
			{
			d2count++;
			}
		}
		int d1count = 0;
		for(i = 0; rcv[i]; i++) {
			if(rcv[i] == '\n') {
			d1count++;
			}
		}

		//Sizeof for files
		printf("SNT count for %s: %d\n", directory, d2count);
		printf("RCV count for %s: %d\n", directory, d1count);

		//Separating SNT and RCV files into an array
		printf("ARRAYS for D2\n");
		int f = 0;
		char *s = strtok (snt, "\n");
		char *d2array[d2count];

		while (s != NULL) {
			d2array[f++] = s;
			s = strtok (NULL, "\n");
		}
		//for (f = 0; f < d2count; ++f) 
		//	printf("%s\n", d2array[f]);

		int d = 0;
		char *t = strtok (rcv, "\n");
		char *d1array[d1count];
		while (t != NULL) {
			d1array[d++] = t;
			t = strtok (NULL, "\n");
		}
		
		//for (d = 0; d < d1count; ++d) 
		//	printf("%s\n", d1array[d]);

		//Creating missing files in directory 2
		bool found = false;
		char addfile[1000];
		for(int i = 0; i < d1count; i++){
			for(int j = 0; j < d2count; j++){
				//If lengths do not match, neither can the files
				if((strlen(d1array[i]) == strlen(d2array[j])) && !memcmp(d1array[i], d2array[j], strlen(d1array[i]))) {
					found = true;
				}
				printf("%s and %s: ", d1array[i], d2array[j]);
				printf("%d\n", found);
				if(found){
					break;
				}
			}
			if(!found){
				sprintf(addfile, "touch %s/%s", directory, d1array[i]);
				system(addfile);
				printf("Adding: %s\n", addfile);
			}
			found = false;
		}
		//erase contents of d2.txt, then add names of all empty files
		sprintf(missing_files, ": > %s.txt | find \"%s\" -size 0 | cut -c 4- >> %s.txt", directory, directory, directory);
		system(missing_files);


		sprintf(txt, "%s.txt", directory);
		printf("Directory and file: %s, %s\n", directory, txt);	
		if (write(p2[1], txt, 7) < 0){ //write to pipe p2
			return 1;
		}
		if (read(p1[0], txt, 7) < 0) { //read from pipe p1
			return 1;
		}
		printf("Directory and file: %s, %s\n", directory, txt);	

		FILE *fpipe4;
		sprintf(command, "grep txt %s", txt);
		char *filecnt4 = command;
		char c4 = 0;

		if (0 == (fpipe4 = (FILE*)popen(filecnt4, "r"))) {
			exit(EXIT_FAILURE);
		}

		while (fread(&c4, sizeof c4, 1, fpipe4)) {
			//printf("%c", c);
			sprintf(m2, "%s%c", m2, c4);
		}
		pclose(fpipe4);

		int l, m2count = 0;
		for(l = 0; m2[l]; l++) {
			if(m2[l] == '\n') {
			m2count++;
			}
		}
		printf("M2: %s\nM2count: %d\n", m2, m2count);

		
		int ff = 0;
		char *ss = strtok (m2, "\n");
		char *m2array[m2count];

		while (ss != NULL) {
			m2array[ff++] = ss;
			ss = strtok (NULL, "\n");
		}
		printf("Array test: %s %s %s\n", m2array[0], m2array[1], m2array[2]); //alpha, bravo, charlie

		sprintf(command, ": > %s", txt);
		system(command);

		char appender[1000];
		//Writing to d1.txt
		for(int i = 0; i < m2count; i++){
			sprintf(appender, "echo %s >> %s", m2array[i], txt);
			system(appender);
			sprintf(appender, "echo $(cat %s/%s) >> %s", directory, m2array[i], txt);
			system(appender);
		}
		system("cat d1.txt");

		if (write(p2[1], txt, 7) < 0){ //write to pipe p2
			return 1;
		}
		if (read(p1[0], txt, 7) < 0) { //read from pipe p1
			return 1;
		}
		printf("Directory and file: %s, %s\n", directory, txt);	

			FILE *fpipe6;
		sprintf(command, "cat %s", txt);
		char *filecnt6 = command;
		char c6 = 0;

		if (0 == (fpipe6 = (FILE*)popen(filecnt6, "r"))) {
			exit(EXIT_FAILURE);
		}

		while (fread(&c6, sizeof c6, 1, fpipe6)) {
			//printf("%c", c);
			sprintf(mf2, "%s%c", mf2, c6);
		}
		pclose(fpipe6);

		int ll, mf2count = 0;
		for(ll = 0; mf2[ll]; ll++) {
			if(mf2[ll] == '\n') {
			mf2count++;
			}
		}
		printf("MF2: %s\nMF2count: %d\n", mf2, mf2count); 

		int fff = 0;
		char *sss = strtok (mf2, "\n");
		char *mf2array[mf2count];

		while (sss != NULL) {
			mf2array[fff++] = sss;
			sss = strtok (NULL, "\n");
		}
		
		char updater[1000];
		for(int i = 0; i < mf2count; i = i + 2){
			sprintf(updater, "printf %s > %s/%s", mf2array[i + 1], directory, mf2array[i]);
			system(updater);
		}
		
		system("cat d2.txt");
		
	}	
	//==============================End Child 2 - Begin Child 1=====================================
	/*
	 - Begin by assigning directory 1 to child 1
	 - Files present in directory 'd1' are added to list 'd1.txt' by name
	 - Child 1 sends list 'd1.txt' to child 2 via pipe, receiving 'd2.txt' via pipe in return
	 - Child 1 translates 'd1.txt' into a variable before sending it to child 2 via pipe, 
	 	receiving 'd2.txt' as a variable in return
	 - Separate file names in list into an array for parsing
	 - Identify files in directory 'd2' missing from directory 'd1' and create them in directory 'd1'
	*/

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
		
		//Child sends list to other child
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
			sprintf(snt, "%s%c", snt, c2);
		}
		pclose(fpipe2);

		int len = strlen(snt);
		int *rlen = malloc(sizeof *rlen);
		*rlen = len;
		printf("Size of %s: %d\n", directory, len);
		printf("Size of OG %s: %d\n", directory, *rlen);

		//Pipe file size to other child (JUST SWAPS FILE SIZES FOR USE)
		if (write(p1[1], &len, sizeof(len)) < 0){ //write to pipe p1
			return 1;
		}
		if (read(p2[0], &len, sizeof(len)) < 0) { //read from pipe p2
			return 1;
		}
		printf("Size of RCV %s: %d\n", directory, len);

		char rcv[len];
		//Pipe file list to other child
		printf("Files in d1:%s", snt);
		if (write(p1[1], snt, *rlen + 1) < 0){ //write to pipe p1
			return 1;
		}
		if (read(p2[0], rcv, len + 1) < 0) { //read from pipe p2
			return 1;
		}
		printf("Files in d2:%s", snt); 
		printf("Size: %lu\n", strlen(snt));
		
		//exit(EXIT_SUCCESS);
		
		//SNT IS D1, RCV IS D2
		printf("RCV %s %s", directory, rcv);
		printf("SNT %s %s", directory, snt);
		
		//Retrieve number of files in list
		int j, d1count = 0;
		for(j = 0; snt[j]; j++) {
			if(snt[j] == '\n') {
			d1count++;
			}
		}
		int d2count = 0;
		for(j = 0; rcv[j]; j++) {
			if(rcv[j] == '\n') {
			d2count++;
			}
		}

		//Sizeof for files
		printf("SNT count for %s: %d\n", directory, d1count);
		printf("RCV count for %s: %d\n", directory, d2count);

		//Separating SNT and RCV files into an array
		printf("ARRAYS for D1\n");
		int c = 0;
		char *s = strtok (snt, "\n");
		char *d1array[d1count];

		while (s != NULL) {
			d1array[c++] = s;
			s = strtok (NULL, "\n");
		}
		//for (c = 0; c < d1count; ++c) 
		//	printf("%s\n", d1array[c]);

		int d = 0;
		char *t = strtok (rcv, "\n");
		char *d2array[d2count];
		while (t != NULL) {
			d2array[d++] = t;
			t = strtok (NULL, "\n");
		}
		
		//for (d = 0; d < d2count; ++d) 
		//	printf("%s\n", d2array[d]);	


		char addfile[1000];
		bool found = false;
			for(int i = 0; i < d2count; i++){
				for(int j = 0; j < d1count; j++){
					//If lengths do not match, neither can the files (memcmp returns 0 for true, so flip)
					if((strlen(d2array[i]) == strlen(d1array[j])) && !memcmp(d2array[i], d1array[j], strlen(d1array[i]))) {
						found = true;
					}
					printf("%s and %s: ", d2array[i], d1array[j]);
					printf("%d\n", found);
					if(found) {
						break;
					}
				}
				if(!found) {
					sprintf(addfile, "touch %s/%s", directory, d2array[i]);
					system(addfile);
					printf("Adding: %s\n", addfile);
				}
				found = false;
			}

		//erase contents of d2.txt, then add names of all empty files
		
		sprintf(missing_files, ": > %s.txt | find \"%s\" -size 0 | cut -c 4- >> %s.txt", directory, directory, directory);
		system(missing_files);
		sprintf(txt, "%s.txt", directory);
		printf("Directory and file: %s, %s\n", directory, txt);	
		//Send list of files that need content to d2
		if (write(p1[1], txt, 7) < 0){ //write to pipe p1
		return 1;
		}
		if (read(p2[0], txt, 7) < 0) { //read from pipe p2
			return 1;
		}
		printf("Directory and file: %s, %s\n", directory, txt);

		FILE *fpipe3;
		sprintf(command, "grep txt %s", txt);
		char *filecnt3 = command;
		char c3 = 0;

		if (0 == (fpipe3 = (FILE*)popen(filecnt3, "r"))) {
			exit(EXIT_FAILURE);
		}

		while (fread(&c3, sizeof c3, 1, fpipe3)) {
			//printf("%c", c);
			sprintf(m1, "%s%c", m1, c3);
		}
		pclose(fpipe3);
	
		int k, m1count = 0;
		for(k = 0; m1[k]; k++) {
			if(m1[k] == '\n') {
			m1count++;
			}
		}
		printf("M1: %s\nM1count: %d\n", m1, m1count);

		int ee = 0;
		char *tt = strtok (m1, "\n");
		char *m1array[m1count];

		while (tt != NULL) {
			m1array[ee++] = tt;
			tt = strtok (NULL, "\n");
		}
		printf("Array test: %s %s %s\n", m1array[0], m1array[1], m1array[2]); 

		//Clear txt file
		sprintf(command, ": > %s", txt);
		system(command); 

		char appender[1000];
		//Writing to d2.txt
		for(int i = 0; i < m1count; i++){
			sprintf(appender, "echo %s >> %s", m1array[i], txt);
			system(appender);
			sprintf(appender, "echo $(cat %s/%s) >> %s", directory, m1array[i], txt);
			system(appender);
		}
		system("cat d2.txt");
		
		if (write(p1[1], txt, 7) < 0){ //write to pipe p1
		return 1;
		}
		if (read(p2[0], txt, 7) < 0) { //read from pipe p2
			return 1;
		}
		printf("Directory and file: %s, %s\n", directory, txt);
		
		FILE *fpipe5;
		sprintf(command, "cat %s", txt);
		char *filecnt5 = command;
		char c5 = 0;

		if (0 == (fpipe5 = (FILE*)popen(filecnt5, "r"))) {
			exit(EXIT_FAILURE);
		}

		while (fread(&c5, sizeof c5, 1, fpipe5)) {
			//printf("%c", c);
			sprintf(mf1, "%s%c", mf1, c5);
		}
		pclose(fpipe5);

		int kk, mf1count = 0;
		for(kk = 0; mf1[kk]; kk++) {
			if(mf1[kk] == '\n') {
			mf1count++;
			}
		}
		printf("MF1: %s\nMF1count: %d\n", mf1, mf1count);

		int eee = 0;
		char *ttt = strtok (mf1, "\n");
		char *mf1array[mf1count];

		while (ttt != NULL) {
			mf1array[eee++] = ttt;
			ttt = strtok (NULL, "\n");
		}

		char updater[1000];
		for(int i = 0; i < mf1count; i = i + 2){
			sprintf(updater, "printf %s > %s/%s", mf1array[i + 1], directory, mf1array[i]);
			system(updater);
			sprintf(updater, "tr -d '\n' < %s/%s", directory, mf1array[i]);
			system(updater);
		}
		
		system("cat d1.txt");
	}
	//===================================End Child 1===========================================
	sleep(2);
	return 0;
	
}