#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include "arduino_secrets.h"


#if defined(BOARD_HAS_SECRET_KEY)
  #define BOARD_ID ""
#endif

String pump_1;
String pump_2;
float volume;

char SSID[32];    // Network SSID (name)
char PASS[63];    // Network password (use for WPA, or use as key for WEP)


void initProperties() {
#if defined(BOARD_HAS_SECRET_KEY)
  ArduinoCloud.setBoardId(BOARD_ID);
  ArduinoCloud.setSecretDeviceKey(SECRET_DEVICE_KEY);
#endif


  ArduinoCloud.addProperty(volume, Permission::Read).publishOnChange(3);
  ArduinoCloud.addProperty(pump_1, Permission::Read).publishOnChange(3);
  ArduinoCloud.addProperty(pump_2, Permission::Read).publishOnChange(3);

}

void preferredConnectionHandler(String ssid, String pass) {

  strcpy(SSID, ssid.c_str());
  strcpy(PASS, pass.c_str());
}
  WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);
