#include <switch.h>

extern "C" void __appInit(void) {
    // Initialize services here if needed
}

extern "C" void __appExit(void) {
    // Clean up services here if needed
}

int main(int argc, char **argv) {
    // Main sysmodule loop
    while (true) {
        svcSleepThread(1000000000ULL); // Sleep for 1 second
    }
    return 0;
}
