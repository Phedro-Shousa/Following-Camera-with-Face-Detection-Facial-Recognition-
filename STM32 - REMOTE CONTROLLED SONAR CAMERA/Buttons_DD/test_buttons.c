#include <fcntl.h>
#include <stdio.h>

#include <string.h>
#include <mqueue.h>
#include <unistd.h>
#include <stdlib.h>

#define BUTTONS "/dev/buttons"

int main (void){

	char buffer[1]={0};
	FILE* fd ;
	fd = fopen (BUTTONS, "r" );
	if(fd == NULL){
		printf ("Device doesn't exist\n");
		return -1;
	}
	fread(buffer,sizeof(int),1,fd);
	printf ("Value on buffer: %s\n", buffer);
	if(buffer[0] == '1'){
	system("echo 1 > ../dev/buttons");
	}
	fclose (fd);

	return 0;
}
