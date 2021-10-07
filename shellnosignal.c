// C Program to design a shell in Linux 
#include<stdio.h>
#include<string.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<sys/wait.h> 
#include<readline/readline.h> 
#include<readline/history.h> 

#define MAXCOM 512 // max number of letters to be supported 
#define MAXLIST 100 // max number of commands to be supported 

// Clearing the shell using escape sequences 
#define clear() printf("\033[H\033[J") 

// Function to take input 
int takeInput(char* str) 
{ 
	char* buf; 

	buf = readline("\n>>> "); 
	if (strlen(buf) != 0) { 
		add_history(buf); 
		strcpy(str, buf); 
		return 1;
	} else { 
		return 0; 
	} 
} 

// Function to print Current Directory. 
void printDir() 
{ 
	char cwd[1024]; 
	getcwd(cwd, sizeof(cwd)); 
	printf("\nDir: %s", cwd); 
} 

// Function where the system command is executed 
void execArgs(char** parsed) 
{
	// Forking a child 
	pid_t pid = fork(); 

	if (pid == -1) { 
		printf("\nFailed forking child.."); 
		return; 
	} else if (pid == 0) { 
		if (execvp(parsed[0], parsed) < 0) { 
			printf("\nCould not execute command.."); 
		} 
		exit(0); 
	} else { 
		// waiting for child to terminate 
		wait(NULL); 
		return; 
	} 
} 

// Function where the piped system commands is executed 
void execArgsPiped(char** parsed, char** parsedpipe) 
{ 
	// 0 is read end, 1 is write end 
	int pipefd[2]; 
	pid_t p1, p2; 

	if (pipe(pipefd) < 0) { 
		printf("\nPipe could not be initialized"); 
		return; 
	} 
	p1 = fork(); 
	if (p1 < 0) { 
		printf("\nCould not fork"); 
		return; 
	} 

	if (p1 == 0) { 
		// Child 1 executing.. 
		// It only needs to write at the write end 
		
		close(pipefd[0]); 
		dup2(pipefd[1], STDOUT_FILENO); 
		close(pipefd[1]); 

		if (execvp(parsed[0], parsed) < 0) { 
			printf("\nCould not execute command 1.."); 
			exit(0); 
		} 
	} else { 
		// Parent executing 
		p2 = fork(); 

		if (p2 < 0) { 
			printf("\nCould not fork"); 
			return; 
		} 

		// Child 2 executing.. 
		// It only needs to read at the read end 
		if (p2 == 0) { 
			close(pipefd[1]); 
			dup2(pipefd[0], STDIN_FILENO); 
			close(pipefd[0]); 
			if (execvp(parsedpipe[0], parsedpipe) < 0) { 
				printf("\nCould not execute command 2.."); 
				exit(0); 
			} 
		} else { 
			// parent executing, waiting for two children 
			wait(NULL); 
			wait(NULL); 
		} 
	} 
} 

// Help command builtin 
void openHelp() 
{ 
	puts(
		"\nList of Commands supported:"
		"\n>cd"
		"\n>ls"
		"\n>quit"
		"\n>all other general commands available in UNIX shell"
		"\n>pipe handling"
		"\n>improper space handling"); 

	return; 
} 

// Function to execute builtin commands 
int ownCmdHandler(char** parsed) 
{ 
	int NoOfOwnCmds = 3, i, switchOwnArg = 0; 
	char* ListOfOwnCmds[NoOfOwnCmds]; 
	char* username; 

	ListOfOwnCmds[0] = "quit"; 
	ListOfOwnCmds[1] = "cd"; 
	ListOfOwnCmds[2] = "help"; 

	for (i = 0; i < NoOfOwnCmds; i++) { 
		if (strcmp(parsed[0], ListOfOwnCmds[i]) == 0) { 
			switchOwnArg = i + 1; 
			break; 
		} 
	} 

	switch (switchOwnArg) { 
		case 1: 
			printf("\nGoodbye\n"); 
			exit(0); 
		case 2: 
			chdir(parsed[1]); 
			return 1; 
		case 3:
			openHelp();
			return 1;
		default: 
			break; 
	} 

	return 0; 
} 

// function for finding pipe 
int parsePipe(char* str, char** strpiped) 
{ 
	int i; 
	for (i = 0; i < 2; i++) { 
		strpiped[i] = strsep(&str, "|"); 
		if (strpiped[i] == NULL) 
			break; 
	} 

	if (strpiped[1] == NULL) 
		return 0; // returns zero if no pipe is found. 
	else { 
		return 1; 
	} 
} 

// function for parsing command words 
void parseSpace(char* str, char** parsed) 
{ 
	int i; 

	for (i = 0; i < MAXLIST; i++) { 
		parsed[i] = strsep(&str, " "); 

		if (parsed[i] == NULL) 
			break; 
		if (strlen(parsed[i]) == 0) 
			i--; 
	} 
} 

int processString(char* str, char** parsed, char** parsedpipe) 
{ 

	char* strpiped[2]; 
	int piped = 0; 

	piped = parsePipe(str, strpiped); 

	if (piped) { 
		parseSpace(strpiped[0], parsed); 
		parseSpace(strpiped[1], parsedpipe); 

	} else { 
		parseSpace(str, parsed); 
	} 

	if (ownCmdHandler(parsed)) 
		return 0; 
	else
		return 1 + piped; 
}

char *trim(char *str)
{
	char *end;

	// Trim leading space
	while(isspace((unsigned char)*str)) str++;

	if(*str == 0)  // All spaces?
	return str;

	// Trim trailing space
	end = str + strlen(str) - 1;
	while(end > str && isspace((unsigned char)*end)) end--;

	// Write new null terminator character
	end[1] = '\0';

	return str;
}

void runCmd(char* cmd){
	char *parsedArgs[MAXLIST];
	char* parsedArgsPiped[MAXLIST];
	int execFlag = 0; 

	char *simcParsed, *string;

	while( (simcParsed = strsep(&cmd,";")) != NULL ){ // Parse semicolon
		if(strlen(trim(simcParsed))==0) continue;
		execFlag = processString(simcParsed, parsedArgs, parsedArgsPiped); 
		
		if (execFlag == 1) 
			execArgs(parsedArgs); 

		if (execFlag == 2) 
			execArgsPiped(parsedArgs, parsedArgsPiped); 
	}
}

int main(int argCount, char *args[]) 
{
  char inputString[MAXCOM];

	if(argCount > 1){ // Batch
		char filename[1000];
		char tmpCMD[1000];
		strcpy(filename, args[1]);
		FILE *fp = fopen(filename, "r");
		if(fp == NULL){
			perror("Cannot find file");
			return 0;
		}
		while(!feof(fp)){
			fgets(tmpCMD, MAXCOM, fp);
			trim(tmpCMD);
			puts(tmpCMD);
			runCmd(tmpCMD);
		}

		fclose(fp);
	}
	else{ // Interactive
		while (1) { 
			printDir(); 
			if (!takeInput(inputString)) {
				continue; 
			}
			runCmd(inputString);
		}
	}
	return 0; 
} 
