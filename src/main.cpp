#include <Homie.h>
#include "switch.h"

#define FW_NAME "sensorkit"
#define FW_VERSION "1.0.0"
#define BRAND_NAME "Fairholme"
#define DEBUG true

/* Magic sequence for Autodetectable Binary Upload */
const char *__FLAGGED_FW_NAME = "\xbf\x84\xe4\x13\x54" FW_NAME "\x93\x44\x6b\xa7\x75";
const char *__FLAGGED_FW_VERSION = "\x6a\x3f\x3e\x0e\xe1" FW_VERSION "\xb0\x30\x48\xd4\x1a";
/* End of magic sequence for Autodetectable Binary Upload */

/**
 * Pin defaults
 * Pin 0 on nodemcu is the flash button, and Homie defaults this to the reset pin.
 * D0 16
 * D1 05
 * D2 04
 * D3 00
 * D4 02
 * D5 14
 * D6 12
 * D7 13
 * D8 15
 */
// #define DEFAULT_PIN_1_INPUT D0
// #define DEFAULT_PIN_1_OUTPUT D1
// #define DEFAULT_PIN_2_INPUT D2
// #define DEFAULT_PIN_2_OUTPUT D3
// #define DEFAULT_PIN_3_INPUT D4
// #define DEFAULT_PIN_3_OUTPUT D5
#define DEFAULT_PIN_4_INPUT 4
#define DEFAULT_PIN_4_OUTPUT 5

// Basic Configuration.
const int PIN_LED = 13;
const int PIN_BUTTON = 0;
const bool ENABLE_RELAY = true;
const bool ENABLE_TEMP = true;

Switch* sw1;
Switch* sw2;
Switch* sw3;
Switch* sw4;

void onHomieEvent(HomieEvent event) {
  if (sw4 != nullptr) {
    sw4->onHomieEvent(event);
  }
}


void setup() {
  Serial.begin(115200);

  sw4 = new Switch("test", true, DEFAULT_PIN_4_INPUT, DEFAULT_PIN_4_OUTPUT);
  sw4->setDebug(DEBUG);

  // Reset the watchdog timer to 8 seconds.
  ESP.wdtDisable();
  ESP.wdtEnable(WDTO_8S);

  Homie_setFirmware(FW_NAME, FW_VERSION);
  Homie_setBrand(BRAND_NAME);

  // Homie.setLedPin(PIN_LED, LOW);
  Homie.disableLedFeedback();
  // Homie.setLoopFunction(loopHandler)
  // Homie.setSetupFunction(setupHandler);
  Homie.setResetTrigger(PIN_BUTTON, LOW, 2000);
  Homie.onEvent(onHomieEvent);
  Homie.setup();
  Serial.println("Setup complete");
}

int loopCounter = 0;

void loop() {
  loopCounter++;
  bool isPrintLoop = false;
  if (loopCounter > 10000) {
    isPrintLoop = true;
    Serial.println("Loop");
    loopCounter = 0;
  }
  Homie.loop();
  if (isPrintLoop) {
    Serial.println("1");
  }
  // if (sw1 != nullptr) {
  //   sw1->loop();
  // }
  // if (sw2 != nullptr) {
  //   sw2->loop();
  // }
  // if (sw3 != nullptr) {
  //   sw3->loop();
  // }
  if (sw4 != nullptr) {
    sw4->loop();
    if (isPrintLoop) {
      Serial.println("2");
    }
  }
  ESP.wdtFeed();
  if (isPrintLoop) {
    Serial.println("3");
  }
}
