
* **DCO ↔ Mainboard (`Serial2` @ 2.5 Mbps):** Bidirectional high-speed link carrying performance notes (`'n'`, `'o'`), expression (`'e'`), modulation streams (`'m'`), parameter updates (`'p'`), and preset block relays (`'a'`–`'d'`).
* **Mainboard ↔ Input (`Serial8` @ 2.5 Mbps):** Ingress of panel edits (`'a'`–`'d'`, `'p'`) and preset directory synchronization (`'N'`, `'O'`, `'L'`).
* **Mainboard ↔ Screen (`Serial1` @ 2.5 Mbps):** Egress of display parameters, ADSR linear bar graphs, filter UI toasts (`ParamId` 191–194), and preset names (`'q'`).
* **DCO USB CDC:** Diagnostic and control link speaking the same inner frames (`enable_usb_control`).

---

## Ingress Tagging & Echo Prevention

Parameter changes can originate from the Panel, MIDI/USB host, or preset recall. To avoid echoing frames back to their sender:
1. Ingress traffic on DCO is tagged (`g_param_ingress = PARAM_SRC_INPUT` vs `PARAM_SRC_USB`).
2. Only parameters and blocks **not** originating from `Serial2` are mirrored back out to the Mainboard.
3. The Mainboard consumes analog envelopes/filters (`'a'`, `'b'`, `'d'`) and relays non-local edits to Input and Screen using dedicated `relay_to_*()` primitives.

---

## Adding a Parameter to the System

1. **Register the ID:** Append the new identifier to `ParamId` in `params_def.h` (never renumber existing entries; ensure numeric value is $\le 255$).
2. **Implement Appliers:** Write `apply_param_<name>()` on each board that acts on the parameter.
3. **Register in Router:** Add an entry `{ PARAM_<NAME>, apply_param_<name> }` to `paramTable[]` in `params.ino` on the target board(s).
4. **Relay if Panel-Reachable:** If the parameter is routed across the Mainboard, ensure the Mainboard's `paramTable[]` forwards the parameter via `serialSendParam16To...()`.

---

## Adding a Serial Command

1. Add the command byte (`CMD_<NAME>`) and payload length constant (`SERIAL_LEN_<NAME>`) to `serial_input_protocol.h`.
2. Implement the `on_frame` callback function in `Serial.ino`.
3. Add the command definition to the board's `SerialCommandDef[]` table.
4. If the command traverses the Mainboard, ensure matching ingress and forwarding definitions exist in `inputSerial8Commands[]` and `mainSerial2Commands[]`.