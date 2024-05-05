#include <Arduino_ConnectionHandler.h>

/* A complete list of supported boards with WiFi is available here:
 * https://github.com/arduino-libraries/ArduinoIoTCloud/#what
 */

  // #define SECRET_SSID "GalaxyA13"
  // #define SECRET_PASS "folp5309..."


/* ESP8266 ESP32*/
#if defined(BOARD_HAS_SECRET_KEY)
  #define SECRET_DEVICE_KEY ""
#endif

