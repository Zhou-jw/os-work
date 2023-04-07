#include <stdio.h>
#include <string.h>
void f(char s[]){
    s[0] = '!';
}
int main(){
    char mybuffer[50];
    sprintf(mybuffer, "%s", "111111111111");
    sprintf(mybuffer, "%s%s", "222222222222", mybuffer);
    f(mybuffer);
    int l = strlen(mybuffer);
    printf("%s is %d", mybuffer, l);
    return 0;
}
