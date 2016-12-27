graph TD;

  subgraph toggle
  isSmartOffline[isSmart]

  didToggleViaHw---toggleStoredState
  toggleStoredState---isOnline
  isOnline-->|n|isSmartOffline
  isOnline-->|y|isSmart
  isOnline-->|y|emitState


  isSmart-->|n|setRelayToNewState

  isSmartOffline-->|n|setRelayToNewState
  isSmartOffline-->|y|isNewStateOn
  isNewStateOn-->|n|setRelayToOff
  isNewStateOn-->|y|isRelayOn
  isRelayOn-->|n|setRelayToOn
  isRelayOn-->|y|toggleRelayOffOn
  end

  subgraph mqttToggle
  toggleStoredStateMqtt(toggleStoredState)
  didToggleViaMqtt-->toggleStoredStateMqtt
  toggleStoredStateMqtt-->isSmart
  end

  subgraph connect
  isSmartReconnecting[isSmart]

  didComeOnline-->isSmartReconnecting
  isSmartReconnecting-->|y|hasChangedSinceLastConnect
  isSmartReconnecting-->|n|emitState
  hasChangedSinceLastConnect-->|y|isStateOn
  isStateOn-->|y|emitState
  isStateOn-->|n|turnOnAndDelay
  turnOnAndDelay-->emitState
  end
