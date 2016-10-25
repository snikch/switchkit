# Requirements

- Submits state change events to mqtt when connected
- Handles lights via relay when mqtt not connected
- Handles wifi / mqtt disconnects without affecting lights
- Can manually turn to offline mode


# TODO

- [ ] Offline mode should not change relay until toggled, avoids "wifi disconnected, turns relay off, wifi connected, turn relay on - light goes on". Next change should attempt to set state from previous value.


# Layout

## toggleState

```
on:
  online:
    current_state:
      on: no-op
      off: Submit "true"
  offline:
    current_state:
      on: no-op
      off:
        relay_state:
          on:
            has_turned_off_in_current_offline_mode:
              true: no-op
              false: turn relay off then on, to ensure lights
          off: relay on

off:
  online:
    current_state:
      on: Submit "false"
      off: no-op
  offline:
    current_state:
      on:
        - turn relay off
        - set has_turned_off_in_current_offline_mode true
      off:
        - set has_turned_off_in_current_offline_mode true
```

## buttonState

```
press:
  - increment count
  - start timer

timeout:
  count == 1:
    emit: single_press
  count >1 < 5:
    emit: unknown_press
  count: 5:
    emit: manual_toggle
```
