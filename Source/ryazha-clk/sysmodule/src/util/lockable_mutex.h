/*
 * Copyright (c) Souldbminer, Lightos_ and Ryazha CLK Contributors
 *
 * This file used to duplicate the LockableMutex class from nxExt. Both
 * copies were identical, but C++ does not deduplicate class definitions
 * across distinct header paths -- including both led to "redefinition of
 * class LockableMutex" build errors.
 *
 * Now this file is a thin shim that forwards to the nxExt version, so
 * existing TUs that #include "util/lockable_mutex.h" continue to work
 * without modification.
 */

#pragma once

#include <nxExt/cpp/lockable_mutex.h>
