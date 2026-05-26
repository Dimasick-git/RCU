/*
 * Shim для совместимости -- содержимое переехало в nxExt.
 * Local-копия дублировала <nxExt/tmp451.h> и приводила к
 * "redefinition" ошибкам когда оба пути попадали в один TU.
 * Здесь оставлен forward на nxExt версию, чтобы существующие
 * #include "tmp451.h" продолжали работать.
 */
#pragma once
#include <nxExt/tmp451.h>
