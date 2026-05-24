/*
 * Copyright (c) 2023 KazushiMe
 * Licensed under the GPLv2
 *
 * Shim для совместимости: содержимое переехало в common/include/i2c.h
 * (унифицированная версия). Раньше тут лежал расходящийся duplicate,
 * который при одновременном включении с <i2c.h> давал redefinition.
 *
 * Все callers могут продолжать #include "i2cDrv.h" -- shim пробрасывает
 * на канонический header.
 */

#pragma once
#include <i2c.h>
