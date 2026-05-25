/*
 * Copyright (c) Souldbminer, Lightos_ and Ryazha CLK Contributors
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


#include "main_gui.h"

#include "fatal_gui.h"
#include "app_profile_gui.h"
#include "global_override_gui.h"
#include "misc_gui.h"
#include "living_ladder_gui.h"
#include "../../i18n.hpp"

void MainGui::listUI()
{
    tsl::elm::ListItem* appProfileItem = new tsl::elm::ListItem(i18n::t("Edit App Profile"));
    appProfileItem->setClickListener([this](u64 keys) {
        if((keys & HidNpadButton_A) == HidNpadButton_A && this->context)
        {
            AppProfileGui::changeTo(this->context->applicationId);
            return true;
        }
        return false;
    });
    this->listElement->addItem(appProfileItem);

    tsl::elm::ListItem* globalProfileItem = new tsl::elm::ListItem(i18n::t("Edit Global Profile"));
    globalProfileItem->setClickListener([this](u64 keys) {
        if((keys & HidNpadButton_A) == HidNpadButton_A && this->context)
        {
            AppProfileGui::changeTo(RCLK_GLOBAL_PROFILE_TID);
            return true;
        }
        return false;
    });
    this->listElement->addItem(globalProfileItem);

    tsl::elm::ListItem* globalOverrideItem = new tsl::elm::ListItem(i18n::t("Temporary Overrides"));
    globalOverrideItem->setClickListener([this](u64 keys) {
        if((keys & HidNpadButton_A) == HidNpadButton_A && this->context)
        {
            tsl::changeTo<GlobalOverrideGui>();
            return true;
        }
        return false;
    });
    this->listElement->addItem(globalOverrideItem);

    // Note: Ryazha-Авто / VRR -- скрытое меню. Открывается ТОЛЬКО
    // через X-shortcut в handleInput() ниже. Visible item убран по
    // просьбе юзера ("только скрытое меню на кнопку").


    tsl::elm::ListItem* miscItem = new tsl::elm::ListItem(i18n::t("Settings"));
    miscItem->setClickListener([this](u64 keys) {
        if((keys & HidNpadButton_A) == HidNpadButton_A && this->context)
        {
            tsl::changeTo<MiscGui>();
            return true;
        }
        return false;
    });
    this->listElement->addItem(miscItem);
}

bool MainGui::handleInput(u64 keysDown, u64 keysHeld,
                          const HidTouchState& touchPos,
                          HidAnalogStickState leftStick,
                          HidAnalogStickState rightStick)
{
    if ((keysDown & HidNpadButton_X) && this->context) {
        tsl::changeTo<LivingLadderGui>();
        return true;
    }
    return BaseMenuGui::handleInput(keysDown, keysHeld, touchPos, leftStick, rightStick);
}

void MainGui::refresh()
{
    BaseMenuGui::refresh();
}
