#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

// Direct syscall without glibc wrapper
long direct_time_syscall() {
    long result;
    asm volatile (
        "mov $201, %%rax\n"      // SYS_time
        "mov $0, %%rdi\n"        // NULL argument
        "syscall\n"
        "mov %%rax, %0\n"
        : "=r" (result)
        :
        : "rax", "rdi"
    );
    return result;
}

int main() {
    printf("Direct syscall test started\n");
    
    long t = direct_time_syscall();
    printf("Direct time syscall result: %ld\n", t);
    
    printf("Direct syscall test finished\n");
    return 0;
}