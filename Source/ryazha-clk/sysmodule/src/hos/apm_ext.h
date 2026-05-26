/*
 * Shim для совместимости -- содержимое переехало в nxExt.
 * Local-копия дублировала <nxExt/apm_ext.h> и приводила к
 * "redefinition" ошибкам когда оба пути попадали в один TU.
 * Здесь оставлен forward на nxExt версию, чтобы существующие
 * #include "apm_ext.h" продолжали работать.
 */
#pragma once
#include <nxExt/apm_ext.h>
