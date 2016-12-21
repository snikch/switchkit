#include <Homie.h>
#include "probe.h"
#include <OneWire.h>

#define FW_NAME "sensorkit"
#define FW_VERSION "1.0.0"
#define BRAND_NAME "Fairholme"

#define TEMPERATURE_UNIT "°C"

/* Magic sequence for Autodetectable Binary Upload */
const char *__FLAGGED_FW_NAME = "\xbf\x84\xe4\x13\x54" FW_NAME "\x93\x44\x6b\xa7\x75";
const char *__FLAGGED_FW_VERSION = "\x6a\x3f\x3e\x0e\xe1" FW_VERSION "\xb0\x30\x48\xd4\x1a";
/* End of magic sequence for Autodetectable Binary Upload */

// HomieSetting<bool> dhtSetting("dht", "Enable Temp and Humidity");
// HomieSetting<unsigned long> dhtIntervalSetting("dht_interval", "DHT reporting interval");

// Basic Configuration.
const int PIN_RELAY = 12;
const int PIN_LED = 13;
const int PIN_BUTTON = 0;
const bool ENABLE_RELAY = true;
const bool ENABLE_TEMP = true;


HomieNode switchNode("switch", "switch");

// State booleans.
bool isOnline = false;
bool isManualOverride = false;
bool currentState = false;
const bool toggleMode = true;


// offlineMode returns true if we aren't online, or if we're in manual override.
bool offlineMode() {
  return !isOnline || isManualOverride;
}

// setOnlineState sets whether we're able to communicate with the network broker.
void setOnlineState(bool online) {
  if (isOnline == online) {
    return;
  }

  isOnline = online;

  if (offlineMode()) {
    // If we're offline, the relay should represent the light state.
    digitalWrite(PIN_RELAY, currentState ? HIGH : LOW);
  } else {
    // If we're online, the relay is always on - state managed via bulb.
    digitalWrite(PIN_RELAY, HIGH);
  }
}

// Handle any connect or disconnect events and set the current status.
void onHomieEvent(HomieEvent event) {
  switch(event.type) {
    case HomieEventType::MQTT_CONNECTED:
      Serial.println("Received connected event");
      setOnlineState(true);
      break;
    case HomieEventType::WIFI_DISCONNECTED:
    case HomieEventType::MQTT_DISCONNECTED:
      Serial.println("Received disconnected event");
      setOnlineState(false);
      break;
  }
}

void stateDidChange(bool on) {
  // If we're in online mode, we don't need to do much.
  Homie.setNodeProperty(switchNode, "on").send(on ? "true" : "false");

  // Always show the current state via the LED.
  if (on) {
    Serial.println("Switch turned on");
    digitalWrite(PIN_LED, LOW);
  } else {
    Serial.println("Switch turned off");
    digitalWrite(PIN_LED, HIGH);
  }

  // When we're in offline mode, activate the relay as the state.
  if (ENABLE_RELAY && offlineMode()) {
    if (on) {
      digitalWrite(PIN_RELAY, HIGH);
      Serial.println("Offline: Turning relay on");
    } else {
      digitalWrite(PIN_RELAY, LOW);
      Serial.println("Offline: Turning relay off");
    }
  }
}

void setState(bool newState) {
  if (currentState == newState) {
    return;
  }
  currentState = newState;
  stateDidChange(newState);
}

bool switchOnHandler(HomieRange range, String value) {
  bool on = value == "true";
  setState(on);

  return true;
}

void didPressButton() {
  Serial.println("Button state change initiated");
  setState(!currentState);
}

void didToggleManualOverride() {
  Serial.println("Manual override toggled");
  isManualOverride = !isManualOverride;
}

void handleClickAndHold(int duration) {
  Serial.printf("Click and Hold %d\n", duration);
}

HomieNode temperatureNode("temperature", "temperature");

float onedp(float n) {
  return ((int)((n + 0.05) * 10)) / 10.0;
}

void tempDidChange(float temp) {
  temp = onedp(temp);
  Serial.println("Temp did change");
  Serial.println(temp);
  Homie.setNodeProperty(temperatureNode, TEMPERATURE_UNIT).send(String(temp));
}

const byte onewireData = D5; // one-wire data
OneWire onewire(onewireData);  // declare instance of the OneWire class to communicate with onewire sensors
probe temp(&onewire);

void loopHandler() {
  probe::startConv();                            // start conversion for all sensors
  if (probe::isReady()) {                        // update sensors when conversion complete
    temp.update();
    tempDidChange(temp.getTemp());
  }
}
void setup() {
  Serial.begin(115200);
  if (ENABLE_RELAY) {
    pinMode(PIN_RELAY, OUTPUT);
    digitalWrite(PIN_RELAY, HIGH);
  }
  ESP.wdtDisable();
  ESP.wdtEnable(WDTO_8S);
  Homie_setFirmware(FW_NAME, FW_VERSION);
  Homie_setBrand(BRAND_NAME);
  Homie.setLedPin(PIN_LED, LOW).disableResetTrigger();
  Homie.onEvent(onHomieEvent);
  Homie.setResetTrigger(PIN_BUTTON, LOW, 2000);
  Homie.setLoopFunction(loopHandler).setResetTrigger(PIN_BUTTON, LOW, 2000);

  switchNode.advertise("on").settable(switchOnHandler);

  // dhtSetting.setDefaultValue(false);
  // dhtIntervalSetting.setDefaultValue(REPORT_INTERVAL_DEFAULT);

  // button.onClick(1, didPressButton);
  // button.onClick(5, didToggleManualOverride);
  // // button.onHold(1, handleClickAndHold);
  // button.setToggleMode(true);
  // button.onToggle(didPressButton);

  Homie.setup();
  Serial.println("Setup complete");
  // Serial.println(dhtSetting.get());
  // Serial.println(dhtIntervalSetting.get());
}

unsigned long lastHeapPrint;

void loop() {
  // button.loop();
  Homie.loop();
  ESP.wdtFeed();
}
