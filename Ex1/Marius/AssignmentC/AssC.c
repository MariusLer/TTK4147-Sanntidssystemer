#include <stdio.h>

void allocate(int value){
  int* ptr = NULL;
  ptr = malloc(1024*1024*sizeof(char));
  if (ptr == NULL){
    perror("Error: ");
    return;
  }
  *ptr = value;
  printf("Test of allocated memory: %i\n");
}

int main(int argc, char const *argv[]) {
  allocate(5);
  return 0;
}
