#include "port.h"
#include "user_fmc_fru.h"
#include "utils.h"
#include "uart_debug.h"

size_t fmc_fru_info_build( uint8_t **buffer )
{
    uint8_t *hdr_ptr, *board_ptr, *product_ptr, *fmc_subtype_ptr, *vadj_ptr, *p3v3_ptr, *p12v_ptr;
    uint8_t board_off = 0, product_off = 0, fmc_subtype_off = 0, vadj_off = 0, p3v3_off = 0, p12v_off = 0, multirec_off = 0;
    size_t board_sz = 0, product_sz = 0, fmc_subtype_sz = 0, vadj_sz = 0, p3v3_sz = 0, p12v_sz = 0;
    size_t offset = 0;

    printf(">FMC FRU Information:\n");

    /* Skip the common header */
    offset += 8;

    /* Board Information Area */
    board_off = offset;
    board_sz = board_info_area_build( &board_ptr, FMC_LANG_CODE, FMC_BOARD_MANUFACTURING_TIME, FMC_BOARD_MANUFACTURER, FMC_BOARD_NAME, FMC_BOARD_SN, FMC_BOARD_PN, FMC_FRU_FILE_ID );
    printf("\t-Board info area:\n");
    printf("\t\t-Language Code: %d\n", FMC_LANG_CODE);
    printf("\t\t-Manuf time: %d\n", FMC_BOARD_MANUFACTURING_TIME);
    printf("\t\t-Manufacturer: %s\n", FMC_BOARD_MANUFACTURER);
    printf("\t\t-Name: %s\n", FMC_BOARD_NAME);
    printf("\t\t-Serial Number: %s\n", FMC_BOARD_SN);
    printf("\t\t-Part Number: %s\n", FMC_BOARD_PN);
    printf("\t\t-File ID: %s\n", FMC_FRU_FILE_ID);

    offset += board_sz;

    /* Chassis Information Area */
    /* Not needed in FMC boards */
    printf("No Chassis info area\n");

    /* Internal Use Area */
    /* To be implemented by user */
    printf("No internal use area\n");

    /* Product Information Area */
    product_off = offset;
    product_sz = product_info_area_build( &product_ptr, FMC_LANG_CODE, FMC_PRODUCT_MANUFACTURER, FMC_PRODUCT_NAME, FMC_PRODUCT_PN, FMC_PRODUCT_VERSION, FMC_PRODUCT_SN, FMC_PRODUCT_ASSET_TAG, FMC_FRU_FILE_ID );
    printf("\t-Product info area:\n");
    printf("\t\t-Language Code: %d\n", FMC_LANG_CODE);
    printf("\t\t-Manufacturer: %s\n", FMC_PRODUCT_MANUFACTURER);
    printf("\t\t-Name: %s\n", FMC_PRODUCT_NAME);
    printf("\t\t-Part Number: %s\n", FMC_PRODUCT_PN);
    printf("\t\t-Version: %s\n", FMC_PRODUCT_VERSION);
    printf("\t\t-Asset Tag: %s\n", FMC_PRODUCT_ASSET_TAG);
    printf("\t\t-Serial Number: %s\n", FMC_PRODUCT_SN);
    printf("\t\t-File ID: %s\n", FMC_FRU_FILE_ID);
    offset += product_sz;

    /* Multirecord Area */
    multirec_off = offset;
    printf("\t-Multirecord Area: \n");

    /* Board Current requirement */
    fmc_subtype_off = offset;
    fmc_subtype_sz = fmc_subtype_record_build( &fmc_subtype_ptr, FMC_CLOCK_DIR, FMC_MODULE_SIZE, FMC_P1_CONN_SIZE, FMC_P2_CONN_SIZE, FMC_P1_BANK_A_COUNT, FMC_P1_BANK_B_COUNT, FMC_P2_BANK_A_COUNT, FMC_P2_BANK_B_COUNT, FMC_P1_GBT, FMC_P2_GBT, 0);
    offset += fmc_subtype_sz;

    /* FMC DC Load */

    /* VAdj */
    vadj_off = offset;
    vadj_sz = dc_load_record_build( &vadj_ptr, FMC_VADJ_VOLT, FMC_VADJ_MIN, FMC_VADJ_MAX, FMC_VADJ_RIPPLE, FMC_VADJ_MIN_LOAD, FMC_VADJ_MAX_LOAD, 0);
    offset += vadj_sz;

    /* P3v3 */
    p3v3_off = offset;
    p3v3_sz = dc_load_record_build( &p3v3_ptr, FMC_P3V3_VOLT, FMC_P3V3_MIN, FMC_P3V3_MAX, FMC_P3V3_RIPPLE, FMC_P3V3_MIN_LOAD, FMC_P3V3_MAX_LOAD, 0);
    offset += p3v3_sz;

    /* P12v */ /* Last record */
    p12v_off = offset;
    p12v_sz = dc_load_record_build( &p12v_ptr, FMC_P12V_VOLT, FMC_P12V_MIN, FMC_P12V_MAX, FMC_P12V_RIPPLE, FMC_P12V_MIN_LOAD, FMC_P12V_MAX_LOAD, 1);
    offset += p12v_sz;


    /* FMC DC Outputs */
    /* Not implemented */

    /* Common Header */
    fru_header_build( &hdr_ptr, 0, 0, board_off, product_off, multirec_off );

    *buffer = pvPortMalloc(offset);
    printf(">FMC FRU total size: %d bytes\n", offset);

    memcpy( (*buffer)+0, hdr_ptr, 8);
    memcpy( (*buffer)+board_off, board_ptr, board_sz);
    memcpy( (*buffer)+product_off, product_ptr, product_sz);
    memcpy( (*buffer)+fmc_subtype_off, fmc_subtype_ptr, fmc_subtype_sz);
    memcpy( (*buffer)+vadj_off, vadj_ptr, vadj_sz);
    memcpy( (*buffer)+p3v3_off, p3v3_ptr, p3v3_sz);
    memcpy( (*buffer)+p12v_off, p12v_ptr, p12v_sz);

    vPortFree(hdr_ptr);
    vPortFree(board_ptr);
    vPortFree(product_ptr);
    vPortFree(fmc_subtype_ptr);
    vPortFree(vadj_ptr);
    vPortFree(p3v3_ptr);
    vPortFree(p12v_ptr);

    return offset;
}

void fru_fmc_parse_sn( uint8_t *img )
{
    fru_common_header_t *cmn_hdr = (fru_common_header_t *)&img[0];

    if ( !((calculate_chksum( (uint8_t *)cmn_hdr, sizeof(fru_common_header_t) ) == cmn_hdr->checksum )
           && (cmn_hdr->format_version == 1) )) {
        return;
    }

    uint8_t *board = &img[8*cmn_hdr->board_info_offset];
    uint8_t board_sz = board[1]*8;

    if (calculate_chksum( &board[0], board_sz-1) != board[board_sz-1]) {
        return;
    }

    uint8_t i = 6;

    uint8_t manuf_len = board[i] & 0x3F;
    i += manuf_len;
    uint8_t name_len = board[i] & 0x3F;
    i += name_len;
    uint8_t serial_len = board[i] & 0x3F;

    printf("\t Board SN: %s", &board[i+1]);
}
