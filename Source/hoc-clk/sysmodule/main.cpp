#include <switch.h>

extern "C" {
    __attribute__((weak)) void* __stack_chk_guard = nullptr;
    __attribute__((weak)) void __stack_chk_fail(void) { }
}

int main(int argc, char** argv) {
    // Minimal sysmodule entry point
    return 0;
}
