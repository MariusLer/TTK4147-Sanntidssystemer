#include <stdio.h>
#include <stdlib.h>
void allocate(int value) {
	int *ptr = NULL;
	ptr = malloc(1024*1024*sizeof(char));
	*ptr = value;
	if(ptr==NULL){
		perror("could not malloc");
		return;
	}
	printf("test of allocated memory: %i\n");
}

int main(){
	while(1){
		allocate(3);
	}
	return 0;
	}
