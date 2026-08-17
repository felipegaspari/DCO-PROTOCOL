#ifndef SERIAL_INPUT_PROTOCOL_H
#define SERIAL_INPUT_PROTOCOL_H

#include <stdint.h>

#ifndef INPUT_ALWAYS_INLINE
#if defined(__GNUC__) || defined(__clang__)
#define INPUT_ALWAYS_INLINE __attribute__((always_inline))
#else
#define INPUT_ALWAYS_INLINE
#endif
#endif

// =============================================================================
// Canonical System Dimensions (Single Source of Truth for all MCUs)
// =============================================================================
static constexpr uint16_t PRESET_NUM_SLOTS = 256;  // Total presets in RAM / LittleFS
static constexpr uint8_t  PRESET_NAME_LEN  = 16;   // ASCII characters per preset name
static constexpr uint8_t  MOD_SLOT_COUNT   = 8;    // Modulation matrix slots

// Universal Command Set
enum SharedSerialCmd : uint8_t {
  CMD_ADSR1_BLOCK        = 'a',
  CMD_ADSR2_BLOCK        = 'b',
  CMD_ADSR3_BLOCK        = 'c',
  CMD_FILTER_BLOCK       = 'd',
  CMD_PARAM_16           = 'p',
  CMD_PRESET_NAME        = 'q',
  CMD_PARAM_32           = 'x',
  CMD_BULK_CHUNK         = 'B',
  CMD_BULK_COMMIT        = 'C',
  CMD_PRESET_DIR_REQUEST = 'N',
  CMD_PRESET_DIR_ENTRY   = 'O',
  CMD_PRESET_LOADED      = 'L',
  CMD_NOTE_ON            = 'n',
  CMD_NOTE_OFF           = 'o',
  CMD_EXPRESSION         = 'e',
  CMD_MOD_STREAM         = 'm',
  CMD_BENCH_TEXT         = 't',
  CMD_SCREEN_SIGNAL      = 's',
  CMD_PARAM_8            = 'w',
  CMD_PARAM_NAV_BYTE     = 'y',
  CMD_CHAR_SELECT        = 'k',
  CMD_BLOCK_OSC          = 'v',
  CMD_BLOCK_LFO          = 'l',
  CMD_BLOCK_MOD          = 'M',
};

// Screen signal values
static constexpr uint8_t SCREEN_SIGNAL_PRESET_SCROLL = 1;
static constexpr uint8_t SCREEN_SIGNAL_SILENT        = 6;

// Payload sizes (derived directly from canonical dimensions)
static constexpr uint8_t SERIAL_LEN_ADSR_BLOCK           = 8;
static constexpr uint8_t SERIAL_LEN_FILTER_BLOCK         = 8;
static constexpr uint8_t SERIAL_LEN_PARAM_16             = 3;
static constexpr uint8_t SERIAL_LEN_PRESET_NAME          = PRESET_NAME_LEN;                     // 16
static constexpr uint8_t SERIAL_LEN_PARAM_32             = 5;
static constexpr uint8_t SERIAL_LEN_BULK_CHUNK           = 36;
static constexpr uint8_t SERIAL_LEN_BULK_COMMIT          = 8;
static constexpr uint8_t SERIAL_LEN_PRESET_DIR_REQUEST   = 1;
static constexpr uint8_t SERIAL_LEN_PRESET_DIR_ENTRY     = (uint8_t)(1u + PRESET_NAME_LEN);     // 17: [slot:1][name:16]
static constexpr uint8_t SERIAL_LEN_PRESET_LOADED        = 1;
static constexpr uint8_t SERIAL_LEN_NOTE_ON              = 4;
static constexpr uint8_t SERIAL_LEN_NOTE_OFF             = 1;
static constexpr uint8_t SERIAL_LEN_EXPRESSION           = 4;
static constexpr uint8_t SERIAL_LEN_MOD_STREAM           = 16;
static constexpr uint8_t SERIAL_LEN_BENCH_TEXT           = 16;
static constexpr uint8_t SERIAL_LEN_SCREEN_SIGNAL        = 1;
static constexpr uint8_t SERIAL_LEN_SCREEN_PRESET_SCROLL = (uint8_t)(1u + PRESET_NAME_LEN);     // 17: [slot:1][name:16]
static constexpr uint8_t SERIAL_LEN_PARAM_8              = 2;
static constexpr uint8_t SERIAL_LEN_PARAM_NAV_BYTE       = 2;
static constexpr uint8_t SERIAL_LEN_CHAR_SELECT          = 1;

static constexpr uint8_t NOTE_FLAG_RETRIGGER             = (1u << 0);
static constexpr uint8_t NOTE_FLAG_PORTA_ONLY            = (1u << 1);
static constexpr uint8_t SERIAL_BENCH_TEXT_DATA_MAX      = 15;

static constexpr uint8_t SERIAL_LEN_BLOCK_OSC = 22;
static constexpr uint8_t SERIAL_LEN_BLOCK_LFO = 33;
static constexpr uint8_t SERIAL_LEN_BLOCK_MOD = (uint8_t)(MOD_SLOT_COUNT * 4u); // 32

// Compatibility aliases
#define INPUT_CMD_PRESET_DIR_ENTRY   CMD_PRESET_DIR_ENTRY
#define INPUT_SERIAL_LEN_BULK_CHUNK  SERIAL_LEN_BULK_CHUNK
#define INPUT_SERIAL_LEN_BULK_COMMIT SERIAL_LEN_BULK_COMMIT

#pragma pack(push, 1)
struct PatchOscBlock {
  uint16_t wave_enables;
  int8_t   osc1_interval;
  int8_t   osc2_interval;
  int8_t   osc3_interval;
  uint16_t osc2_detune;
  int16_t  unison_detune;
  uint8_t  voice_mode;
  uint8_t  voice_alloc_mode;
  uint8_t  sync_mode;
  uint8_t  soft_sync;
  uint8_t  subosc_divide;
  int8_t   analog_drift;
  int16_t  analog_drift_speed;
  int8_t   analog_drift_spread;
  uint16_t portamento_time;
  uint8_t  portamento_mode;
  uint8_t  character;
};

struct PatchLfoBlock {
  uint8_t  lfo1_waveform;
  uint8_t  lfo2_waveform;
  uint16_t lfo1_speed;
  uint16_t lfo2_speed;
  uint16_t lfo1_to_dco;
  uint8_t  lfo1_to_osc1;
  uint8_t  lfo1_to_osc2;
  uint8_t  lfo1_to_osc3;
  uint16_t lfo2_to_osc2;
  uint16_t lfo2_to_osc3;
  uint16_t lfo2_to_osc2_coarse;
  uint16_t lfo2_to_osc3_coarse;
  uint16_t lfo2_to_pw;
  uint16_t lfo1_to_vca;
  uint16_t pw_value;
  int16_t  adsr1_to_vca;
  int16_t  adsr3_to_pwm;
  int16_t  adsr3_to_detune1;
  uint8_t  adsr3_pitch_mode;
  int8_t   adsr3_to_osc_select;
};

struct ModSlotPacked {
  uint8_t src;
  uint8_t dest;
  int16_t depth;
};

struct PatchModBlock {
  ModSlotPacked slots[MOD_SLOT_COUNT];
};
#pragma pack(pop)

#endif // SERIAL_INPUT_PROTOCOL_H
