#include "ipmi.h"
#include "led.h"
#include "sdr.h"
#include "fru.h"

extern void ipmi_storage_get_fru_inventory( ipmi_msg *req, ipmi_msg *rsp );
extern void ipmi_storage_read_fru_data( ipmi_msg *req, ipmi_msg *rsp );

extern void ipmi_app_get_device_id ( ipmi_msg *req, ipmi_msg *rsp );

extern void ipmi_picmg_set_led ( ipmi_msg *req, ipmi_msg *rsp );
extern void ipmi_picmg_get_properties ( ipmi_msg *req, ipmi_msg *rsp );
extern void ipmi_picmg_set_amc_port ( ipmi_msg *req, ipmi_msg *rsp );
extern void ipmi_picmg_fru_control ( ipmi_msg *req, ipmi_msg *rsp );

extern void ipmi_se_get_sdr_info( ipmi_msg *req, ipmi_msg* rsp);
extern void ipmi_se_reserve_device_sdr( ipmi_msg *req, ipmi_msg* rsp);
extern void ipmi_se_get_sdr( ipmi_msg *req, ipmi_msg* rsp);
extern void ipmi_se_get_sensor_reading( ipmi_msg *req, ipmi_msg* rsp);

t_req_handler_record handlers[MAX_HANDLERS]={
    {
        .netfn   = NETFN_GRPEXT,
        .cmd     = IPMI_PICMG_CMD_GET_PROPERTIES,
        .req_handler = ipmi_picmg_get_properties
    },
    {
        .netfn   = NETFN_GRPEXT,
        .cmd     = IPMI_PICMG_CMD_SET_FRU_LED_STATE,
        .req_handler = ipmi_picmg_set_led
    },
    {
        .netfn   = NETFN_GRPEXT,
        .cmd     = IPMI_PICMG_CMD_SET_AMC_PORT_STATE,
        .req_handler = ipmi_picmg_set_amc_port
    },
    {
	.netfn   = NETFN_GRPEXT,
	.cmd     = IPMI_PICMG_CMD_FRU_CONTROL,
        .req_handler = ipmi_picmg_fru_control
    },
    {
        .netfn   = NETFN_SE,
        .cmd     = IPMI_GET_DEVICE_SDR_INFO_CMD,
        .req_handler = ipmi_se_get_sdr_info
    },
    {
        .netfn   = NETFN_SE,
        .cmd     = IPMI_RESERVE_DEVICE_SDR_REPOSITORY_CMD,
        .req_handler = ipmi_se_reserve_device_sdr
    },
    {
        .netfn   = NETFN_SE,
        .cmd     = IPMI_GET_DEVICE_SDR_CMD,
        .req_handler = ipmi_se_get_sdr
    },
    {
        .netfn   = NETFN_SE,
        .cmd     = IPMI_GET_SENSOR_READING_CMD,
        .req_handler = ipmi_se_get_sensor_reading
    },
    {
        .netfn   = NETFN_APP,
        .cmd     = IPMI_GET_DEVICE_ID_CMD,
        .req_handler = ipmi_app_get_device_id
    },
    {
        .netfn   = NETFN_STORAGE,
        .cmd     = IPMI_GET_FRU_INVENTORY_AREA_INFO_CMD,
        .req_handler = ipmi_storage_get_fru_inventory
    },
    {
        .netfn   = NETFN_STORAGE,
        .cmd     = IPMI_READ_FRU_DATA_CMD,
        .req_handler = ipmi_storage_read_fru_data
    }
};
