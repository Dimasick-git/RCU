#include <switch.h>
#include <cstdio>
#include <cstring>
#include <atomic>
#include <algorithm>

// Service management
static Service g_hocGenService = {0};
static std::atomic<bool> g_Initialized(false);
static std::atomic<bool> g_HasControl(false);
static Handle g_ControlEvent = INVALID_HANDLE;

// Forward declarations
static Result hocGenInitialize(void);
static void hocGenExit(void);
static Result hocGenGetService(Service* service);
static Result hocGenRequestControl(Handle* event);
static Result hocGenReleaseControl(void);

static Result hocGenInitialize(void) {
    if (g_Initialized.load()) {
        return 0;
    }

    Result rc = smGetService(&g_hocGenService, "hoc");
    if (R_FAILED(rc)) {
        return rc;
    }

    g_Initialized.store(true);
    return 0;
}

static void hocGenExit(void) {
    if (g_Initialized.load()) {
        serviceClose(&g_hocGenService);
        g_Initialized.store(false);
    }
    if (g_HasControl.load()) {
        svcCloseHandle(g_ControlEvent);
        g_HasControl.store(false);
    }
}

static Result hocGenGetService(Service* service) {
    if (!g_Initialized.load()) {
        return MAKERESULT(Module_Libnx, LibnxError_NotInitialized);
    }
    return serviceClone(&g_hocGenService, service);
}

static Result hocGenRequestControl(Handle* event) {
    if (g_HasControl.load()) {
        return MAKERESULT(Module_Libnx, LibnxError_AlreadyInitialized);
    }

    Result rc = 0;
    Handle tmp_handle = INVALID_HANDLE;

    rc = serviceDispatch(&g_hocGenService, 0,
        .out_handle_attrs = { SfOutHandleAttr_HipcCopy },
        .out_handles = &tmp_handle,
    );

    if (R_SUCCEEDED(rc)) {
        g_ControlEvent = tmp_handle;
        g_HasControl.store(true);
        *event = tmp_handle;
    }

    return rc;
}

static Result hocGenReleaseControl(void) {
    if (!g_HasControl.load()) {
        return MAKERESULT(Module_Libnx, LibnxError_NotInitialized);
    }

    svcCloseHandle(g_ControlEvent);
    g_ControlEvent = INVALID_HANDLE;
    g_HasControl.store(false);
    return 0;
}

extern "C" void __appInit(void) {
    Result rc;

    rc = smInitialize();
    if (R_FAILED(rc)) {
        fatalThrow(rc);
    }

    rc = fsInitialize();
    if (R_FAILED(rc)) {
        fatalThrow(rc);
    }

    rc = fsdevMountSdmc();
    if (R_FAILED(rc)) {
        fatalThrow(rc);
    }

    rc = pmshellInitialize();
    if (R_FAILED(rc)) {
        fatalThrow(rc);
    }

    rc = hocGenInitialize();
    if (R_FAILED(rc)) {
        fatalThrow(rc);
    }
}

extern "C" void __appExit(void) {
    hocGenExit();
    pmshellExit();
    fsdevUnmountAll();
    fsExit();
    smExit();
}

int main(int argc, char* argv[]) {
    // Main loop for the sysmodule
    while (true) {
        svcSleepThread(1000000000ULL); // Sleep for 1 second
    }
    return 0;
}