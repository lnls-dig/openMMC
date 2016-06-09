#include "user_amc_fru.h"
#include "utils.h"
#include "uart_debug.h"

size_t amc_fru_info_build( uint8_t **buffer )
{
    uint8_t *hdr_ptr, *board_ptr, *product_ptr, *current_ptr, *clk_ptr, *p2p_ptr, *z3_ptr;
    uint8_t int_use_off = 0, chassis_off = 0, board_off = 0, product_off = 0, current_off = 0, p2p_off = 0, clk_off = 0, z3_compat_off = 0, multirec_off = 0;
    size_t int_use_sz = 0, chassis_sz = 0, board_sz = 0, product_sz = 0, current_sz = 0, p2p_sz = 0, clk_sz = 0, z3_compat_sz = 0;
    size_t offset = 0;

    DEBUG_MSG(">AMC FRU Information:\n");

    /* Skip the common header */
    offset += 8;

    /* Board Information Area */
    board_off = offset;
    board_sz = board_info_area_build( &board_ptr, AMC_LANG_CODE, AMC_BOARD_MANUFACTURING_TIME, AMC_BOARD_MANUFACTURER, AMC_BOARD_NAME, AMC_BOARD_SN, AMC_BOARD_PN, AMC_FRU_FILE_ID );
    DEBUG_MSG("\t-Board info area:\n");
    DEBUG_MSG("\t\t-Language Code: %d\n", AMC_LANG_CODE);
    DEBUG_MSG("\t\t-Manuf time: %d\n", AMC_BOARD_MANUFACTURING_TIME);
    DEBUG_MSG("\t\t-Manufacturer: %s\n", AMC_BOARD_MANUFACTURER);
    DEBUG_MSG("\t\t-Name: %s\n", AMC_BOARD_NAME);
    DEBUG_MSG("\t\t-Serial Number: %s\n", AMC_BOARD_SN);
    DEBUG_MSG("\t\t-Part Number: %s\n", AMC_BOARD_PN);
    DEBUG_MSG("\t\t-File ID: %s\n", AMC_FRU_FILE_ID);
    offset += board_sz;

    /* Chassis Information Area */
    /* Not needed in AMC boards */
    DEBUG_MSG("No Chassis info area\n");
    offset += chassis_sz;

    /* Internal Use Area */
    /* To be implemented by user */
    DEBUG_MSG("No internal use area\n");
    offset += int_use_sz;

    /* Product Information Area */
    product_off = offset;
    product_sz = product_info_area_build( &product_ptr, AMC_LANG_CODE, AMC_PRODUCT_MANUFACTURER, AMC_PRODUCT_NAME, AMC_PRODUCT_PN, AMC_PRODUCT_VERSION, AMC_PRODUCT_SN, AMC_PRODUCT_ASSET_TAG, AMC_FRU_FILE_ID );
    DEBUG_MSG("\t-Product info area:\n");
    DEBUG_MSG("\t\t-Language Code: %d\n", AMC_LANG_CODE);
    DEBUG_MSG("\t\t-Manufacturer: %s\n", AMC_PRODUCT_MANUFACTURER);
    DEBUG_MSG("\t\t-Name: %s\n", AMC_PRODUCT_NAME);
    DEBUG_MSG("\t\t-Part Number: %s\n", AMC_PRODUCT_PN);
    DEBUG_MSG("\t\t-Version: %s\n", AMC_PRODUCT_VERSION);
    DEBUG_MSG("\t\t-Asset Tag: %s\n", AMC_PRODUCT_ASSET_TAG);
    DEBUG_MSG("\t\t-Serial Number: %s\n", AMC_PRODUCT_SN);
    DEBUG_MSG("\t\t-File ID: %s\n", AMC_FRU_FILE_ID);
    offset += product_sz;

    /* Multirecord Area */
    multirec_off = offset;
    DEBUG_MSG("\t-Multirecord Area: \n");

    /* Board Current requirement */
    current_off = offset;
    current_sz += module_current_record_build( &current_ptr, AMC_MODULE_CURRENT_RECORD );
    DEBUG_MSG("\t\t-Module Current: %d A\n", AMC_MODULE_CURRENT_RECORD/10);
    offset += current_sz;

    /* Clock Point-to-Point Conectivity */
    clk_off = offset;
    clock_config_descriptor_t clk_desc[] = { AMC_CLOCK_CONFIGURATION_LIST };
    clk_sz += amc_point_to_point_clock_build( &clk_ptr, clk_desc, sizeof(clk_desc)/sizeof(clk_desc[0]));
    offset += clk_sz;

    /* AMC Point-to-Point Conectivity */
    p2p_off = offset;
    amc_p2p_descriptor_t p2p_desc[] = { AMC_POINT_TO_POINT_RECORD_LIST };
    p2p_sz += amc_point_to_point_record_build( &p2p_ptr, p2p_desc, sizeof(p2p_desc)/sizeof(p2p_desc[0]));
    offset += p2p_sz;

    /* Zone3 Connector Compatibility */
    z3_compat_off = offset;
    DEBUG_MSG("\t\t-Zone3 Compatibility code: 0x%X\n", AMC_COMPATIBILITY_CODE);
    z3_compat_sz += zone3_compatibility_record_build( &z3_ptr, AMC_COMPATIBILITY_CODE );
    offset += z3_compat_sz;

    /* Common Header */
    fru_header_build( &hdr_ptr, int_use_off, chassis_off, board_off, product_off, multirec_off );

    *buffer = pvPortMalloc(offset);
    DEBUG_MSG(">AMC FRU total size: %d bytes\n", offset);

    memcpy( (*buffer)+0, hdr_ptr, 8);
    memcpy( (*buffer)+board_off, board_ptr, board_sz);
    memcpy( (*buffer)+product_off, product_ptr, product_sz);
    memcpy( (*buffer)+current_off, current_ptr, current_sz);
    memcpy( (*buffer)+clk_off, clk_ptr, clk_sz);
    memcpy( (*buffer)+p2p_off, p2p_ptr, p2p_sz);
    memcpy( (*buffer)+z3_compat_off, z3_ptr, z3_compat_sz);

    vPortFree(hdr_ptr);
    vPortFree(board_ptr);
    vPortFree(product_ptr);
    vPortFree(current_ptr);
    vPortFree(clk_ptr);
    vPortFree(p2p_ptr);
    vPortFree(z3_ptr);

    return offset;
}
