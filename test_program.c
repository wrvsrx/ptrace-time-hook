#include <stdio.h>
#include <time.h>
#include <unistd.h>

int main() {
    printf("Test program started\n");
    
    for (int i = 0; i < 3; i++) {
        time_t current_time = time(NULL);
        printf("time() call %d: %ld\n", i + 1, current_time);
        sleep(1);
    }
    
    printf("Test program finished\n");
    return 0;
}