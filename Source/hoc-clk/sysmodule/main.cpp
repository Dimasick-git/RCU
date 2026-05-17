#include <switch.h>

extern "C" void __appInit(void)
{
    svcSleepThread(10000000000L);

    Result rc;

    rc = smInitialize();
    if (R_FAILED(rc))
        fatalThrow(rc);

    rc = fsInitialize();
    if (R_FAILED(rc))
        fatalThrow(rc);

    rc = fsdevMountSdmc();
    if (R_FAILED(rc))
        fatalThrow(rc);

    rc = timeInitialize();
    if (R_FAILED(rc))
        fatalThrow(rc);

    rc = hidInitialize();
    if (R_FAILED(rc))
        fatalThrow(rc);
}

extern "C" void __appExit(void)
{
    hidExit();
    timeExit();
    fsdevUnmountAll();
    fsExit();
    smExit();
}

int main(int argc, char **argv)
{
    // Main loop
    while (true)
    {
        svcSleepThread(1000000000L);
    }

    return 0;
}
