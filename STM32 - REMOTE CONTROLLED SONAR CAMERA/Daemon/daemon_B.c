#include <sys/types.h> 
#include <sys/stat.h>
#include <unistd.h> 
#include <stdlib.h>
#include <string.h>
#include <stdio.h> 
#include <fcntl.h>
#include <time.h>


#define BUTTONS "/dev/buttons"

int main(int argc, char *argv[]){


	printf("TESTE\n");	
	pid_t pid, sid;
	const time_t timebuf;

	pid = fork(); // create a new process

	if (pid < 0) { // on error exit
		perror("fork");
	exit(EXIT_FAILURE);
	}

	if (pid > 0){  
		printf("Deamon PID: %d\n", pid);	
		exit(EXIT_SUCCESS); // parent process (exit)
	}
	sid = setsid(); // create a new session

	if (sid < 0) { // on error exit
		perror("setsid");
		exit(EXIT_FAILURE);
	}
	// make '/' the root directory
	if (chdir("/") < 0) { // on error exit
		perror("chdir");
		exit(EXIT_FAILURE);
	}
	umask(0);
	close(STDIN_FILENO);  // close standard input file descriptor
	close(STDOUT_FILENO); // close standard output file descriptor
	close(STDERR_FILENO); // close standard error file descriptor
printf("Deamon PID: %d\n", pid);

		
	char buffer[1]={0};
	FILE* fd;
	while (1) {
	
	fd = fopen (BUTTONS, "r" );
	if(fd == NULL){
		perror ("Device doesn't exist\n");
		exit(EXIT_FAILURE);
	}
	fread(buffer,sizeof(int),1,fd);
	if(buffer[0] == '1'){
	system("echo 1 > ../dev/buttons");
	}
	printf ("Value on buffer: %s\n", buffer);
	fclose (fd);
	}
exit(EXIT_SUCCESS);		
}
