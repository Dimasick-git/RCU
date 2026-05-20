/*
 * Copyright (c) Souldbminer, Lightos_ and Ryazha-CLK Contributors
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
     * if RClkContext changes in newer versions and the client is not recompiled.
     */
    struct {
        #define RClkModuleStable_EnumMax 5
        #define RClkThermalSensorStable_EnumMax 11
        #define RClkPowerSensorStable_EnumMax 2
        #define RClkPartLoadStable_EnumMax 10
        #define RClkVoltageStable_EnumMax 7

        u32 freqs[RClkModuleStable_EnumMax];
        u32 realFreqs[RClkModuleStable_EnumMax];
        u32 overrideFreqs[RClkModuleStable_EnumMax];
        s32 temps[RClkThermalSensorStable_EnumMax];
        s32 power[RClkPowerSensorStable_EnumMax];
        u32 partLoad[RClkPartLoadStable_EnumMax];
        u32 voltages[RClkVoltageStable_EnumMax];
    } stable;

    uint64_t applicationId;
    RClkProfile profile;
    uint32_t freqs[RClkModule_EnumMax];
    uint32_t realFreqs[RClkModule_EnumMax];
    uint32_t overrideFreqs[RClkModule_EnumMax];
    int32_t temps[RClkThermalSensor_EnumMax];
    int32_t power[RClkPowerSensor_EnumMax];
    uint32_t partLoad[RClkPartLoad_EnumMax];
    uint32_t voltages[RClkVoltage_EnumMax];
    u16 speedos[RClkSpeedo_EnumMax];
    u16 iddq[RClkSpeedo_EnumMax];
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
    RClkConsoleType consoleType;

    // FPS / Resolution
    u8 fps;
    u16 resolutionHeight;
    u8 custRev;
    u16 kipVersion;

    // Reserved for future use
    u8 reserved[0x35B];
} RClkContext;

typedef struct
{
    union {
        uint32_t mhz[+RClkProfile_EnumMax * +RClkModule_EnumMax];
        uint32_t mhzMap[+RClkProfile_EnumMax][+RClkModule_EnumMax];
    };
} RClkTitleProfileList;

#define RCLK_FREQ_LIST_MAX 48

#define RCLK_GLOBAL_PROFILE_TID 0xA111111111111111

static_assert(sizeof(RClkContext) == 0x500);