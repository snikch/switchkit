#include "config.h"
#include <Homie.h>
#include "switch.h"

#define FW_NAME "switchkit"
#define FW_VERSION "1.0.0"
#ifndef BRAND_NAME
  #define BRAND_NAME "switchkit"
#endif
#ifndef DEBUG
  #define DEBUG true
#endif

/* Magic sequence for Autodetectable Binary Upload */
const char *__FLAGGED_FW_NAME = "\xbf\x84\xe4\x13\x54" FW_NAME "\x93\x44\x6b\xa7\x75";
const char *__FLAGGED_FW_VERSION = "\x6a\x3f\x3e\x0e\xe1" FW_VERSION "\xb0\x30\x48\xd4\x1a";
/* End of magic sequence for Autodetectable Binary Upload */

/**
 * Pin defaults
 * Pin 0 on nodemcu is the flash button, and Homie defaults this to the reset pin.
 * D0 16 User Wake
 * D1 05 (Electrodragon GPIO)
 * D2 04 (Electrodragon GPIO)
 * D3 00 Flash
 * D4 02 TXD1 (NodeMCU LED)
 * D5 14 HSCLK
 * D6 12 HMISO (Electrodragon Relay 2, Sonoff Relay 1)
 * D7 13 RXD2 HMOSI (Electrodragon Relay 1, Sonoff LED)
 * D8 15 TXD2 (Electrodragon GPIO)
 */
#define DEFAULT_PIN_1_INPUT D1
#define DEFAULT_PIN_1_OUTPUT D6
#define DEFAULT_PIN_2_INPUT D2
#define DEFAULT_PIN_2_OUTPUT D7
#define DEFAULT_PIN_3_INPUT D4
#define DEFAULT_PIN_3_OUTPUT D0

// Basic Configuration.
const int PIN_LED = 13;
const int PIN_BUTTON = 0;
const bool ENABLE_RELAY = true;
const bool ENABLE_TEMP = true;

Switch* sw1;
Switch* sw2;
Switch* sw3;

void onHomieEvent(HomieEvent event) {
  // if (DEBUG) {
  //   Homie.getLogger() << "Received Homie Event\n";
  // }
  sw1->onHomieEvent(event);
  sw2->onHomieEvent(event);
  sw3->onHomieEvent(event);
}


void setup() {
  Serial.begin(115200);

  sw1 = new Switch("sw1", true, DEFAULT_PIN_1_INPUT, DEFAULT_PIN_1_OUTPUT);
  sw1->setDebug(DEBUG);
  sw2 = new Switch("sw2", false, DEFAULT_PIN_2_INPUT, DEFAULT_PIN_2_OUTPUT);
  sw2->setDebug(DEBUG);
  sw3 = new Switch("sw3", false, DEFAULT_PIN_3_INPUT, DEFAULT_PIN_3_OUTPUT);
  sw3->setDebug(DEBUG);

  // Reset the watchdog timer to 8 seconds.
  ESP.wdtDisable();
  ESP.wdtEnable(WDTO_8S);

  Homie_setFirmware(FW_NAME, FW_VERSION);
  Homie_setBrand(BRAND_NAME);

  // Homie.setLedPin(PIN_LED, LOW);
  Homie.disableLedFeedback();
  // Homie.setLoopFunction(loopHandler)
  // Homie.setSetupFunction(setupHandler);
  // Homie.disableResetTrigger();
  Homie.setResetTrigger(PIN_BUTTON, LOW, 2000);
  Homie.onEvent(onHomieEvent);
  Homie.setup();
  Serial.println("Setup complete");
}

int loopCounter = 0;

void loop() {
  Homie.loop();
  sw1->loop();
  sw2->loop();
  sw3->loop();
  // // if (sw1 != nullptr) {
  // //   sw1->loop();
  // // }
  // ESP.wdtFeed();
}
