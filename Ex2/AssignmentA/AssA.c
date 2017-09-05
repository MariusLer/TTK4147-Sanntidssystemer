#include <unistd.h>
#include <stdio.h>

int global = 0;

int main(){
	int local = 0;
	vfork();
	int i;
	for(i = 0; i < 50000; i++){
		global++;
		local++;
	}
	printf("Global :%d\n",global);
	printf("Local :%d\n",local);
	_exit(2);
	return 0;
}

