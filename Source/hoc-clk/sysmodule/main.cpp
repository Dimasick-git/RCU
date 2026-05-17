#include <switch.h>

extern "C" {
    void __libnx_init(void);
    void __libnx_exit(void);
}

void __libnx_init(void) {
    // Initialize services if needed
}

void __libnx_exit(void) {
    // Cleanup services if needed
}

int main(int argc, char **argv) {
    // Main sysmodule loop
    // This sysmodule doesn't need to do anything in main
    // as it's handled by the overlay
    while (true) {
        svcSleepThread(1000000000ULL);
    }
    return 0;
}
