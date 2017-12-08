#include "Arduino.h"
#include "config.h"
#include <Homie.h>
#include "switch.h"
#ifdef SONOFF_DUAL
#include "sonoff_dual.h"
#endif

#ifndef FW_NAME
#ifdef SONOFF
#define FW_NAME "switchkit-sonoff"
#endif
#ifdef SONOFF_DUAL
#define FW_NAME "switchkit-sonoff-dual"
#endif
#ifdef ELECTRODRAGON
#define FW_NAME "switchkit-electrodragon"
#endif
#ifndef FW_NAME
#define FW_NAME "switchkit"
#endif
#endif
#define FW_VERSION "1.0.5"
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
 * D4 02 TXD1 (NodeMCU LED, Electrodragon Btn 1)
 * D5 14 HSCLK (Sonoff Jumper Pin 5)
 * D6 12 HMISO (Electrodragon Relay 2, Sonoff Relay 1)
 * D7 13 RXD2 HMOSI (Electrodragon Relay 1, Sonoff LED)
 * D8 15 TXD2 (Electrodragon GPIO)
 */
#ifdef SONOFF
#define DEFAULT_PIN_1_INPUT 14  //D5
#define DEFAULT_PIN_1_OUTPUT 12 //D6
#define PIN_BUTTON_STATE LOW
#endif
#ifdef SONOFF_DUAL
#define DEFAULT_PIN_1_INPUT 4
#define DEFAULT_PIN_1_OUTPUT 12
#define DEFAULT_PIN_2_INPUT 14   //D2
#define DEFAULT_PIN_2_OUTPUT 13 //D7
#define PIN_BUTTON_STATE LOW
#endif
#ifdef ELECTRODRAGON
#define DEFAULT_PIN_1_INPUT 5   //D1
#define DEFAULT_PIN_1_OUTPUT 12 //D6
#define DEFAULT_PIN_2_INPUT 4   //D2
#define DEFAULT_PIN_2_OUTPUT 13 //D7
#define PIN_BUTTON_STATE LOW
#endif
#ifndef DEFAULT_PIN_1_INPUT
#define DEFAULT_PIN_1_INPUT 5   //D1
#define DEFAULT_PIN_1_OUTPUT 12 //D6
#define DEFAULT_PIN_2_INPUT 4   //D2
#define DEFAULT_PIN_2_OUTPUT 13 //D7
#define DEFAULT_PIN_3_INPUT 2   //D4
#define DEFAULT_PIN_3_OUTPUT 16 //D0
#define PIN_BUTTON_STATE LOW
#endif

// Basic Configuration.
const int PIN_LED = 13;
const int PIN_BUTTON = 0;
const bool ENABLE_RELAY = true;
const bool ENABLE_TEMP = true;

// Set up three different switch pointers, which may or may not be populated
// with switch objects depending on the build type.
Switch *sw1;
Switch *sw2;
Switch *sw3;

// onHomieEvent handles fanning out the homie events to the switches.
void onHomieEvent(HomieEvent event)
{
  if (sw1 != nullptr)
  {
    sw1->onHomieEvent(event);
  }
  if (sw2 != nullptr)
  {
    sw2->onHomieEvent(event);
  }
  if (sw3 != nullptr)
  {
    sw3->onHomieEvent(event);
  }
}

// Add three settings for configuring a switch as smart.
HomieSetting<bool> sw1SmartSetting("sw1_smart", "Is Switch 1 Smart?");
HomieSetting<bool> sw2SmartSetting("sw2_smart", "Is Switch 2 Smart?");
HomieSetting<bool> sw3SmartSetting("sw3_smart", "Is Switch 3 Smart?");

// setupHandler is called by Homie once initialised, and is where we set the
// smart configuration. This has to happen after Homie is initialised so that
// the settings have been populated.
void setupHandler()
{
  if (sw1 != nullptr)
  {
    sw1->setSmart(sw1SmartSetting.get());
  }
  if (sw2 != nullptr)
  {
    sw2->setSmart(sw2SmartSetting.get());
  }
  if (sw3 != nullptr)
  {
    sw3->setSmart(sw3SmartSetting.get());
  }
}

// describePins outputs the pins in use.
void describePins()
{
  Serial.println("Pin Config (Input, Output)");
#ifdef DEFAULT_PIN_1_INPUT
  Serial.printf("Switch 1 %d %d\n", DEFAULT_PIN_1_INPUT, DEFAULT_PIN_1_OUTPUT);
#endif
#ifdef DEFAULT_PIN_2_INPUT
  Serial.printf("Switch 2 %d %d\n", DEFAULT_PIN_2_INPUT, DEFAULT_PIN_2_OUTPUT);
#endif
#ifdef DEFAULT_PIN_3_INPUT
  Homie.getLogger() << "Switch 3 " << DEFAULT_PIN_3_INPUT << " " << DEFAULT_PIN_3_OUTPUT << endl;
#endif
}

void setup()
{
// Sonoff dual always has two switches, with the dual state callbacks.
#ifdef SONOFF_DUAL
  // Avoid logging due to serial chip on duals
  Homie.disableLogging();
  Serial.begin(19200);
//  SonoffDual.setup();
  sw1 = new Switch("sw1", DEFAULT_PIN_1_INPUT, DEFAULT_PIN_1_OUTPUT);
  sw1->setDebug(DEBUG);
  sw1->onStateChange(dualDidChangeStateOne);
  sw2 = new Switch("sw2", DEFAULT_PIN_2_INPUT, DEFAULT_PIN_2_OUTPUT);
  sw2->setDebug(DEBUG);
  sw2->onStateChange(dualDidChangeStateTwo);
#endif
#ifndef SONOFF_DUAL
  Serial.begin(115200);
#ifdef DEFAULT_PIN_1_INPUT
  sw1 = new Switch("sw1", DEFAULT_PIN_1_INPUT, DEFAULT_PIN_1_OUTPUT);
  sw1->setDebug(DEBUG);
#endif
#ifdef DEFAULT_PIN_2_INPUT
  sw2 = new Switch("sw2", DEFAULT_PIN_2_INPUT, DEFAULT_PIN_2_OUTPUT);
  sw2->setDebug(DEBUG);
#endif
#ifdef DEFAULT_PIN_3_INPUT
  sw3 = new Switch("sw3", DEFAULT_PIN_3_INPUT, DEFAULT_PIN_3_OUTPUT);
  sw3->setDebug(DEBUG);
#endif
#endif
describePins();

  sw1SmartSetting.setDefaultValue(false);
  sw2SmartSetting.setDefaultValue(false);
  sw3SmartSetting.setDefaultValue(false);

  // Reset the watchdog timer to 8 seconds.
  ESP.wdtDisable();
  ESP.wdtEnable(WDTO_8S);

  Homie_setFirmware(FW_NAME, FW_VERSION);
  Homie_setBrand(BRAND_NAME);

  Homie.setLedPin(PIN_LED, LOW);
  // Sonoff dual is not LED compatible, avoid enabling LED feedback.
  Homie.disableLedFeedback();
  Homie.setSetupFunction(setupHandler);
  Homie.setResetTrigger(PIN_BUTTON, PIN_BUTTON_STATE, 2000);
  Homie.onEvent(onHomieEvent);
  Homie.setup();
  Serial.println("Setup complete");
}

void loop()
{
  Homie.loop();
  if (sw1 != nullptr)
  {
    sw1->loop();
  }
  if (sw2 != nullptr)
  {
    sw2->loop();
  }
  if (sw3 != nullptr)
  {
    sw3->loop();
  }
  ESP.wdtFeed();
}
