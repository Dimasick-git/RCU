#include <switch.h>

int main(int argc, char **argv) {
    // Initialize the sysmodule
    consoleInit(NULL);
    
    // Main loop
    while (appletMainLoop()) {
        consoleUpdate(NULL);
    }
    
    consoleExit(NULL);
    return 0;
}