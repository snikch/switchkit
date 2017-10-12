#include <Homie.h>
#include "pressing.h"
#include <functional>

// typedef std::function<void(bool active)> stateCallback;
typedef void (*stateCallback)(bool active);

namespace SwitchKit
{
const bool Smart = true;
const bool Simple = false;
}

class Switch
{
public:
  Switch(const char *name, int inputPin, int outputPin);
  Switch &onStateChange(stateCallback fn)
  {
    _stateCallback = fn;
  };
  void onHomieEvent(HomieEvent event);
  void setDebug(bool debug);
  void setSmart(bool isSmart);
  void loop();

private:
  // Configuration core types.
  int _inputPin;
  int _outputPin;
  String _name;

  // Configuration objects.
  Pressing *_input;
  HomieNode *_node;

  // Callbacks functions
  stateCallback _stateCallback;

  // State booleans.
  bool _isSmart = false;
  bool _isOnline = false;
  bool _isManualOverride = false;
  bool _currentState = false;
  bool _toggleMode = true;
  bool _hasChangedSinceOffline = false;
  bool _relayState = false;
  bool _debug = false;

  // Private methods.
  bool isOfflineMode();
  void emitState();
  void setOutputToState(bool state);
  void didComeOnline();
  void didGoOffline();
  bool didToggleViaMQTT(HomieRange range, String value);
  void didToggleViaHW();
  void didToggleManualOverride();
  void handleClickAndHold(int duration);
};
