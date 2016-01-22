#define USER_FLASH_START_ADDR (0x2000)
#define USER_FLASH_START_SECTOR (0x2)
#define USER_FLASH_END_ADDR   (0x10000)
#define USER_FLASH_END_SECTOR   (0xF)
/* Last 4 bytes are reserved for Firmware Version ID */
#define USER_FLASH_ID_ADDR    (0xFFFC)

#define UPGRADE_FLASH_START_ADDR (0x10000)
#define UPGRADE_FLASH_START_SECTOR (0x10)
#define UPGRADE_FLASH_END_ADDR (0x1E000)
#define UPGRADE_FLASH_END_SECTOR (0x11)
/* Last 4 bytes are reserved for Firmware Version ID */
#define UPGRADE_FLASH_ID_ADDR (0x1DFFC)

typedef void (*USER_ENTRY_PFN)();

void erase_sector( uint32_t sector_start, uint32_t sector_end );
void execute_user_code( void );
void update_firmware( void );
