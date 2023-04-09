#include <stdio.h>
#include <dirent.h>
#include <string.h>

int main()
{
    DIR *dir;
    struct dirent *dp;
    char * file_name;
    dir = opendir("/home/zjw/lectures/os-workbench/pstree");
    while ((dp=readdir(dir)) != NULL) {
        
        file_name = dp->d_name; 
        printf("d_type is %d\n", dp->d_type);
        printf("The File Name :-  \"%s\"\n",file_name);
    }
    closedir(dir);
    return 0;
}