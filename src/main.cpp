#include <Homie.h>
#include "switch.h"

#define FW_NAME "sensorkit"
#define FW_VERSION "1.0.0"
#define BRAND_NAME "Fairholme"

/* Magic sequence for Autodetectable Binary Upload */
const char *__FLAGGED_FW_NAME = "\xbf\x84\xe4\x13\x54" FW_NAME "\x93\x44\x6b\xa7\x75";
const char *__FLAGGED_FW_VERSION = "\x6a\x3f\x3e\x0e\xe1" FW_VERSION "\xb0\x30\x48\xd4\x1a";
/* End of magic sequence for Autodetectable Binary Upload */

#define DEFAULT_PIN_1_INPUT = 1
#define DEFAULT_PIN_1_OUTPUT = 2
#define DEFAULT_PIN_2_INPUT = 3
#define DEFAULT_PIN_2_OUTPUT = 4
#define DEFAULT_PIN_3_INPUT = 5
#define DEFAULT_PIN_3_OUTPUT = 12
#define DEFAULT_PIN_4_INPUT = 14
#define DEFAULT_PIN_4_OUTPUT = 15

// Basic Configuration.
const int PIN_LED = 13;
const int PIN_BUTTON = 0;
const bool ENABLE_RELAY = true;
const bool ENABLE_TEMP = true;

// HomieNode temperatureNode("temperature", "temperature");
//
// float onedp(float n) {
//   return ((int)((n + 0.05) * 10)) / 10.0;
// }
//
// void tempDidChange(float temp) {
//   temp = onedp(temp);
//   Serial.println("Temp did change");
//   Serial.println(temp);
//   Homie.setNodeProperty(temperatureNode, TEMPERATURE_UNIT).send(String(temp));
// }
//
// const byte onewireData = D5; // one-wire data
// OneWire onewire(onewireData);  // declare instance of the OneWire class to communicate with onewire sensors
// probe temp(&onewire);
//
// void loopHandler() {
//   probe::startConv();                            // start conversion for all sensors
//   if (probe::isReady()) {                        // update sensors when conversion complete
//     temp.update();
//     tempDidChange(temp.getTemp());
//   }
// }

Switch* sw1;
Switch* sw2;
Switch* sw3;
Switch* sw4;

void setup() {
  Serial.begin(115200);

  // Reset the watchdog timer to 8 seconds.
  ESP.wdtDisable();
  ESP.wdtEnable(WDTO_8S);

  Homie_setFirmware(FW_NAME, FW_VERSION);
  Homie_setBrand(BRAND_NAME);

  Homie.setLedPin(PIN_LED, LOW);
  // Homie.setLoopFunction(loopHandler)
  Homie.setResetTrigger(PIN_BUTTON, LOW, 2000);

  Switch sw = Switch("test", true, 12, 13);
  sw1 = &sw;

  // switchNode.advertise("on").settable(switchOnHandler);

  Homie.setup();
  Serial.println("Setup complete");
}

void loop() {
  Homie.loop();
  if (sw1 != nullptr) {
    sw1->loop();
  }
  if (sw2 != nullptr) {
    sw2->loop();
  }
  if (sw3 != nullptr) {
    sw3->loop();
  }
  if (sw4 != nullptr) {
    sw4->loop();
  }
  ESP.wdtFeed();
}
