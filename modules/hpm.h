/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2015-2016  Henrique Silva <henrique.silva@lnls.br>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 */

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

#define HPM_BLOCK_SIZE 20

/* Components ID */
enum {
    HPM_BOOTLOADER_COMPONENT_ID = 0,
    HPM_IPMC_COMPONENT_ID,
    HPM_PAYLOAD_COMPONENT_ID,
    HPM_MAX_COMPONENTS
};

typedef uint8_t (* t_hpm_upload_block)(uint8_t * block, uint16_t size);
typedef uint8_t (* t_hpm_finish_upload)(uint32_t image_size);
typedef uint8_t (* t_hpm_prepare_comp)(void);
typedef uint8_t (* t_hpm_get_upgrade_status)(void);
typedef uint8_t (* t_hpm_activate_firmware)(void);

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
    t_hpm_get_upgrade_status hpm_get_upgrade_status_f;
    t_hpm_activate_firmware hpm_activate_firmware_f;
} t_component;

#endif
