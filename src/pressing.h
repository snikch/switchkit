#ifdef ARDUINO
#include "Arduino.h"
#endif
#include <functional>

typedef std::function<void()> clickCallback;
typedef std::function<void(const int duration)> holdCallback;

class Pressing
{
public:
  // ----- Constructor -----
  Pressing(int pin, int active, int delay);

  Pressing& setToggleMode(bool on);
  Pressing& onClick(int count, clickCallback fn);
  Pressing& onToggle(clickCallback fn);
  Pressing& onHold(int count, holdCallback fn);
  clickCallback getClickCallback(int count);
  void loop(void);

private:
  void transition(int);
  void finishClicking(void);
  void finishToggle(void);

  int _pin;
  bool _toggleMode = false;
  int _clickTicks;
  int _pressTicks;
  int _toggleTicks;
  int _clicks;
  const int _debounceTicks = 50;
  int _state;
  int _previousState;
  unsigned long _startTime; // will be set in state 1

  int _buttonReleased;
  int _buttonPressed;

  bool _isLongPressed;

  // int _clickCallbacksSize = 1;
  clickCallback  _clickCallbacks[10];
  holdCallback  _holdCallbacks[10];
  clickCallback _toggleCallback;
};
