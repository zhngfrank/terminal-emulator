#ifndef EXEC_H_
#define EXEC_H_

enum cmdtype {AMPERSAND, NORMAL, OUTREDIRECT};
typedef enum cmdtype cmdtype;
typedef enum signals {QUIT, HISTORY, CD, NO} signals;

int singleexec(char** argv);

//helper function for multipipeexec
int pipedexec(int fdin, int fdout, char** argv);

//****wrapper for pipedexec*****
//start: first index in argvs that is piped (outputs only)
//end: last index in arvs that is piped (receives only)
int multipipeexec(int start, int end, char*** argvs, cmdtype type);

int backgroundexec(char** argvs);

signals isbuiltin(char** argv);

int builtinexec(char** argv);

char* removenewline(char* str);

char** execmulticmd(int numcmds, char*** argvs);

int outfileexec(int start, char*** argvs, cmdtype type);

int infileexec(int start, char*** argvs, cmdtype type);

#endif