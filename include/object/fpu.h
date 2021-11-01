#pragma once

#include <types.h>

#ifdef CONFIG_HAVE_FPU

exception_t decodeFPUControlInvocation(word_t invLabel, word_t length,
                                       cte_t *srcSlot, word_t *buffer);
exception_t invokeFPUControl(void *region, cte_t *slot, cte_t *untypedSlot);

#endif /* CONFIG_HAVE_FPU */
