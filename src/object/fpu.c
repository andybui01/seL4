#include <types.h>
#include <object/fpu.h>

#ifdef CONFIG_HAVE_FPU

exception_t decodeFPUControlInvocation(word_t invLabel, word_t length,
                                       cte_t *srcSlot, word_t *buffer)
{
    if (invLabel != FPUIssueFPUObject) {
        userError("FPUControl: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        return EXCEPTION_SYSCALL_ERROR;
    }

    word_t index, depth;
    cte_t *untypedSlot, *destSlot;
    cap_t untyped, root;
    lookupSlot_ret_t lu_ret;
    void *region;
    exception_t status;

    if (unlikely(length < 2 ||
                 current_extra_caps.excaprefs[0] == NULL ||
                 current_extra_caps.excaprefs[1] == NULL)) {
        userError("FPUControl: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        return EXCEPTION_SYSCALL_ERROR;
    }

    index = getSyscallArg(0, buffer);
    depth = getSyscallArg(1, buffer);

    untypedSlot = current_extra_caps.excaprefs[0];
    untyped = untypedSlot->cap;
    root = current_extra_caps.excaprefs[1]->cap;

    if (unlikely(cap_get_capType(untyped) != cap_untyped_cap ||
                 cap_untyped_cap_get_capBlockSize(untyped) != seL4_FPUBits ||
                 cap_untyped_cap_get_capIsDevice(untyped))) {
        userError("FPUControl: Invalid untyped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalidCapNumber = 0;

        return EXCEPTION_SYSCALL_ERROR;
    }

    status = ensureNoChildren(untypedSlot);
    if (unlikely(status != EXCEPTION_NONE)) {
        userError("FPUControl: Untyped has children.");
        return status;
    }

    region = WORD_PTR(cap_untyped_cap_get_capPtr(untyped));

    lu_ret = lookupTargetSlot(root, index, depth);
    if (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Target slot for new FPU cap invalid: cap %lu.",
                  getExtraCPtr(buffer, 0));
        return lu_ret.status;
    }
    destSlot = lu_ret.slot;

    status = ensureEmptySlot(destSlot);
    if (unlikely(status != EXCEPTION_NONE)) {
        userError("Target slot for new FPU cap not empty: cap %lu.",
                  getExtraCPtr(buffer, 0));
        return status;
    }

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    return invokeFPUControl(region, destSlot, untypedSlot);
}

exception_t invokeFPUControl(void *region, cte_t *slot, cte_t *untypedSlot)
{
    cap_untyped_cap_ptr_set_capFreeIndex(&(untypedSlot->cap),
                                         MAX_FREE_INDEX(cap_untyped_cap_get_capBlockSize(untypedSlot->cap)));
    memzero(region, BIT(seL4_FPUBits));

    cteInsert(cap_fpu_cap_new(0, WORD_REF(region)), untypedSlot, slot);

    return EXCEPTION_NONE;
}

#endif /* CONFIG_HAVE_FPU */
