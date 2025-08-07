#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/syscall.h>

#define FIXED_TIME 1640995200L

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <program>\n", argv[0]);
        return 1;
    }

    pid_t child = fork();
    if (child == 0) {
        ptrace(PTRACE_TRACEME);
        execvp(argv[1], &argv[1]);
        return 1;
    }

    int status;
    struct user_regs_struct regs;
    
    wait(&status);  // Wait for initial stop
    
    while (1) {
        // Continue until next syscall
        ptrace(PTRACE_SYSCALL, child, 0, 0);
        wait(&status);
        
        if (WIFEXITED(status)) break;
        
        // Get registers at syscall entry
        ptrace(PTRACE_GETREGS, child, 0, &regs);
        
        if (regs.orig_rax == SYS_time) {
            printf("Found time() syscall!\n");
            
            // Continue to syscall exit
            ptrace(PTRACE_SYSCALL, child, 0, 0);
            wait(&status);
            
            if (WIFEXITED(status)) break;
            
            // Modify return value
            ptrace(PTRACE_GETREGS, child, 0, &regs);
            regs.rax = FIXED_TIME;
            ptrace(PTRACE_SETREGS, child, 0, &regs);
            
            printf("Modified time() return to %ld\n", FIXED_TIME);
        }
    }
    
    printf("Process finished\n");
    return 0;
}