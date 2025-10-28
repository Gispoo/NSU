#include <unistd.h>
#include <sys/syscall.h> // Äëÿ syscall

void my_write(int fd, const void *buf, size_t count) {
    syscall(SYS_write, fd, buf, count);
}

int main() {
    my_write(1, "Hello world\n", 12);

    return 0;
}
