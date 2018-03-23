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

/**
 * @file ipmi.h
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 *
 * @brief IPMI module definitions
 * @ingroup IPMI
 */

/**
 * @defgroup IPMI IPMI - Intelligent Platform Management Interface
 */

#ifndef IPMI_H_
#define IPMI_H_

#include "ipmb.h"
#include "sdr.h"

/**
 * @brief Maximum data bytes in an IPMI message
 */
#define IPMI_MAX_DATA_LEN 24

/**
 * @brief Device ID - User-defined
 * @note 00h: Unspecified
 */
#define IPMI_APP_DEV_ID                 0x00

/**
 * @brief Device Revision
 *
 * [7]   (1) = device provides Device SDRs <br>
 *       (0) = device does not provide Device SDRs <br>
 * [6:4] reserved. Return as 0. <br>
 * [3:0] Device Revision, binary encoded.
 */
#define IPMI_APP_DEV_REV                0x80

/**
 * @brief Device Firmware Revision 1
 *
 *  [7] Device available: 0=normal operation, 1= device firmware, SDR  <br>
 *      Repository update or self-initialization in progress. <br>
 *  [6:0] Major Firmware Revision, binary encoded.
 */
#define IPMI_APP_DEV_FW_REV_UPPER       0x01

/**
 * @brief Device Firmware Revision 2
 *
 * Minor Firmware Revision. BCD encoded.
 */
#define IPMI_APP_DEV_FW_REV_LOWER       0x01

/**
 * @brief Supported IPMI Version ( BCD encoded )
 *
 * [7:4] Least Significant bits <br>
 * [3:0] Most Significant bits
 *
 * @note 00h = reserved.
 */
#define IPMI_APP_IPMI_VERSION           0x02

/**
 * @brief Additional Device Support
 *
 * [7] Chassis Device (device functions as chassis device per ICMB spec.) <br>
 * [6] Bridge (device responds to Bridge NetFn commands) <br>
 * [5] IPMB Event Generator (device generates event messages [platform event request messages] onto the IPMB) <br>
 * [4] IPMB Event Receiver (device accepts event messages [platform event request messages] from the IPMB) <br>
 * [3] FRU Inventory Device <br>
 * [2] SEL Device <br>
 * [1] SDR Repository Device <br>
 * [0] Sensor Device
 */
#define IPMI_APP_DEV_SUP                0x3B

/**
 * @brief Manufacturer ID LSB
 *
 * @note PICMG ID = 0x315A
 */
#define IPMI_APP_MANUF_LSB              0x5A

/**
 * @brief Manufacturer ID MIDB
 *
 * @note PICMG ID = 0x315A
 */
#define IPMI_APP_MANUF_MIDB             0x31

/**
 * @brief Manufacturer ID MSB
 *
 * @note PICMG ID = 0x315A
 */
#define IPMI_APP_MANUF_MSB              0x00

/**
 * @brief Product ID LSB (User-defined)
 */
#define IPMI_APP_PROD_ID_LSB            0x00

/**
 * @brief Product ID MSB (User-defined)
 */
#define IPMI_APP_PROD_ID_MSB            0x00

/**
 * @brief PICMG Extension Version
 *
 * Indicates the version of PICMG extensions implemented by this IPM Controller.
 *
 * [7:4] = BCD encoded minor version <br>
 * [3:0] = BCD encoded major version
 */
#define IPMI_EXTENSION_VERSION 0x23

/**
 * @brief Max FRU Device ID
 *
 * The numerically largest FRU Device ID for the managed FRUs implemented by this IPM Controller.
 *
 * @warning If the RTM module is implemented, the Max FRU Dev ID should be set to 1
 */
#ifdef MODULE_RTM
#define MAX_FRU_ID             0x01
#else
#define MAX_FRU_ID             0x00
#endif

/**
 * @brief FRU Device ID for IPM Controller
 *
 * Indicates a FRU Device ID for the FRU containing the IPM Controller.
 *
 * @note IPM Controller implementing the extensions defined by PICMG specifications v2.3 must report 0
 */
#define FRU_DEVICE_ID          0x00

/**
 * @defgroup IPMI_CMD IPMI Commands
 * @ingroup IPMI
 * @{
 */

/**
 * @defgroup IPMI_NETFN IPMI Net Functions
 * @{
 */
/* Known NetFn codes (even request codes only) */
#define NETFN_CHASSIS                                           0x00
#define NETFN_BRIDGE                                            0x02
#define NETFN_SE                                                0x04
#define NETFN_APP                                               0x06
#define NETFN_FIRMWARE                                          0x08
#define NETFN_STORAGE                                           0x0A
#define NETFN_TRANSPORT                                         0x0C
#define NETFN_GRPEXT                                            0x2C
/* Custom extension for UWHEP MMC functions */
#define NETFN_CUSTOM                                            0x32
/**
 * @}
 */

/**
 * @defgroup IPMI_CHASSIS_CMD IPMI Commands - Chassis (0x00)
 * @ingroup IPMI_CMD
 * @{
 */
#define IPMI_GET_CHASSIS_CAPABILITIES_CMD                       0x00
#define IPMI_GET_CHASSIS_STATUS_CMD                             0x01
#define IPMI_CHASSIS_CONTROL_CMD                                0x02
#define IPMI_CHASSIS_RESET_CMD                                  0x03
#define IPMI_CHASSIS_IDENTIFY_CMD                               0x04
#define IPMI_SET_CHASSIS_CAPABILITIES_CMD                       0x05
#define IPMI_SET_POWER_RESTORE_POLICY_CMD                       0x06
#define IPMI_GET_SYSTEM_RESTART_CAUSE_CMD                       0x07
#define IPMI_SET_SYSTEM_BOOT_OPTIONS_CMD                        0x08
#define IPMI_GET_SYSTEM_BOOT_OPTIONS_CMD                        0x09
#define IPMI_GET_POH_COUNTER_CMD                                0x0F
/**
 * @}
 */

/**
 * @defgroup IPMI_BRIGDE_CMD IPMI Commands - Brigde (0x02)
 * @ingroup IPMI_CMD
 * @{
 */
#define IPMI_GET_BRIDGE_STATE_CMD                               0x00
#define IPMI_SET_BRIDGE_STATE_CMD                               0x01
#define IPMI_GET_ICMB_ADDRESS_CMD                               0x02
#define IPMI_SET_ICMB_ADDRESS_CMD                               0x03
#define IPMI_SET_BRIDGE_PROXY_ADDRESS_CMD                       0x04
#define IPMI_GET_BRIDGE_STATISTICS_CMD                          0x05
#define IPMI_GET_ICMB_CAPABILITIES_CMD                          0x06
#define IPMI_CLEAR_BRIDGE_STATISTICS_CMD                        0x08
#define IPMI_GET_BRIDGE_PROXY_ADDRESS_CMD                       0x09
#define IPMI_GET_ICMB_CONNECTOR_INFO_CMD                        0x0A
#define IPMI_SET_ICMB_CONNECTOR_INFO_CMD                        0x0B
#define IPMI_SEND_ICMB_CONNECTION_ID_CMD                        0x0C
#define IPMI_PREPARE_FOR_DISCOVERY_CMD                          0x10
#define IPMI_GET_ADDRESSES_CMD                                  0x11
#define IPMI_SET_DISCOVERED_CMD                                 0x12
#define IPMI_GET_CHASSIS_DEVICE_ID_CMD                          0x13
#define IPMI_SET_CHASSIS_DEVICE_ID_CMD                          0x14
#define IPMI_BRIDGE_REQUEST_CMD                                 0x20
#define IPMI_BRIDGE_MESSAGE_CMD                                 0x21
#define IPMI_GET_EVENT_COUNT_CMD                                0x30
#define IPMI_SET_EVENT_DESTINATION_CMD                          0x31
#define IPMI_SET_EVENT_RECEPTION_STATE_CMD                      0x32
#define IPMI_SEND_ICMB_EVENT_MESSAGE_CMD                        0x33
#define IPMI_GET_EVENT_DESTIATION_CMD                           0x34
#define IPMI_GET_EVENT_RECEPTION_STATE_CMD                      0x35
#define IPMI_ERROR_REPORT_CMD                                   0xFF
/**
 * @}
 */

/**
 * @defgroup IPMI_SENSOR_CMD IPMI Commands - Sensor (0x04)
 * @ingroup IPMI_CMD
 * @{
 */
#define IPMI_SET_EVENT_RECEIVER_CMD                             0x00
#define IPMI_GET_EVENT_RECEIVER_CMD                             0x01
#define IPMI_PLATFORM_EVENT_CMD                                 0x02
#define IPMI_GET_PEF_CAPABILITIES_CMD                           0x10
#define IPMI_ARM_PEF_POSTPONE_TIMER_CMD                         0x11
#define IPMI_SET_PEF_CONFIG_PARMS_CMD                           0x12
#define IPMI_GET_PEF_CONFIG_PARMS_CMD                           0x13
#define IPMI_SET_LAST_PROCESSED_EVENT_ID_CMD                    0x14
#define IPMI_GET_LAST_PROCESSED_EVENT_ID_CMD                    0x15
#define IPMI_ALERT_IMMEDIATE_CMD                                0x16
#define IPMI_PET_ACKNOWLEDGE_CMD                                0x17
#define IPMI_GET_DEVICE_SDR_INFO_CMD                            0x20
#define IPMI_GET_DEVICE_SDR_CMD                                 0x21
#define IPMI_RESERVE_DEVICE_SDR_REPOSITORY_CMD                  0x22
#define IPMI_GET_SENSOR_READING_FACTORS_CMD                     0x23
#define IPMI_SET_SENSOR_HYSTERESIS_CMD                          0x24
#define IPMI_GET_SENSOR_HYSTERESIS_CMD                          0x25
#define IPMI_SET_SENSOR_THRESHOLD_CMD                           0x26
#define IPMI_GET_SENSOR_THRESHOLD_CMD                           0x27
#define IPMI_SET_SENSOR_EVENT_ENABLE_CMD                        0x28
#define IPMI_GET_SENSOR_EVENT_ENABLE_CMD                        0x29
#define IPMI_REARM_SENSOR_EVENTS_CMD                            0x2A
#define IPMI_GET_SENSOR_EVENT_STATUS_CMD                        0x2B
#define IPMI_GET_SENSOR_READING_CMD                             0x2D
#define IPMI_SET_SENSOR_TYPE_CMD                                0x2E
#define IPMI_GET_SENSOR_TYPE_CMD                                0x2F
/**
 * @}
 */

/**
 * @defgroup IPMI_APP_CMD IPMI Commands - Application (0x06)
 * @ingroup IPMI_CMD
 * @{
 */
#define IPMI_GET_DEVICE_ID_CMD                                  0x01
#define IPMI_BROADCAST_GET_DEVICE_ID_CMD                        0x01
#define IPMI_COLD_RESET_CMD                                     0x02
#define IPMI_WARM_RESET_CMD                                     0x03
#define IPMI_GET_SELF_TEST_RESULTS_CMD                          0x04
#define IPMI_MANUFACTURING_TEST_ON_CMD                          0x05
#define IPMI_SET_ACPI_POWER_STATE_CMD                           0x06
#define IPMI_GET_ACPI_POWER_STATE_CMD                           0x07
#define IPMI_GET_DEVICE_GUID_CMD                                0x08
#define IPMI_RESET_WATCHDOG_TIMER_CMD                           0x22
#define IPMI_SET_WATCHDOG_TIMER_CMD                             0x24
#define IPMI_GET_WATCHDOG_TIMER_CMD                             0x25
#define IPMI_SET_BMC_GLOBAL_ENABLES_CMD                         0x2E
#define IPMI_GET_BMC_GLOBAL_ENABLES_CMD                         0x2F
#define IPMI_CLEAR_MSG_FLAGS_CMD                                0x30
#define IPMI_GET_MSG_FLAGS_CMD                                  0x31
#define IPMI_ENABLE_MESSAGE_CHANNEL_RCV_CMD                     0x32
#define IPMI_GET_MSG_CMD                                        0x33
#define IPMI_SEND_MSG_CMD                                       0x34
#define IPMI_READ_EVENT_MSG_BUFFER_CMD                          0x35
#define IPMI_GET_BT_INTERFACE_CAPABILITIES_CMD                  0x36
#define IPMI_GET_SYSTEM_GUID_CMD                                0x37
#define IPMI_GET_CHANNEL_AUTH_CAPABILITIES_CMD                  0x38
#define IPMI_GET_SESSION_CHALLENGE_CMD                          0x39
#define IPMI_ACTIVATE_SESSION_CMD                               0x3A
#define IPMI_SET_SESSION_PRIVILEGE_CMD                          0x3B
#define IPMI_CLOSE_SESSION_CMD                                  0x3C
#define IPMI_GET_SESSION_INFO_CMD                               0x3D
#define IPMI_GET_AUTHCODE_CMD                                   0x3F
#define IPMI_SET_CHANNEL_ACCESS_CMD                             0x40
#define IPMI_GET_CHANNEL_ACCESS_CMD                             0x41
#define IPMI_GET_CHANNEL_INFO_CMD                               0x42
#define IPMI_SET_USER_ACCESS_CMD                                0x43
#define IPMI_GET_USER_ACCESS_CMD                                0x44
#define IPMI_SET_USER_NAME_CMD                                  0x45
#define IPMI_GET_USER_NAME_CMD                                  0x46
#define IPMI_SET_USER_PASSWORD_CMD                              0x47
#define IPMI_ACTIVATE_PAYLOAD_CMD                               0x48
#define IPMI_DEACTIVATE_PAYLOAD_CMD                             0x49
#define IPMI_GET_PAYLOAD_ACTIVATION_STATUS_CMD                  0x4A
#define IPMI_GET_PAYLOAD_INSTANCE_INFO_CMD                      0x4B
#define IPMI_SET_USER_PAYLOAD_ACCESS_CMD                        0x4C
#define IPMI_GET_USER_PAYLOAD_ACCESS_CMD                        0x4D
#define IPMI_GET_CHANNEL_PAYLOAD_SUPPORT_CMD                    0x4E
#define IPMI_GET_CHANNEL_PAYLOAD_VERSION_CMD                    0x4F
#define IPMI_GET_CHANNEL_OEM_PAYLOAD_INFO_CMD                   0x50
#define IPMI_MASTER_READ_WRITE_CMD                              0x52
#define IPMI_GET_CHANNEL_CIPHER_SUITES_CMD                      0x54
#define IPMI_SUSPEND_RESUME_PAYLOAD_ENCRYPTION_CMD              0x55
#define IPMI_SET_CHANNEL_SECURITY_KEY_CMD                       0x56
#define IPMI_GET_SYSTEM_INTERFACE_CAPABILITIES_CMD              0x57
/**
 * @}
 */

/**
 * @defgroup IPMI_STORAGE_CMD IPMI Commands - Storage (0x0A)
 * @ingroup IPMI_CMD
 * @{
 */
#define IPMI_GET_FRU_INVENTORY_AREA_INFO_CMD                    0x10
#define IPMI_READ_FRU_DATA_CMD                                  0x11
#define IPMI_WRITE_FRU_DATA_CMD                                 0x12
#define IPMI_GET_SDR_REPOSITORY_INFO_CMD                        0x20
#define IPMI_GET_SDR_REPOSITORY_ALLOC_INFO_CMD                  0x21
#define IPMI_RESERVE_SDR_REPOSITORY_CMD                         0x22
#define IPMI_GET_SDR_CMD                                        0x23
#define IPMI_ADD_SDR_CMD                                        0x24
#define IPMI_PARTIAL_ADD_SDR_CMD                                0x25
#define IPMI_DELETE_SDR_CMD                                     0x26
#define IPMI_CLEAR_SDR_REPOSITORY_CMD                           0x27
#define IPMI_GET_SDR_REPOSITORY_TIME_CMD                        0x28
#define IPMI_SET_SDR_REPOSITORY_TIME_CMD                        0x29
#define IPMI_ENTER_SDR_REPOSITORY_UPDATE_CMD                    0x2A
#define IPMI_EXIT_SDR_REPOSITORY_UPDATE_CMD                     0x2B
#define IPMI_RUN_INITIALIZATION_AGENT_CMD                       0x2C
#define IPMI_GET_SEL_INFO_CMD                                   0x40
#define IPMI_GET_SEL_ALLOCATION_INFO_CMD                        0x41
#define IPMI_RESERVE_SEL_CMD                                    0x42
#define IPMI_GET_SEL_ENTRY_CMD                                  0x43
#define IPMI_ADD_SEL_ENTRY_CMD                                  0x44
#define IPMI_PARTIAL_ADD_SEL_ENTRY_CMD                          0x45
#define IPMI_DELETE_SEL_ENTRY_CMD                               0x46
#define IPMI_CLEAR_SEL_CMD                                      0x47
#define IPMI_GET_SEL_TIME_CMD                                   0x48
#define IPMI_SET_SEL_TIME_CMD                                   0x49
#define IPMI_GET_AUXILIARY_LOG_STATUS_CMD                       0x5A
#define IPMI_SET_AUXILIARY_LOG_STATUS_CMD                       0x5B
/**
 * @}
 */

/**
 * @defgroup IPMI_TRANSPORT_CMD IPMI Commands - Storage (0x0C)
 * @ingroup IPMI_CMD
 * @{
 */
#define IPMI_SET_LAN_CONFIG_PARMS_CMD                           0x01
#define IPMI_GET_LAN_CONFIG_PARMS_CMD                           0x02
#define IPMI_SUSPEND_BMC_ARPS_CMD                               0x03
#define IPMI_GET_IP_UDP_RMCP_STATS_CMD                          0x04
#define IPMI_SET_SERIAL_MODEM_CONFIG_CMD                        0x10
#define IPMI_GET_SERIAL_MODEM_CONFIG_CMD                        0x11
#define IPMI_SET_SERIAL_MODEM_MUX_CMD                           0x12
#define IPMI_GET_TAP_RESPONSE_CODES_CMD                         0x13
#define IPMI_SET_PPP_UDP_PROXY_XMIT_DATA_CMD                    0x14
#define IPMI_GET_PPP_UDP_PROXY_XMIT_DATA_CMD                    0x15
#define IPMI_SEND_PPP_UDP_PROXY_PACKET_CMD                      0x16
#define IPMI_GET_PPP_UDP_PROXY_RECV_DATA_CMD                    0x17
#define IPMI_SERIAL_MODEM_CONN_ACTIVE_CMD                       0x18
#define IPMI_CALLBACK_CMD                                       0x19
#define IPMI_SET_USER_CALLBACK_OPTIONS_CMD                      0x1A
#define IPMI_GET_USER_CALLBACK_OPTIONS_CMD                      0x1B
#define IPMI_SOL_ACTIVATING_CMD                                 0x20
#define IPMI_SET_SOL_CONFIGURATION_PARAMETERS                   0x21
#define IPMI_GET_SOL_CONFIGURATION_PARAMETERS                   0x22
/**
 * @}
 */

/**
 * @defgroup IPMI_PICMG_CMD IPMI Commands - PICMG (0x2C)
 * @ingroup IPMI_CMD
 * @{
 */
/* The Group Extension defined for PICMG. */
#define IPMI_PICMG_GRP_EXT                                      0x00

#define IPMI_PICMG_CMD_GET_PROPERTIES                           0x00
#define IPMI_PICMG_CMD_GET_ADDRESS_INFO                         0x01
#define IPMI_PICMG_CMD_GET_SHELF_ADDRESS_INFO                   0x02
#define IPMI_PICMG_CMD_SET_SHELF_ADDRESS_INFO                   0x03
#define IPMI_PICMG_CMD_FRU_CONTROL                              0x04
#define IPMI_PICMG_CMD_GET_FRU_LED_PROPERTIES                   0x05
#define IPMI_PICMG_CMD_GET_LED_COLOR_CAPABILITIES               0x06
#define IPMI_PICMG_CMD_SET_FRU_LED_STATE                        0x07
#define IPMI_PICMG_CMD_GET_FRU_LED_STATE                        0x08
#define IPMI_PICMG_CMD_SET_IPMB_STATE                           0x09
#define IPMI_PICMG_CMD_SET_FRU_ACTIVATION_POLICY                0x0A
#define IPMI_PICMG_CMD_GET_FRU_ACTIVATION_POLICY                0x0B
#define IPMI_PICMG_CMD_SET_FRU_ACTIVATION                       0x0C
#define IPMI_PICMG_CMD_GET_DEVICE_LOCATOR_RECORD                0x0D
#define IPMI_PICMG_CMD_SET_PORT_STATE                           0x0E
#define IPMI_PICMG_CMD_GET_PORT_STATE                           0x0F
#define IPMI_PICMG_CMD_COMPUTE_POWER_PROPERTIES                 0x10
#define IPMI_PICMG_CMD_SET_POWER_LEVEL                          0x11
#define IPMI_PICMG_CMD_GET_POWER_LEVEL                          0x12
#define IPMI_PICMG_CMD_RENEGOTIATE_POWER                        0x13
#define IPMI_PICMG_CMD_GET_FAN_SPEED_PROPERTIES                 0x14
#define IPMI_PICMG_CMD_SET_FAN_LEVEL                            0x15
#define IPMI_PICMG_CMD_GET_FAN_LEVEL                            0x16
#define IPMI_PICMG_CMD_BUSED_RESOURCE                           0x17
#define IPMI_PICMG_CMD_IPMB_LINK_INFO                           0x18
#define IPMI_PICMG_CMD_SET_AMC_PORT_STATE                       0x19
#define IPMI_PICMG_CMD_GET_AMC_PORT_STATE                       0x1A
#define IPMI_PICMG_CMD_SHELF_MANAGER_IPMB_ADDRESS               0x1B
#define IPMI_PICMG_CMD_SET_FAN_POLICY                           0x1C
#define IPMI_PICMG_CMD_GET_FAN_POLICY                           0x1D
#define IPMI_PICMG_CMD_FRU_CONTROL_CAPABILITIES                 0x1E
#define IPMI_PICMG_CMD_FRU_INVENTORY_DEVICE_LOCK_CONTROL        0x1F
#define IPMI_PICMG_CMD_FRU_INVENTORY_DEVICE_WRITE               0x20
#define IPMI_PICMG_CMD_GET_SHELF_MANAGER_IP_ADDRESSES           0x21
#define IPMI_PICMG_CMD_SHELF_POWER_ALLOCATION                   0x22
#define IPMI_PICMG_CMD_GET_TELCO_ALARM_CAPABILITY               0x29
/* HPM Commands */
#define IPMI_PICMG_CMD_HPM_GET_UPGRADE_CAPABILITIES             0x2E
#define IPMI_PICMG_CMD_HPM_GET_COMPONENT_PROPERTIES             0x2F
#define IPMI_PICMG_CMD_HPM_ABORT_FIRMWARE_UPGRADE               0x30
#define IPMI_PICMG_CMD_HPM_INITIATE_UPGRADE_ACTION              0x31
#define IPMI_PICMG_CMD_HPM_UPLOAD_FIRMWARE_BLOCK                0x32
#define IPMI_PICMG_CMD_HPM_FINISH_FIRMWARE_UPLOAD               0x33
#define IPMI_PICMG_CMD_HPM_GET_UPGRADE_STATUS                   0x34
#define IPMI_PICMG_CMD_HPM_ACTIVATE_FIRMWARE                    0x35
#define IPMI_PICMG_CMD_HPM_QUERY_SELF_RESULTS                   0x36
#define IPMI_PICMG_CMD_HPM_QUERY_ROLLBACK_STATUS                0x37
#define IPMI_PICMG_CMD_HPM_INITIATE_MANUAL_ROLLBACK             0x38
/**
 * @}
 */

#define IPMI_EVENT_MESSAGE_REV                                  0x04

/**
 * @defgroup IPMI_CC IPMI Completion Codes
 * @{
 */
/* Completion Codes */
#define IPMI_CC_OK                                              0x00
#define IPMI_CC_NODE_BUSY                                       0xC0
#define IPMI_CC_INV_CMD                                         0xC1
#define IPMI_CC_INV_CMD_FOR_LUN                                 0xC2
#define IPMI_CC_TIMEOUT                                         0xC3
#define IPMI_CC_OUT_OF_SPACE                                    0xC4
#define IPMI_CC_RES_CANCELED                                    0xC5
#define IPMI_CC_REQ_DATA_TRUNC                                  0xC6
#define IPMI_CC_REQ_DATA_INV_LENGTH                             0xC7
#define IPMI_CC_REQ_DATA_FIELD_EXCEED                           0xC8
#define IPMI_CC_PARAM_OUT_OF_RANGE                              0xC9
#define IPMI_CC_CANT_RET_NUM_REQ_BYTES                          0xCA
#define IPMI_CC_REQ_DATA_NOT_PRESENT                            0xCB
#define IPMI_CC_INV_DATA_FIELD_IN_REQ                           0xCC
#define IPMI_CC_ILL_SENSOR_OR_RECORD                            0xCD
#define IPMI_CC_RESP_COULD_NOT_BE_PRV                           0xCE
#define IPMI_CC_CANT_RESP_DUPLI_REQ                             0xCF
#define IPMI_CC_CANT_RESP_SDRR_UPDATE                           0xD0
#define IPMI_CC_CANT_RESP_FIRM_UPDATE                           0xD1
#define IPMI_CC_CANT_RESP_BMC_INIT                              0xD2
#define IPMI_CC_DESTINATION_UNAVAILABLE                         0xD3
#define IPMI_CC_INSUFFICIENT_PRIVILEGES                         0xD4
#define IPMI_CC_NOT_SUPPORTED_PRESENT_STATE                     0xD5
#define IPMI_CC_ILLEGAL_COMMAND_DISABLED                        0xD6
#define IPMI_CC_COMMAND_IN_PROGRESS                             0x80
#define IPMI_CC_UNSPECIFIED_ERROR                               0xFF
/**
 * @}
 */

/**
 * @}
 */

/* FRU Control codes */
#define FRU_CTLCODE_COLD_RST                                    0x00
#define FRU_CTLCODE_WARM_RST                                    0x01
#define FRU_CTLCODE_REBOOT                                      0x02
#define FRU_CTLCODE_DIAGNOSTIC_INTERRUPT                        0x03
#define FRU_CTLCODE_QUIESCE                                     0x04

/**
 * @brief IPMI Handler function type definition
 *
 * All IPMI Handler functions must follow this signature, receiving pointers to both request and response structs and returning void.
 */
typedef void (* t_req_handler)(ipmi_msg * req, ipmi_msg * resp);

/**
 * @brief IPMI Handler record structure used to index all handlers
 */
typedef struct{
    uint8_t netfn;                 /**< Net Function */
    uint8_t cmd;                   /**< Command */
    t_req_handler req_handler;     /**< IPMI handler function */
} t_req_handler_record;

/**
 * @brief Pointer to IPMI Handler record list start byte stored in ROM
 */
extern const t_req_handler_record *_ipmi_handlers;

/**
 * @brief Pointer to IPMI Handler record list final byte stored in ROM
 */
extern const t_req_handler_record *_eipmi_handlers;

/**
 * @brief Macro to implement unique IPMI handler functions
 *
 * This macro declares a IPMI handler function with a specific name (based on Netfn and CMD).
 * The handler is placed in a spefic memory region with all handlers, so we have all of them stored sequentially.
 *
 * @warning In order to use IPMI_HANDLER macro the user is required to have .ipmi_handlers section defined in the linker script, like the following example:
 * @code
 * .ipmi_handlers : ALIGN(4)
 * {
 *       _ipmi_handlers = .;
 *       KEEP(*(.ipmi_handlers))
 *       _eipmi_handlers = .;
 * } >FLASHAREA
 * @endcode
 */
#define IPMI_HANDLER(name, netfn_id, cmd_id, args...)                   \
    void ipmi_handler_##netfn_id##__##cmd_id##_f(args);                 \
    const t_req_handler_record __attribute__ ((section (".ipmi_handlers"))) ipmi_handler_##netfn_id##__##cmd_id##_s = { .req_handler = ipmi_handler_##netfn_id##__##cmd_id##_f , .netfn = netfn_id, .cmd = cmd_id }; \
    void ipmi_handler_##netfn_id##__##cmd_id##_f(args)

/* Function Prototypes */

/**
 * @brief IPMI dispatcher task
 *
 * This task handles all the incoming IPMI messages previously decoded by IPMB tasks.
 * Here the netfunction and commands are analyzed and the respective handler function is called.
 *
 * @param pvParameters Pointer to parameters buffer passed to this task in initialization
 */
void IPMITask ( void *pvParameters );

/**
 * @brief Initializes the IPMI Dispatcher
 *
 * This function initializes the IPMB Layer, registers the RX queue for incoming requests and creates the IPMI task
 */
void ipmi_init ( void );

/**
 * @brief Finds a handler associated with a given netfunction and command.
 *
 * @param netfn 8-bit network function code
 * @param cmd 8-bit command code
 *
 * @return Pointer to the function which will handle this command, as defined in the netfn handler list.
 */
t_req_handler ipmi_retrieve_handler(uint8_t netfn, uint8_t cmd);

/**
 * @brief Sends an event message (Platform Event) via IPMI
 *
 * @param sensor          Pointer to sensor information struct
 * @param assert_deassert Evetn transition direction (0) for assertion, (1) for Deassertion
 * @param evData          Pointer to event message buffer
 * @param length          Event message buffer len (max len = 3)
 *
 * @return ipmb_error
 *
 * @see sdr.h
 * @see ipmb.h
 */
ipmb_error ipmi_event_send( sensor_t * sensor, uint8_t assert_deassert, uint8_t *evData, uint8_t length);

#endif
