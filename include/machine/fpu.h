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

#ifdef CONFIG_ARCH_AARCH64
void switchLocalFpuOwner(fpu_t *new_owner);
#else
void switchLocalFpuOwner(user_fpu_state_t *new_owner);
#endif

/* Switch the current owner of the FPU state on the core specified by 'cpu'. */
#ifdef CONFIG_ARCH_AARCH64
void switchFpuOwner(fpu_t *new_owner, word_t cpu);
#else
void switchFpuOwner(user_fpu_state_t *new_owner, word_t cpu);
#endif

/* Returns whether or not the passed thread is using the current active fpu state */
static inline bool_t nativeThreadUsingFPU(tcb_t *thread)
{
#ifdef CONFIG_ARCH_AARCH64
    return &thread->tcbArch.fpu ==
           NODE_STATE_ON_CORE(ksActiveFPU, thread->tcbAffinity);
#else
    return &thread->tcbArch.tcbContext.fpuState ==
           NODE_STATE_ON_CORE(ksActiveFPUState, thread->tcbAffinity);
#endif
}

#ifdef CONFIG_ARCH_AARCH64
static inline void FORCE_INLINE eagerFPURestore(tcb_t *thread)
{
    /* Check if thread has an FPU capability */
    if ((cap_get_capType(TCB_PTR_CTE_PTR(thread, tcbFPU)->cap) != cap_fpu_cap)) {
        /* only disable FPU if its enabled */
        if ((isFPUEnabledCached[CURRENT_CPU_INDEX()])) {
            /* ID: 3/4r */
            disableFpu();
        } else {
            /* 1 */
        }

        return;
    }

    if (nativeThreadUsingFPU(thread)) {
        /* ID: 3r/4 */
        enableFpu();
    } else {
        /* ID: 2 */
        switchLocalFpuOwner(&thread->tcbArch.fpu);
}
#else
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
#endif

#endif /* CONFIG_HAVE_FPU */
