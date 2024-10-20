/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
/*
 * Arm SCP/MCP Software
 * Copyright (c) 2013-2021, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <machine/assembler.h>

BEGIN_FUNC(lockTLBEntry)
    ret
END_FUNC(lockTLBEntry)

/*
 * This macro can be used for implementing various data cache operations `op`
 */
.macro do_dcache_maintenance_by_va op
    /* Exit early if size is zero */
    cbz x1, exit_loop_\op
    dcache_line_size x2, x3
    add x1, x0, x1
    sub x3, x2, #1
    bic x0, x0, x3
loop_\op:
    dc  \op, x0
    add x0, x0, x2
    cmp x0, x1
    b.lo    loop_\op
    dsb sy
exit_loop_\op:
    ret
.endm

/**
 * clean_dcache_va - Clean base VA to size.
 * x0 start va
 * x1 size
 */
BEGIN_FUNC(clean_dcache_va)
    do_dcache_maintenance_by_va cvac
END_FUNC(clean_dcache_va)

/**
 * clean_inval_dcache_va - Clean + Invalidate base VA to size.
 * x0 start va
 * x1 size
 */
BEGIN_FUNC(clean_dcache_va)
    do_dcache_maintenance_by_va civac
END_FUNC(clean_dcache_va)
