#include "rtm_user_fru.h"

#ifndef RTM_COMPATIBILITY_MANUFACTURER_ID
#define RTM_COMPATIBILITY_MANUFACTURER_ID   0x315A
#endif

size_t rtm_fru_info_build( uint8_t **buffer )
{
    uint8_t *hdr_ptr, *board_ptr, *product_ptr, *z3_ptr;
    uint8_t int_use_off = 0, chassis_off = 0, board_off = 0, product_off = 0, z3_compat_off = 0, multirec_off = 0;
    size_t int_use_sz = 0, chassis_sz = 0, board_sz = 0, product_sz = 0, z3_compat_sz = 0;
    size_t offset = 0;

    /* Skip the common header */
    offset += 8;

    /* Board Information Area */
    board_off = offset;
    board_sz = board_info_area_build( &board_ptr, RTM_LANG_CODE, RTM_BOARD_MANUFACTURING_TIME, RTM_BOARD_MANUFACTURER, RTM_BOARD_NAME, RTM_BOARD_SN, RTM_BOARD_PN, RTM_FRU_FILE_ID );
    offset += board_sz;

    /* Product Information Area */
    product_off = offset;
    product_sz = product_info_area_build( &product_ptr, RTM_LANG_CODE, RTM_PRODUCT_MANUFACTURER, RTM_PRODUCT_NAME, RTM_PRODUCT_PN, RTM_PRODUCT_VERSION, RTM_PRODUCT_SN, RTM_PRODUCT_ASSET_TAG, RTM_FRU_FILE_ID );
    offset += product_sz;

    /* Multirecord Area */
    multirec_off = offset;

    /* Zone3 Connector Compatibility */
    z3_compat_off = offset;
    z3_compat_sz += zone3_compatibility_record_build( &z3_ptr, RTM_COMPATIBILITY_MANUFACTURER_ID, RTM_COMPATIBILITY_CODE );
    offset += z3_compat_sz;

    /* Common Header */
    fru_header_build( &hdr_ptr, int_use_off, chassis_off, board_off, product_off, multirec_off );

    *buffer = pvPortMalloc(offset);

    memcpy( (*buffer)+0, hdr_ptr, 8);
    memcpy( (*buffer)+board_off, board_ptr, board_sz);
    memcpy( (*buffer)+product_off, product_ptr, product_sz);
    memcpy( (*buffer)+z3_compat_off, z3_ptr, z3_compat_sz);

    vPortFree(hdr_ptr);
    vPortFree(board_ptr);
    vPortFree(product_ptr);
    vPortFree(z3_ptr);

    return offset;
}
