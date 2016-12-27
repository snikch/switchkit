#include <Homie.h>
#include "pressing.h"

namespace SwitchKit {
  const bool Smart = true;
  const bool Simple = false;
}

class Switch
{
public:
  Switch(const char* name, bool isSmart, int outputPin, int inputPin);
  void loop();

private:
  // Configuration core types.
  int _inputPin;
  int _outputPin;
  String _name;

  // Configuration objects.
  Pressing* _input;
  HomieNode* _node;

  // State booleans.
  bool _isSmart = false;
  bool _isOnline = false;
  bool _isManualOverride = false;
  bool _currentState = false;
  bool _toggleMode = true;
  bool _hasChangedSinceOffline = false;
  bool _relayState = false;

  // Private methods.
  void onHomieEvent(HomieEvent event);
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
