#include <dirent.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    DIR *dir;
    struct dirent *direntry;

    if ( (dir = opendir("/")) == NULL)
    {
        printf("bruh\n");
        exit(0);
    }

    while ((direntry = readdir(dir)) != NULL)
    {
        printf("%10d %s\n", direntry->d_ino, direntry->d_name);
    }
    closedir(dir);

    return 0;
}

