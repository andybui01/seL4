/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#pragma once

#include <config.h>
#include <object/structures.h>
#include <model/statedata.h>
#include <arch/machine/fpu.h>

#ifdef CONFIG_HAVE_FPU

/* Perform any actions required for the deletion of the given thread. */
void fpuThreadDelete(tcb_t *thread);

/* Handle an FPU exception. */
exception_t handleFPUFault(void);

void switchLocalFpuOwner(user_fpu_state_t *new_owner);

/* Switch the current owner of the FPU state on the core specified by 'cpu'. */
void switchFpuOwner(user_fpu_state_t *new_owner, word_t cpu);

/* Returns whether or not the passed thread is using the current active fpu state */
static inline bool_t nativeThreadUsingFPU(tcb_t *thread)
{
#ifdef CONFIG_ARCH_AARCH64
    cap_t cap = TCB_PTR_CTE_PTR(thread, tcbFPU)->cap;
    return &FPU_PTR(cap_fpu_cap_get_capFPUPtr(cap))->fpuState ==
           NODE_STATE_ON_CORE(ksActiveFPUState, thread->tcbAffinity);
#else
    return &thread->tcbArch.tcbContext.fpuState ==
           NODE_STATE_ON_CORE(ksActiveFPUState, thread->tcbAffinity);
#endif
}

static inline void FORCE_INLINE lazyFPURestore(tcb_t *thread)
{
    if (unlikely(NODE_STATE(ksActiveFPUState))) {
        /* If we have enabled/disabled the FPU too many times without
         * someone else trying to use it, we assume it is no longer
         * in use and switch out its state. */
        if (unlikely(NODE_STATE(ksFPURestoresSinceSwitch) > CONFIG_FPU_MAX_RESTORES_SINCE_SWITCH)) {
            switchLocalFpuOwner(NULL);
            NODE_STATE(ksFPURestoresSinceSwitch) = 0;
        } else {
            if (likely(nativeThreadUsingFPU(thread))) {
                /* We are using the FPU, make sure it is enabled */
                enableFpu();
            } else {
                /* Someone is using the FPU and it might be enabled */
                disableFpu();
            }
            NODE_STATE(ksFPURestoresSinceSwitch)++;
        }
    } else {
        /* No-one (including us) is using the FPU, so we assume it
         * is currently disabled */
    }
}

#endif /* CONFIG_HAVE_FPU */
