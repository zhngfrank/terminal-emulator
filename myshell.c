/*
*
* Author: Frank Zhang
* Project 1
* Operating Systems CS 255
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <ctype.h>
#include "history.h"
#include <string.h>
#include "parser.h"
#include "exec.h"

const int STRLENGTH = 2048;
const int PATHLENGTH = 512;
//globals:
ll history; //this makes managing it easier since its shared between all functions
char* clear = "history -c\0"; //need this to exist through life of program

char* exechistory(char* returnstatus, char* lastcommand) {
	printf("in history\n");
	if(returnstatus == NULL) {
		printall(&history);
		printf("outside of print history\n");
	}
	else if(strcmp(returnstatus, "-c" ) == 0) {
		int len = getlength(lastcommand);
		char* cmd = (char*)malloc((len+1)*sizeof(char));
		strcpy(cmd, lastcommand);
		empty(&history);
		createll(&history);
		push_back(&history, cmd);
	}
	else {
		int i = 0, validnum = 1;
		while(returnstatus[i] != '\0') {
			if(!isdigit(returnstatus[i])) {
				validnum = 0;
			}
			i++;
		}
		if (validnum != 0) {
			printf("valid num detected\n");
			char* str = accessLL(&history, atoi(returnstatus))->str;
			int len = getlength(str);
			char* cmd = (char*)malloc((len+1)*sizeof(char));
			strcpy(cmd, str);
			return cmd;
		}
		else {
			printf("offset '%s' is not valid\n", returnstatus);
		}
	}
	printf("returning form exechistory\n");
	return NULL;
}


int main(int argc, char ** argv) {
	bool keeprunning = true;
	char* nextcmd = NULL; //always free this, it should only be dynamically allocated
	char cmd[STRLENGTH];
	char path[PATHLENGTH];
	createll(&history);

	char*** fullcmd;
	while(keeprunning) {
		if(getcwd(path, PATHLENGTH) != NULL) {
			printf("%s$ ", path);
			char* command; //never allocate memory for this because it won't be freed
			if(nextcmd == NULL) {
				fgets(cmd, STRLENGTH, stdin);
				command = copystr(removenewline(cmd));
				if(command[0] == '\0') {
					continue;
				}
			}
			else {
				strcpy(command, nextcmd);
				free(nextcmd);
				nextcmd = NULL;
				
			}
			push_back(&history, command);

			fullcmd = getargs(command);
			int err = verifyinput(fullcmd);
			if(err != -1) {
				printf("-myshell: syntax error near unexpected token '%s'\n", fullcmd[err][0]);
				continue;
			}
			int numcmds = gettriplestarlength(fullcmd);
			if(numcmds > 1) {
				char** returnstatus = execmulticmd(numcmds, fullcmd);
				if( returnstatus != NULL){
					if(strcmp(returnstatus[0], "exit")==0){
						break;
					}
					else if(strcmp(returnstatus[0], "history") ==0) {
						nextcmd = exechistory(returnstatus[1], command);
					}
					else if(strcmp(returnstatus[0], "cd") == 0) {
						printf("changing directories\n");
						if(returnstatus[1] != NULL)
							chdir(returnstatus[1]);
					}
				}
			}
			else if(numcmds == 1) {
				signals x = isbuiltin(fullcmd[0]);
				if(x!= NO) {
					char** returnstatus = fullcmd[0];
					if(strcmp(returnstatus[0], "exit")==0){
						break;
					}
					else if(strcmp(returnstatus[0], "history") ==0) {
						nextcmd = exechistory(returnstatus[1], command);
					}
					else if(strcmp(returnstatus[0], "cd") == 0) {
						printf("changing directories\n");
						if(returnstatus[1] != NULL)
							chdir(returnstatus[1]);
					}
				}
				else {singleexec(fullcmd[0]);}
			}
		}//end if we get the path, if not loop back around and attempt to recover
		deleteargvs(fullcmd);
	}//end while

	return 0;
}

