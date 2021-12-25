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
pid_t pid; 							// needed for exit builtin



void get_cmd(){
	char* username = getenv("USER");		// showing the user the username
    fprintf(stdout, "%s $> ", username);
    // remove trailing newline
    if (fgets(cmd, MAX_SIZE_CMD, stdin) == NULL) {
		if (*cmd == '\n') // if newline is inputted show the prompt again
			get_cmd();
        if (feof(stdin)) { // if EOF is inputted (Ctrl+D)
			fprintf(stdout, "\nBye.\n");
			exit(0); 								///// isn't effective, it kills child process one by one !!!
		} else {
			perror("Failed to read the input stream");
		}
    } else {
        cmd[strlen(cmd) - 1] = '\0';
    };
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
}

// handling builtin commmands
int check_builtins() {
	// cd needs more tweaks (e.g perm denied for dirs, non existing dirs)
	if (!strcmp(cmd, "cd")) {
		if (argv[1] == NULL) {
			chdir(getenv("HOME")); // default cd without arg behaviour
		} else {
			chdir(argv[1]);
		}
		return 1;
	}
	// check if the child pid gets killed or not (to avoid zombie processes)
	if (!strcmp(cmd, "exit")) {						////////////////////////////////////////////////////////////////////////////////////////////////////////
		kill(pid, SIGTERM); // SIGTERM is nicer		// this doesn't kill all cpids, try inputing multiple cd .. then exit, you have to issue it many time to kill
 	}												// each process, needs some tweaking!!
													//////////////////////////////////////////////////////////////////////////////////////////////

	return 0; // not a builtin
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
    	// fork and execute the command;
		pid = fork();
		if (pid == -1) {
			perror("failed to create a child\n");
		} else if (pid == 0) {
			// execute a command
			if (!check_builtins() == 1) { // if it's not a builtin execute the binary/script associated with the command
				if (execvp(argv[0], argv) == -1) { // must add a mechanism to check if the uid has permission to run binary (since it returns the same value if so)
					fprintf(stderr, "%s: command not found\n", argv[0]);
				}
			}
		} else {
			// wait for the command to finish if "&" is not present
			if (argv[i] == NULL) {
            waitpid(pid, NULL, 0);
		}
    }
}


void c_shell() {
    while(1) {
		// get the command from user
		get_cmd();
		// fit the command into *argv[]
		convert_cmd();
	    // execute commands
	    execute_cmd();
    }
}
