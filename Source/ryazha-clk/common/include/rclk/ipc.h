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
#include "clock_manager.h"

#define RCLK_IPC_API_VERSION 2
#define RCLK_IPC_SERVICE_NAME "rclk:clk"

enum RyazhaClkIpcCmd
{
    RyazhaClkIpcCmd_GetApiVersion = 0,
    RyazhaClkIpcCmd_GetVersionString = 1,
    RyazhaClkIpcCmd_GetCurrentContext = 2,
    RyazhaClkIpcCmd_Exit = 3,
    RyazhaClkIpcCmd_GetProfileCount = 4,
    RyazhaClkIpcCmd_GetProfiles = 5,
    RyazhaClkIpcCmd_SetProfiles = 6,
    RyazhaClkIpcCmd_SetEnabled = 7,
    RyazhaClkIpcCmd_SetOverride = 8,
    RyazhaClkIpcCmd_GetConfigValues = 9,
    RyazhaClkIpcCmd_SetConfigValues = 10,
    RyazhaClkIpcCmd_GetFreqList = 11,
    RyazhaClkIpcCmd_SetKipData = 12,
    RyazhaClkIpcCmd_GetKipData = 13,
};


typedef struct
{
    uint64_t tid;
    RyazhaClkTitleProfileList profiles;
} RyazhaClkIpc_SetProfiles_Args;

typedef struct
{
    RyazhaClkModule module;
    uint32_t hz;
} RyazhaClkIpc_SetOverride_Args;

typedef struct
{
    RyazhaClkModule module;
    uint32_t maxCount;
} RyazhaClkIpc_GetFreqList_Args;