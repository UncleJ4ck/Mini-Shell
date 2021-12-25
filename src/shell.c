#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#define	MAX_SIZE_CMD	256
#define	MAX_SIZE_ARG	16

char cmd[MAX_SIZE_CMD];				// string holder for the command
char *argv[MAX_SIZE_ARG];			// an array for command and arguments
unsigned char i;					// global variable for the child process ID

void get_cmd(){
	char* username = getenv("USER");		// showing the user the username
    fprintf(stdout, "%s $> ", username);
    // remove trailing newline
	if (fgets(cmd, MAX_SIZE_CMD, stdin) == NULL) {
		if (*cmd == '\n')
			get_cmd();
        if (feof(stdin)) { // if EOF is inputted (Ctrl+D)
			fprintf(stdout, "\nBye.\n");
			exit(0);
		} else {
			perror("Failed to read the input stream");
		}
    } else {
        cmd[strlen(cmd) - 1] = '\0';
    }
}

void convert_cmd() {
    // split strings into argv
    char *ptr;
    i = 0;
    ptr = strtok(cmd, " ");
    while (ptr != NULL) {
        argv[i] = ptr;
        i++;
        ptr = strtok(NULL, " ");
    }
	 argv[i] = NULL;
/*	 if (!strcmp("&", argv[i - 1])) {
	    argv[i - 1] = "&";
	    argv[i] = NULL;
	} else {
	    argv[i] = NULL;
	} */
	//printf("%d\n", i);
}

void log_handle(){
	//printf("[LOG] child proccess terminated.\n");
	FILE *pFile;
    pFile = fopen("log.txt", "a");
    if (pFile == NULL) {
        perror("Error opening file.");
    } else {
        fprintf(pFile, "[LOG] child proccess terminated.\n");
        fclose(pFile);
    }
}

void execute_cmd() {
    	// fork and execute the command
        pid_t pid;
		pid = fork();
		if (pid == -1) {
			perror("failed to create a child\n");
		} else if (pid == 0) {
			// printf("hello from child\n");
			// execute a command
			if (execvp(argv[0], argv) == -1) { // numerous reasons for that except "not found" (e.g permission denied)
				fprintf(stderr, "%s: Command not found\n", argv[0]); 
				get_cmd();
			}
		} else {
			// printf("hello from parent\n");
			// wait for the command to finish if "&" is not present
			if (argv[i] == NULL) {
            waitpid(pid, NULL, 0);
		}
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
// troublemaker function, comment it/fix it if you want it to work (removed on the stable branch) //
///////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// this function works now, a bit more advanced and efficent way is on the stable branch (through error describing) //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void check_cmd() {
    while (1) {
		// check for "exit" command
    	if (!strcmp("exit", cmd)) {
        	exit(0);
    	}
    	char *path = malloc(1000); // needs to be fixed 
//		path = "/usr/bin"; // strings are immutable ;)
		strcpy(path, "/usr/bin/"); // copying the string to buffer instead of direct assignment
		strcat(path, argv[0]);
		if(access(path, F_OK) != 0){ 
			printf("Command not found\n");
			get_cmd();
		} else {
			break;
		}
    }
}

void c_shell() {
    while(1) {
	// get the command from user
	get_cmd();
	// fit the command into *argv[]
	convert_cmd();
	// check for commands
	check_cmd();
    // execute commands
    execute_cmd();
    }
}
