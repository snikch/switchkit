#include "pressing.h"

/**
 * States
 * 0: Waiting for button to be pressed
 * 1: Button is pressed, awaiting click timeout
 * 2: Button is released, awaiting action complete timeout
 */
Pressing::Pressing(int pin, int activeLow, int delay)
{
  pinMode(pin, INPUT);
  _pin = pin;
  _clicks = 0;

  _clickTicks = delay;
  _pressTicks = delay;
  _toggleTicks = delay;

  _state = 0; // starting with state 0: waiting for button to be pressed
  _isLongPressed = false;  // Keep track of long press state

  if (activeLow) {
    _buttonReleased = HIGH;
    _buttonPressed = LOW;
    digitalWrite(pin, HIGH);
  } else {
    // button connects VCC to the pin when pressed.
    _buttonReleased = LOW;
    _buttonPressed = HIGH;
  }
}

Pressing& Pressing::setToggleMode(bool on)
{
  _toggleMode = on;
}

// onClick registers a callback handler for the specific click count.
Pressing& Pressing::onClick(int count, clickCallback fn)
{
  // // Don't allow setting of callback out of bounds.
  // if (_clickCallbacksSize < count) {
  //   clickCallback* temp = new clickCallback[count];
  //   for (int i = 0; i < _clickCallbacksSize; i++) {
  //       temp[i] = _clickCallbacks[i];
  //   }
  //   delete [] _clickCallbacks;
  //   _clickCallbacks = temp;
  // }
  if (count > 10) {
    // Serial.println("Maximum of 10 click count available");
  } else {
    int index = count - 1;
    _clickCallbacks[index] = fn;
  }
  // _clickCallbacksSize = count;
  return *this;
}

clickCallback Pressing::getClickCallback(int count)
{
  if (sizeof(_clickCallbacks) < count) {
    return nullptr;
  }
  return _clickCallbacks[count-1];
}

Pressing& Pressing::onToggle(clickCallback fn)
{
  _toggleCallback = fn;
}

// onHold registers a hold handler for the specific click count.
Pressing& Pressing::onHold(int count, holdCallback fn)
{
  if (count > 10) {
    // Serial.println("Maximum of 10 hold count available");
  } else {
    int index = count - 1;
    // Don't allow setting of callback out of bounds.
    // if (sizeof(_holdCallbacks) < count) {
    //   holdCallback* temp = new holdCallback[count];
    //   int j = sizeof(_holdCallbacks);
    //   for (int i = 0; i < j; i++) {
    //       temp[i] = _holdCallbacks[i];
    //   }
    //   for (int i = j; i < count; i++) {
    //     temp[i] = nullptr;
    //   }
    //   delete [] _holdCallbacks;
    //   _holdCallbacks = temp;
    // }
    _holdCallbacks[index] = fn;
  }
  return *this;
}

void Pressing::transition(int state)
{
  _previousState = _state;
  _state = state;
}

void Pressing::finishToggle(void)
{
  // Serial.println("Pressing: did toggle");
  if (_toggleCallback != nullptr) {
    _toggleCallback();
  }
}

void Pressing::finishClicking(void)
{
  this->transition(0);
  if (_clicks == 0) {
    return;
  }
  // Size check on callbacks first.
  if (sizeof(_clickCallbacks) >= _clicks) {
    clickCallback fn = _clickCallbacks[_clicks-1];
    if (fn != nullptr) {
      fn();
    }
  }
  _clicks = 0;
}

void Pressing::loop(void)
{
  int currentLevel = digitalRead(_pin);
  if (_state == 0) {
    // state 0: Waiting for press
    if (currentLevel == _buttonPressed) {
      // Serial.println("Pressing: state 0: button pressed");
      this->transition(1);
      _startTime = millis();
    }
  } else if (_state == 1) {
    // state 1: Waiting for release
    unsigned long now = millis();
    unsigned long timeSinceStart = now - _startTime;
    if (currentLevel == _buttonReleased) {
      // Serial.println("Pressing: state 1: button released");
      // Serial.println(timeSinceStart);
      if (timeSinceStart < _debounceTicks) {
        // Looks like we had a false move to pressed - debouncing.
        // Serial.println("Pressing: state 1: debouncing");
        // Serial.println(_previousState);
        this->transition(_previousState);
      } else if (timeSinceStart < _clickTicks) {
        // Serial.println("Pressing: state 1: registering click");
        // Successfully registered a button click, move to state two and start
        // timing from now.
        _clicks++;
        this->transition(2);
        _startTime = now;
      } else {
        // Serial.println("Pressing: state 1: click too long");
        this->finishClicking();
      }
    } else if (_toggleMode && timeSinceStart > _toggleTicks) {
      this->transition(3);
      this->finishToggle();
      _startTime = now;
    }
  } else if (_state == 2) {
    // state 2: waiting for more clicks
    // We've timed out waiting for more clicks. Run the callback and reset.
    unsigned long now = millis();
    if ((unsigned long)(now - _startTime) > _clickTicks) {
      // Serial.println("Pressing: Timeout waiting for more clicks, considering done");
      // Serial.println(_clicks);
      this->finishClicking();
    } else if (currentLevel == _buttonPressed) {
      // The button is pressed, move to state 1.
      // Serial.println("Pressing: state 2: button pressed");
      this->transition(1);
      _startTime = now;
    }
  } else if (_state == 3) {
    // state 3: toggled on, waiting for toggle off
    if (currentLevel == _buttonReleased) {
      // Serial.println("Pressing: state 3: button released");
      this->transition(4);
      _startTime = millis();
    }
  } else if (_state == 4) {
    // state 4: toggle on, but button released for toggle off
    unsigned long timeSinceStart = millis() - _startTime;
    if (currentLevel == _buttonPressed) {
      // Serial.println("Pressing: state 4: button pressed");
      // Serial.println(timeSinceStart);
      if (timeSinceStart < _toggleTicks) {
        // Looks like we had a false move to released - debouncing.
        // Serial.println("Pressing: state 4: debouncing");
        // Serial.println(_previousState);
        this->transition(_previousState);
      }
    } else if (timeSinceStart > _toggleTicks) {
      this->transition(1);
      this->finishToggle();
    }
  } else {
    // state ?: doesn't exist.
    this->transition(0);
  }
}
