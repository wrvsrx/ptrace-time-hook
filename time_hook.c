#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define FIXED_TIME 1640995200L // 2022-01-01 00:00:00 UTC

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <target_binary> [args...]\n", argv[0]);
    return 1;
  }

  pid_t child_pid = fork();

  if (child_pid == 0) {
    // Child process: execute the target binary
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    execvp(argv[1], &argv[1]);
    perror("execvp failed");
    exit(1);
  } else if (child_pid > 0) {
    // Parent process: trace the child
    int status;
    struct user_regs_struct regs;

    // Wait for child to stop
    wait(&status);

    int syscall_entry = 1;

    while (WIFSTOPPED(status)) {
      // Get current registers
      if (ptrace(PTRACE_GETREGS, child_pid, NULL, &regs) == -1) {
        perror("PTRACE_GETREGS");
        break;
      }

      if (syscall_entry) {
        // We're at syscall entry
        if (regs.orig_rax == SYS_time || regs.orig_rax == SYS_clock_gettime ||
            regs.orig_rax == SYS_gettimeofday) {
          printf("Intercepted time-related syscall %llu entry\n",
                 regs.orig_rax);
        }
      } else {
        // We're at syscall exit
        if (regs.orig_rax == SYS_time) {
          printf("Intercepted time() syscall exit, original return: %lld\n",
                 (long long)regs.rax);

          // Set the return value to our fixed time
          regs.rax = FIXED_TIME;

          if (ptrace(PTRACE_SETREGS, child_pid, NULL, &regs) == -1) {
            perror("PTRACE_SETREGS");
            break;
          }

          printf("Modified time() return value to %ld\n", FIXED_TIME);
        } else if (regs.orig_rax == SYS_clock_gettime) {
          printf("Intercepted clock_gettime() syscall exit, return: %lld\n",
                 (long long)regs.rax);
          // Modify the timespec structure in memory
          // The second argument (rsi) points to the timespec structure
          if (regs.rax == 0) { // Success
            struct timespec fixed_ts = {FIXED_TIME, 0};
            long addr = regs.rsi;

            // Write the fixed timespec to the target process memory
            if (ptrace(PTRACE_POKEDATA, child_pid, addr,
                       *(long *)&fixed_ts.tv_sec) == -1) {
              perror("PTRACE_POKEDATA tv_sec");
            } else if (ptrace(PTRACE_POKEDATA, child_pid, addr + sizeof(long),
                              *(long *)&fixed_ts.tv_nsec) == -1) {
              perror("PTRACE_POKEDATA tv_nsec");
            } else {
              printf("Modified clock_gettime() timespec to %ld.%09ld\n",
                     fixed_ts.tv_sec, fixed_ts.tv_nsec);
            }
          }
        } else if (regs.orig_rax == SYS_gettimeofday) {
          printf("Intercepted gettimeofday() syscall exit\n");
          // Similar complexity as clock_gettime
        }
      }

      syscall_entry = !syscall_entry;

      // Continue execution
      ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL);
      wait(&status);
    }

    printf("Target process exited\n");
  } else {
    perror("fork failed");
    return 1;
  }

  return 0;
}