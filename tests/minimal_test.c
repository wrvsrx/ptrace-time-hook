#include <unistd.h>
#include <sys/syscall.h>

int main() {
    // Direct syscall without any library dependencies
    long t = syscall(SYS_time, 0);
    
    // Simple output using write syscall
    char msg[] = "Time: ";
    syscall(SYS_write, 1, msg, sizeof(msg)-1);
    
    // Convert number to string and output
    char num[20];
    int len = 0;
    long temp = t;
    if (temp == 0) {
        num[len++] = '0';
    } else {
        while (temp > 0) {
            num[len++] = '0' + (temp % 10);
            temp /= 10;
        }
        // Reverse the digits
        for (int i = 0; i < len/2; i++) {
            char c = num[i];
            num[i] = num[len-1-i];
            num[len-1-i] = c;
        }
    }
    
    syscall(SYS_write, 1, num, len);
    syscall(SYS_write, 1, "\n", 1);
    
    return 0;
}