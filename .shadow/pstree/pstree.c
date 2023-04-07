#include <assert.h>
#include <dirent.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

int pid_num = 0;

struct Processinfo {
    pid_t pid, ppid;
    char name[50];
};
struct Processinfo processinfos[500];

pid_t get_stat(pid_t pid, char name[]) {
    char str[20];
    sprintf(str, "%d", pid);
    char processpath[20] = "/proc/";
    strcat(processpath, str);
    strcat(processpath, "/stat");

    FILE *fp = fopen(processpath, "r");
    if (fp) {
        pid_t _pid, _ppid;
        char stat, pname[40];
        fscanf(fp, "%d (%s %c %d", &_pid, pname, &stat, &_ppid);
        pname[strlen(pname) - 1] = '\0';
        strcpy(name, pname);
        return _ppid;
    }
    fclose(fp);
    printf("get_stat() failed");
    exit(1);
}

void get_Processinfo() {
    DIR *dir = opendir("/proc");
    struct dirent *dp;
    int pid;
    while ((dp = readdir(dir)) != NULL) {
        pid = atoi(dp->d_name);
        if (pid == 0) {
            continue;
        }
        else {
            processinfos[pid_num].pid = pid;
            processinfos[pid_num].ppid = get_stat(pid, processinfos[pid_num].name);
            pid_num++;
        }
    }
    closedir(dir);
}

int get_child(pid_t ppid, struct Processinfo *child, int *counter) {
    while(*counter < pid_num) {
        *counter += 1;  //*counter++ will do counter++ then *(counter)
        if(processinfos[*counter].ppid == ppid){
            *child = processinfos[*counter];
            
            return 1;
        }
    }
    return 0;
}


void P(struct Processinfo root,char strcp[]) {   //值传递
    struct Processinfo child;
    int processinfo_counter = 0, has_children = 0;
    char str[1000]={0};
    strcpy(str, strcp);
    // print self
    printf("-%s(%d)", root.name, root.pid);
    sprintf(str, "%s-%s(%d)", str, root.name, root.pid);

    //print children
    while(get_child(root.pid, &child, &processinfo_counter)) {
        if(!has_children) {//the first child
            printf("-");
            sprintf(str, "%s-", str);
            has_children = 1;
            P(child, str);
            continue;
        }
        for(int j = 0; j < strlen(str)-1; j++) {
            // printf("%d of %s\n", strlen(str), str);
            printf(" ");
        }
        printf("+");
        // sprintf(str, "%s+", str);
        P(child, str);
    }
    if(!has_children) { //leaf
        printf("\n");
    }
}

void N(struct Processinfo root,char strcp[]) {   //值传递
    struct Processinfo child;
    int processinfo_counter = 0, has_children = 0;
    char str[1000]={0};
    strcpy(str, strcp);
    // print self
    printf("-%s", root.name);
    sprintf(str, "%s-%s", str, root.name);

    //print children
    while(get_child(root.pid, &child, &processinfo_counter)) {
        if(!has_children) {//the first child
            printf("-");
            sprintf(str, "%s-", str);
            has_children = 1;
            N(child, str);
            continue;
        }
        for(int j = 0; j < strlen(str)-1; j++) {
            // printf("%d of %s\n", strlen(str), str);
            printf(" ");
        }
        printf("+");
        // sprintf(str, "%s+", str);
        N(child, str);
    }
    if(!has_children) { //leaf
        printf("\n");
    }
}
int main(int argc, char *argv[]) {
    // printf("%d\n", argc);
    // for(int i = 0; i < argc; i++) {
    //     printf("argv[%d] = %s\n", i, argv[i]);
    // }
    // assert(!argv[argc]);

    char proc[100000] = "/proc";
    int opt;
    char *short_options = "pnv";
    /*
      option {
        const char *name;
        int has_arg; // no_argument, required_argument and optional_argument
        int *flag;
        int val;
      }
      int getopt_long (int argc, char *const *argv, const char *shortopts, const
      struct option *longopts, int *indexptr)
      // if flag in a null pointer, getopt_long() returns the contens of val
      // if it is not a null pointer, getopt_long() returns 0
      // when there is no more options to handle, it returns -1
    */
    struct option long_options[] = { {"show-pids", 0, 0, 'p'},
                                    {"numeric-sort", 0, 0, 'n'},
                                    {"version", 0, 0, 'v'},
                                    {0, 0, 0, 0} };

    while (-1 != (opt = getopt_long(argc, argv, short_options, long_options, NULL))) {
        switch (opt) {
        case 'p':
            get_Processinfo();
            P(processinfos[0], "");
            break;
        case 'n':
            // N();
            get_Processinfo();
            N(processinfos[0], "");
            break;
        case 'v':
            // V();
            printf("pstree 1.0\n");
            break;
        case '?':  // 未定义参数项
            printf("arg err:\r\n");
        }
    }

    // printf("%s\n", proc);
    return 0;
}
