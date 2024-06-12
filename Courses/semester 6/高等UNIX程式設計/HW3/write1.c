#include "libmini.h"

const char *hello_list[4] = {
    "Hello",
    " ",
    "world",
    "!\n"};

int main() {
    for (int i = 0; i < 4; i++) {
        write(1, hello_list[i], strlen(hello_list[i]));
    }
}
