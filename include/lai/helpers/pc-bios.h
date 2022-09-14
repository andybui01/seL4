/*
 * Lightweight AML Interpreter
 * Copyright (C) 2018-2022 The lai authors
 */

#pragma once

#include <lai/core.h>

#ifdef __cplusplus
extern "C" {
#endif

struct lai_rsdp_info {
    // ACPI version (1 or 2).
    int acpi_version;
    // Physical addresses of RSDP and RSDT.
    word_t rsdp_address;
    word_t rsdt_address;
    word_t xsdt_address;
};

lai_api_error_t lai_bios_detect_rsdp_within(word_t base, word_t length,
                                            struct lai_rsdp_info *info);

lai_api_error_t lai_bios_detect_rsdp(struct lai_rsdp_info *info);

#ifdef __cplusplus
}
#endif
