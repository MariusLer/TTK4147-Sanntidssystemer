#include <stdio.h>
#include <stdlib.h>

void allocate(int value){
  int* ptr = NULL;
  ptr = malloc(1024*1024*sizeof(char));
  *ptr = value;
  printf("Test of allocated memory: %i\n");
}

int main(int argc, char const *argv[]) {
  int i = 0;
  while (1){
    allocate(i++);
  }
  return 0;
}
