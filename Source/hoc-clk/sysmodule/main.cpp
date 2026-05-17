#include <switch.h>

extern "C" void __appInit(void) {
    svcSleepThread(10000000000L);
}

extern "C" void __appExit(void) {
}

int main(int argc, char **argv) {
    consoleInit(NULL);

    printf("ryazha-clk sysmodule started\n");

    while (appletMainLoop()) {
        consoleUpdate(NULL);
        svcSleepThread(1000000000L);
    }

    consoleExit(NULL);
    return 0;
}