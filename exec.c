#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include "history.h"
#include <string.h>
#include <ctype.h>
#include <fcntl.h> // for open
#include "parser.h"
#include "exec.h"

const char* specialcommands[] = {"history", "exit", "cd"};
const char* normalsymbols[] = {";", ">", "&&", "<"};
const int numnormalsymbols = 4;
const char* allsymbols[] = {";", ">", "&&", "<", "|", "&"};
const int specialcommandlength = 3;


void printPID() { printf("PID:= %d, ", getpid()); }

int singleexec(char** argv) {
	int rc = fork();
	if(rc < 0) {
		printf("parent ");
        printPID();
		printf(": unable to spawn child process\n");
	}
	else if(rc == 0) {
		printPID();
		execvp(argv[0], argv);
        printf("%s: command not found\n", argv[0]);
		exit(1);
	}
	else {
		int status;
		if(waitpid(rc, &status, 0) == -1) {
			printf("error terminating child process\n");
			return 0;
		}
		if(WIFEXITED(status)) {
			int i = WEXITSTATUS(status);
			printf("child exited with status %d\n", i);
			if(status == 0) {
				return 1;
			}
			else {
				return 0;
			}
		}
	}
}

//helper function for multipipeexec
int pipedexec(int fdin, int fdout, char** argv) {
	int rc = fork();
	if(rc < 0 ) {
		printf("Error: forking failed\n"); 
	}
	else if(rc == 0){
		if( fdin != 0 ){
			dup2(fdin, 0);
			close(fdin);
		}
		if(fdout != 1) {
			dup2 (fdout, 1);
			close (fdout);
		}

		return execvp(argv[0], argv);
	}
}

//this function is a mess and needs refactoring
//****wrapper for pipedexec*****
//start: first index in argvs that is piped (outputs only)
//end: last index in arvs that is piped (receives only)
int multipipeexec(int start, int end, char*** argvs, cmdtype type){
	int length = end - start;
	int fdin, fd[2];

	fdin = 0;
	for(int i = start; i < end; i+=2) {
		pipe(fd);
		pipedexec(fdin, fd[1], argvs[i]);
		close(fd[1]);
		fdin = fd[0];
	}

	//break loop to do the last one
	if(type == AMPERSAND) {
		if(fork() == 0) {
			if(fdin != 0) {
				dup2(fdin, 0);
			}
			execvp(argvs[end][0], argvs[end]);
	    }
	}
	else if(type == NORMAL) {
		int rc = fork();
		if (rc == 0) {
			if(fdin != 0) {
				dup2(fdin, 0);
			}
			execvp(argvs[end][0], argvs[end]);
		}
		else if(rc > 0) {
			int status;
			if(waitpid(rc, &status, 0) == -1) {
				printf("error terminating child process\n");
				return 0;
			}
			if(WIFEXITED(status)) {
				int i = WEXITSTATUS(status);
				printf("child exited with status %d\n", i);
				return 1;
				if(status == 0) {
					return 1;
				}
				else {
					return 0;
				}
			}
		}
	}
	else {
		int rc = fork();
		if(rc == 0) {
			//end + 1 should be index of file to out to
			int file = open(argvs[end+2][0], O_RDWR | O_CREAT, 0666);
			if(file != -1) {
				if(fdin != -1)
					dup2(fdin, 0);
				dup2(file, 1); 
				close(file);
				close(fdin);
				execvp(argvs[start][0], argvs[start]);
				printf("Error: could not find \"%s\"\n", argvs[start][0]);
			}
			else {
				printf("IO Error: %s failed to open\n", argvs[start+2][0]);
			}
		}
		else if(rc > 0) {
			int status;
			if(waitpid(rc, &status, 0) == -1) {
				printf("error terminating child process\n");
				return 0;
			}
			if(WIFEXITED(status)) {
				int i = WEXITSTATUS(status);
				printf("child exited with status %d\n", i);
				return 1;
				if(status == 0) {
					return 1;
				}
				else {
					return 0;
				}
			}
		}
	}
}


int backgroundexec(char** argv) {
	int rc = fork();
	if(rc < 0) {
		printf("parent ");
        printPID();
		printf("forking failed\n");
	}
	else if(rc == 0) {
		execvp(argv[0], argv);
        printf("Error: could not find \"%s\"\n", argv[0]);
	}
	else {
		printf("back to parent of backgroundexec\n");
		return 1;
	}
}

//this function will operate on:
//argvs[start]: the command
//argvs[start + 1]: the ">" symbol
//argvs[start + 2]: the file to write to;
int outfileexec(int start, char*** argvs, cmdtype type) {
	//assume only one string for the filename: project says to not worry about quotes
	//which is the only reason there would be more than one word in the file name
	int rc = fork();
	if (rc < 0) {
		printf("parent ");
        printPID();
		printf(": unable to spawn child process\n");
	}
	else if(rc == 0) {
		int file = open(argvs[start+2][0], O_RDWR | O_CREAT, 0666);
		if(file != -1) {
			dup2(file, 1); 
			close(file);
			execvp(argvs[start][0], argvs[start]);
			printf("Error: could not find \"%s\"\n", argvs[start][0]);
		}
		else {
			printf("IO Error: %s failed to open", argvs[start+2][0]);
		}
		
	}
	else {
		if(type == NORMAL) {
			int status;
			if(waitpid(rc, &status, 0) == -1) {
				printf("error terminating child process\n");
				return 0;
			}
			if(WIFEXITED(status)) {
				int i = WEXITSTATUS(status);
				printf("child exited with status %d\n", i);
				if(status == 0) {
					return 1;
				}
				else {
					return 0;
				}
			}
		}//end type normal, run in foreground
		//this function expects me to return something so just return true-
		//it doesn't really matter, since the following command is & anyways
		//which we used here already
		return 1;
	}
}

//same mojo as above function
int infileexec(int start, char*** argvs, cmdtype type) {
	//assume only one string for the filename: project says to not worry about quotes
	//which is the only reason there would be more than one word in the file name
	int rc = fork();
	if (rc < 0) {
		printf("parent ");
        printPID();
		printf(": unable to spawn child process\n");
	}
	else if(rc == 0) {
		int file = open(argvs[start+2][0], O_RDONLY);

		dup2(file, 0); 
		close(file);
		execvp(argvs[start][0], argvs[start]);
		printf("Error: could not find \"%s\"\n", argvs[start][0]);
	}
	else {
		if(type == NORMAL) {
			int status;
			if(waitpid(rc, &status, 0) == -1) {
				printf("error terminating child process\n");
				return 0;
			}
			if(WIFEXITED(status)) {
				int i = WEXITSTATUS(status);
				printf("child exited with status %d\n", i);
				if(status == 0) {
					return 1;
				}
				else {
					return 0;
				}
			}
		}//end type normal, run in foreground
		//this function expects me to return something so just return true-
		//it doesn't really matter, since the following command is & anyways
		//which we used here already
		return 1;
	}
}

signals isbuiltin(char** argv) {
	if(strcmp(argv[0], "exit") == 0 ) {
		return QUIT;
	}
	else if(strcmp(argv[0], "history") == 0) {
		return HISTORY;
	}
	else if(strcmp(argv[0], "cd") == 0) {
		return CD;
	}
	return NO;
}


char* removenewline(char* str) {
	int i = 0;
	int length = 0;
	while(str[length] != '\0') {
		length++;
	}
	char* toreturn = (char*)malloc((length+1) * sizeof(char));
	while(str[i]!='\0') {
		if(str[i] == '\n') {
			toreturn[i] = '\0';
		}
		else {
			toreturn[i] = str[i];
		}
		i++;
	}
	toreturn[i] = '\0';
	return toreturn;
}

bool issymbol(char* str) {
	char* syms[] = {"<", ">", "&", "&&", ";", "|"};
	for(int i =0; i < 6; i ++) {
		if(strcmp(str, syms[i]) == 0) {
			return true;
		}
	}
	return false;
}

/*the meat of the program, after a command is parsed,
go here to actually run it

possibly the ugliest function in the entire program:
structured as a bunch of if elses for what symbol is modifying
which arg

acts as an FSM where we look at what the next symbol is, and if the 
previous symbol is a pipe, ampersand, or double ampersand
*/
char** execmulticmd(int numcmds, char*** fullcmd) {
    //always look ahead one symbol before executing the current command
    //in the case of | or & keep looking ahead until we find a different symbol
    int startoffset = 0; //maintain where to start executing in segment of cmd
    int returnstatus = 1;
    bool pipeflag = false, andflag = false, redirectflag; 
    for(int i = 0; i < numcmds; i++) {
		signals x = isbuiltin(fullcmd[i]);
		if(x != NO) {
			return fullcmd[i];
		}
		if(strcmp(fullcmd[i][0], ";") == 0) {
            if(andflag) {
                if(returnstatus) {
                    singleexec(fullcmd[i-1]);
                }
                andflag = false;
            } else if(pipeflag) {
                returnstatus = multipipeexec(startoffset, i - 1, fullcmd, NORMAL);
				pipeflag = false;
            } else {
				returnstatus = singleexec(fullcmd[i-1]);
			}
        } 
		//for "<" and ">" process commands to the left and right of the symbol
		//since it's really just one command that is redirected elsewhere
		else if(strcmp(fullcmd[i][0], ">") == 0) {
            if(andflag) {
                if(returnstatus){
                    returnstatus = outfileexec(i -1, fullcmd, NORMAL);
					i++;
                }
            } else if(pipeflag) {
				returnstatus = multipipeexec(startoffset, i-1, fullcmd, OUTREDIRECT);
				pipeflag = false;
				i++; //skip past next arg which is the file name
            } else if(i+2 < numcmds && strcmp(fullcmd[i+2][0], "&") == 0) {	
				returnstatus = outfileexec(i -1, fullcmd, AMPERSAND);
				i+=2;//skip past file name and &
            }
			else {
				returnstatus = outfileexec(i-1, fullcmd, NORMAL);
				i++;
			}
			redirectflag = true;
        } else if(strcmp(fullcmd[i][0], "<") == 0) {
            if(andflag) {
                if(returnstatus){
                    returnstatus = infileexec(i -1, fullcmd, NORMAL);
					i++;
                }
            } else if(pipeflag) {
				printf("Error: cannot redirect input to file because pipe already exists\n");
				break;
            } else if(i + 2 < numcmds && strcmp(fullcmd[i+2][0], "&") == 0) {	
				returnstatus = infileexec(i -1, fullcmd, AMPERSAND);
				i+=2;//skip past file name and &
            }
			else {
				returnstatus = infileexec(i-1, fullcmd, NORMAL);
				i++;
			}
			redirectflag = true;
        } else if(strcmp (fullcmd[i][0], "&&") == 0) {
            if(andflag) {
                if(returnstatus) {
                    returnstatus = singleexec(fullcmd[i-1]);
                }
            }
            else if(pipeflag) {
                returnstatus = multipipeexec(startoffset, i - 1, fullcmd, NORMAL);
                pipeflag = false;
            }else {
				returnstatus = singleexec(fullcmd[i-1]);
			}
            andflag = true;
        } else if(strcmp(fullcmd[i][0], "|") == 0){
            //dont run anything until we run out of pipes
            if(andflag) {
                if(returnstatus) {
					pipeflag = true;
					startoffset = i-1;
                }
            }
            else if(!pipeflag){
                pipeflag = true;
                startoffset = i - 1;
            }

        } else if(strcmp(fullcmd[i][0], "&") == 0) {
            if(andflag){
                if(returnstatus){
					backgroundexec(fullcmd[i-1]);
				}
            }
            else if(pipeflag){
                pipeflag = false;
                multipipeexec(startoffset, i -1 , fullcmd, AMPERSAND);
            }
			else {
				backgroundexec(fullcmd[i-1]);
			}
        } 
    }//end for loop through the arv structure
	if(pipeflag) {
		pipeflag = false;
		multipipeexec(startoffset, numcmds - 1, fullcmd, NORMAL);
	}
	else if (andflag){
		if(returnstatus)
			returnstatus = singleexec(fullcmd[numcmds - 1]);
	}
	else if(!redirectflag && !issymbol(fullcmd[numcmds - 1][0])) {
		returnstatus = singleexec(fullcmd[numcmds - 1]);
	}

	return NULL;
}
