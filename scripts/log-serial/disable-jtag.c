#include <ftdi.h>
#include <stdio.h>

int main() {
  struct ftdi_context *ftdi;
  int ret;

  ftdi = ftdi_new();
  if ((ret = ftdi_init(ftdi)) < 0) {
    fprintf(stderr, "Failed to initialize FTDI context: %s\n",
            ftdi_get_error_string(ftdi));
    ftdi_free(ftdi);
    return 1;
  }

  /* FT4232H
   * Vendor ID: 0403h
   * Device ID: 6011h
   */
  if ((ret = ftdi_usb_open_desc(ftdi, 0x0403, 0x6011, NULL, NULL)) < 0) {
    fprintf(stderr, "Failed to open device: %s\n", ftdi_get_error_string(ftdi));
    return 1;
  }

  // Device connected on Interface A
  if ((ret = ftdi_set_interface(ftdi, INTERFACE_A)) < 0) {
    fprintf(stderr, "Failed to select channel: %s\n",
            ftdi_get_error_string(ftdi));
    return 1;
  }

  // ADBUS7 disables AMC access. Enable bitbang mode for it.
  if ((ret = ftdi_set_bitmode(ftdi, 0b10000000, BITMODE_BITBANG)) < 0) {
    fprintf(stderr, "Failed to enable bit-bang mode: %s\n",
            ftdi_get_error_string(ftdi));
    return 1;
  }

  uint8_t value = 0b10000000;
  if ((ret = ftdi_write_data(ftdi, &value, 1)) < 0) {
    fprintf(stderr, "Failed to write data: %s\n", ftdi_get_error_string(ftdi));
    return 1;
  }

  ftdi_usb_close(ftdi);
  ftdi_deinit(ftdi);

  return 0;
}
