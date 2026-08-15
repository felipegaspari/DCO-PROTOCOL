
---

### `docs/PARAMETER_ROUTING.md`

```markdown
# Parameter Architecture & O(1) Routing Guide

This document describes how parameter IDs, data types, routing tables, and bidirectional state synchronization are implemented across all hardware boards in the **DCO3** and **DCO4** synthesizer ecosystems.

---

## 1. Core Architecture Principles

1. **Single Source of Truth:** Every logical synthesizer parameter has a unique, permanent numerical ID defined in `_build_libs/DCO-PROTOCOL/params_def.h` (`enum ParamId : uint16_t`). IDs $\le 255$ fit in a single byte over the wire.
2. **Zero-Allocation $O(1)$ Jump Dispatch:** Linear scans and large `switch` statements are eliminated. Every MCU builds a 256-pointer lookup table (`param_router_build_jump`) on boot. Executing `update_parameters(id, value)` jumps directly to the applier function in constant time ($O(1)$), guaranteeing no audio/UI loop jitter during parameter bursts.
3. **Universal 16-bit Wire Format:** All parameter updates across all serial and USB links use the 3-byte inner frame `'p'` (`[CMD_PARAM_16:1][id:u8][value:i16 LE]`).
4. **Clear Board Responsibilities:**
   * **DCO:** The DSP authority. Applies parameters to PIO clock dividers, PWM slices, LFO pitch scales, portamento, and LittleFS preset/calibration storage.
   * **Mainboard:** The analog authority and central router. Applies parameters to analog filter/VCA CVs and mixer DACs; forwards DCO-relevant parameters across UART.
   * **Input Controller:** The UI state authority. Maintains in-RAM mirrors of synthesizer state so physical faders, LEDs, and preset saves remain perfectly synchronized.
   * **Screen Controller:** The display authority. Formats parameters into names and values for LVGL widgets and pop-up toasts.

---

## 2. The $O(1)$ Router Lifecycle (`param_router.h`)

Every board compiles the same templated router header:

```cpp
template<typename ValueT>
struct ParamDescriptorT {
  ParamId id;
  void (*apply)(ValueT value);
};
```

### Execution Flow:
```
1. Boot (setup()):
   paramTable[] (array of ID + function pointers)
        │
        ▼
   param_router_build_jump() ──► Fills paramJump[256] array

2. Runtime ('p' frame arrives):
   update_parameters(id, value)
        │
        ▼
   param_router_apply(paramJump, id, value) ──► Direct function jump: paramJump[id](value)
```

---

## 3. Data Flow & Bidirectional Synchronization

A parameter change can originate from three places. Ingress tagging (`g_param_ingress`) ensures that updates flow everywhere without creating infinite echo loops:

```
                      ┌──────────────────────┐
                      │   Input Controller   │
                      │ (Knobs / Faders / UI)│
                      └──────────┬───────────┘
                                 │ Ingress 'p'
                                 ▼
┌─────────────┐   'p' Toasts   ┌──────────────────────┐    'p' Edit     ┌─────────────┐
│   Screen    │◄───────────────┤   STM32 Mainboard    ├────────────────►│  DCO Voice  │
│ Controller  │                │ (Analog CV / Router) │                 │   Engine    │
└─────────────┘                └──────────────────────┘                 └──────┬──────┘
                                         ▲                                     │
                                         │ Preset Load / MIDI CC / USB Mirror  │
                                         └─────────────────────────────────────┘
```

### Scenario A: User turns a physical pot/fader on the Panel
1. **Input Controller:** Encodes `'p'` and transmits to Mainboard on `Serial8`. (Also sends direct UI toast to Screen if configured).
2. **Mainboard:** `update_parameters()` applies local analog CV/DACs. If the parameter affects pitch or DCO engine, `forward_dco()` transmits `'p'` to DCO on `Serial2`.
3. **DCO:** `update_parameters()` applies changes to PIO/timers and registers state in `preset_shadow_capture()`. Because `g_param_ingress == PARAM_SRC_INPUT`, it does **not** echo back to the Mainboard.

### Scenario B: Host sends a parameter edit over USB CDC or MIDI CC
1. **DCO:** Receives edit (`g_param_ingress == PARAM_SRC_USB`). Calls `update_parameters()` to update audio engine and triggers `serial_echo_persistable_param16()`.
2. **Mainboard:** Receives mirrored `'p'` from DCO on `Serial2`. Applies local analog CVs, forwards to Input Controller (`relay_to_input()`), and forwards to Screen (`relay_to_screen()`).
3. **Input Controller:** `update_parameters()` updates local RAM variables so the next preset save captures the host edit.
4. **Screen Controller:** Displays parameter name and value pop-up toast.

### Scenario C: Preset Recall
1. **DCO:** Loads patch chunk from LittleFS. Restores all DSP globals and bursts the patch parameters as `'p'` (and blocks as `'a'`–`'d'`) over `Serial2`.
2. **Mainboard & Input & Screen:** Follow the exact same mirror path as Scenario B, synchronizing all analog CVs, panel fader memories, and screen titles in under 1 ms.

---

## 4. How to Add a New Parameter

Adding a parameter is completely deterministic and requires four steps:

### Step 1: Register ID in `_build_libs/DCO-PROTOCOL/params_def.h`
```cpp
enum ParamId : uint16_t {
  // ...
  PARAM_NEW_FEATURE = 105, // Use next unused integer <= 255. Never renumber!
};
```

### Step 2: Implement on DCO (`DCO/params.ino`)
```cpp
static void apply_param_new_feature(int16_t v) {
  myNewFeatureGlobal = v;
  // Recompute DSP / PIO if needed
}

// In paramTable[]:
{ PARAM_NEW_FEATURE, apply_param_new_feature },
```

### Step 3: Implement on Mainboard if analog/relayed (`MAINBOARD/params.ino`)
```cpp
static void apply_param_new_feature(int16_t v) {
  // If Mainboard controls analog hardware for it:
  set_analog_hardware(v);
  // If DCO also needs it:
  forward_dco(PARAM_NEW_FEATURE, v);
}

// In paramTable[]:
{ PARAM_NEW_FEATURE, apply_param_new_feature },
```

### Step 4: Implement on Input Controller if mirrored (`INPUT-CONTROLLER/params.ino`)
```cpp
static void apply_param_new_feature(int16_t v) {
  myNewFeatureMirror = v;
}

// In paramTable[]:
{ PARAM_NEW_FEATURE, apply_param_new_feature },
```

---

## 5. Summary Table of Special Parameter Ranges

| Range / IDs | Domain | Function |
|:---:|:---:|---|
| **1 – 4, 84 – 89** | Wave Enables | DG411 / 74HC595 analog waveform switches. |
| **10 – 37** | Oscillator / Voice | Intervals, detune, sync modes, portamento, voice alloc. |
| **40 – 47, 216 – 220** | LFO / Modulation | LFO waveforms, speeds, per-osc pitch depths, routing matrix. |
| **60 – 83** | Mod Matrix | 8 slots $\times$ (Source, Dest, Depth). |
| **150 – 162** | Calibration | Auto-tune trigger, manual-cal stages, 440 Hz trims, PW center. |
| **170 – 174** | Presets & Bench | Preset save/load/dump, cal table dumps, benchmark triggers. |
| **191 – 194** | Screen UI Only | Toasts for analog pots (Cutoff, Res, Env→VCF, LFO2→VCF). |
| **210, 222, 223** | Voice Extras | Pulse Width (210), EnvVCA→VCA (222), EnvDCO pitch mode (223). |
```

---

You now have complete documentation covering:
1. `_build_libs/DCO-PROTOCOL/README.md` (Protocol, DMA, framing, and command bytes)
2. `docs/README_serial_and_params.md` (Network topology and routing overview)
3. `docs/PARAMETER_ROUTING.md` (In-depth parameter lifecycle, $O(1)$ router internals, and developer how-to guide)
