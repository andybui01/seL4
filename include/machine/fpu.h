/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#pragma once

#include <config.h>
#include <object/structures.h>
#include <model/statedata.h>

extern bool_t isFPUEnabledCached[CONFIG_MAX_NUM_NODES];

/* Check if FPU is enable */
static inline bool_t isFpuEnable(void)
{
    return isFPUEnabledCached[CURRENT_CPU_INDEX()];
}

#include <arch/machine/fpu.h>

#ifdef CONFIG_HAVE_FPU

/* Perform any actions required for the deletion of the given thread. */
void fpuThreadDelete(tcb_t *thread);

/* Handle an FPU exception. */
exception_t handleFPUFault(void);

void switchLocalFpuOwner(tcb_fpu_t *new_owner);

/* Switch the current owner of the FPU state on the core specified by 'cpu'. */
void switchFpuOwner(tcb_fpu_t *new_owner, word_t cpu);

/* Returns whether or not the passed thread is using the current active fpu state */
static inline bool_t nativeThreadUsingFPU(tcb_t *thread)
{
    return &thread->tcbArch.tcbFpu ==
           NODE_STATE_ON_CORE(ksActiveFPU, thread->tcbAffinity);
}

static inline void FORCE_INLINE eagerFPURestore(tcb_t *thread)
{
    /* If next thread doesn't have an fpu object and fpu is currently
     * enabled, we can disable fpu */
    if (!thread->tcbArch.tcbFpu.tcbBoundFpu
        && isFpuEnable()) {
        disableFpu();
    } else {
        if (nativeThreadUsingFPU(thread)) {
            enableFpu();
        } else {
            switchLocalFpuOwner(&thread->tcbArch.tcbFpu);
        }
    }
}

static inline void doUnbindFpu(fpu_t *fpuPtr, tcb_t *tcb)
{
#ifndef CONFIG_ARCH_X86
    memzero(fpuPtr, sizeof(fpu_t));
#endif
    memzero(&tcb->tcbArch.tcbFpu, sizeof(tcb_fpu_t));
}

static void UNUSED unbindMaybeFpu(fpu_t *fpuPtr)
{
    tcb_t *tcb = fpuPtr->fpuBoundTCB;
    if (tcb) {
        doUnbindFpu(fpuPtr, tcb);
    }
}

static void unbindFpu(tcb_t *tcb)
{
    fpu_t *fpuPtr = tcb->tcbArch.tcbFpu.tcbBoundFpu;
    if (fpuPtr) {
        doUnbindFpu(fpuPtr, tcb);
    }
}

static void bindFpu(tcb_t *tcb, fpu_t *fpuPtr)
{
    fpuPtr->fpuBoundTCB = tcb;
    tcb->tcbArch.tcbFpu.tcbBoundFpu = fpuPtr;
#ifdef CONFIG_ARCH_X86
    initFpuContext(tcb);
#endif
}

#endif /* CONFIG_HAVE_FPU */
