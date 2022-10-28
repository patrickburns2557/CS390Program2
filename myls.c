/*
Patrick Burns
October 27, 2022
Class: CS 390
Assignment: Program 2
Program: My ls program
*/
#include <dirent.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{
    DIR *dir;
    struct dirent *direntry;
    char aArgPresent;
    char* path;
    int i = 0;

    /* iterate through the args and test if -a is found*/
    for(i = 0; i < argc; i++)
    {
        if(!strcmp(argv[i], "-a"))
        {
            aArgPresent = 1;
        }
    }

    /* iterate through args and print out the files for each directory given */
    /*  EX: if the command given is "myls . / /usr", the program will list files for
        the current directory, the root directory, and /usr directory */
    for(i = 0; i < argc; i ++)
    {
        path = argv[i];
        
        /*check if there are no args given, or if the only arg given was -a.
        If either of these are true, then print out the current directory */
        if(argc == 1 || (i == 0 && argc == 2 && aArgPresent))
            path = ".";
        
        /* if the path doesn't exist, skip to next arg*/
        if((dir = opendir(path)) == NULL)
            continue;


        printf("%s:\n", path);
        /* iterate through the directory and print each file name */
        while ((direntry = readdir(dir)) != NULL)
        {
            char* name = direntry->d_name;

            /* only print files that start with '.' if -a was given */
            if(name[0] != '.' || aArgPresent)
            {
                printf("%10d %s\n", (int)direntry->d_ino, direntry->d_name);
            } 
        }
        printf("\n");
    }
    
    closedir(dir);

    return 0;
}

