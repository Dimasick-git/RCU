/*
 * Copyright (c) Souldbminer, Lightos_ and Horizon OC Contributors
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/* --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */


#pragma once

#include <stdint.h>
#include "board.h"

typedef struct {

    /*
     * This "stable struct" must never be modified. It provides a fixed memory layout so external clients can safely read the expected fields even
     * if RyazhaClkContext changes in newer versions and the client is not recompiled.
     */
    struct {
        #define RyazhaClkModuleStable_EnumMax 5
        #define RyazhaClkThermalSensorStable_EnumMax 11
        #define RyazhaClkPowerSensorStable_EnumMax 2
        #define RyazhaClkPartLoadStable_EnumMax 10
        #define RyazhaClkVoltageStable_EnumMax 7

        u32 freqs[RyazhaClkModuleStable_EnumMax];
        u32 realFreqs[RyazhaClkModuleStable_EnumMax];
        u32 overrideFreqs[RyazhaClkModuleStable_EnumMax];
        s32 temps[RyazhaClkThermalSensorStable_EnumMax];
        s32 power[RyazhaClkPowerSensorStable_EnumMax];
        u32 partLoad[RyazhaClkPartLoadStable_EnumMax];
        u32 voltages[RyazhaClkVoltageStable_EnumMax];
    } stable;

    uint64_t applicationId;
    RyazhaClkProfile profile;
    uint32_t freqs[RyazhaClkModule_EnumMax];
    uint32_t realFreqs[RyazhaClkModule_EnumMax];
    uint32_t overrideFreqs[RyazhaClkModule_EnumMax];
    int32_t temps[RyazhaClkThermalSensor_EnumMax];
    int32_t power[RyazhaClkPowerSensor_EnumMax];
    uint32_t partLoad[RyazhaClkPartLoad_EnumMax];
    uint32_t voltages[RyazhaClkVoltage_EnumMax];
    u16 speedos[RyazhaClkSpeedo_EnumMax];
    u16 iddq[RyazhaClkSpeedo_EnumMax];
    s16 waferX;
    s16 waferY;

    // Misc stuff
    GpuSchedulingMode gpuSchedulingMode;
    bool isSysDockInstalled;
    bool isSaltyNXInstalled;
    bool isUsingRetroSuper;
    u8 maxDisplayFreq;
    u8 dramID;
    bool isDram8GB;
    RyazhaClkConsoleType consoleType;

    // FPS / Resolution
    u8 fps;
    u16 resolutionHeight;
    u8 custRev;
    u16 kipVersion;

    // Reserved for future use
    u8 reserved[0x35B];
} RyazhaClkContext;

typedef struct
{
    union {
        uint32_t mhz[+RyazhaClkProfile_EnumMax * +RyazhaClkModule_EnumMax];
        uint32_t mhzMap[+RyazhaClkProfile_EnumMax][+RyazhaClkModule_EnumMax];
    };
} RyazhaClkTitleProfileList;

#define RCLK_FREQ_LIST_MAX 48

#define RCLK_GLOBAL_PROFILE_TID 0xA111111111111111

static_assert(sizeof(RyazhaClkContext) == 0x500);