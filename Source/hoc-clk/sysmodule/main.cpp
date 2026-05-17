#include <switch.h>
#include <string.h>
#include <stdio.h>

// Service manager handle
static Service g_serv;
static Handle g_hdl;

// Forward declarations
static Result serviceInit(void);
static void serviceExit(void);
static Result serviceDispatch(void);

int main(int argc, char **argv)
{
    Result rc;
    
    // Initialize services
    rc = serviceInit();
    if (R_FAILED(rc)) {
        // Failed to initialize, but we still need to return
        // so the process doesn't crash
        return 0;
    }
    
    // Main service loop
    while (true) {
        rc = serviceDispatch();
        if (R_FAILED(rc)) {
            break;
        }
    }
    
    // Cleanup
    serviceExit();
    
    return 0;
}

static Result serviceInit(void)
{
    Result rc = 0;
    
    // Initialize the service
    rc = smGetService(&g_serv, "clk");
    if (R_FAILED(rc)) {
        // Try to register the service
        rc = smRegisterService(&g_hdl, "clk", false, 1);
        if (R_FAILED(rc)) {
            return rc;
        }
    }
    
    return 0;
}

static void serviceExit(void)
{
    serviceClose(&g_serv);
    if (g_hdl != INVALID_HANDLE) {
        smUnregisterService(g_hdl);
        svcCloseHandle(g_hdl);
        g_hdl = INVALID_HANDLE;
    }
}

static Result serviceDispatch(void)
{
    Result rc = 0;
    u32 *cmdbuf = armGetThreadCommandBuffer();
    
    // Handle IPC commands
    u32 cmd_id = cmdbuf[0] >> 16;
    
    switch (cmd_id) {
        case 0:
            // Example command handler
            cmdbuf[0] = MAKERESULT(RL_SUCCESS, RS_SUCCESS, 0, 0);
            cmdbuf[1] = 0;
            break;
            
        default:
            cmdbuf[0] = MAKERESULT(RL_SUCCESS, RS_SUCCESS, 0, 0);
            cmdbuf[1] = 0;
            break;
    }
    
    return rc;
}