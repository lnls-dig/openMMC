#include "clock_config.h"

uint8_t clock_config[16];

/*
 *  Function to configure the clock switch via ipmi.
 *  The configuration is sent as an array in the data field.
 *
*/
IPMI_HANDLER(ipmi_custom_cmd_write_clock_config, NETFN_CUSTOM, IPMI_CUSTOM_CMD_WRITE_CLOCK_CONFIG, ipmi_msg *req, ipmi_msg *rsp)
{
    memcpy(clock_config, req->data, req->data_len);
    payload_send_message(FRU_AMC, PAYLOAD_MESSAGE_CLOCK_CONFIG);
    rsp->completion_code = IPMI_CC_OK;
}


/*
 * Function to read the clock switch configuration via ipmi.
 */
IPMI_HANDLER(ipmi_custom_cmd_read_clock_config, NETFN_CUSTOM, IPMI_CUSTOM_CMD_READ_CLOCK_CONFIG, ipmi_msg *req, ipmi_msg *rsp)
{
    rsp->data_len = 16;
    memcpy(rsp->data, clock_config, rsp->data_len);
    rsp->completion_code = IPMI_CC_OK;
}
