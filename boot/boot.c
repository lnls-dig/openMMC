#include "chip.h"
#include "boot.h"
#include "iap.h"

int main (void)
{
    SystemCoreClockUpdate();

    bool upgrade = FALSE;
    uint32_t upgr_fw_id, user_fw_id;

    Chip_GPIO_Init(LPC_GPIO);
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 9);

    user_fw_id = *(uint32_t*)USER_FLASH_ID_ADDR;
    upgr_fw_id = *(uint32_t*)UPGRADE_FLASH_ID_ADDR;

    //if(user_fw_id == 0xFFFFFFFF || ((upgr_fw_id != 0xFFFFFFFF) && (user_fw_id < upgr_fw_id))) {
    if(upgr_fw_id != 0xFFFFFFFF) {
        upgrade = TRUE;
        Chip_GPIO_SetPinState(LPC_GPIO, 1, 9, 0 );
    }

    if (upgrade) {
        update_firmware();
    }

    execute_user_code();

    while (1);
}

void program_page( uint32_t address, uint32_t * data, uint32_t size )
{
    uint32_t sector_start = (address & 0xFF000) >> 12;
    uint32_t sector_end = ((address+size) & 0xFF000) >> 12;

    if (size % 256) {
        /* Data should be a 256 byte boundary */
        return;
    }

    if (Chip_IAP_PreSectorForReadWrite( sector_start, sector_end ) != IAP_CMD_SUCCESS) {
        return;
    }

    Chip_IAP_CopyRamToFlash( address, data, size );
}

void erase_sector( uint32_t sector_start, uint32_t sector_end )
{
    if (Chip_IAP_PreSectorForReadWrite( sector_start, sector_end ) != IAP_CMD_SUCCESS) {
        return;
    }

    Chip_IAP_EraseSector( sector_start, sector_end );
}

void execute_user_code( void )
{
    USER_ENTRY_PFN user_entry;

    user_entry = (USER_ENTRY_PFN)*((uint32_t*)(USER_FLASH_START_ADDR +4));
    if (user_entry) {
        (user_entry)();
    }
}

void update_firmware( void )
{
    uint32_t dst = USER_FLASH_START_ADDR;
    uint32_t *src = (uint32_t *)UPGRADE_FLASH_START_ADDR;

    uint32_t page[64];

    for(uint8_t j=0; j<(sizeof(page)/sizeof(uint32_t)); j++) {
        page[j] = 0xFFFFFFFF;
    }

    uint32_t fw_size = USER_FLASH_END_ADDR - USER_FLASH_START_ADDR;
    uint32_t offset;


    erase_sector( USER_FLASH_START_SECTOR , USER_FLASH_END_SECTOR );

    for ( offset = 0; offset < fw_size; dst += sizeof(page)) {
        /* Populate a page from source address */
        for (uint8_t i = 0; i<sizeof(page)/4; i++, src++, offset+=4 ) {
            page[i] = *src;
        }
        /* Program it into dst */
        program_page( dst, page, sizeof(page) );

        /* Reset the data in local storage */
        for(uint8_t j=0; j<(sizeof(page)/sizeof(uint32_t)); j++) {
            page[j] = 0xFFFFFFFF;
        }
    }

    //erase_sector( UPGRADE_FLASH_START_SECTOR , UPGRADE_FLASH_END_SECTOR );
}
