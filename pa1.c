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
#include <errno.h>

/*
 - Program begins by clearing file names from lists and building pipes. 
 - Once pipes are created successfully, parent forks. 
*/
int main(int argc, char* argv[]) {

	char txt[1000];
	char command[1000];
	char snt[1000];
	char missing_files[1000];
	char m1[1000];
	char m2[1000];
	char mf1[1000];
	char mf2[1000];
	int errno;

	//Verify initial discrepancies between d1 and d2
	errno = system("diff d1 d2");
	if(errno == -1){
		printf("Error.");
		return 1;
	}

	//Clear both files
	errno = system(": > d1.txt");
	if(errno == -1){
		printf("Error.");
		return 1;
	}
	errno = system(": > d2.txt");
	if(errno == -1){
		printf("Error.");
		return 1;
	}


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
	 - Send list of files missing contents to child 2 via pipe, receiving list in return
	 - Child 2 retrieves contents for list of files and sends to child 1 via pipe, receiving contents in return.
	*/

	//Child AKA child 2
	if (pid == 0) {
		//Assign child 2 to directory 2, then add file names to list and print
		char* directory = "d2"; 
		char main[1000];
		sprintf(main, "%s.txt", directory);
		sprintf(txt, "%s.txt", directory);
		sprintf(command, "ls %s >> %s.txt", directory, directory);
		system(command); 
		
		//Child sends list to other child
		if (write(p2[1], txt, 7) < 0){ //write to pipe p2
			return 1;
		}
		if (read(p1[0], txt, 7) < 0) { //read from pipe p1
			return 1;
		}

	
		//Translate contents into variable for parsing
		FILE *fpipe;
		sprintf(command, "cat %s", main);
		char *filecnt = command;
		char c = 0;

		if (0 == (fpipe = (FILE*)popen(filecnt, "r"))) {
			exit(EXIT_FAILURE);
		}

		while (fread(&c, sizeof c, 1, fpipe)) {
			//printf("%c", c);
			sprintf(snt, "%s%c", snt, c);
			
		}
		pclose(fpipe);

		//Getting length of contents to send to other child, then sending lengths
		int len2 = strlen(snt);
		int *rlen2 = malloc(sizeof *rlen2);
		*rlen2 = len2;

		if (write(p2[1], &len2, sizeof(len2)) < 0){ 
			return 1;
		}
		if (read(p1[0], &len2, sizeof(len2)) < 0) { 
			return 1;
		}

		char rcv[len2 + 1];

		//Pipe file list to other child using received lengths
		if (write(p2[1], snt, *rlen2 + 1) < 0){
			return 1;
		}
		if (read(p1[0], rcv, len2 + 1) < 0) {
			return 1;
		}

		//Retrieve number of files in sent(snt) list and received(rcv) list
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

		//Separating SNT and RCV files into an array to parse each file name separately
		int f = 0;
		char *s = strtok (snt, "\n");
		char *d2array[d2count];

		while (s != NULL) {
			d2array[f++] = s;
			s = strtok (NULL, "\n");
		}

		int d = 0;
		char *t = strtok (rcv, "\n");
		char *d1array[d1count];
		while (t != NULL) {
			d1array[d++] = t;
			t = strtok (NULL, "\n");
		}

		//Creating missing files in directory 2
		bool found = false;
		char addfile[1000];
		for(int i = 0; i < d1count; i++){
			for(int j = 0; j < d2count; j++){
				//If lengths do not match, neither can the files
				if((strlen(d1array[i]) == strlen(d2array[j])) && !memcmp(d1array[i], d2array[j], strlen(d1array[i]))) {
					found = true;
				}
				//If match is found between both directories, no action
				if(found){
					break;
				}
			}
			//If no match is found, file will be created in child's corresponding directory
			if(!found){
				sprintf(addfile, "touch %s/%s", directory, d1array[i]);
				system(addfile);
			}
			found = false;
		}
		//erase contents of file, then add names of all empty files and send file to other child
		sprintf(missing_files, ": > %s.txt | find \"%s\" -size 0 | cut -c 4- >> %s.txt", directory, directory, directory);
		system(missing_files);
		sprintf(txt, "%s.txt", directory);
		
		if (write(p2[1], txt, 7) < 0){
			return 1;
		}
		if (read(p1[0], txt, 7) < 0) {
			return 1;
		}
		
		//Translate file of names of empty files to variable for parsing
		FILE *fpipe4;
		sprintf(command, "cat %s", txt);
		char *filecnt4 = command;
		char c4 = 0;

		if (0 == (fpipe4 = (FILE*)popen(filecnt4, "r"))) {
			exit(EXIT_FAILURE);
		}

		while (fread(&c4, sizeof c4, 1, fpipe4)) {
			sprintf(m2, "%s%c", m2, c4);
		}
		pclose(fpipe4);

		//Count number of file names in file
		int l, m2count = 0;
		for(l = 0; m2[l]; l++) {
			if(m2[l] == '\n') {
			m2count++;
			}
		}
		
		//Separate file names into an array
		int ff = 0;
		char *ss = strtok (m2, "\n");
		char *m2array[m2count];
		while (ss != NULL) {
			m2array[ff++] = ss;
			ss = strtok (NULL, "\n");
		}
		
		//Rewrite file with file names and contents of files, then send to other child
		sprintf(command, ": > %s", txt);
		system(command);
		char appender[1000];
		for(int i = 0; i < m2count; i++){
			sprintf(appender, "echo %s >> %s", m2array[i], txt);
			system(appender);
			sprintf(appender, "echo $(cat %s/%s) >> %s", directory, m2array[i], txt);
			system(appender);
		}
		if (write(p2[1], txt, 7) < 0){ //write to pipe p2
			return 1;
		}
		if (read(p1[0], txt, 7) < 0) { //read from pipe p1
			return 1;
		}
	
		//Translate file of file names and contents to variable for parsing
		FILE *fpipe6;
		sprintf(command, "cat %s", txt);
		char *filecnt6 = command;
		char c6 = 0;

		if (0 == (fpipe6 = (FILE*)popen(filecnt6, "r"))) {
			exit(EXIT_FAILURE);
		}

		while (fread(&c6, sizeof c6, 1, fpipe6)) {
			sprintf(mf2, "%s%c", mf2, c6);
		}
		pclose(fpipe6);

		//Count number of entities in file and separate file names and contents into an array
		int ll, mf2count = 0;
		for(ll = 0; mf2[ll]; ll++) {
			if(mf2[ll] == '\n') {
			mf2count++;
			}
		}
		
		int fff = 0;
		char *sss = strtok (mf2, "\n");
		char *mf2array[mf2count];

		while (sss != NULL) {
			mf2array[fff++] = sss;
			sss = strtok (NULL, "\n");
		}
		
		//Fill empty files with received contents
		char updater[1000];
		for(int i = 0; i < mf2count; i = i + 2){
			sprintf(updater, "printf %s > %s/%s", mf2array[i + 1], directory, mf2array[i]);
			system(updater);
		}
		
	}	
	//==============================End Child 2 - Begin Child 1=====================================//
	
	/*
	 - Begin by assigning directory 1 to child 1
	 - Files present in directory 'd1' are added to list 'd1.txt' by name
	 - Child 1 sends list 'd1.txt' to child 2 via pipe, receiving 'd2.txt' via pipe in return
	 - Child 1 translates 'd1.txt' into a variable before sending it to child 2 via pipe, 
	 	receiving 'd2.txt' as a variable in return
	 - Separate file names in list into an array for parsing
	 - Identify files in directory 'd2' missing from directory 'd1' and create them in directory 'd1'
	 - Send list of files missing contents to child 1 via pipe, receiving list in return
	 - Child 1 retrieves contents for list of files and sends to child 2 via pipe, receiving contents in return.
	*/

	//main AKA child 1
	else {
		//assign main process to directory 1 and add file names to list and print
		char* directory = "d1"; 
		char main[1000];
		sprintf(main, "%s.txt", directory);
		sprintf(txt, "%s.txt", directory);
		sprintf(command, "ls %s >> %s", directory, txt);
		system(command); 
		
		//Child sends list to other child
		if (write(p1[1], txt, 7) < 0){ //write to pipe p1
			return 1;
		}
		if (read(p2[0], txt, 7) < 0) { //read from pipe p2
			return 1;
		}

		//Translate contents into variable for parsing
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

		//Getting length of contents to send to other child, then sending lengths
		int len = strlen(snt);
		int *rlen = malloc(sizeof *rlen);
		*rlen = len;
		if (write(p1[1], &len, sizeof(len)) < 0){
			return 1;
		}
		if (read(p2[0], &len, sizeof(len)) < 0) {
			return 1;
		}

		char rcv[len];

		//Pipe file list to other child using received lengths
		if (write(p1[1], snt, *rlen + 1) < 0){
			return 1;
		}
		if (read(p2[0], rcv, len + 1) < 0) {
			return 1;
		}
		
		//Retrieve number of files in sent(snt) list and received(rcv) list
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

		//Separating SNT and RCV files into an array to parse each file name separately
		int c = 0;
		char *s = strtok (snt, "\n");
		char *d1array[d1count];

		while (s != NULL) {
			d1array[c++] = s;
			s = strtok (NULL, "\n");
		}

		int d = 0;
		char *t = strtok (rcv, "\n");
		char *d2array[d2count];
		while (t != NULL) {
			d2array[d++] = t;
			t = strtok (NULL, "\n");
		}

		//Creating missing files in directory 1
		char addfile[1000];
		bool found = false;
			for(int i = 0; i < d2count; i++){
				for(int j = 0; j < d1count; j++){
					//If lengths do not match, neither can the files
					if((strlen(d2array[i]) == strlen(d1array[j])) && !memcmp(d2array[i], d1array[j], strlen(d1array[i]))) {
						found = true;
					}
					//If match is found between both directories, no action
					if(found) {
						break;
					}
				}
				//If no match is found, file will be created in child's corresponding directory
				if(!found) {
					sprintf(addfile, "touch %s/%s", directory, d2array[i]);
					system(addfile);
				}
				found = false;
			}

		//erase contents of file, then add names of all empty files and send file to other child
		sprintf(missing_files, ": > %s.txt | find \"%s\" -size 0 | cut -c 4- >> %s.txt", directory, directory, directory);
		system(missing_files);
		sprintf(txt, "%s.txt", directory);
		if (write(p1[1], txt, 7) < 0){ 
		return 1;
		}
		if (read(p2[0], txt, 7) < 0) { 
			return 1;
		}

		//Translate file of names of empty files to variable for parsing
		FILE *fpipe3;
		sprintf(command, "grep txt %s", txt);
		char *filecnt3 = command;
		char c3 = 0;

		if (0 == (fpipe3 = (FILE*)popen(filecnt3, "r"))) {
			exit(EXIT_FAILURE);
		}

		while (fread(&c3, sizeof c3, 1, fpipe3)) {
			sprintf(m1, "%s%c", m1, c3);
		}
		pclose(fpipe3);
	
		//Count number of file names in file
		int k, m1count = 0;
		for(k = 0; m1[k]; k++) {
			if(m1[k] == '\n') {
			m1count++;
			}
		}

		//Separate file names into an array
		int ee = 0;
		char *tt = strtok (m1, "\n");
		char *m1array[m1count];

		while (tt != NULL) {
			m1array[ee++] = tt;
			tt = strtok (NULL, "\n");
		}

		//Rewrite file with file names and contents of files, then send to other child
		sprintf(command, ": > %s", txt);
		system(command); 
		char appender[1000];
		for(int i = 0; i < m1count; i++){
			sprintf(appender, "echo %s >> %s", m1array[i], txt);
			system(appender);
			sprintf(appender, "echo $(cat %s/%s) >> %s", directory, m1array[i], txt);
			system(appender);
		}
	
		if (write(p1[1], txt, 7) < 0){ 
		return 1;
		}
		if (read(p2[0], txt, 7) < 0) { 
			return 1;
		}
		
		//Translate file of file names and contents to variable for parsing
		FILE *fpipe5;
		sprintf(command, "cat %s", txt);
		char *filecnt5 = command;
		char c5 = 0;

		if (0 == (fpipe5 = (FILE*)popen(filecnt5, "r"))) {
			exit(EXIT_FAILURE);
		}

		while (fread(&c5, sizeof c5, 1, fpipe5)) {
			sprintf(mf1, "%s%c", mf1, c5);
		}
		pclose(fpipe5);

		//Count number of entities in file and separate file names and contents into an array
		int kk, mf1count = 0;
		for(kk = 0; mf1[kk]; kk++) {
			if(mf1[kk] == '\n') {
			mf1count++;
			}
		}

		int eee = 0;
		char *ttt = strtok (mf1, "\n");
		char *mf1array[mf1count];

		while (ttt != NULL) {
			mf1array[eee++] = ttt;
			ttt = strtok (NULL, "\n");
		}

		//Fill empty files with received contents
		char updater[1000];
		for(int i = 0; i < mf1count; i = i + 2){
			sprintf(updater, "printf %s > %s/%s", mf1array[i + 1], directory, mf1array[i]);
			system(updater);
		}
	
	}
	//===================================End Child 1===========================================//
	if(pid != 0){
		printf("\n\n");
		sleep(1);
		system("ls d1 d2");
		printf("\n\nTransfer complete.");
		
	}
	
	return 0;
	
}