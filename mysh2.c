/*
Patrick Burns
October 24, 2022
Class: CS 390
Assignment: Program 2
Program: My Shell
*/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#include "sys/stat.h"
#include "unistd.h"
#include "wait.h"
#define MAX_TOKENS 128

/* Function Prototypes */
void command_echo(char* tokens[MAX_TOKENS], int numTokens);
void command_ps1(char* tokens[MAX_TOKENS], int numTokens);
void command_cat(char* tokens[MAX_TOKENS], int numTokens);
void command_cp(char* tokens[MAX_TOKENS], int numTokens);
void command_rm(char* tokens[MAX_TOKENS], int numTokens);
void command_mkdir(char* tokens[MAX_TOKENS], int numTokens);
void command_rmdir(char* tokens[MAX_TOKENS], int numTokens);
void command_exec(char* tokens[MAX_TOKENS], int numTokens);
char* getPath(char* input);


/* Global Variable */
char ps1[1024] = "$";


int main(int argc, char* argv[])
{
	char userInput[1024];
	char* token;
	char* tokens[MAX_TOKENS];
	int numTokens = 0;
	int i = 0;
	
	do
	{
		/* reset tokens back to zero for next input */
		numTokens = 0;
		
		/* print out the ps1 variable and get user input */
		printf("%s ", ps1);
		fgets(userInput, 1024, stdin);

		/* tokenize and store input into tokens array */
		token = strtok(userInput, " ");
		while(token != NULL)
		{
			if(numTokens < (MAX_TOKENS - 1))
			{
				tokens[numTokens++] = token;
				token = strtok(NULL, " ");
			}
			else
			{
				fprintf(stderr, "Too many arguments, max is: %d\n", MAX_TOKENS);
				exit(-1);
			}
		}

		/* Replace the newline character in the last token with a 0 */
		for(i = 0;;i++)
		{
			if(tokens[numTokens-1][i] == '\n')
			{
				tokens[numTokens-1][i] = 0;
				break;
			}
		}
		

		if (strcmp(tokens[0], "echo") == 0)
		{
			command_echo(tokens, numTokens);
		}
		else if ((strcmp(tokens[0], "ps1") == 0) || (strcmp(tokens[0], "PS1") == 0)) /* allow user to use "ps1" or "PS1" */
		{
			command_ps1(tokens, numTokens);
		}
		else if (strcmp(tokens[0], "cat") == 0)
		{
			command_cat(tokens, numTokens);
		}
		else if (strcmp(tokens[0], "cp") == 0)
		{
			command_cp(tokens, numTokens);
		}
		else if (strcmp(tokens[0], "rm") == 0)
		{
			command_rm(tokens, numTokens);
		}
		else if (strcmp(tokens[0], "mkdir") == 0)
		{
			command_mkdir(tokens, numTokens);
		}
		else if (strcmp(tokens[0], "rmdir") == 0)
		{
			command_rmdir(tokens, numTokens);
		}
		else if (strcmp(tokens[0], "exit") == 0)
		{
			break;
		}
		else
		{
			command_exec(tokens, numTokens);
		}

	} while (1);

	return 0;
}

/* Function handles the echo command */
void command_echo(char* tokens[MAX_TOKENS], int numTokens)
{
	bool nArg = false; /* used to determine whether to apply -n */
	int i = 0; /* declared here since using -ansi disallows var declarations inside for loops */
	
	/* test for -n flag right after the "echo" token */
	if(strcmp(tokens[1], "-n") == 0)
	{
		nArg = true;
	}

	/* Print out the message. If "-n" was found as the token immediately after "echo", skip printing it in the output */
	if(nArg)
	{
		for(i = 2; i < numTokens; i++)
		{
			printf("%s ", tokens[i]);
		}
	}
	else
	{
		for(i = 1; i < numTokens; i++)
		{
			printf("%s ", tokens[i]);
		}
	}
	
	/* only print a new line if the -n flag wasn't found */
	if(!nArg)
	{
		printf("\n");
	}	
}

/* Function handles the PS1 command */
void command_ps1(char* tokens[MAX_TOKENS], int numTokens)
{
	char newPs1[1024] = "";
	
	int i = 0;  /* declared here since using -ansi disallows var declarations inside for loops */

	/* concatenate each token to the newPs1 string */
	for(i = 1; i<numTokens;i++)
	{
		strcat(newPs1, tokens[i]);
		if(i < numTokens-1) /* don't add a space after the last string */
		{
			strcat(newPs1, " ");
		}
	}

	/* set the new PS1 variable */
	strcpy(ps1, newPs1);

}

/* Function handles the cat command */
void command_cat(char* tokens[MAX_TOKENS], int numTokens)
{
	FILE *inputFile;
	int i = 0;
	char c = '0';

	/* Open file. If file not found, exit function */
	if ((inputFile = fopen(tokens[1], "r")) == NULL)
	{
		printf("File '%s' not found\n", tokens[1]);
		return;
	}

	/* iterate through entire file and print out its contents character by character */
	c = fgetc(inputFile);
	while(!feof(inputFile))
	{
		printf("%c", c);
		c = fgetc(inputFile);
	}


	fclose(inputFile);
}

/* Function handles the cp command */
void command_cp(char* tokens[MAX_TOKENS], int numTokens)
{
	FILE *source;
	FILE *destination;
	char c;

	/* open source file. Exit function if failed */
	source = fopen(tokens[1], "rb");
	if(source == NULL)
	{
		printf("Failed to open source file '%s'\n", tokens[1]);
		return;
	}

	/* open destination file. Close source file and exit function if failed */
	destination = fopen(tokens[2], "wb");
	if(destination == NULL)
	{
		printf("Failed to open destination file '%s'\n", tokens[2]);
		fclose(source);
		return;
	}
	
	/* write to destination file character by character until the end of source is reached */
	c = fgetc(source);
	while(!feof(source))
	{
		fputc(c, destination);
		c = fgetc(source);
	}

	printf("Successfully copied '%s' to '%s'\n", tokens[1], tokens[2]);

	fclose(source);
	fclose(destination);
}

/* Function handles the rm command */
void command_rm(char* tokens[MAX_TOKENS], int numTokens)
{
	/* remove file and print results of operation */
	if(remove(tokens[1]) == 0)
	{
		printf("Deleted '%s'\n", tokens[1]);
	}
	else
	{
		printf("Cannot remove '%s' : No such file or directory\n", tokens[1]);
	}
}

/* Function handles the mkdir command */
void command_mkdir(char* tokens[MAX_TOKENS], int numTokens)
{
	/* uses stat call to see if the directory exists or not. If it doesn't, it creates the directory */
	struct stat st = {0};
	if (stat(tokens[1], &st) == -1)
	{
		mkdir(tokens[1], 0777);
		printf("Directory created\n");
	}
	else
	{
		printf("Cannot create directory. Directory already exists\n");
	}
	
}

/* Function handles the rmdir command */
void command_rmdir(char* tokens[MAX_TOKENS], int numTokens)
{
	/* remove folder and print results of operation */
	if(remove(tokens[1]) == 0)
	{
		printf("Deleted '%s'\n", tokens[1]);
	}
	else
	{
		printf("Cannot remove '%s' : Either no such directory, or directory is not empty\n", tokens[1]);
	}
}

/* Function handles executing external programs*/
void command_exec(char* tokens[MAX_TOKENS], int numTokens)
{
	/* puts a NULL pointer at the end of the argument array
	   because execv( ) expects this to appear */
	tokens[numTokens] = NULL;
	char* path;

	/* if a '/' is found anywhere in the input command, it suggests it's an absolute path*/
	/* don't execute getPath if an absolute path to a program is input */
	if(strchr(tokens[0], '/') != NULL)
	{
		path = tokens[0];
	}
	else
	{
		path = getPath(tokens[0]);
	}
	
	/* if getPath returns "NOT FOUND" the child process will
	simply print that the command wasn't found and exit*/
	switch(fork())
	{
		case -1: exit(100);
		case 0: 
			if(execv(path, tokens) < 0)
			{
				fprintf(stderr, "Command not found\n");
				exit(200);
			}
		default:
			wait(NULL); /* wait for the forked process to finish before continuing */
	}
	
}

/* Function handles finding external programs and returns their path if found
   if not found, returns the string "NOT FOUND"*/
char* getPath(char* input)
{
	char path[2048];
	char *pathToken;
	char temppath[128], pathArray[100][1000];
	int numPaths = 0, i;
	
	strcpy(path, getenv("PATH"));

	/* tokenize the returned path variable */
	pathToken = strtok(path, ":");
	while(pathToken != NULL)
	{
		strcpy(pathArray[numPaths++], pathToken);
		pathToken = strtok(NULL, ":");
	}
	
	/* iterate through the path variables, appending the name of the program to the end of each
	   and test if the program is found. Return the path of the program if found*/
	for(i = 0; i < numPaths; i++)
	{
		strcpy(temppath, pathArray[i]);
		strcat(temppath, "/");
		strcat(temppath, input);
		if(access(temppath, F_OK) == 0)
		{
			/* must copy the path to a pointer because the array is stored
			   on the stack and will be destroyed after the function ends*/
			pathToken = temppath; 
			return pathToken;
		}
	}
	/* return if program not found*/
	return "NOT FOUND";

}