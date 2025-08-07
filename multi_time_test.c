#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

long direct_time() {
    long result;
    asm volatile (
        "mov $201, %%rax\n"
        "mov $0, %%rdi\n"
        "syscall\n"
        "mov %%rax, %0\n"
        : "=r" (result)
        :
        : "rax", "rdi"
    );
    return result;
}

int main() {
    printf("Multi-time test started\n");
    
    for (int i = 0; i < 3; i++) {
        long t = direct_time();
        printf("Call %d: time = %ld\n", i+1, t);
        sleep(1);
    }
    
    printf("Multi-time test finished\n");
    return 0;
}