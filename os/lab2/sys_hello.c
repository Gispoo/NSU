#include <unistd.h> // ��� write

int main() {
    write(1, "hello world\n", 12);
    return 0;
}

//