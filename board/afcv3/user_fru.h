#include "fru.h"

/*********************************************
 * Common defines
 *********************************************/
#define LANG_CODE               0
#define FRU_FILE_ID             "AFCFRU"       //Allows knowing the source of the FRU present in the memory

#define BOARD_INFO_AREA_ENABLE
#define PRODUCT_INFO_AREA_ENABLE
#define MULTIRECORD_AREA_ENABLE

/*********************************************
 * Board information area
 *********************************************/
#define BOARD_MANUFACTURER      "Creotech"
#define BOARD_NAME              "AMC-FMC-Carrier"
#define BOARD_SN                "000000001"
#define BOARD_PN                "AFC"

/*********************************************
 * Product information area
 *********************************************/
#define PRODUCT_MANUFACTURER    "LNLS"
#define PRODUCT_NAME            "AFC BPM"
#define PRODUCT_PN              "00001"
#define PRODUCT_VERSION         "v3"
#define PRODUCT_SN              "00001"
#define PRODUCT_ASSET_TAG       "No tag"

/*********************************************
 * AMC: Point to point connectivity record
 *********************************************/
#define AMC_POINT_TO_POINT_RECORD_CNT           2
#define AMC_POINT_TO_POINT_RECORD_LIST                                  \
    GENERIC_POINT_TO_POINT_RECORD(0, PORT(4), PORT(5), PORT(6), PORT(7), PCIE, GEN1_NO_SSC, MATCHES_10); \
    GENERIC_POINT_TO_POINT_RECORD(1, PORT(8), PORT(9), PORT(10), PORT(11), PCIE, GEN1_NO_SSC, MATCHES_10);

/*********************************************
 * AMC: Point to point clock record
 *********************************************/
#define AMC_CLOCK_CONFIGURATION_DESCRIPTORS_CNT               2
/* Example:
   DIRECT_CLOCK_CONNECTION(CLOCK_ID, ACTIVATION, PLL_USE, SOURCE/RECEIVER, CLOCK_FAMILY, ACCURACY, FREQUENCY, MIN FREQUENCY, MAX FREQUENCY) */
#define AMC_CLOCK_CONFIGURATION_LIST                                    \
    DIRECT_CLOCK_CONNECTION(FCLKA, CIPMC, NO_PLL, RECEIVER, PCI_RESERVED, 0, MHz(100), MHz(99), MHz(101)), \
        DIRECT_CLOCK_CONNECTION(TCLKA, APP, NO_PLL, RECEIVER, UNSPEC_FAMILY, 0, MHz(11), MHz(10), MHz(12))
/**********************************************
 * PICMG: Module current record
 **********************************************/
#define MODULE_CURRENT_RECORD            current_in_ma(6500)
