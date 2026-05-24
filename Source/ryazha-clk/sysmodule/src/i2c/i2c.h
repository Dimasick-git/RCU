/*
 * Shim для совместимости -- содержимое переехало в nxExt.
 * Local-копия дублировала <nxExt/i2c.h> и приводила к
 * "redefinition" ошибкам когда оба пути попадали в один TU.
 * Здесь оставлен forward на nxExt версию, чтобы существующие
 * #include "i2c.h" продолжали работать.
 */
#pragma once
#include <nxExt/i2c.h>
