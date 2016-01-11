#ifndef HPM_H_
#define HPM_H_

#define HPM_SUPPORTED_VERSION 0x00

#define FW_REV_MAJOR 1 /* Binary encoded */
#define FW_REV_MINOR 0 /* BCD encoded */
#define FW_REV_AUX_0 0
#define FW_REV_AUX_1 0
#define FW_REV_AUX_2 0
#define FW_REV_AUX_3 0

#define HPM_UPGRADE_TIMEOUT 10 /* in 5 seconds counts */
#define HPM_SELF_TEST_TIMEOUT 5 /* in 5 seconds counts */
#define HPM_ROLLBACK_TIMEOUT 10 /* in 5 seconds counts */
#define HPM_INACCESSIBILITY_TIMEOUT 4 /* in 5 seconds counts */

/* Components ID */
#define HPM_BOOTLOADER_COMPONENT_ID 0
#define HPM_IPMC_COMPONENT_ID 1
#define HPM_PAYLOAD_COMPONENT_ID 2

#define HPM_MAX_COMPONENTS HPM_PAYLOAD_COMPONENT_ID+1

typedef union {
    struct {
        uint8_t upgrade_undesirable:1;
        uint8_t automatic_rollback_overridden:1;
        uint8_t ipmc_degraded_in_update:1;
        uint8_t deferred_activation:1;
        uint8_t services_affected:1;
        uint8_t manual_rollback:1;
        uint8_t automatic_rollback:1;
        uint8_t self_test:1;
    } flags;
    uint8_t byte;
} t_ipmc_capabilities;

typedef union __attribute__ ((__packed__)) {
    struct {
        uint8_t reserved:2,
            cold_reset_required:1,
            deferred_activation_supported:1,
            comparison_supported:1,
            preparation_support:1,
            rollback_backup_support:2;
    } flags;
    uint8_t byte;
} t_comp_properties;

#endif
