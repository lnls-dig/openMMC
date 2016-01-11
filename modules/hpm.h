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
#define HPM_INACCESSIBILITY_TIMEOUT 10 /* in 5 seconds counts */

/* Components ID */
enum {
    HPM_BOOTLOADER_COMPONENT_ID = 0,
    HPM_IPMC_COMPONENT_ID,
    HPM_PAYLOAD_COMPONENT_ID,
    HPM_MAX_COMPONENTS
};

typedef uint32_t (* t_hpm_upload_block)(uint8_t * block, uint8_t size);
typedef uint8_t (* t_hpm_finish_upload)(uint32_t image_size);
typedef uint8_t (* t_hpm_prepare_comp)(void);

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

typedef struct {
    t_comp_properties properties;
    char description[12];
    t_hpm_prepare_comp hpm_prepare_comp_f;
    t_hpm_upload_block hpm_upload_block_f;
    t_hpm_finish_upload hpm_finish_upload_f;
} t_component;

#define HPM_BLOCK_SIZE 20

#endif
