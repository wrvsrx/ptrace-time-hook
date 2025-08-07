#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <string.h>
#include <getopt.h>

static int verbose = 0;
static long target_time = 0; // Default to Unix epoch (1970-01-01 00:00:00)

void print_usage(const char *program_name) {
    printf("Usage: %s [OPTIONS] <program> [args...]\n", program_name);
    printf("Hook time() syscalls and replace with specified time\n\n");
    printf("Options:\n");
    printf("  -v, --verbose     Enable verbose output\n");
    printf("  -t, --time TIME   Set target time (Unix timestamp, default: 0)\n");
    printf("  -h, --help        Show this help message\n");
    printf("\nExamples:\n");
    printf("  %s ./test_program\n", program_name);
    printf("  %s --time 1640995200 ./test_program  # 2022-01-01 00:00:00 UTC\n", program_name);
    printf("  %s --verbose --time 0 ./test_program\n", program_name);
}

int parse_args(int argc, char *argv[], int *target_argc, char ***target_argv) {
    static struct option long_options[] = {
        {"verbose", no_argument, 0, 'v'},
        {"time", required_argument, 0, 't'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int c;
    int option_index = 0;

    while ((c = getopt_long(argc, argv, "vt:h", long_options, &option_index)) != -1) {
        switch (c) {
            case 'v':
                verbose = 1;
                break;
            case 't':
                target_time = strtol(optarg, NULL, 10);
                break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            case '?':
                return -1;
            default:
                return -1;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Error: No target program specified\n");
        print_usage(argv[0]);
        return -1;
    }

    *target_argc = argc - optind;
    *target_argv = &argv[optind];
    return 1;
}

int main(int argc, char *argv[]) {
    int target_argc;
    char **target_argv;
    
    int parse_result = parse_args(argc, argv, &target_argc, &target_argv);
    if (parse_result <= 0) {
        return parse_result == 0 ? 0 : 1;
    }

    if (verbose) {
        printf("time-hook: Target program: %s\n", target_argv[0]);
        printf("time-hook: Target time: %ld\n", target_time);
    }

    pid_t child = fork();
    if (child == 0) {
        ptrace(PTRACE_TRACEME);
        execvp(target_argv[0], target_argv);
        perror("execvp failed");
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
            if (verbose) {
                printf("time-hook: Found time() syscall\n");
            }
            
            // Continue to syscall exit
            ptrace(PTRACE_SYSCALL, child, 0, 0);
            wait(&status);
            
            if (WIFEXITED(status)) break;
            
            // Modify return value
            ptrace(PTRACE_GETREGS, child, 0, &regs);
            regs.rax = target_time;
            ptrace(PTRACE_SETREGS, child, 0, &regs);
            
            if (verbose) {
                printf("time-hook: Modified time() return to %ld\n", target_time);
            }
        }
    }
    
    if (verbose) {
        printf("time-hook: Process finished\n");
    }
    return 0;
}