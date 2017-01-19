#include "switch.h"
#include <functional>
using namespace std::placeholders;

Switch::Switch(const char* name, bool isSmart, int inputPin, int outputPin)
: _name(name),
  _inputPin(inputPin),
  _outputPin(outputPin),
  _isSmart(isSmart)
{
  _input = new Pressing(inputPin, HIGH, 400);
  auto didPressButton = std::bind(&Switch::didToggleViaHW, this);
  auto didToggleManualOverride = std::bind(&Switch::didToggleManualOverride, this);
  // auto handleClickAndHold = std::bind(&Switch::handleClickAndHold, this);
  _input->onClick(1, didPressButton);
  _input->onClick(5, didToggleManualOverride);
  // _input->onHold(1, handleClickAndHold);
  _input->setToggleMode(true);
  _input->onToggle(didPressButton);

  pinMode(outputPin, OUTPUT);

  _node = new HomieNode(name, "switch");
  auto didToggleViaMQTT = std::bind(&Switch::didToggleViaMQTT, this, _1, _2);
  _node->advertise("on").settable(didToggleViaMQTT);

  // Now we need to set some initial state.
  if (_isSmart) {
    this->setOutputToState(true);
  } else {
    this->setOutputToState(_currentState);
  }
}

void Switch::loop() {
  _input->loop();
}

void Switch::setDebug(bool debug) {
  _debug = debug;
}

// Handle any connect or disconnect events and set the current status.
void Switch::onHomieEvent(HomieEvent event) {
  switch(event.type) {
    case HomieEventType::MQTT_CONNECTED:
      if (_debug) {
        Homie.getLogger() << "Received connected event\n";
      }
      this->didComeOnline();
      _isOnline = true;
      break;
    case HomieEventType::WIFI_DISCONNECTED:
    case HomieEventType::MQTT_DISCONNECTED:
      if (_debug) {
        Homie.getLogger() << "Received disconnected event\n";
      }
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
  if (_debug) {
    Homie.getLogger() << _name;
    Homie.getLogger() << " Emitting State: ";
    Homie.getLogger() << (_currentState ? "true\n" : "false\n");
  }
  _node->setProperty("on").send(_currentState ? "true" : "false");
}

void Switch::setOutputToState(bool state) {
  if (_debug) {
    Homie.getLogger() << _name;
    Homie.getLogger() << " Setting Relay Output to ";
    Homie.getLogger() << (_currentState ? "true\n" : "false\n");
  }
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
  if (_debug) {
    Homie.getLogger() << _name;
    Homie.getLogger() << " toggled via MQTT\n";
  }
  bool newState = value == "true";
  if (_currentState == newState) {
    // While this shouldn't be necessary, if the state somehow gets out of sync
    // with the mqtt state, this is required to bring it back in line.
    this->emitState();
    return true;
  }

  _currentState = !_currentState;
  if (!_isSmart) {
    this->setOutputToState(_currentState);
  }
  this->emitState();
  return true;
}

void Switch::didToggleViaHW() {
  if (_debug) {
    Homie.getLogger() << _name;
    Homie.getLogger() << " toggled via Hardware\n";
  }
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
  if (_debug) {
    Homie.getLogger() << _name;
    Homie.getLogger() << " toggled manual override\n";
  }
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
  if (_debug) {
    Homie.getLogger() << _name;
    Homie.getLogger() << " click and held\n";
  }
}
