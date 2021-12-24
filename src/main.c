#include <stdio.h>
#include "../include/shell.h"
#include <signal.h>

int main() {
    // tie the handler to the SGNCHLD signal
    signal(SIGCHLD, log_handle);
    // start the shell
    c_shell();
    return 0;
}
