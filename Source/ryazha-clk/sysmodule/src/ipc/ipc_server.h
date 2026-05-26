/*
 * Shim для совместимости -- содержимое переехало в nxExt.
 * Local-копия дублировала <nxExt/ipc_server.h> и приводила к
 * "redefinition" ошибкам когда оба пути попадали в один TU.
 * Здесь оставлен forward на nxExt версию, чтобы существующие
 * #include "ipc_server.h" продолжали работать.
 */
#pragma once
#include <nxExt/ipc_server.h>
