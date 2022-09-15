/*
 * Lightweight AML Interpreter
 * Copyright (C) 2018-2022 The lai authors
 */

#pragma once

#include <stdarg.h>
#include <types.h>

#include <plat/machine/acpi.h>

#ifdef __cplusplus
extern "C" {
#endif

struct lai_nsnode;
typedef struct lai_nsnode lai_nsnode_t;

struct lai_variable_t;
typedef struct lai_variable_t lai_variable_t;

#define LAI_DEBUG_LOG 1
#define LAI_WARN_LOG 2

/* ACPI system description tables */
#define LAI_FADT 0
#define LAI_DSDT 1
#define LAI_SSDT 2
#define LAI_PSDT 3

struct lai_sync_state {
    // Used internally by LAI. Read-only for the host.
    unsigned int val;

    // Freely available to the host.
    // Intended to implement a mutex that protects p.
    unsigned int s;

    // Freely available to the host.
    // Intended to hold a pointer to a wait queue.
    void *p;
};

void laihost_set_rsdp(acpi_rsdp_t *acpi_rsdp);

// OS-specific functions.
void *laihost_malloc(word_t);
void *laihost_realloc(void *, word_t newsize, word_t oldsize);
void laihost_free(void *, word_t);

__attribute__((weak)) void laihost_log(int, const char *);
__attribute__((weak, noreturn)) void laihost_panic(const char *);

__attribute__((weak)) void *laihost_scan(int, word_t);
__attribute__((weak)) void *laihost_map(word_t, word_t);
__attribute__((weak)) void laihost_unmap(void *, word_t);
__attribute__((weak)) void laihost_outb(uint16_t, uint8_t);
__attribute__((weak)) void laihost_outw(uint16_t, uint16_t);
__attribute__((weak)) void laihost_outd(uint16_t, uint32_t);
__attribute__((weak)) uint8_t laihost_inb(uint16_t);
__attribute__((weak)) uint16_t laihost_inw(uint16_t);
__attribute__((weak)) uint32_t laihost_ind(uint16_t);

__attribute__((weak)) void laihost_pci_writeb(uint16_t, uint8_t, uint8_t, uint8_t, uint16_t,
                                              uint8_t);
__attribute__((weak)) uint8_t laihost_pci_readb(uint16_t, uint8_t, uint8_t, uint8_t, uint16_t);
__attribute__((weak)) void laihost_pci_writew(uint16_t, uint8_t, uint8_t, uint8_t, uint16_t,
                                              uint16_t);
__attribute__((weak)) uint16_t laihost_pci_readw(uint16_t, uint8_t, uint8_t, uint8_t, uint16_t);
__attribute__((weak)) void laihost_pci_writed(uint16_t, uint8_t, uint8_t, uint8_t, uint16_t,
                                              uint32_t);
__attribute__((weak)) uint32_t laihost_pci_readd(uint16_t, uint8_t, uint8_t, uint8_t, uint16_t);

__attribute__((weak)) void laihost_sleep(uint64_t);
__attribute__((weak)) uint64_t laihost_timer(void);

__attribute__((weak)) int laihost_sync_wait(struct lai_sync_state *, unsigned int val,
                                            int64_t deadline);
__attribute__((weak)) void laihost_sync_wake(struct lai_sync_state *);

__attribute__((weak)) void laihost_handle_amldebug(lai_variable_t *);
__attribute__((weak)) void laihost_handle_global_notify(lai_nsnode_t *, int);

#ifdef __cplusplus
}
#endif
