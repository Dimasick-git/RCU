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

#include <rclk.h>
#include <switch.h>
#include "../hos/apm_ext.h"
#include <i2c.h>
#include "../i2c/i2cDrv.h"
#include <t210.h>
#include <max17050.h>
#include <tmp451.h>
#include <ipc_server.h>
#include <lockable_mutex.h>
#include <cmath>
#include <battery.h>
#include <pwm.h>
#include "board.hpp"
#include "../tsensor/soctherm.hpp"
#include "../tsensor/aotag.hpp"
#include "../tsensor/bq24193.hpp"
#include "../file/config.hpp"

namespace board {

    s32 GetTemperatureMilli(RyazhaClkThermalSensor sensor) {
        s32 millis = 0;
        BatteryChargeInfo info;

        tsensor::TSensorTemps temps = {};
        tsensor::ReadTSensors(temps);

        switch(sensor) {
            case RyazhaClkThermalSensor_SOC: {
                millis = tmp451TempSoc();
                break;
            }
            case RyazhaClkThermalSensor_PCB: {
                millis = tmp451TempPcb();
                break;
            }
            case RyazhaClkThermalSensor_Skin: {
                if (HOSSVC_HAS_TC) {
                    Result rc;
                    rc = tcGetSkinTemperatureMilliC(&millis);
                    ASSERT_RESULT_OK(rc, "tcGetSkinTemperatureMilliC");
                }
                break;
            }
            case RyazhaClkThermalSensor_Battery: {
                batteryInfoGetChargeInfo(&info);
                millis = batteryInfoGetTemperatureMiliCelsius(&info);
                break;
            }
            case RyazhaClkThermalSensor_PMIC: {
                millis = 50000;
                break;
            }
            case RyazhaClkThermalSensor_CPU: {
                millis = temps.cpu;
                break;
            }
            case RyazhaClkThermalSensor_GPU: {
                millis = temps.gpu;
                break;
            }
            case RyazhaClkThermalSensor_MEM: {
                if (board::GetSocType() == RyazhaClkSocType_Mariko && tsensor::IsInitialized() && tsensor::ReadAotag() > 0) {
                    millis = (temps.pllx * 0.10f) + (tsensor::ReadAotag() * 0.90f);
                } else {
                    millis = board::GetSocType() == RyazhaClkSocType_Mariko ? temps.pllx : temps.mem;
                }
                break;
            }
            case RyazhaClkThermalSensor_PLLX: {
                millis = temps.pllx;
                break;
            }
            case RyazhaClkThermalSensor_BQ24193: {
                millis = bq24193::getBQTemp();
                break;
            }
            case RyazhaClkThermalSensor_AO: {
                millis = tsensor::ReadAotag();
                break;
            }
            default: {
                ASSERT_ENUM_VALID(RyazhaClkThermalSensor, sensor);
            }
        }

        return std::max(0, millis);
    }

    s32 GetPowerMw(RyazhaClkPowerSensor sensor) {
        switch (sensor) {
            case RyazhaClkPowerSensor_Now:
                return max17050PowerNow();
            case RyazhaClkPowerSensor_Avg:
                return max17050PowerAvg();
            default:
                ASSERT_ENUM_VALID(RyazhaClkPowerSensor, sensor);
        }

        return 0;
    }

}
