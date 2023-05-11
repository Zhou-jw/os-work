#include <common.h>
#include <am.h>
#include <thread.h>

void do_test_0() {
    
}

int main(int argc, char *argv[]) {
    if(argc < 2) {
        printf("please input the num of test case");
        exit(1);
    }
    
    switch (atoi(argv[1])) {
        case 0: do_test_0();
    }
}