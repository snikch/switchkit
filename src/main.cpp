#define DHT_DEBUG true
// #include "EspSaveCrash.h"
// #include <../gdbstub/gdbstub.h>
#include <Homie.h>
// #include "pressing.h"
extern "C" {
  #include "user_interface.h"
}

#define FW_NAME "sensorkit"
#define FW_VERSION "1.0.0"
#define BRAND_NAME "Fairholme"

#define TEMPERATURE_UNIT "degrees"
#define HUMIDITY_UNIT "percent"

/* Magic sequence for Autodetectable Binary Upload */
const char *__FLAGGED_FW_NAME = "\xbf\x84\xe4\x13\x54" FW_NAME "\x93\x44\x6b\xa7\x75";
const char *__FLAGGED_FW_VERSION = "\x6a\x3f\x3e\x0e\xe1" FW_VERSION "\xb0\x30\x48\xd4\x1a";
/* End of magic sequence for Autodetectable Binary Upload */

HomieSetting<bool> dhtSetting("dht", "Enable Temp and Humidity");
HomieSetting<unsigned long> dhtIntervalSetting("dht_interval", "DHT reporting interval");

// Basic Configuration.
const int PIN_RELAY = 12;
const int PIN_LED = 13;
const int PIN_BUTTON = 0;
const bool ENABLE_RELAY = true;
const bool ENABLE_TEMP = true;
const bool ENABLE_HUMIDITY = true;


HomieNode switchNode("switch", "switch");
// Pressing button(PIN_BUTTON, HIGH, 400);

// State booleans.
bool isOnline = false;
bool isManualOverride = false;
bool currentState = false;
bool isDHTInitialized = false;
bool isDHTStarted;       // flag to indicate we started acquisition
bool isDHTEnabled = false;
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
HomieNode humidityNode("humidity", "humidity");


void tempDidChange(float temp) {
  Serial.println("Temp did change");
  Serial.println(temp);
  Homie.setNodeProperty(temperatureNode, TEMPERATURE_UNIT).send(String(temp));
}

void humidityDidChange(float humidity) {
  Serial.println("humidity did change");
  Serial.println(humidity);
  Homie.setNodeProperty(humidityNode, HUMIDITY_UNIT).send(String(humidity));
}

void setupHandler() {
  // set the enabled value after loading
  isDHTEnabled = dhtSetting.get();
}

#include "PietteTech_DHT.h"

// system defines
#define DHTTYPE  DHT11           // Sensor type DHT11/21/22/AM2301/AM2302
#define DHTPIN   D5              // Digital pin for communications

#define REPORT_INTERVAL_DEFAULT 15000 // in msec must > 2000

// to check dht
unsigned long startMills;
float t, h, d;
float t0, h0, d0;
int acquireresult;
int acquirestatus;
int reportInterval = REPORT_INTERVAL_DEFAULT;

//declaration
void dht_wrapper(); // must be declared before the lib initialization

// Lib instantiate
PietteTech_DHT DHT(DHTPIN, DHTTYPE, dht_wrapper);

// globals

// This wrapper is in charge of calling
// must be defined like this for the lib work
void dht_wrapper() {
  DHT.isrCallback();
}

void dhtLoop() {
  if (isDHTStarted) {
    acquirestatus = DHT.acquiring();
    if (!acquirestatus) {
      acquireresult = DHT.getStatus();
      if ( acquireresult == 0 ) {
        t = DHT.getCelsius();
        h = DHT.getHumidity();
        d = DHT.getDewPoint();
      }
      isDHTStarted = false;
    }
  }

  if ((millis() - startMills) > dhtIntervalSetting.get()) {
    if ( acquireresult == 0 ) {
      // if (h != h0) {
      if (h == 0) {
        Serial.println("Got a phony 0 reading for humidity");
      } else {
        Serial.print("Humidity (%): ");
        Serial.println(h);
        humidityDidChange(h);
        h0 = h;
      }
      // if (t != t0) {
      if (h == 0) {
        Serial.println("Got a phony 0 reading for temperature");
      } else {
        Serial.print("Temperature (oC): ");
        Serial.println(t);
        tempDidChange(t);
        t0 = t;
      }

    } else {
      Serial.println("Is dht11 connected?");
      Serial.println(acquireresult);
      Homie.setNodeProperty(temperatureNode, TEMPERATURE_UNIT).send(String(""));
      Homie.setNodeProperty(humidityNode, HUMIDITY_UNIT).send(String(""));
    }
    startMills = millis();

    // to remove lock
    if (acquirestatus == 1) {
      DHT.reset();
    }

    if (!isDHTStarted) {
      // non blocking method
      DHT.acquire();
      isDHTStarted = true;
    }
  }
}

void loopHandler() {
  if (isDHTEnabled) {
    if (!isDHTInitialized) {
      Serial.println("Initializaing dht");
      Homie.setNodeProperty(temperatureNode, "unit").send("c");
      Homie.setNodeProperty(humidityNode, "unit").send("percent");
      temperatureNode.advertise("unit");
      temperatureNode.advertise("degrees");
      humidityNode.advertise("unit");
      humidityNode.advertise("percent");
      Serial.println("Complete");
      isDHTInitialized = true;
    }
    // Only ready from the dht sensor when connected, and active.
    dhtLoop();
  }
}

void setup() {
  Serial.begin(115200);
  if (ENABLE_RELAY) {
    pinMode(PIN_RELAY, OUTPUT);
    digitalWrite(PIN_RELAY, HIGH);
  }

  Homie_setFirmware(FW_NAME, FW_VERSION);
  Homie_setBrand(BRAND_NAME);
  Homie.setLedPin(PIN_LED, LOW).disableResetTrigger();
  Homie.onEvent(onHomieEvent);
  Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler).setResetTrigger(PIN_BUTTON, LOW, 2000);

  switchNode.advertise("on").settable(switchOnHandler);

  dhtSetting.setDefaultValue(false);
  dhtIntervalSetting.setDefaultValue(REPORT_INTERVAL_DEFAULT);

  // button.onClick(1, didPressButton);
  // button.onClick(5, didToggleManualOverride);
  // // button.onHold(1, handleClickAndHold);
  // button.setToggleMode(true);
  // button.onToggle(didPressButton);

  Homie.setup();
  Serial.println("Setup complete");
  Serial.println(dhtSetting.get());
  Serial.println(dhtIntervalSetting.get());
}

unsigned long lastHeapPrint;

void loop() {
  // button.loop();
  Homie.loop();
}
