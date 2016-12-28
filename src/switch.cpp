#include "switch.h"
#include <functional>
using namespace std::placeholders;

Switch::Switch(const char* name, bool isSmart, int outputPin, int inputPin)
: _name(name),
  _inputPin(inputPin),
  _outputPin(outputPin),
  _isSmart(isSmart)
{
  Pressing i = Pressing(inputPin, HIGH, 400);
  _input = &i;
  auto didPressButton = std::bind(&Switch::didToggleViaHW, this);
  auto didToggleManualOverride = std::bind(&Switch::didToggleManualOverride, this);
  // auto handleClickAndHold = std::bind(&Switch::handleClickAndHold, this);
  _input->onClick(1, didPressButton);
  _input->onClick(5, didToggleManualOverride);
  // _input->onHold(1, handleClickAndHold);
  _input->setToggleMode(true);
  _input->onToggle(didPressButton);

  pinMode(outputPin, OUTPUT);

  HomieNode _node(name, "switch");
  auto didToggleViaMQTT = std::bind(&Switch::didToggleViaMQTT, this, _1, _2);
  _node.advertise("on").settable(didToggleViaMQTT);

  auto onHomieEvent = std::bind(&Switch::onHomieEvent, this, _1);
  Homie.onEvent(onHomieEvent);
}

void Switch::loop() {
  _input->loop();
}

// Handle any connect or disconnect events and set the current status.
void Switch::onHomieEvent(HomieEvent event) {
  switch(event.type) {
    case HomieEventType::MQTT_CONNECTED:
      Serial.println("Received connected event");
      this->didComeOnline();
      _isOnline = true;
      break;
    case HomieEventType::WIFI_DISCONNECTED:
    case HomieEventType::MQTT_DISCONNECTED:
      Serial.println("Received disconnected event");
      this->didGoOffline();
      _isOnline = false;
      break;
  }
}

// offlineMode returns true if we aren't online, or if we're in manual override.
bool Switch::isOfflineMode() {
  return !_isOnline || _isManualOverride;
}

void Switch::emitState() {
  Homie.setNodeProperty(*_node, "on").send(_currentState ? "true" : "false");
}

void Switch::setOutputToState(bool state) {
  digitalWrite(_outputPin, state ? HIGH : LOW);
}

void Switch::didComeOnline() {
  if (!_isSmart) {
    this->emitState();
    return;
  }

  if (!_hasChangedSinceOffline) {
    return;
  }

  if (_currentState == false) {
    this->setOutputToState(true);
    // Delay 1 second
    delayMicroseconds(1000000);
  }

  this->emitState();
}

void Switch::didGoOffline() {
  if (this->isOfflineMode()) {
    return;
  }
  _hasChangedSinceOffline = false;
}

bool Switch::didToggleViaMQTT(HomieRange range, String value) {
  bool newState = value == "true";
  if (_currentState == newState) {
    return true;
  }

  _currentState = !_currentState;
  if (!_isSmart) {
    this->setOutputToState(_currentState);
  }
  return true;
}

void Switch::didToggleViaHW() {
  Serial.println("Button state change initiated");
  _currentState = !_currentState;
  if (!this->isOfflineMode()) {
    this->emitState();

    if (!_isSmart) {
      this->setOutputToState(_currentState);
    }
    return;
  }

  if (!_isSmart) {
    this->setOutputToState(_currentState);
    return;
  }

  if (_currentState == false) {
    this->setOutputToState(false);
    return;
  }

  if (_relayState == true) {
    this->setOutputToState(false);
    // Delay 1 second
    delayMicroseconds(1000000);
  }

  this->setOutputToState(true);
}

// TODO: Add this to flow chart
void Switch::didToggleManualOverride() {
  Serial.println("Manual override toggled");
  _isManualOverride = !_isManualOverride;
  // Are we going offline?
  if (_isManualOverride && _isOnline) {
    this->didGoOffline();
    // Are we coming online?
  } else if (!_isManualOverride && _isOnline) {
    this->didComeOnline();
  }
}

void Switch::handleClickAndHold(int duration) {
  Serial.printf("Click and Hold %d\n", duration);
}
