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
#include <stddef.h>
#include <stdbool.h>
#include <switch/types.h>
typedef enum
{
    RyazhaClkSocType_Erista = 0, // T210, found in Icosa and Copper
    RyazhaClkSocType_Mariko,     // T214/T210B01, found in Hoag, Iowa, Calcio and Aula
//  RyazhaClkSocType_Drake,      // T239, found in Switch 2. Maybe someday...
    RyazhaClkSocType_EnumMax
} RyazhaClkSocType;

typedef enum
{
    RyazhaClkConsoleType_Icosa = 0, // V1
    RyazhaClkConsoleType_Iowa,      // V2
    RyazhaClkConsoleType_Hoag,      // Lite
    RyazhaClkConsoleType_Aula,      // OLED
    RyazhaClkConsoleType_EnumMax,
} RyazhaClkConsoleType;

typedef enum {
    RyazhaClkVoltage_SOC = 0, // VDD_SOC rail. 
    RyazhaClkVoltage_EMCVDD2, // DRAM VDD2 rail
    RyazhaClkVoltage_CPU,     // CPU rail
    RyazhaClkVoltage_GPU,     // GPU rail
    RyazhaClkVoltage_EMCVDDQ, // DRAM VDDQ rail
    RyazhaClkVoltage_Display, // Display rail
    RyazhaClkVoltage_Battery, // Battery voltage
    RyazhaClkVoltage_EnumMax,
} RyazhaClkVoltage;

typedef enum
{
    RyazhaClkProfile_Handheld = 0,
    RyazhaClkProfile_HandheldCharging, // Not a real profile, just a marker
    RyazhaClkProfile_HandheldChargingUSB,
    RyazhaClkProfile_HandheldChargingOfficial,
    RyazhaClkProfile_Docked, // Not shown on Lites
    RyazhaClkProfile_EnumMax
} RyazhaClkProfile;

typedef enum
{
    RyazhaClkModule_CPU = 0, 
    RyazhaClkModule_GPU,
    RyazhaClkModule_MEM,
    RyazhaClkModule_Governor,
    RyazhaClkModule_Display,
    RyazhaClkModule_EnumMax,
} RyazhaClkModule;

typedef enum
{
    RyazhaClkThermalSensor_SOC = 0, // SoC temperature in millicelcius
    RyazhaClkThermalSensor_PCB,     // PCB temperature in millicelcius
    RyazhaClkThermalSensor_Skin,    // "Skin" temperature in millicelcius
    RyazhaClkThermalSensor_Battery, // Battery temperature in millicelcius
    RyazhaClkThermalSensor_PMIC, // Always return 50.0C, as thats the only reasonable value the PMIC sensor can generate
    RyazhaClkThermalSensor_CPU, // CPU temperature in millicelcius
    RyazhaClkThermalSensor_GPU, // GPU temperature in millicelcius
    RyazhaClkThermalSensor_MEM, // MEM temperature in millicelcius. Returns the PLLX sensor value on Mariko
    RyazhaClkThermalSensor_PLLX, // PLLX temperature in millicelcius
    RyazhaClkThermalSensor_AO, // AOTAG
    RyazhaClkThermalSensor_BQ24193, // BQ24193 temperature. Refer to BQ24193Temp for returned values
    RyazhaClkThermalSensor_EnumMax
} RyazhaClkThermalSensor;

typedef enum
{
    RyazhaClkPowerSensor_Now = 0,
    RyazhaClkPowerSensor_Avg,
    RyazhaClkPowerSensor_EnumMax
} RyazhaClkPowerSensor;

typedef enum
{
    RyazhaClkPartLoad_EMC = 0,
    RyazhaClkPartLoad_EMCCpu,
    RyazhaClkPartLoad_GPU,
    RyazhaClkPartLoad_CPUMax,
    RyazhaClkPartLoad_BAT, // Battery raw charge percentage
    RyazhaClkPartLoad_FAN,
    RyazhaClkPartLoad_RamBWAll,
    RyazhaClkPartLoad_RamBWCpu,
    RyazhaClkPartLoad_RamBWGpu,
    RyazhaClkPartLoad_RamBWPeak, // Maximum possible RAM bandwidth
    RyazhaClkPartLoad_EnumMax
} RyazhaClkPartLoad;

typedef enum {
    RyazhaClkSpeedo_CPU = 0,
    RyazhaClkSpeedo_GPU,
    RyazhaClkSpeedo_SOC,
    RyazhaClkSpeedo_EnumMax,
} RyazhaClkSpeedo;

typedef enum {
    GPUUVLevel_HiOPT = 0,
    GPUUVLevel_HiOPT15,
    GPUUVLevel_HighUV,
    GPUUVLevel_EnumMax,
} GPUUndervoltLevel;

enum {
    DVFSMode_Disabled = 0,
    DVFSMode_Hijack, // PCV hijack dvfs
    // DVFSMode_OfficialService,
    // DVFSMode_Hack,
    DVFSMode_EnumMax,
};

typedef enum {
    GpuSchedulingMode_DoNotOverride = 0,
    GpuSchedulingMode_Enabled,
    GpuSchedulingMode_Disabled,
    GpuSchedulingMode_EnumMax,
} GpuSchedulingMode;

typedef enum {
    GpuSchedulingOverrideMethod_Ini = 0,
    GpuSchedulingOverrideMethod_NvService,
    GpuSchedulingOverrideMethod_EnumMax,
} GpuSchedulingOverrideMethod;
typedef enum {
    ComponentGovernor_DoNotOverride = 0,
    ComponentGovernor_Disabled      = 1,
    ComponentGovernor_Enabled       = 2,
    ComponentGovernor_EnumMax,
} ComponentGovernorState;
typedef enum {
    RamDisplayMode_VDD2 = 0,
    RamDisplayMode_VDDQ,
    RamDisplayMode_EnumMax,
} RamDisplayMode;

typedef enum {
    MemoryFrequencyMeasurementMode_PLL = 0,
    MemoryFrequencyMeasurementMode_Actmon,
    MemoryFrequencyMeasurementMode_EnumMax,
} MemoryFrequencyMeasurementMode;

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
    AulaDisplayColorMode_PowerReset = 0x20, // Reset to on power on
    AulaDisplayColorMode_Natural = 0x23,
    AulaDisplayColorMode_Vivid = 0x65,
    AulaDisplayColorMode_Night0 = 0x43,
    AulaDisplayColorMode_Night1 = 0x15,
    AulaDisplayColorMode_Night2 = 0x35,
    AulaDisplayColorMode_Night3 = 0x75,
} AulaColorMode;

// typedef enum {
// 	PANEL_JDI_XXX062M     = 0x10,
// 	PANEL_JDI_LAM062M109A = 0x0910, // SI.
// 	PANEL_JDI_LPM062M326A = 0x2610, // LTPS.
// 	PANEL_INL_P062CCA_AZ1 = 0x0F20,
// 	PANEL_AUO_A062TAN01   = 0x0F30,
// 	PANEL_INL_2J055IA_27A = 0x1020,
// 	PANEL_AUO_A055TAN01   = 0x1030,
// 	PANEL_SHP_LQ055T1SW10 = 0x1040,
// 	PANEL_SAM_AMS699VC01  = 0x2050,
    
// 	PANEL_RR_SUPER5_OLED_V1  = 0x10E0,
// 	PANEL_RR_SUPER5_OLED_HD_V1  = 0x10E1,
// 	PANEL_RR_SUPER7_IPS_V1  = 0x0FE0,
// 	PANEL_RR_SUPER7_IPS_HD_V1  = 0x0FE1
// 	// Found on 6/2" clones. Unknown markings. Clone of AUO A062TAN01.
// 	// Quality seems JDI like. Has bad low backlight scaling. ID: [83] 94 [0F]. Sometimes reports [30] 94 [0F]. Both IDs have correct CRC16.
// 	PANEL_OEM_CLONE_6_2   = 0x0F83,
// 	// Found on 5.5" clones with AUO A055TAN02 (59.05A30.001) fake markings.
// 	PANEL_OEM_CLONE_5_5   = 0x00B3,
// 	// Found on 5.5" clones with AUO A055TAN02 (59.05A30.001) fake markings.
// 	PANEL_OEM_CLONE       = 0x0000
// 	//0x0F40 [40] 94 [0F], 5.5" clone
// } RyazhaClkDisplayPanel;

#define HOCCLK_ENUM_VALID(n, v) ((v) < n##_EnumMax)

// Packed u32
// Bits 0-7 - CPU
// Bits 8-15 - GPU
// Bits 16-23 - VRR
// Bits 24-32 - unused

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

static inline const char* hocclkFormatModule(RyazhaClkModule module, bool pretty)
{
    switch(module)
    {
        case RyazhaClkModule_CPU:
            return pretty ? "CPU" : "cpu";
        case RyazhaClkModule_GPU:
            return pretty ? "GPU" : "gpu";
        case RyazhaClkModule_MEM:
            return pretty ? "Memory" : "mem";
        case RyazhaClkModule_Display:
            return pretty ? "Display" : "display";
        case RyazhaClkModule_Governor:
            return pretty ? "Governor" : "governor";
        default:
            return "null";
    }
}

static inline const char* hocclkFormatThermalSensor(RyazhaClkThermalSensor thermSensor, bool pretty)
{
    switch(thermSensor) {
        case RyazhaClkThermalSensor_SOC:
            return pretty ? "SOC" : "soc";
        case RyazhaClkThermalSensor_PCB:
            return pretty ? "PCB" : "pcb";
        case RyazhaClkThermalSensor_Skin:
            return pretty ? "Skin" : "skin";
        case RyazhaClkThermalSensor_Battery:
            return pretty ? "BAT" : "battery";
        case RyazhaClkThermalSensor_PMIC:
            return pretty ? "PMIC" : "pmic";
        case RyazhaClkThermalSensor_CPU:
            return pretty ? "CPU" : "cpu";
        case RyazhaClkThermalSensor_GPU:
            return pretty ? "GPU" : "gpu";
        case RyazhaClkThermalSensor_MEM:
            return pretty ? "MEM" : "mem";
        case RyazhaClkThermalSensor_PLLX:
            return pretty ? "PLLX" : "pllx";
        case RyazhaClkThermalSensor_AO:
            return pretty ? "AO" : "ao";
        case RyazhaClkThermalSensor_BQ24193:
            return pretty ? "BQ24193" : "bq24193";
        default:
            return "unknown";
    }
}

static inline const char* hocclkFormatPowerSensor(RyazhaClkPowerSensor powSensor, bool pretty)
{
    switch(powSensor)
    {
        case RyazhaClkPowerSensor_Now:
            return pretty ? "Now" : "now";
        case RyazhaClkPowerSensor_Avg:
            return pretty ? "Avg" : "avg";
        default:
            return "unknown";
    }
}

static inline const char* hocclkFormatProfile(RyazhaClkProfile profile, bool pretty)
{
    switch(profile)
    {
        case RyazhaClkProfile_Docked:
            return pretty ? "Docked" : "docked";
        case RyazhaClkProfile_Handheld:
            return pretty ? "Handheld" : "handheld";
        case RyazhaClkProfile_HandheldCharging:
            return pretty ? "Charging" : "handheld_charging";
        case RyazhaClkProfile_HandheldChargingUSB:
            return pretty ? "USB Charger" : "handheld_charging_usb";
        case RyazhaClkProfile_HandheldChargingOfficial:
            return pretty ? "PD Charger" : "handheld_charging_official";
        default:
            return "unknown";
    }
}


static inline const char* hocClkFormatVoltage(RyazhaClkVoltage voltage, bool pretty)
{
    switch(voltage)
    {
        case RyazhaClkVoltage_CPU:
            return pretty ? "CPU" : "cpu";
        case RyazhaClkVoltage_GPU:
            return pretty ? "GPU" : "gpu";
        case RyazhaClkVoltage_EMCVDD2:
            return pretty ? "VDD2" : "vdd2";
        case RyazhaClkVoltage_EMCVDDQ:
            return pretty ? "VDDQ" : "vddq";
        case RyazhaClkVoltage_SOC:
            return pretty ? "SOC" : "soc";
        case RyazhaClkVoltage_Display:
            return pretty ? "Display" : "display";
        default:
            return "unknown";
    }
}

static inline const char* hocClkFormatConsoleType(RyazhaClkConsoleType consoleType, bool pretty)
{
    switch(consoleType)
    {
        case RyazhaClkConsoleType_Icosa:
            return pretty ? "Icosa (V1)" : "icosa";
        case RyazhaClkConsoleType_Iowa:
            return pretty ? "Iowa (V2)" : "iowa";
        case RyazhaClkConsoleType_Hoag:
            return pretty ? "Hoag (Lite)" : "hoag";
        case RyazhaClkConsoleType_Aula:
            return pretty ? "Aula (OLED)" : "aula";
        default:
            return "unknown";
    }
}

// static inline const char* hocClkFormatPanel(RyazhaClkDisplayPanel panel, bool pretty)
// {
//     switch(panel)
//     {
//         case PANEL_JDI_XXX062M:
//             return pretty ? "JDI XXX062M" : "jdi_xxx062m";
//         case PANEL_JDI_LAM062M109A:
//             return pretty ? "JDI LAM062M109A" : "jdi_lam062m109a";
//         case PANEL_JDI_LPM062M326A:
//             return pretty ? "JDI LPM062M326A" : "jdi_lpm062m326a";
//         case PANEL_INL_P062CCA_AZ1:
//             return pretty ? "INL P062CCA-AZ1" : "inl_p062cca_az1";
//         case PANEL_AUO_A062TAN01:
//             return pretty ? "AUO A062TAN01" : "auo_a062tan01";
//         case PANEL_INL_2J055IA_27A:
//             return pretty ? "INL 2J055IA-27A" : "inl_2j055ia_27a";
//         case PANEL_AUO_A055TAN01:
//             return pretty ? "AUO A055TAN01" : "auo_a055tan01";
//         case PANEL_SHP_LQ055T1SW10:
//             return pretty ? "SHP LQ055T1SW10" : "shp_lq055t1sw10";
//         case PANEL_SAM_AMS699VC01:
//             return pretty ? "SAM AMS699VC01" : "sam_ams699vc01";
//         case PANEL_RR_SUPER5_OLED_V1:
//             return pretty ? "SUPER5 OLED" : "rr_super5_oled_v1";
//         case PANEL_RR_SUPER5_OLED_HD_V1:
//             return pretty ? "SUPER5 OLED HD" : "rr_super5_oled_hd_v1";
//         case PANEL_RR_SUPER7_IPS_V1:
//             return pretty ? "SUPER7 IPS" : "rr_super7_ips_v1";
//         case PANEL_RR_SUPER7_IPS_HD_V1:
//             return pretty ? "RR Super7 IPS HD V1" : "rr_super7_ips_hd_v1";
//         case PANEL_OEM_CLONE_6_2:
//             return pretty ? "OEM Clone 6.2" : "oem_clone_6_2";
//         case PANEL_OEM_CLONE_5_5:
//             return pretty ? "OEM Clone 5.5" : "oem_clone_5_5";
//         case PANEL_OEM_CLONE:
//             return pretty ? "OEM Clone" : "oem_clone";
//         default:
//             return "unknown";
//     }
// }