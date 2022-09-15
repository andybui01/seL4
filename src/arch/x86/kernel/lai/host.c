
#include <stdarg.h>
#include <types.h>

#include <lai/host.h>

#include <plat/machine/acpi.h>

static lai_acpi_rsdp_t *acpi_rsdp;
static lai_acpi_fadt_t *fadt_data;
static lai_acpi_aml_t *dsdt_data;
static lai_acpi_aml_t *ssdt_data;

const char lai_acpi_str_fadt[] = {'F', 'A', 'C', 'P', 0};
const char lai_acpi_str_dsdt[] = {'D', 'S', 'D', 'T', 0};
const char lai_acpi_str_ssdt[] = {'S', 'S', 'D', 'T', 0};

void laihost_set_rsdp(acpi_rsdp_t *ptr)
{
    acpi_rsdp = (lai_acpi_rsdp_t *) ptr;
}

void *laihost_malloc(word_t size)
{
    printf("%s not implemented!\n", __func__);
    assert(0);
}

void *laihost_realloc(void *oldptr, word_t newsize, word_t oldsize)
{
    printf("%s not implemented!\n", __func__);
    assert(0);
}

void laihost_free(void *ptr, word_t size)
{
    printf("%s not implemented!\n", __func__);
    assert(0);
}

void laihost_log(int level, const char *msg)
{
    if (level == LAI_DEBUG_LOG) {
        printf("DEBUG: ");
    } else {
        printf("WARN: ");
    }

    printf("%s\n", msg);
}

void laihost_panic(const char *msg)
{
    laihost_log(LAI_WARN_LOG, msg);
    assert(0);
}

void *laihost_scan(int table, word_t index)
{
    /* Don't bother with the PSDT */
    if (table == LAI_PSDT) {
        return NULL;
    }

    if (table == LAI_FADT) {
        printf("Scanning for FADT\n");
        lai_acpi_rsdt_t *acpi_rsdt_mapped;
        lai_acpi_fadt_t *acpi_fadt;
        lai_acpi_fadt_t *acpi_fadt_mapped;
        acpi_rsdt_mapped = (lai_acpi_rsdt_t *)acpi_table_init((lai_acpi_rsdt_t *)(word_t)acpi_rsdp->rsdt, ACPI_RSDT);
        assert(acpi_rsdt_mapped->header.length >= sizeof(lai_acpi_header_t));

        /* According to the ACPI spec, the FADT is always the first table in the RSDT */
        acpi_fadt = (lai_acpi_fadt_t *)(word_t)acpi_rsdt_mapped->tables[0];
        acpi_fadt_mapped = (lai_acpi_fadt_t *)acpi_table_init(acpi_fadt, ACPI_RSDT);

        assert(strncmp(lai_acpi_str_fadt, acpi_fadt_mapped->header.signature, 4) == 0);
        assert(acpi_calc_checksum((char *) acpi_fadt_mapped, acpi_fadt_mapped->header.length) == 0);
        memcpy(fadt_data, acpi_fadt_mapped, sizeof(lai_acpi_fadt_t));
        return fadt_data;
    } else if (table == LAI_DSDT) {
        printf("Scanning for DSDT\n");
        lai_acpi_aml_t *acpi_dsdt;
        lai_acpi_aml_t *acpi_dsdt_mapped;

        /* DSDT is referred to in FADT, we assume the FADT has already been scanned */
        assert(fadt_data);

        /* Get physical address of DSDT */
        acpi_dsdt = (lai_acpi_aml_t *)(word_t)fadt_data->dsdt;
        acpi_dsdt_mapped = (lai_acpi_aml_t *)acpi_table_init(acpi_dsdt, ACPI_AML);

        assert(strncmp(lai_acpi_str_dsdt, acpi_dsdt_mapped->header.signature, 4) == 0);
        assert(acpi_calc_checksum((char *) acpi_dsdt_mapped, acpi_dsdt_mapped->header.length) == 0);
        memcpy(dsdt_data, acpi_dsdt_mapped, sizeof(lai_acpi_aml_t));
        return dsdt_data;
    } else if (table == LAI_SSDT) {
        printf("Scanning for SSDT\n");
        unsigned int entries;
        word_t count = 0;

        lai_acpi_rsdt_t *acpi_rsdt_mapped;
        lai_acpi_aml_t *acpi_ssdt;
        lai_acpi_aml_t *acpi_ssdt_mapped;

        acpi_rsdt_mapped = (lai_acpi_rsdt_t *)acpi_table_init((lai_acpi_rsdt_t *)(word_t)acpi_rsdp->rsdt, ACPI_RSDT);
        assert(acpi_rsdt_mapped->header.length >= sizeof(lai_acpi_header_t));

        /* Divide by uint32_t explicitly as this is the size as mandated by the ACPI standard */
        entries = (acpi_rsdt_mapped->header.length - sizeof(lai_acpi_header_t)) / sizeof(uint32_t);
        for (int i = 0; i < entries; i++) {
            acpi_ssdt = (lai_acpi_aml_t *)(word_t)acpi_rsdt_mapped->tables[i];
            acpi_ssdt_mapped = (lai_acpi_aml_t *)acpi_table_init(acpi_ssdt, ACPI_AML);

            if (strncmp(lai_acpi_str_ssdt, acpi_ssdt_mapped->header.signature, 4) == 0) {
                if (count != index) {
                    count++;
                    continue;
                }

                /* We've reached the nth entry for the SSDT */
                assert(acpi_calc_checksum((char *) acpi_ssdt_mapped, acpi_ssdt_mapped->header.length) == 0);
                memcpy(ssdt_data, acpi_ssdt_mapped, sizeof(lai_acpi_aml_t));
                return ssdt_data;
            }
        }
    }

    return NULL;
}

void *laihost_map(word_t address, word_t count)
{
    printf("%s not implemented!\n", __func__);
    assert(0);
}

void laihost_unmap(void *pointer, word_t count)
{
    printf("%s not implemented!\n", __func__);
    assert(0);
}


void laihost_outb(uint16_t port, uint8_t val)
{
    printf("%s not implemented!\n", __func__);
    assert(0);
}

void laihost_outw(uint16_t port, uint16_t val)
{
    printf("%s not implemented!\n", __func__);
    assert(0);
}

void laihost_outd(uint16_t port, uint32_t val)
{
    printf("%s not implemented!\n", __func__);
    assert(0);
}


uint8_t laihost_inb(uint16_t port)
{
    printf("%s not implemented!\n", __func__);
    assert(0);
}

uint16_t laihost_inw(uint16_t port)
{
    printf("%s not implemented!\n", __func__);
    assert(0);
}

uint32_t laihost_ind(uint16_t port)
{
    printf("%s not implemented!\n", __func__);
    assert(0);
}


void laihost_pci_writeb(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset,
                                              uint8_t val)
{
    printf("%s not implemented!\n", __func__);
    assert(0);
}

void laihost_pci_writew(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset,
                                              uint16_t val)
{
    printf("%s not implemented!\n", __func__);
    assert(0);
}

void laihost_pci_writed(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset,
                                              uint32_t val)
{
    printf("%s not implemented!\n", __func__);
    assert(0);
}


uint8_t laihost_pci_readb(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset)
{
    printf("%s not implemented!\n", __func__);
    assert(0);
}

uint16_t laihost_pci_readw(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset)
{
    printf("%s not implemented!\n", __func__);
    assert(0);
}

uint32_t laihost_pci_readd(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset)
{
    printf("%s not implemented!\n", __func__);
    assert(0);
}


void laihost_sleep(uint64_t ms)
{
    printf("%s not implemented!\n", __func__);
    assert(0);
}

uint64_t laihost_timer(void)
{
    printf("%s not implemented!\n", __func__);
    assert(0);
}


int laihost_sync_wait(struct lai_sync_state *sync, unsigned int val,
                                            int64_t deadline)
{
    printf("%s not implemented!\n", __func__);
    assert(0);
}

void laihost_sync_wake(struct lai_sync_state *sync)
{
    printf("%s not implemented!\n", __func__);
    assert(0);
}


void laihost_handle_amldebug(lai_variable_t *var)
{
    printf("%s not implemented!\n", __func__);
    assert(0);
}

void laihost_handle_global_notify(lai_nsnode_t *node, int code)
{
    printf("%s not implemented!\n", __func__);
    assert(0);
}
