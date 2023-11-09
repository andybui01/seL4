/*
 * Copyright 2023, NIO
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>
#include <stdint.h>
#include <util.h>
#include <machine/io.h>
#include <plat/machine/devices_gen.h>

#define TCU_TX_REG          (0)
#define TX_NUM_BYTES(n)     ((n) << 24)
#define TX_FLUSH            BIT(26)
#define TX_INTR_TRIGGER     BIT(31)
#define UART_REG(mmio, x)   ((volatile uint32_t *)(mmio + (x)))

#ifdef CONFIG_PRINTING
void uart_drv_putchar(unsigned char c)
{
    uint32_t reg_val;

    /* We are writing one byte */
    reg_val = TX_NUM_BYTES(1);
    reg_val |= TX_INTR_TRIGGER;
    reg_val |= c;

    if (c == '\r' || c == '\n') {
        reg_val |= TX_FLUSH;
    }

    while (*UART_REG(UART_PPTR, TCU_TX_REG) & TX_INTR_TRIGGER);

    *UART_REG(UART_PPTR, TCU_TX_REG) = reg_val;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned char uart_drv_getchar(void)
{
    return 0;
}
#endif /* CONFIG_DEBUG_BUILD */
