/*
 * Shim для совместимости -- содержимое переехало в nxExt.
 * Local-копия дублировала <nxExt/max17050.h> и приводила к
 * "redefinition" ошибкам когда оба пути попадали в один TU.
 * Здесь оставлен forward на nxExt версию, чтобы существующие
 * #include "max17050.h" продолжали работать.
 */
#pragma once
#include <nxExt/max17050.h>
