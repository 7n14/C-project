#include <stdio.h>
#include <string.h>

int main(void){
  char str[2];
  sprintf(str, "%d", 42);
  printf("%s\n",str);
  return 0;
}
