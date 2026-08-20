/**
 * @file params_def.h
 * @brief Canonical parameter definitions and manual calibration stage helpers.
 *
 * @details Every microcontroller in both DCO3-MONOSYNTH and DCO4-REBORN compiles
 * this single header from the shared DCO-PROTOCOL library to guarantee protocol
 * synchronization without numerical drift. It represents the strict union of all
 * parameter IDs across both architectures.
 *
 * MCUs ignore ParamIds they do not own or implement. For example:
 * - Sub-oscillator parameters (90–99) are monosynth/RP2350 specific.
 * - Preset commands (170–174) are strictly owned by the DCO LittleFS store.
 * - Filter/VCA curve shapers (48–56) are Mainboard-local.
 *
 * @note IMPORTANT ARCHITECTURAL RULES:
 * - Do not change the numerical values of existing IDs.
 * - New parameters must always receive new, unused integer IDs.
 * - ParamId semantics (name and integer identifier) must remain stable across all MCUs.
 * - This header is verified against host tooling via `gen_midi_map.py --check`.
 */

 #ifndef PARAMS_DEF_H
 #define PARAMS_DEF_H
 
 #include <stdint.h>
 
 #if defined(__has_include)
 #  if __has_include("project_config.h")
 #    include "project_config.h"
 #  endif
 #endif
 
 /**
  * @enum ParamId
  * @brief Global jump-table parameter identifiers transmitted via slim little-endian 'p' frames.
  */
 enum ParamId : uint16_t {
   // =========================================================================
   // Per-Oscillator Analog Waveform Enables (74HC595 -> DG411 Mux)
   // =========================================================================
 
   /**
    * @brief OSC1 Sawtooth waveform enable switch.
    * @details Controls analog multiplexer gate for OSC1 Saw. Value: 0 = Off, 1 = On.
    */
   PARAM_OSC1_SAW_ENABLE          = 1,
 
   /**
    * @brief OSC1 Pulse/Square waveform enable switch.
    * @details Controls analog multiplexer gate for OSC1 Pulse. Value: 0 = Off, 1 = On.
    */
   PARAM_OSC1_PULSE_ENABLE        = 2,
 
   /**
    * @brief OSC1 Triangle waveform enable switch.
    * @details Controls analog multiplexer gate for OSC1 Triangle. Value: 0 = Off, 1 = On.
    */
   PARAM_OSC1_TRI_ENABLE          = 3,
 
   /**
    * @brief Deprecated Sine status flag (retained to prevent ID reuse).
    */
   PARAM_SINE_STATUS              = 4,
 
   // IDs 5 and 6 are reserved (formerly PARAM_SQR1/SQR2_STATUS). Do not reuse casually.
 
   // =========================================================================
   // Mainboard-Local Control & ADSR Re-trigger Settings
   // =========================================================================
 
   /**
    * @brief Analog resonance compensation gain level.
    * @details Mainboard-local CV adjustment to maintain bass pass-band volume as resonance increases.
    */
   PARAM_RESONANCE_COMPENSATION   = 7,
 
   /**
    * @brief VCA Envelope (ADSR1) restart mode on new note-on.
    * @details 0 = Legato/continue from current level, 1 = Hard reset to zero.
    */
   PARAM_VCA_ADSR_RESTART         = 8,
 
   /**
    * @brief VCF Envelope (ADSR2) restart mode on new note-on.
    * @details 0 = Legato/continue from current level, 1 = Hard reset to zero.
    */
   PARAM_VCF_ADSR_RESTART         = 9,
 
   // =========================================================================
   // Shared Oscillator Routing & Interval Parameters
   // =========================================================================
 
   /**
    * @brief Routing selector for ADSR3 (Auxiliary/DCO envelope) modulation target.
    * @details Selects destination oscillator (e.g., OSC1, OSC2, Both).
    */
   PARAM_ADSR3_TO_OSC_SELECT      = 10,
 
   /**
    * @brief LFO1 Waveform shape selector.
    * @details 0 = Triangle, 1 = Saw, 2 = Ramp, 3 = Square, 4 = Random/S&H.
    */
   PARAM_LFO1_WAVEFORM            = 11,
 
   /**
    * @brief LFO2 Waveform shape selector.
    * @details 0 = Triangle, 1 = Saw, 2 = Ramp, 3 = Square, 4 = Random/S&H.
    */
   PARAM_LFO2_WAVEFORM            = 12,
 
   /**
    * @brief Master octave/interval shift (Global OSC1 interval).
    * @details Semitone/octave coarse pitch transposition.
    */
   PARAM_OSC1_INTERVAL            = 13,
 
   /**
    * @brief OSC2 interval shift relative to master pitch.
    * @details Semitone/octave transposition for OSC2.
    */
   PARAM_OSC2_INTERVAL            = 14,
 
   /**
    * @brief OSC2 fine detune value.
    * @details High-resolution pitch offset for chorusing/beating effects.
    */
   PARAM_OSC2_DETUNE_VAL          = 15,
 
   /**
    * @brief LFO2 modulation depth applied to OSC2 frequency.
    */
   PARAM_LFO2_TO_OSC2             = 16,
 
   /**
    * @brief Oscillator phase sync trigger.
    * @details Force-aligns oscillator phase accumulation on key strike.
    */
   PARAM_OSC_PHASE_SYNC           = 17,
 
   /**
    * @brief Portamento (glide) slew time rate.
    * @details Q15 time coefficient applied to pitch transitions.
    */
   PARAM_PORTAMENTO_TIME          = 18,
 
   // =========================================================================
   // Mainboard-Local Filter & Velocity Routing
   // =========================================================================
 
   /**
    * @brief VCF Cutoff keyboard tracking amount (0..1023).
    */
   PARAM_VCF_KEYTRACK             = 19,
 
   /**
    * @brief Note velocity to VCF Cutoff modulation depth.
    */
   PARAM_VELOCITY_TO_VCF          = 20,
 
   /**
    * @brief Note velocity to VCA Level modulation depth.
    */
   PARAM_VELOCITY_TO_VCA          = 21,
 
   /**
    * @brief OSC1 mixer level (PWM-driven level VCA).
    */
   PARAM_OSC1_LEVEL               = 22,
 
   /**
    * @brief OSC2 mixer level (PWM-driven level VCA).
    */
   PARAM_OSC2_LEVEL               = 23,
 
   /**
    * @brief Sub-oscillator mixer level (PWM-driven level VCA).
    */
   PARAM_SUB_LEVEL                = 24,
 
   // =========================================================================
   // Shared Calibration, Voice Architecture & Allocation
   // =========================================================================
 
   /**
    * @brief Generic calibration value / register transfer.
    */
   PARAM_CALIBRATION_VALUE        = 25,
 
   /**
    * @brief Voice operating mode.
    * @details 0 = Polyphonic, 1 = Monophonic, 2 = Unison.
    */
   PARAM_VOICE_MODE               = 26,
 
   /**
    * @brief Voice allocation and note-stealing policy.
    * @details
    * - 0: Round-robin (Poly: Least-recently-used | Mono: Last-note priority)
    * - 1: Oldest (Poly: Oldest active trigger | Mono: First-note priority)
    * - 2: Quietest (Poly: Lowest EnvVCA level | Mono: Last-note priority)
    * - 3: Quietest keep low (Poly: Lowest EnvVCA, preserves lowest | Mono: Low-note)
    * - 4: Quietest keep high (Poly: Lowest EnvVCA, preserves highest | Mono: High-note)
    * - 5: No stealing (Poly: Drop note-on when full | Mono: First-note locks out)
    */
   PARAM_VOICE_ALLOC_MODE         = 27,
 
   /**
    * @brief Pitch detune spread applied across voices in Unison mode.
    */
   PARAM_UNISON_DETUNE            = 28,
 
   /**
    * @brief Pitch drift simulation depth (analog instability emulation).
    */
   PARAM_ANALOG_DRIFT_AMOUNT      = 29,
 
   /**
    * @brief Pitch drift LFO speed / slew rate.
    */
   PARAM_ANALOG_DRIFT_SPEED       = 30,
 
   /**
    * @brief Voice-to-voice decorrelation spread of analog pitch drift.
    */
   PARAM_ANALOG_DRIFT_SPREAD      = 31,
 
   /**
    * @brief Oscillator hard/soft synchronization routing mode.
    */
   PARAM_SYNC_MODE                = 32,
 
   /**
    * @brief Portamento curve / behavior mode selector (DCO-owned).
    */
   PARAM_PORTAMENTO_MODE          = 33,
 
   /**
    * @brief OSC3 interval shift relative to master pitch (DCO3 monosynth).
    */
   PARAM_OSC3_INTERVAL            = 34,
 
   /**
    * @brief OSC3 fine detune value (DCO3 monosynth).
    */
   PARAM_OSC3_DETUNE_VAL          = 35,
 
   /**
    * @brief LFO2 modulation depth applied to OSC3 frequency (DCO3 monosynth).
    */
   PARAM_LFO2_TO_OSC3             = 36,
 
   /**
    * @brief Soft sync flavor / ramp-catching threshold.
    * @details 0 = Hard sync (master reset pin sideset); 1..3 = Soft sync windowing.
    */
   PARAM_SOFT_SYNC                = 37,
 
   /**
    * @brief Sub-oscillator clock division ratio.
    * @details 0 = Off, 2 = 1 octave down (/2), 4 = 2 octaves down (/4).
    * Also aliases OSC3 Level (38) on legacy monosynth mixer.
    */
   PARAM_SUBOSC_DIVIDE            = 38,
   PARAM_OSC3_LEVEL               = 39, ///< Shared mixer ID for legacy OSC3 Level
 
   // =========================================================================
   // Shared LFO Modulation Routing
   // =========================================================================
 
   /**
    * @brief Global LFO1 pitch modulation depth applied to all DCOs.
    */
   PARAM_LFO1_TO_DCO              = 40,
 
   /**
    * @brief LFO1 primary rate / frequency.
    */
   PARAM_LFO1_SPEED               = 41,
 
   /**
    * @brief LFO2 primary rate / frequency.
    */
   PARAM_LFO2_SPEED               = 42,
 
   // =========================================================================
   // Mainboard-Only VCA & Tremolo Routing
   // =========================================================================
 
   /**
    * @brief Master static VCA bias level / initial gain.
    */
   PARAM_VCA_LEVEL                = 43,
 
   /**
    * @brief LFO1 modulation depth applied to VCA level (Tremolo).
    */
   PARAM_LFO1_TO_VCA              = 44,
 
   // =========================================================================
   // PWM & ADSR3 Modulation Destinations (Shared DCO / Mainboard)
   // =========================================================================
 
   /**
    * @brief LFO2 modulation depth applied to Pulse Width.
    */
   PARAM_LFO2_TO_PW               = 45,
 
   /**
    * @brief ADSR3 envelope modulation depth applied to Pulse Width.
    */
   PARAM_ADSR3_TO_PWM             = 46,
 
   /**
    * @brief ADSR3 envelope modulation depth applied to OSC1 detune / pitch tap.
    */
   PARAM_ADSR3_TO_DETUNE1         = 47,
 
   // =========================================================================
   // ADSR Hardware Log/Linear Curve Shaping (Mainboard-Local)
   // =========================================================================
 
   PARAM_ADSR1_ATTACK_CURVE       = 48, ///< VCA Envelope Attack curvature profile
   PARAM_ADSR1_DECAY_CURVE        = 49, ///< VCA Envelope Decay curvature profile
   PARAM_ADSR1_RELEASE_CURVE      = 50, ///< VCA Envelope Release curvature profile
   PARAM_ADSR2_ATTACK_CURVE       = 51, ///< VCF Envelope Attack curvature profile
   PARAM_ADSR2_DECAY_CURVE        = 52, ///< VCF Envelope Decay curvature profile
   PARAM_ADSR2_RELEASE_CURVE      = 53, ///< VCF Envelope Release curvature profile
   PARAM_ADSR3_ATTACK_CURVE       = 54, ///< AUX/DCO Envelope Attack curvature profile
   PARAM_ADSR3_DECAY_CURVE        = 55, ///< AUX/DCO Envelope Decay curvature profile
   PARAM_ADSR3_RELEASE_CURVE      = 56, ///< AUX/DCO Envelope Release curvature profile
 
   /**
    * @brief VCF envelope re-trigger gating configuration.
    */
   PARAM_VCF_TRIGGER_MODE         = 57,
 
   // =========================================================================
   // Analog Post-Filter Distortion CVs
   // =========================================================================
 
   /**
    * @brief Post-filter analog distortion Drive VCA level.
    */
   PARAM_DIST_DRIVE               = 58,
 
   /**
    * @brief Post-filter analog distortion Dry/Wet mix level.
    */
   PARAM_DIST_MIX                 = 59,
 
   /**
    * @brief Multimode filter topology select (AS3320 pole routing).
    */
   PARAM_FILTER_MODE              = 60,
 
   // IDs 61 and 62 reserved for FX_PROGRAM and FX_MIX.
 
   // =========================================================================
   // Modulation Matrix: 8 Slots x (Source, Destination, Depth)
   // =========================================================================
 
   PARAM_MOD_SLOT0_SOURCE         = 63, ///< Mod Matrix Slot 0: Source ID (0..15, 0xFF=Off)
   PARAM_MOD_SLOT0_DEST           = 64, ///< Mod Matrix Slot 0: Destination ID (0..11)
   PARAM_MOD_SLOT0_DEPTH          = 65, ///< Mod Matrix Slot 0: Bipolar depth (int16, +/-1023)
   PARAM_MOD_SLOT1_SOURCE         = 66, ///< Mod Matrix Slot 1: Source ID
   PARAM_MOD_SLOT1_DEST           = 67, ///< Mod Matrix Slot 1: Destination ID
   PARAM_MOD_SLOT1_DEPTH          = 68, ///< Mod Matrix Slot 1: Bipolar depth
   PARAM_MOD_SLOT2_SOURCE         = 69, ///< Mod Matrix Slot 2: Source ID
   PARAM_MOD_SLOT2_DEST           = 70, ///< Mod Matrix Slot 2: Destination ID
   PARAM_MOD_SLOT2_DEPTH          = 71, ///< Mod Matrix Slot 2: Bipolar depth
   PARAM_MOD_SLOT3_SOURCE         = 72, ///< Mod Matrix Slot 3: Source ID
   PARAM_MOD_SLOT3_DEST           = 73, ///< Mod Matrix Slot 3: Destination ID
   PARAM_MOD_SLOT3_DEPTH          = 74, ///< Mod Matrix Slot 3: Bipolar depth
   PARAM_MOD_SLOT4_SOURCE         = 75, ///< Mod Matrix Slot 4: Source ID
   PARAM_MOD_SLOT4_DEST           = 76, ///< Mod Matrix Slot 4: Destination ID
   PARAM_MOD_SLOT4_DEPTH          = 77, ///< Mod Matrix Slot 4: Bipolar depth
   PARAM_MOD_SLOT5_SOURCE         = 78, ///< Mod Matrix Slot 5: Source ID
   PARAM_MOD_SLOT5_DEST           = 79, ///< Mod Matrix Slot 5: Destination ID
   PARAM_MOD_SLOT5_DEPTH          = 80, ///< Mod Matrix Slot 5: Bipolar depth
   PARAM_MOD_SLOT6_SOURCE         = 81, ///< Mod Matrix Slot 6: Source ID
   PARAM_MOD_SLOT6_DEST           = 82, ///< Mod Matrix Slot 6: Destination ID
   PARAM_MOD_SLOT6_DEPTH          = 83, ///< Mod Matrix Slot 6: Bipolar depth
   PARAM_MOD_SLOT7_SOURCE         = 84, ///< Mod Matrix Slot 7: Source ID
   PARAM_MOD_SLOT7_DEST           = 85, ///< Mod Matrix Slot 7: Destination ID
   PARAM_MOD_SLOT7_DEPTH          = 86, ///< Mod Matrix Slot 7: Bipolar depth
 
   // =========================================================================
   // OSC2 / OSC3 Waveform Enables (DG411 Multiplexer Gates)
   // =========================================================================
 
   PARAM_OSC2_SAW_ENABLE          = 87, ///< OSC2 Saw enable switch
   PARAM_OSC2_PULSE_ENABLE        = 88, ///< OSC2 Pulse/Square enable switch
   PARAM_OSC2_TRI_ENABLE          = 89, ///< OSC2 Triangle enable switch
   PARAM_OSC3_SAW_ENABLE          = 90, ///< OSC3 Saw enable switch (monosynth)
   PARAM_OSC3_PULSE_ENABLE        = 91, ///< OSC3 Pulse enable switch (monosynth)
   PARAM_OSC3_TRI_ENABLE          = 92, ///< OSC3 Triangle enable switch (monosynth)
 
   // =========================================================================
   // Advanced Sub-Oscillators (ENABLE_SUBOSC_ENGINE2; RP2350 Exclusive)
   // =========================================================================
 
   /**
    * @brief Sub-oscillator 1 clock divide ratio (0 = Off, 1 = 1:1, 2..8).
    */
   PARAM_SUB1_DIVIDE              = 93,
 
   /**
    * @brief Sub-oscillator 2 clock divide ratio (0 = Off, 1 = 1:1, 2..8).
    */
   PARAM_SUB2_DIVIDE              = 94,
 
   /**
    * @brief Master oscillator lock for Sub 1 (0 = OSC1, 1 = OSC2, 2 = OSC3).
    */
   PARAM_SUB1_MASTER              = 95,
 
   /**
    * @brief Master oscillator lock for Sub 2 (0 = OSC1, 1 = OSC2, 2 = OSC3).
    */
   PARAM_SUB2_MASTER              = 96,
 
   /**
    * @brief Sub 1 phase offset relative to master reset (0..359 degrees).
    */
   PARAM_SUB1_PHASE               = 97,
 
   /**
    * @brief Sub 2 phase offset relative to master reset (0..359 degrees).
    */
   PARAM_SUB2_PHASE               = 98,
 
   /**
    * @brief Sub 1 pulse width duty (1..255; 128 = 50% square).
    */
   PARAM_SUB1_WIDTH               = 99,
 
   /**
    * @brief Sub 2 pulse width duty (1..255; 128 = 50% square).
    */
   PARAM_SUB2_WIDTH               = 100,
 
   /**
    * @brief Sub-oscillator boolean logic combinator.
    * @details 0 = Off, 1 = XOR, 2 = AND, 3 = OR, 4 = XNOR, 5 = NAND, 6 = NOR, 7 = Sub1, 8 = Sub2.
    */
   PARAM_SUB_LOGIC_OP             = 101,
 
   // =========================================================================
   // Control Panel Modes & Manual Overrides (Input & Screen)
   // =========================================================================
 
   PARAM_CALIBRATION_MODE         = 102, ///< Calibration sub-mode UI flag
   PARAM_FADERS_CONTROL_MANUAL    = 120, ///< Global manual takeover flag for faders
   PARAM_FADER_ROW1_CONTROL_MANUAL= 121, ///< Row 1 fader takeover flag
   PARAM_FADER_ROW2_CONTROL_MANUAL= 122, ///< Row 2 fader takeover flag
   PARAM_VCF_POTS_CONTROL_MANUAL  = 123, ///< VCF potentiometers manual takeover flag
   PARAM_PWM_POTS_CONTROL_MANUAL  = 124, ///< PWM potentiometers manual takeover flag
   PARAM_ALL_CONTROLS_MANUAL      = 125, ///< Full physical control surface manual takeover
   PARAM_ADSR3_ENABLED            = 126, ///< Global enable/bypass toggle for ADSR3
   PARAM_FUNCTION_KEY             = 127, ///< Panel secondary Function / Shift key state
   PARAM_VCA_POTS_CONTROL_MANUAL  = 128, ///< VCA potentiometers manual takeover flag
   PARAM_POTS_CONTROL_MANUAL      = 129, ///< Generic analog potentiometers manual takeover
 
   // =========================================================================
   // Screen UI Navigation & Analog UI Bridge Parameters
   // =========================================================================
 
   PARAM_UI_MENU_POSITION         = 190, ///< Active LVGL menu cursor/page index
 
   // Filter UI bridge parameters (Input -> Screen only; derived from 'd' block pots):
   PARAM_UI_CUTOFF                = 191, ///< Screen toast bridge for Cutoff
   PARAM_UI_RESONANCE             = 192, ///< Screen toast bridge for Resonance
   PARAM_UI_ADSR2_TO_VCF          = 193, ///< Screen toast bridge for VCF Env depth
   PARAM_UI_LFO2_TO_VCF           = 194, ///< Screen toast bridge for VCF LFO2 depth
 
   PARAM_UI_CALIBRATION_DISMISS   = 199, ///< Screen trigger to exit calibration UI
   PARAM_UI_CALIBRATION_MENU_MODE = 200, ///< Calibration menu focus state
   PARAM_UI_MENU_MODE             = 201, ///< Active general UI menu screen identifier
 
   // =========================================================================
   // Pulse Width, Auxiliary LFOs & High-Resolution Pitch Offsets
   // =========================================================================
 
   /**
    * @brief Base manual Pulse Width duty cycle value (former Input 'f' block).
    */
   PARAM_PW_VALUE                 = 210,
 
   PARAM_LFO3_SPEED               = 211, ///< LFO3 rate / frequency
   PARAM_LFO3_WAVEFORM            = 212, ///< LFO3 waveform shape selector
   PARAM_ADSR3_RESTART            = 214, ///< ADSR3 envelope restart mode on note-on
   PARAM_VCA_LEVEL_ALT            = 215, ///< Auxiliary VCA baseline trim
 
   PARAM_LFO1_TO_OSC1             = 216, ///< Additive LFO1 pitch depth for OSC1
   PARAM_LFO1_TO_OSC2             = 217, ///< Additive LFO1 pitch depth for OSC2
   PARAM_LFO1_TO_OSC3             = 218, ///< Additive LFO1 pitch depth for OSC3
   PARAM_LFO2_TO_OSC2_COARSE      = 219, ///< Coarse LFO2 pitch depth for OSC2 (0..511)
   PARAM_LFO2_TO_OSC3_COARSE      = 220, ///< Coarse LFO2 pitch depth for OSC3 (0..511)
 
   /**
    * @brief Analog Character / saturation saturation amount (0..128).
    */
   PARAM_CHARACTER                = 221,
 
   /**
    * @brief EnvVCA -> VCA depth level (former Input 'e' block).
    */
   PARAM_ADSR1_TO_VCA             = 222,
 
   /**
    * @brief ADSR3 envelope pitch modulation mode.
    * @details 0 = Unipolar (positive only), 1 = Centered bipolar (+/-2 octaves).
    */
   PARAM_ADSR3_PITCH_MODE         = 223,
 
   // =========================================================================
   // Calibration Commands & Measurement Telemetry
   // =========================================================================
 
   /**
    * @brief Auto-calibration execution trigger.
    * @details
    * - 0: Cancel running calibration
    * - 1: Amplitude compensation tables only
    * - 2: Pulse Width center/limits only
    * - 3: Full calibration (PW then Amplitude)
    * - 5/6/7: Fine-precision modes for stages 1/2/3
    */
   PARAM_CALIBRATION_FLAG         = 150,
 
   PARAM_MANUAL_CALIBRATION_FLAG  = 151, ///< Manual calibration active state
   PARAM_MANUAL_CALIBRATION_STAGE = 152, ///< Manual calibration walk stage index
   PARAM_MANUAL_CALIBRATION_OFFSET= 153, ///< Manual calibration fine offset trim
 
   /**
    * @brief Frequency gap telemetry from DCO (DCO -> Mainboard -> Input -> Screen).
    */
   PARAM_GAP_FROM_DCO             = 154,
 
   /**
    * @brief Manual calibration stored offset telemetry reported back to Input.
    */
   PARAM_MANUAL_CALIBRATION_OFFSET_FROM_DCO = 155,
 
   /**
    * @brief Commit manual calibration offsets to non-volatile storage.
    */
   PARAM_MANUAL_CALIBRATION_STORE = 156,
 
   /**
    * @brief UI voice topology notification (Input -> Screen). Value = NUM_OSCILLATORS.
    */
   PARAM_UI_VOICE_TOPOLOGY        = 157,
 
   /**
    * @brief Manual calibration step override (0 = low note trim, 1 = 440 Hz).
    */
   PARAM_MANUAL_CALIBRATION_STEP  = 158,
 
   /**
    * @brief Absolute range-PWM amplitude compensation value at 440 Hz.
    */
   PARAM_AMP_COMP_440             = 159,
 
   /**
    * @brief Per-oscillator duty target trim (+/-500 in 0.01% duty units).
    */
   PARAM_AMP_COMP_DUTY_OFFSET     = 161,
 
   /**
    * @brief DCO4 A-oscillator Pulse Width center trim point (0..1023).
    */
   PARAM_CAL_PW_CENTER            = 162,
 
   // =========================================================================
   // LittleFS Preset Store & Diagnostics (DCO-Owned)
   // =========================================================================
 
   PARAM_PRESET_SAVE              = 170, ///< Save live state to LittleFS slot (0..255)
   PARAM_PRESET_LOAD              = 171, ///< Recall state from LittleFS slot (0..255)
   PARAM_PRESET_DUMP              = 172, ///< Dump preset hex record (-1 = dir, 0..255 = slot)
   PARAM_CAL_DUMP                 = 173, ///< Dump calibration data tables to console
   PARAM_UI_PRESET_SCROLL         = 174, ///< Update UI preset scroll view without loading
 
   /**
    * @brief Bench and low-level firmware diagnostic command trigger.
    */
   PARAM_DEBUG_COMMAND            = 160
 };
 
 // ===========================================================================
 // Persistent Parameter List (Stored in presets)
 // ===========================================================================

static constexpr ParamId PERSISTABLE_PARAMS[] = {
  // --- Waveform Enables (DG411 Multiplexer Gates) ---
  PARAM_OSC1_SAW_ENABLE,          // 1
  PARAM_OSC1_PULSE_ENABLE,        // 2
  PARAM_OSC1_TRI_ENABLE,          // 3
  PARAM_OSC2_SAW_ENABLE,          // 87
  PARAM_OSC2_PULSE_ENABLE,        // 88
  PARAM_OSC2_TRI_ENABLE,          // 89
  PARAM_OSC3_SAW_ENABLE,          // 90
  PARAM_OSC3_PULSE_ENABLE,        // 91
  PARAM_OSC3_TRI_ENABLE,          // 92

  // --- Core Oscillator Pitch, Interval & Detune ---
  PARAM_OSC1_INTERVAL,            // 13
  PARAM_OSC2_INTERVAL,            // 14
  PARAM_OSC3_INTERVAL,            // 34
  PARAM_OSC2_DETUNE_VAL,          // 15
  PARAM_OSC3_DETUNE_VAL,          // 35
  PARAM_UNISON_DETUNE,            // 28
  PARAM_PORTAMENTO_TIME,          // 18
  PARAM_PORTAMENTO_MODE,          // 33

  // --- Voice Architecture, Drift & Sync ---
  PARAM_VOICE_MODE,               // 26
  PARAM_VOICE_ALLOC_MODE,         // 27
  PARAM_SYNC_MODE,                // 32
  PARAM_SOFT_SYNC,                // 37
  PARAM_OSC_PHASE_SYNC,           // 17
  PARAM_ANALOG_DRIFT_AMOUNT,      // 29
  PARAM_ANALOG_DRIFT_SPEED,       // 30
  PARAM_ANALOG_DRIFT_SPREAD,      // 31

  // --- Mixer Levels & Dynamics ---
  PARAM_OSC1_LEVEL,               // 22
  PARAM_OSC2_LEVEL,               // 23
  PARAM_OSC3_LEVEL,               // 39
  PARAM_SUB_LEVEL,                // 24
  PARAM_SUBOSC_DIVIDE,            // 38
  PARAM_VCA_LEVEL,                // 43
  PARAM_VCA_LEVEL_ALT,            // 215
  PARAM_VELOCITY_TO_VCA,          // 21
  PARAM_ADSR1_TO_VCA,             // 222

  // --- Filter & Distortion ---
  PARAM_VCF_KEYTRACK,             // 19
  PARAM_VELOCITY_TO_VCF,          // 20
  PARAM_RESONANCE_COMPENSATION,   // 7
  PARAM_FILTER_MODE,              // 60
  PARAM_DIST_DRIVE,               // 58
  PARAM_DIST_MIX,                 // 59

  // --- Pulse Width, Modulation & Character ---
  PARAM_PW_VALUE,                 // 210
  PARAM_LFO2_TO_PW,               // 45
  PARAM_ADSR3_TO_PWM,             // 46
  PARAM_ADSR3_TO_DETUNE1,         // 47
  PARAM_CHARACTER,                // 221
  PARAM_ADSR3_ENABLED,            // 126
  PARAM_ADSR3_PITCH_MODE,         // 223
  PARAM_ADSR3_TO_OSC_SELECT,      // 10

  // --- LFOs & Routing ---
  PARAM_LFO1_WAVEFORM,            // 11
  PARAM_LFO2_WAVEFORM,            // 12
  PARAM_LFO3_WAVEFORM,            // 212
  PARAM_LFO1_SPEED,               // 41
  PARAM_LFO2_SPEED,               // 42
  PARAM_LFO3_SPEED,               // 211
  PARAM_LFO1_TO_DCO,              // 40
  PARAM_LFO1_TO_VCA,              // 44
  PARAM_LFO2_TO_OSC2,             // 16
  PARAM_LFO2_TO_OSC3,             // 36
  PARAM_LFO1_TO_OSC1,             // 216
  PARAM_LFO1_TO_OSC2,             // 217
  PARAM_LFO1_TO_OSC3,             // 218
  PARAM_LFO2_TO_OSC2_COARSE,      // 219
  PARAM_LFO2_TO_OSC3_COARSE,      // 220

  // --- ADSR Hardware Curves & Retrigger ---
  PARAM_VCA_ADSR_RESTART,         // 8
  PARAM_VCF_ADSR_RESTART,         // 9
  PARAM_ADSR3_RESTART,            // 214
  PARAM_VCF_TRIGGER_MODE,         // 57
  PARAM_ADSR1_ATTACK_CURVE,       // 48
  PARAM_ADSR1_DECAY_CURVE,        // 49
  PARAM_ADSR1_RELEASE_CURVE,      // 50
  PARAM_ADSR2_ATTACK_CURVE,       // 51
  PARAM_ADSR2_DECAY_CURVE,        // 52
  PARAM_ADSR2_RELEASE_CURVE,      // 53
  PARAM_ADSR3_ATTACK_CURVE,       // 54
  PARAM_ADSR3_DECAY_CURVE,        // 55
  PARAM_ADSR3_RELEASE_CURVE,      // 56

  // --- Modulation Matrix (8 Slots x Source, Dest, Depth) ---
  PARAM_MOD_SLOT0_SOURCE,         // 63
  PARAM_MOD_SLOT0_DEST,           // 64
  PARAM_MOD_SLOT0_DEPTH,          // 65
  PARAM_MOD_SLOT1_SOURCE,         // 66
  PARAM_MOD_SLOT1_DEST,           // 67
  PARAM_MOD_SLOT1_DEPTH,          // 68
  PARAM_MOD_SLOT2_SOURCE,         // 69
  PARAM_MOD_SLOT2_DEST,           // 70
  PARAM_MOD_SLOT2_DEPTH,          // 71
  PARAM_MOD_SLOT3_SOURCE,         // 72
  PARAM_MOD_SLOT3_DEST,           // 73
  PARAM_MOD_SLOT3_DEPTH,          // 74
  PARAM_MOD_SLOT4_SOURCE,         // 75
  PARAM_MOD_SLOT4_DEST,           // 76
  PARAM_MOD_SLOT4_DEPTH,          // 77
  PARAM_MOD_SLOT5_SOURCE,         // 78
  PARAM_MOD_SLOT5_DEST,           // 79
  PARAM_MOD_SLOT5_DEPTH,          // 80
  PARAM_MOD_SLOT6_SOURCE,         // 81
  PARAM_MOD_SLOT6_DEST,           // 82
  PARAM_MOD_SLOT6_DEPTH,          // 83
  PARAM_MOD_SLOT7_SOURCE,         // 84
  PARAM_MOD_SLOT7_DEST,           // 85
  PARAM_MOD_SLOT7_DEPTH,          // 86

  // --- Advanced Sub-Oscillators (ENABLE_SUBOSC_ENGINE2) ---
  PARAM_SUB1_DIVIDE,              // 93
  PARAM_SUB2_DIVIDE,              // 94
  PARAM_SUB1_MASTER,              // 95
  PARAM_SUB2_MASTER,              // 96
  PARAM_SUB1_PHASE,               // 97
  PARAM_SUB2_PHASE,               // 98
  PARAM_SUB1_WIDTH,               // 99
  PARAM_SUB2_WIDTH,               // 100
  PARAM_SUB_LOGIC_OP,             // 101
};

static constexpr size_t NUM_PERSISTABLE_PARAMS =
    sizeof(PERSISTABLE_PARAMS) / sizeof(PERSISTABLE_PARAMS[0]);

/**
 * @brief Checks if a parameter ID belongs in persistent preset storage.
 */
static inline bool param_is_persistable(uint8_t id) {
  for (size_t i = 0; i < NUM_PERSISTABLE_PARAMS; ++i) {
    if (static_cast<uint8_t>(PERSISTABLE_PARAMS[i]) == id) {
      return true;
    }
  }
  return false;
}


 //=====================================================================
 // Calibration Geometry Constants & Macro Fallbacks
 // ===========================================================================

 #ifndef CAL_STAGES_PER_OSC
 /** @brief Default calibration stages per oscillator for monosynth builds. */
 #define CAL_STAGES_PER_OSC 3
 #endif
 
 #ifndef AMP_COMP_440_MIN
 #ifdef RANGE_PWM_WRAP
 /** @brief Minimum allowable 440 Hz amp-comp value based on PWM wrap. */
 #define AMP_COMP_440_MIN (RANGE_PWM_WRAP / 20)
 /** @brief Maximum allowable 440 Hz amp-comp value based on PWM wrap. */
 #define AMP_COMP_440_MAX (RANGE_PWM_WRAP / 5)
 #else
 #define AMP_COMP_440_MIN 700
 #define AMP_COMP_440_MAX 2800
 #endif
 #endif
 
 #ifndef CAL_PW_CENTER_MAX
 /** @brief Maximum numerical value for calibrated PW center point (10-bit PWM). */
 #define CAL_PW_CENTER_MAX 1023
 #endif
 
 /**
  * @enum CalStageKind
  * @brief Sub-stage classification for manual calibration sequences.
  */
 enum CalStageKind : uint8_t {
   CAL_KIND_SAW = 0,    ///< Sawtooth waveform offset calibration
   CAL_KIND_TRI,        ///< Triangle waveform offset calibration
   CAL_KIND_PULSE,      ///< Pulse/Square baseline offset calibration
   CAL_KIND_PULSE_PW,   ///< Pulse width duty center point calibration
   CAL_KIND_440         ///< 440 Hz amplitude compensation anchoring stage
 };
 
 /**
  * @brief Calculate the total number of manual calibration stages for a given oscillator count.
  * @param nOsc Total number of oscillators (e.g. 3 for Mono, 8 for DCO4).
  * @return Total calibration stages count (uniform 3/osc for Mono; 7 per voice pair for DCO4).
  */
 static inline uint8_t cal_stage_count_n(uint8_t nOsc) {
   if (nOsc <= 3) return (uint8_t)(nOsc * 3u);
   return (uint8_t)((nOsc / 2u) * 7u);
 }
 
 /**
  * @brief Get the maximum valid calibration stage index.
  * @param nOsc Total number of oscillators.
  * @return Maximum 0-indexed stage number.
  */
 static inline uint8_t cal_stage_max_n(uint8_t nOsc) {
   const uint8_t n = cal_stage_count_n(nOsc);
   return (n == 0) ? 0 : (uint8_t)(n - 1u);
 }
 
 /**
  * @brief Map a calibration stage index to its physical target oscillator.
  * @param stage Zero-based calibration stage index.
  * @param nOsc Total number of oscillators.
  * @return Physical oscillator index (0..nOsc-1).
  */
 static inline uint8_t cal_stage_to_osc_n(uint8_t stage, uint8_t nOsc) {
   if (nOsc == 0) return 0;
   if (nOsc <= 3) {
     uint8_t osc = (uint8_t)(stage / 3u);
     return (osc >= nOsc) ? (uint8_t)(nOsc - 1u) : osc;
   }
   uint8_t voice = 0;
   uint8_t remain = stage;
   const uint8_t nVoice = (uint8_t)(nOsc / 2u);
   while (remain >= 7u && (uint8_t)(voice + 1u) < nVoice) {
     remain = (uint8_t)(remain - 7u);
     voice++;
   }
   if (remain < 4u) return (uint8_t)(voice * 2u);
   uint8_t osc = (uint8_t)(voice * 2u + 1u);
   return (osc >= nOsc) ? (uint8_t)(nOsc - 1u) : osc;
 }
 
 /**
  * @brief Determine the waveform or functional kind of a given calibration stage.
  * @param stage Zero-based calibration stage index.
  * @param nOsc Total number of oscillators.
  * @return CalStageKind enum value.
  */
 static inline CalStageKind cal_stage_kind_n(uint8_t stage, uint8_t nOsc) {
   if (nOsc <= 3) {
     switch (stage % 3u) {
       case 1:  return CAL_KIND_PULSE;
       case 2:  return CAL_KIND_440;
       default: return CAL_KIND_SAW;
     }
   }
   uint8_t remain = stage;
   const uint8_t nVoice = (uint8_t)(nOsc / 2u);
   uint8_t voice = 0;
   while (remain >= 7u && (uint8_t)(voice + 1u) < nVoice) {
     remain = (uint8_t)(remain - 7u);
     voice++;
   }
   if (remain < 4u) {
     switch (remain) {
       case 1:  return CAL_KIND_TRI;
       case 2:  return CAL_KIND_PULSE_PW;
       case 3:  return CAL_KIND_440;
       default: return CAL_KIND_SAW;
     }
   }
   switch ((uint8_t)(remain - 4u)) {
     case 1:  return CAL_KIND_PULSE;
     case 2:  return CAL_KIND_440;
     default: return CAL_KIND_SAW;
   }
 }
 
 /**
  * @brief Check if a stage represents the 440 Hz amp-comp anchor calibration.
  * @param stage Zero-based calibration stage index.
  * @param nOsc Total number of oscillators.
  * @return True if stage is CAL_KIND_440, false otherwise.
  */
 static inline bool cal_stage_is_440_n(uint8_t stage, uint8_t nOsc) {
   return cal_stage_kind_n(stage, nOsc) == CAL_KIND_440;
 }
 
 /**
  * @brief Check if a stage is calibrating a Sawtooth waveform.
  * @param stage Zero-based calibration stage index.
  * @param nOsc Total number of oscillators.
  * @return True if stage is CAL_KIND_SAW, false otherwise.
  */
 static inline bool cal_stage_is_saw_n(uint8_t stage, uint8_t nOsc) {
   return cal_stage_kind_n(stage, nOsc) == CAL_KIND_SAW;
 }
 
 /**
  * @brief Check if a stage is calibrating a Triangle waveform.
  * @param stage Zero-based calibration stage index.
  * @param nOsc Total number of oscillators.
  * @return True if stage is CAL_KIND_TRI, false otherwise.
  */
 static inline bool cal_stage_is_tri_n(uint8_t stage, uint8_t nOsc) {
   return cal_stage_kind_n(stage, nOsc) == CAL_KIND_TRI;
 }
 
 /**
  * @brief Check if a stage is calibrating Pulse Width duty center.
  * @param stage Zero-based calibration stage index.
  * @param nOsc Total number of oscillators.
  * @return True if stage is CAL_KIND_PULSE_PW, false otherwise.
  */
 static inline bool cal_stage_is_pw_edit_n(uint8_t stage, uint8_t nOsc) {
   return cal_stage_kind_n(stage, nOsc) == CAL_KIND_PULSE_PW;
 }
 
 /**
  * @brief Check if a stage requires digital square/pulse duty cycle measurement.
  * @param stage Zero-based calibration stage index.
  * @param nOsc Total number of oscillators.
  * @return True if stage is Pulse, Pulse-PW, or 440 Hz amp-comp.
  */
 static inline bool cal_stage_is_square_n(uint8_t stage, uint8_t nOsc) {
   const CalStageKind k = cal_stage_kind_n(stage, nOsc);
   return k == CAL_KIND_PULSE || k == CAL_KIND_PULSE_PW || k == CAL_KIND_440;
 }
 
 #endif  // PARAMS_DEF_H