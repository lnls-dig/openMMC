#include "port.h"
#include "user_amc_fru.h"
#include "utils.h"
#include "uart_debug.h"

#ifndef AMC_COMPATIBILITY_MANUFACTURER_ID
#define AMC_COMPATIBILITY_MANUFACTURER_ID   0x315A
#endif

size_t amc_fru_info_build( uint8_t **buffer )
{
    uint8_t *hdr_ptr, *board_ptr, *product_ptr, *current_ptr, *clk_ptr, *p2p_ptr, *z3_ptr;
    uint8_t int_use_off = 0, chassis_off = 0, board_off = 0, product_off = 0, current_off = 0, p2p_off = 0, clk_off = 0, z3_compat_off = 0, multirec_off = 0;
    size_t int_use_sz = 0, chassis_sz = 0, board_sz = 0, product_sz = 0, current_sz = 0, p2p_sz = 0, clk_sz = 0, z3_compat_sz = 0;
    size_t offset = 0;

    printf(">AMC FRU Information:\n");

    /* Skip the common header */
    offset += 8;

    /* Board Information Area */
    board_off = offset;
    board_sz = board_info_area_build( &board_ptr, AMC_LANG_CODE, AMC_BOARD_MANUFACTURING_TIME, AMC_BOARD_MANUFACTURER, AMC_BOARD_NAME, AMC_BOARD_SN, AMC_BOARD_PN, AMC_FRU_FILE_ID );
    printf("\t-Board info area:\n");
    printf("\t\t-Language Code: %d\n", AMC_LANG_CODE);
    printf("\t\t-Manuf time: %d\n", AMC_BOARD_MANUFACTURING_TIME);
    printf("\t\t-Manufacturer: %s\n", AMC_BOARD_MANUFACTURER);
    printf("\t\t-Name: %s\n", AMC_BOARD_NAME);
    printf("\t\t-Serial Number: %s\n", AMC_BOARD_SN);
    printf("\t\t-Part Number: %s\n", AMC_BOARD_PN);
    printf("\t\t-File ID: %s\n", AMC_FRU_FILE_ID);
    offset += board_sz;

    /* Chassis Information Area */
    /* Not needed in AMC boards */
    printf("No Chassis info area\n");
    offset += chassis_sz;

    /* Internal Use Area */
    /* To be implemented by user */
    printf("No internal use area\n");
    offset += int_use_sz;

    /* Product Information Area */
    product_off = offset;
    product_sz = product_info_area_build( &product_ptr, AMC_LANG_CODE, AMC_PRODUCT_MANUFACTURER, AMC_PRODUCT_NAME, AMC_PRODUCT_PN, AMC_PRODUCT_VERSION, AMC_PRODUCT_SN, AMC_PRODUCT_ASSET_TAG, AMC_FRU_FILE_ID );
    printf("\t-Product info area:\n");
    printf("\t\t-Language Code: %d\n", AMC_LANG_CODE);
    printf("\t\t-Manufacturer: %s\n", AMC_PRODUCT_MANUFACTURER);
    printf("\t\t-Name: %s\n", AMC_PRODUCT_NAME);
    printf("\t\t-Part Number: %s\n", AMC_PRODUCT_PN);
    printf("\t\t-Version: %s\n", AMC_PRODUCT_VERSION);
    printf("\t\t-Asset Tag: %s\n", AMC_PRODUCT_ASSET_TAG);
    printf("\t\t-Serial Number: %s\n", AMC_PRODUCT_SN);
    printf("\t\t-File ID: %s\n", AMC_FRU_FILE_ID);
    offset += product_sz;

    /* Multirecord Area */
    multirec_off = offset;
    printf("\t-Multirecord Area: \n");

    /* Board Current requirement */
    current_off = offset;
    current_sz += module_current_record_build( &current_ptr, AMC_MODULE_CURRENT_RECORD );
    printf("\t\t-Module Current: %d A\n", AMC_MODULE_CURRENT_RECORD/10);
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
    printf("\t\t-Zone3 Compatibility code: 0x%X\n", AMC_COMPATIBILITY_CODE);
    z3_compat_sz += zone3_compatibility_record_build( &z3_ptr, AMC_COMPATIBILITY_MANUFACTURER_ID, AMC_COMPATIBILITY_CODE );
    offset += z3_compat_sz;

    /* Common Header */
    fru_header_build( &hdr_ptr, int_use_off, chassis_off, board_off, product_off, multirec_off );

    *buffer = pvPortMalloc(offset);
    printf(">AMC FRU total size: %d bytes\n", offset);

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
