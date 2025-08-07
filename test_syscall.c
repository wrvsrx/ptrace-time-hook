#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <time.h>

int main() {
    printf("Test program with direct syscalls started\n");
    
    // Direct syscall to time()
    time_t t1 = syscall(SYS_time, NULL);
    printf("Direct time() syscall: %ld\n", t1);
    
    // Library call to time()
    time_t t2 = time(NULL);
    printf("Library time() call: %ld\n", t2);
    
    // Direct syscall to clock_gettime
    struct timespec ts;
    syscall(SYS_clock_gettime, CLOCK_REALTIME, &ts);
    printf("Direct clock_gettime() syscall: %ld.%09ld\n", ts.tv_sec, ts.tv_nsec);
    
    printf("Test program finished\n");
    return 0;
}