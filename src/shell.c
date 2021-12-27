#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h> // for handling excevp errors
#include <time.h>


#define RED   "\x1B[31m" // ANSI colors for terminal
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define PUR	  "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"	
#define RESET "\x1B[0m"
#define	MAX_SIZE_CMD	256
#define	MAX_SIZE_ARG	16


char cmd[MAX_SIZE_CMD];				// string holder for the command
char *argv[MAX_SIZE_ARG];			// an array for command and arguments
unsigned char i;					// global variable for the child process ID
pid_t pid; 							// needed for exit builtin



void get_cmd(){
	// showing the time and date
	time_t t = time(NULL);
	struct tm tm = *localtime(&t); 

	char directory[FILENAME_MAX];
	char hostname[_SC_HOST_NAME_MAX];
	char* username = getenv("USER");				// showing the username

	getcwd(directory, sizeof(directory)); 	// getting the current directory 
	gethostname(hostname, sizeof(hostname));		// showing the hostname
	
	fprintf(stdout,RED"┌["CYN"%s"RED"]─["YEL"%d/%02d/%02d-%02d:%02d:%02d"RED"]─["PUR"%s"RED"]\n"RESET, hostname, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, directory);
    fprintf(stdout, RED"└╼"GRN"%s"YEL"$> "RESET, username);
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
		convert_cmd();
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
	execute_cmd();
}

// handling builtin commmands
int check_builtins() {
	int stat; // keep track of builtin execution status
	// cd needs more tweaks (e.g perm denied for dirs, non existing dirs)
	if (!strcmp(cmd, "cd")) {
		if (argv[1] == NULL) {
			stat = chdir(getenv("HOME")); // default cd without arg behaviour
		} else {
			stat = chdir(argv[1]);
		}
		if (stat != 0)
			fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
		return 1;
	}
	if (!strcmp(cmd, "exit")) {
		exit(0);
 	}

	return 0; // not a builtin
}




void log_handle(){
	//printf("[LOG] child proccess terminated.\n");
	FILE *pFile;
    pFile = fopen(".log.txt", "a");
    if (pFile == NULL) {
        perror("Error opening file.");
    } else {
        fprintf(pFile, "[LOG] child proccess terminated.\n");
        fclose(pFile);
    }
}

void execute_cmd() {
		if (!check_builtins() == 1) { // if it's not a builtin execute the binary/script associated with the command
			// fork and execute the command;
			pid = fork();
			if (pid == -1) {
				perror("Failed to create a child\n");
			} else if (pid == 0) {
				// execute a command
					if (execvp(argv[0], argv) == -1) {
						fprintf(stderr, "%s: %s\n", argv[0], strerror(errno)); // explains the type of error
					}
			} else {
				// wait for the command to finish if "&" is not present
				if (argv[i] == NULL) {
		            waitpid(pid, NULL, 0);
				}
		    }
		}
		get_cmd();
}

