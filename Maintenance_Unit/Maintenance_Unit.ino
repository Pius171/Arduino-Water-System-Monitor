/*
  This sketch demonstrates how to exchange data between your board and the Arduino IoT Cloud.

  * Connect a potentiometer (or other analog sensor) to A0.
  * When the potentiometer (or sensor) value changes the data is sent to the Cloud.
  * When you flip the switch in the Cloud dashboard the onboard LED lights gets turned ON or OFF.

  IMPORTANT:
  This sketch works with WiFi, GSM, NB, Ethernet and Lora enabled boards supported by Arduino IoT Cloud.
  On a LoRa board, if it is configured as a class A device (default and preferred option),
  values from Cloud dashboard are received only after a value is sent to Cloud.

  The full list of compatible boards can be found here:
   - https://github.com/arduino-libraries/ArduinoIoTCloud#what
*/

#include "thingProperties.h"
#include "logger.h"
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

#define RESERVIOR_WIDTH  2000 // in cm
#define RESERVIOR_LENGTH 2000
// depth sensor
#define PIN_TRIG 16
#define PIN_ECHO 17

#define PUMP_1_IS_READY_PIN 32
#define PUMP_1_IS_RUNNING_PIN  33
#define PUMP_2_IS_READY_PIN  27
#define PUMP_2_IS_RUNNING_PIN  26

bool flag=true;
bool loggingFlag=true;

String previous_pump_1="";
String previous_pump_2="";
float previous_volume=0;
#if !defined(LED_BUILTIN) && !defined(ARDUINO_NANO_ESP32)
static int const LED_BUILTIN = 100;
#endif


void setup() {
  /* Initialize serial and wait up to 5 seconds for port to open */
  Serial.begin(115200);
    pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PUMP_1_IS_READY_PIN, INPUT_PULLUP);
  pinMode(PUMP_1_IS_RUNNING_PIN, INPUT_PULLUP);
  pinMode(PUMP_2_IS_READY_PIN, INPUT_PULLUP);
  pinMode(PUMP_2_IS_RUNNING_PIN, INPUT_PULLUP);
  for(unsigned long const serialBeginTime = millis(); !Serial && (millis() - serialBeginTime > 5000); ) { }
  WiFiManager wm;

      bool res;
    // res = wm.autoConnect(); // auto generated AP name from chipid
    // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
    res = wm.autoConnect("AutoConnectAP","password"); // password protected ap

    if(!res) {
        Serial.println("Failed to connect");
        // ESP.restart();
    } 
    else {
        //if you get here you have connected to the WiFi    
        Serial.println("connected...yeey :)");
    }
    
Serial.println("wifi details");
Serial.println(wm.getWiFiSSID());
Serial.println(wm.getWiFiPass());

delay(500);
//SD card setup
    if(!SD.begin()){
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    listDir("/", 0);

        if(!SD.exists("/log.csv")){
          
    Serial.println("log file doesn't exist, creating new one");
    Serial.println("adding headers");
    const char* headers = "Date,Time,pump1,pump2,volume\n";
    appendFile("/log.csv", headers);

    }
  /* Configure LED pin as an output */
  // pinMode(LED_BUILTIN, OUTPUT);
  // pinMode(PIN_TRIG, OUTPUT);
  // pinMode(PIN_ECHO, INPUT);
  // pinMode(PUMP_1_IS_READY_PIN, INPUT_PULLUP);
  // pinMode(PUMP_1_IS_RUNNING_PIN, INPUT_PULLUP);
  // pinMode(PUMP_2_IS_READY_PIN, INPUT_PULLUP);
  // pinMode(PUMP_2_IS_RUNNING_PIN, INPUT_PULLUP);

  preferredConnectionHandler(wm.getWiFiSSID(), wm.getWiFiPass());

  /* This function takes care of connecting your sketch variables to the ArduinoIoTCloud object */
  initProperties();

delay(500);
  /* Initialize Arduino IoT Cloud library */
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  setDebugMessageLevel(DBG_INFO);
  ArduinoCloud.printDebugInfo();
  logger_init();
}

void loop() {
  ArduinoCloud.update();
  delay(100);
 if(flag){ // haapens just once
  pump_1="booting";
  pump_2="booting";
  volume=0;
  flag=false;
 }

  // The line of code above may seem unimportant, but it is there
  // so there would be a change in value of pump_1 and pump_2 variables
  // This is important because when the device initailly boots it doesn't
  // send any variables to Arduino cloud, unless the variables changed,
  // this is due to me choosing publishOnChange, because I didnt want it
  // to be sending values all the time, so if the last state of the pump
  // was "running", but the motor is not currently running, arduino IOT 
  // dashboard will show the last state "running" although the motor is not
  // it will update if there is a change. Hence the need for the code block
  // above since the variables will change from "booting", then to their actual
  // value. since there is a change, the variable data will be sent to the cloud

bool pump1Tripped=!digitalRead(PUMP_1_IS_READY_PIN);
bool pump1Running=!digitalRead(PUMP_1_IS_RUNNING_PIN);
bool pump2Tripped=!digitalRead(PUMP_2_IS_READY_PIN);
bool pump2Running=!digitalRead(PUMP_2_IS_RUNNING_PIN);

    // clear trig pin:
   digitalWrite(PIN_TRIG, LOW);  //
   delayMicroseconds(2);
      // Start a new measurement:
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  // Read the result:
  //float depth = (pulseIn(PIN_ECHO, HIGH)/58)/100;// in m
  float depth = pulseIn(PIN_ECHO, HIGH)/58;// in cm
  volume= depth * RESERVIOR_LENGTH * RESERVIOR_WIDTH;
  //Serial.println(depth);
  
if(pump1Running){ // if there is power avialble for the pump

pump_1="running";// string holding text, that will be sent to arduino cloud


}
else if(pump1Tripped){
pump_1="tripped";

}
else{
  pump_1="offline";
  //Serial.println("offline");
}
// for pump2
if(pump2Running){ 
pump_2="running";// string holding text, that will be sent to arduino cloud
}
else if(pump2Tripped){
pump_2="tripped";
}
else{
  pump_2="offline";
}

// logging
//only log when something changes
if(previous_pump_1 != pump_1){
  Log(pump_1,pump_2,volume);
  previous_pump_1=pump_1;
}
if(previous_pump_2 != pump_2){
  Log(pump_1,pump_2,volume);
  previous_pump_2=pump_2;
}

if(previous_volume != volume){
  Log(pump_1,pump_2,volume);
  previous_volume = volume;
}

}


