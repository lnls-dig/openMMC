#include "port.h"
#include "jtag.h"

void init_scansta( void )
{
    gpio_set_pin_dir(SCANSTA_PORT, SCANSTA_ADDR0_PIN, OUTPUT);
    gpio_set_pin_dir(SCANSTA_PORT, SCANSTA_ADDR1_PIN, OUTPUT);
    gpio_set_pin_dir(SCANSTA_PORT, SCANSTA_ADDR2_PIN, OUTPUT);
    gpio_set_pin_dir(SCANSTA_PORT, SCANSTA_ADDR3_PIN, OUTPUT);
    gpio_set_pin_dir(SCANSTA_PORT, SCANSTA_ADDR4_PIN, OUTPUT);
    gpio_set_pin_dir(SCANSTA_PORT, SCANSTA_ADDR5_PIN, OUTPUT);
    gpio_set_pin_dir(SCANSTA_PORT, SCANSTA_ADDR6_PIN, OUTPUT);
    gpio_set_pin_dir(SCANSTA_PORT, SCANSTA_RST_PIN, OUTPUT);

    gpio_set_pin_state(SCANSTA_PORT, SCANSTA_ADDR0_PIN, LOW);
    gpio_set_pin_state(SCANSTA_PORT, SCANSTA_ADDR1_PIN, LOW);
    gpio_set_pin_state(SCANSTA_PORT, SCANSTA_ADDR2_PIN, LOW);
    gpio_set_pin_state(SCANSTA_PORT, SCANSTA_ADDR3_PIN, LOW);
    gpio_set_pin_state(SCANSTA_PORT, SCANSTA_ADDR4_PIN, LOW);
    gpio_set_pin_state(SCANSTA_PORT, SCANSTA_ADDR5_PIN, LOW);
    gpio_set_pin_state(SCANSTA_PORT, SCANSTA_ADDR6_PIN, LOW);
    gpio_set_pin_state(SCANSTA_PORT, SCANSTA_RST_PIN, HIGH);
}
