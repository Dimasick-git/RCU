#include <switch.h>

extern "C" void main(void)
{
    // Initialize the sysmodule
    consoleInit(NULL);

    printf("ryazha-clk sysmodule started\n");

    // Main loop
    while (appletMainLoop())
    {
        // Handle services, IPC, etc.
        svcSleepThread(100000000ULL); // 100ms
    }

    consoleExit(NULL);
}
