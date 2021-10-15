/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#pragma once

#include <mode/object/structures.h>

#define tcbArchCNodeEntries tcbCNodeEntries

static inline bool_t CONST Arch_isCapRevocable(cap_t derivedCap, cap_t srcCap)
{
#if defined(CONFIG_ARCH_AARCH64) && defined(CONFIG_HAVE_FPU)
    switch (cap_get_capType(derivedCap)) {
        case cap_fpu_cap:
            return cap_get_capType(srcCap) ==
                   cap_fpu_cap;
        default:
            return false;
    }
#else
    return false;
#endif
}
