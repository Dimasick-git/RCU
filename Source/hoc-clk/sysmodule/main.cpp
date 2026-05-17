#include <switch.h>

extern "C" {
    void DllMain(void);
}

int main(int argc, char **argv) {
    DllMain();
    return 0;
}