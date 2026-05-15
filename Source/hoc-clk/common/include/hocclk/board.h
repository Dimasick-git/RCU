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
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;
typedef uint64_t u64;
typedef int64_t s64;

typedef enum
{
    HocClkConsoleType_Icosa = 0, // V1
    HocClkConsoleType_Iowa,      // V2
    HocClkConsoleType_Hoag,      // Lite
    HocClkConsoleType_Aula,      // OLED
    HocClkConsoleType_EnumMax,
} HocClkConsoleType;

typedef enum {
    HocClkVoltage_SOC = 0, // VDD_SOC rail. 
    HocClkVoltage_EMCVDD2, // DRAM VDD2 rail
    HocClkVoltage_CPU,     // CPU rail
    HocClkVoltage_GPU,     // GPU rail
    HocClkVoltage_EMCVDDQ, // DRAM VDDQ rail
    HocClkVoltage_Display, // Display rail
    HocClkVoltage_Battery, // Battery voltage
    HocClkVoltage_EnumMax,
} HocClkVoltage;

typedef enum
{
    HocClkSocType_Erista = 0,
    HocClkSocType_Mariko,
    HocClkSocType_EnumMax,
} HocClkSocType;

typedef enum
{
    HocClkModule_CPU = 0, 
    HocClkModule_GPU,
    HocClkModule_MEM,
    HocClkModule_Governor,
    HocClkModule_EnumMax,
} HocClkModule;

typedef enum
{
    HocClkThermalSensor_SOC = 0, // SoC temperature in millicelcius
    HocClkThermalSensor_PCB,     // PCB temperature in millicelcius
    HocClkThermalSensor_Skin,    // "Skin" temperature in millicelcius
    HocClkThermalSensor_Battery, // Battery temperature in millicelcius
    HocClkThermalSensor_PMIC, // Always return 50.0C, as thats the only reasonable value the PMIC sensor can generate
    HocClkThermalSensor_CPU, // CPU temperature in millicelcius
    HocClkThermalSensor_GPU, // GPU temperature in millicelcius
    HocClkThermalSensor_MEM, // MEM temperature in millicelcius. Returns the PLLX sensor value on Mariko
    HocClkThermalSensor_PLLX, // PLLX temperature in millicelcius
    HocClkThermalSensor_AO, // AOTAG
    HocClkThermalSensor_BQ24193, // BQ24193 temperature. Refer to BQ24193Temp for returned values
    HocClkThermalSensor_EnumMax
} HocClkThermalSensor;

typedef enum
{
    HocClkPartLoad_CPU = 0,
    HocClkPartLoad_GPU,
    HocClkPartLoad_RamBWAll,
    HocClkPartLoad_RamBWCpu,
    HocClkPartLoad_RamBWGpu,
    HocClkPartLoad_RamBWPeak, // Maximum possible RAM bandwidth
    HocClkPartLoad_EnumMax
} HocClkPartLoad;

typedef enum
{
    HocClkGovernor_None = 0,
    HocClkGovernor_Performance,
    HocClkGovernor_Powersave,
    HocClkGovernor_EnumMax,
} HocClkGovernor;

enum {
    DVFSMode_Disabled = 0,
    DVFSMode_Hijack, // PCV hijack dvfs
    // DVFSMode_OfficialService,
    // DVFSMode_Hack,
    DVFSMode_EnumMax,
};

typedef struct
{
    u16 cpuSpeedo;
    u16 gpuSpeedo;
    u16 socSpeedo;
    u16 cpuIDDQ;
    u16 gpuIDDQ;
    u16 socIDDQ;
    s16 waferX;
    s16 waferY;
} FuseData;

typedef enum {
    RamDisplayUnit_MHz = 0,
    RamDisplayUnit_MTs,
    RamDisplayUnit_MHzMTs,
    RamDisplayUnit_EnumMax,
} RamDisplayUnit;

typedef enum {
    BQ24193Temp_Normal = 0,
    BQ24193Temp_Warm,
    BQ24193Temp_Hot,
    BQ24193Temp_Overheat,
    BQ24193Temp_EnumMax
} BQ24193Temp;

typedef enum AulaColorMode {
    AulaDisplayColorMode_DoNotOverride = 0xFF,
    AulaDisplayColorMode_Saturated = 0x0,
    AulaDisplayColorMode_Washed = 0x45,
    AulaDisplayColorMode_Basic = 0x03, // Default
    AulaDisplayColorMode_Vivid = 0x43,
    AulaDisplayColorMode_Night1 = 0x15,
    AulaDisplayColorMode_Night2 = 0x35,
    AulaDisplayColorMode_Night3 = 0x75,
} AulaColorMode;

inline u32 GovernorStatePack(u8 cpu, u8 gpu, u8 vrr) {
    return (u32)cpu | ((u32)gpu << 8) | ((u32)vrr << 16);
}

inline u8 GovernorStateCpu(u32 p) {
    return (u8)(p         & 0xFF);
}

inline u8 GovernorStateGpu(u32 p) {
    return (u8)((p >>  8) & 0xFF);
}

inline u8 GovernorStateVrr(u32 p) {
    return (u8)((p >> 16) & 0xFF);
}

static inline const char* hocClkFormatVoltage(HocClkVoltage voltage, bool pretty)
{
    switch(voltage)
    {
        case HocClkVoltage_SOC:
            return pretty ? "SOC" : "soc";
        case HocClkVoltage_EMCVDD2:
            return pretty ? "VDD2" : "vdd2";
        case HocClkVoltage_CPU:
            return pretty ? "CPU" : "cpu";
        case HocClkVoltage_GPU:
            return pretty ? "GPU" : "gpu";
        case HocClkVoltage_EMCVDDQ:
            return pretty ? "VDDQ" : "vddq";
        case HocClkVoltage_Display:
            return pretty ? "Display" : "display";
        case HocClkVoltage_Battery:
            return pretty ? "Battery" : "battery";
        default:
            return "unknown";
    }
}

static inline const char* hocClkFormatThermalSensor(HocClkThermalSensor sensor, bool pretty)
{
    switch(sensor)
    {
        case HocClkThermalSensor_SOC:
            return pretty ? "SOC" : "soc";
        case HocClkThermalSensor_PCB:
            return pretty ? "PCB" : "pcb";
        case HocClkThermalSensor_Skin:
            return pretty ? "Skin" : "skin";
        case HocClkThermalSensor_Battery:
            return pretty ? "Battery" : "battery";
        case HocClkThermalSensor_PMIC:
            return pretty ? "PMIC" : "pmic";
        case HocClkThermalSensor_CPU:
            return pretty ? "CPU" : "cpu";
        case HocClkThermalSensor_GPU:
            return pretty ? "GPU" : "gpu";
        case HocClkThermalSensor_MEM:
            return pretty ? "MEM" : "mem";
        case HocClkThermalSensor_PLLX:
            return pretty ? "PLLX" : "pllx";
        case HocClkThermalSensor_AO:
            return pretty ? "AO" : "ao";
        case HocClkThermalSensor_BQ24193:
            return pretty ? "BQ24193" : "bq24193";
        default:
            return "unknown";
    }
}

static inline const char* hocClkFormatPartLoad(HocClkPartLoad partLoad, bool pretty)
{
    switch(partLoad)
    {
        case HocClkPartLoad_CPU:
            return pretty ? "CPU" : "cpu";
        case HocClkPartLoad_GPU:
            return pretty ? "GPU" : "gpu";
        case HocClkPartLoad_RamBWAll:
            return pretty ? "RAM BW (All)" : "ram_bw_all";
        case HocClkPartLoad_RamBWCpu:
            return pretty ? "RAM BW (CPU)" : "ram_bw_cpu";
        case HocClkPartLoad_RamBWGpu:
            return pretty ? "RAM BW (GPU)" : "ram_bw_gpu";
        case HocClkPartLoad_RamBWPeak:
            return pretty ? "RAM BW (Peak)" : "ram_bw_peak";
        default:
            return "unknown";
    }
}

static inline const char* hocClkFormatGovernor(HocClkGovernor governor, bool pretty)
{
    switch(governor)
    {
        case HocClkGovernor_None:
            return pretty ? "None" : "none";
        case HocClkGovernor_Performance:
            return pretty ? "Performance" : "performance";
        case HocClkGovernor_Powersave:
            return pretty ? "Powersave" : "powersave";
        default:
            return "unknown";
    }
}

static inline const char* hocClkFormatConsoleType(HocClkConsoleType consoleType, bool pretty)
{
    switch(consoleType)
    {
        case HocClkConsoleType_Icosa:
            return pretty ? "Icosa (V1)" : "icosa";
        case HocClkConsoleType_Iowa:
            return pretty ? "Iowa (V2)" : "iowa";
        case HocClkConsoleType_Hoag:
            return pretty ? "Hoag (Lite)" : "hoag";
        case HocClkConsoleType_Aula:
            return pretty ? "Aula (OLED)" : "aula";
        default:
            return "unknown";
    }
}

#ifdef __cplusplus
}
#endif
