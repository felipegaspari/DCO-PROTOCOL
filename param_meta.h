#ifndef PARAM_META_H
#define PARAM_META_H

#include <stdint.h>
#include "params_def.h"

// --- Enum Formatters ---
inline const char* param_curve_name(int32_t val) {
  switch (val) {
    case 0:  return " EXP";
    case 1:  return " SOFT";
    case 2:  return " STEEP";
    case 3:  return " CONCAVE";
    case 4:  return " FAST S";
    case 5:  return " SLOW THEN LIN";
    case 6:  return " ALMOST LIN";
    case 7:  return " LINEAR";
    default: return " EXP";
  }
}

inline const char* param_voice_mode_name(int32_t val) {
  switch (val) {
    case 0:  return " MONO";
    case 1:  return " POLY";
    case 2:  return " UNISON";
    default: return " POLY";
  }
}

inline const char* param_voice_alloc_name(int32_t val) {
  switch (val) {
    case 0:  return " ROUND ROBIN";
    case 1:  return " OLDEST";
    case 2:  return " QUIETEST";
    case 3:  return " QUIETEST LOW";
    case 4:  return " QUIETEST HIGH";
    case 5:  return " NO STEAL";
    default: return " ROUND ROBIN";
  }
}

inline const char* param_lfo_waveform_name(int32_t val) {
  switch (val) {
    case 0:  return " Off";
    case 1:  return " Saw";
    case 2:  return " Tri Slewed";
    case 3:  return " Sine";
    case 4:  return " Square";
    case 5:  return " Sharktooth";
    case 6:  return " Trapezoid";
    case 7:  return " Linear Tri";
    case 8:  return " Staircase";
    case 9:  return " Folded Sine";
    default: return " Off";
  }
}

// --- 1. Math Scaling (Raw wire value -> User display value) ---
inline int32_t param_scale_display_value(ParamId id, int32_t val) {
  switch (id) {
    case ParamId::PARAM_OSC1_INTERVAL:
      return (val - 36) / 12;
    case ParamId::PARAM_OSC2_INTERVAL:
    case ParamId::PARAM_OSC3_INTERVAL:
      return val - 36;
    case ParamId::PARAM_OSC2_DETUNE_VAL:
    case ParamId::PARAM_OSC3_DETUNE_VAL:
      return val - 256;
    case ParamId::PARAM_ADSR3_TO_PWM:
      return val - 512;
    default:
      return val;
  }
}

// --- 2. String Label Lookup ---
inline const char* param_get_name(ParamId id, int32_t inValue) {
  switch (id) {
    // Waveform Enables
    case ParamId::PARAM_OSC1_SAW_ENABLE:   return (inValue != 0) ? " OSC1 SAW ON" : " OSC1 SAW OFF";
    case ParamId::PARAM_OSC1_PULSE_ENABLE: return (inValue != 0) ? " OSC1 PULSE ON" : " OSC1 PULSE OFF";
    case ParamId::PARAM_OSC1_TRI_ENABLE:   return (inValue != 0) ? " OSC1 TRI ON" : " OSC1 TRI OFF";
    case ParamId::PARAM_OSC2_SAW_ENABLE:   return (inValue != 0) ? " OSC2 SAW ON" : " OSC2 SAW OFF";
    case ParamId::PARAM_OSC2_PULSE_ENABLE: return (inValue != 0) ? " OSC2 PULSE ON" : " OSC2 PULSE OFF";
    case ParamId::PARAM_OSC2_TRI_ENABLE:   return (inValue != 0) ? " OSC2 TRI ON" : " OSC2 TRI OFF";
    case ParamId::PARAM_OSC3_SAW_ENABLE:   return (inValue != 0) ? " OSC3 SAW ON" : " OSC3 SAW OFF";
    case ParamId::PARAM_OSC3_PULSE_ENABLE: return (inValue != 0) ? " OSC3 PULSE ON" : " OSC3 PULSE OFF";
    case ParamId::PARAM_OSC3_TRI_ENABLE:   return (inValue != 0) ? " OSC3 TRI ON" : " OSC3 TRI OFF";
    case ParamId::PARAM_SINE_STATUS:       return " SINE (unused)";

    // Switches & Curves
    case ParamId::PARAM_RESONANCE_COMPENSATION: return " ResoAmpComp";
    case ParamId::PARAM_VCA_ADSR_RESTART:       return " ADSR1 Restart";
    case ParamId::PARAM_VCF_ADSR_RESTART:       return " ADSR2 Restart";
    case ParamId::PARAM_ADSR1_ATTACK_CURVE:
    case ParamId::PARAM_ADSR1_DECAY_CURVE:
    case ParamId::PARAM_ADSR2_ATTACK_CURVE:
    case ParamId::PARAM_ADSR2_DECAY_CURVE:      return param_curve_name(inValue);

    // LFOs
    case ParamId::PARAM_LFO1_WAVEFORM:
    case ParamId::PARAM_LFO2_WAVEFORM:
    case ParamId::PARAM_LFO3_WAVEFORM:          return param_lfo_waveform_name(inValue);
    case ParamId::PARAM_LFO1_SPEED:             return " LFO1 Speed";
    case ParamId::PARAM_LFO2_SPEED:             return " LFO2 Speed";
    case ParamId::PARAM_LFO3_SPEED:             return " LFO3 Speed";
    case ParamId::PARAM_LFO1_TO_DCO:            return " LFO1 -> Pitch";
    case ParamId::PARAM_LFO2_TO_PW:             return " LFO2 -> PWM";
    case ParamId::PARAM_LFO1_TO_VCA:            return " LFO1 -> VCA";
    case ParamId::PARAM_LFO2_TO_OSC2:           return " LFO2->OSC2 Pitch";
    case ParamId::PARAM_LFO2_TO_OSC3:           return " LFO2->OSC3 Pitch";
    case ParamId::PARAM_LFO1_TO_OSC1:           return " LFO1 -> OSC1 extra";
    case ParamId::PARAM_LFO1_TO_OSC2:           return " LFO1 -> OSC2 extra";
    case ParamId::PARAM_LFO1_TO_OSC3:           return " LFO1 -> OSC3 extra";
    case ParamId::PARAM_LFO2_TO_OSC2_COARSE:    return " LFO2 -> OSC2 coarse";
    case ParamId::PARAM_LFO2_TO_OSC3_COARSE:    return " LFO2 -> OSC3 coarse";

    // Pitch & Voice
    case ParamId::PARAM_OSC1_INTERVAL:          return " Octave";
    case ParamId::PARAM_OSC2_INTERVAL:          return " OSC2 Interval";
    case ParamId::PARAM_OSC3_INTERVAL:          return " OSC3 Interval";
    case ParamId::PARAM_OSC2_DETUNE_VAL:        return " OSC2 Detune";
    case ParamId::PARAM_OSC3_DETUNE_VAL:        return " OSC3 Detune";
    case ParamId::PARAM_UNISON_DETUNE:          return " Analog Detune";
    case ParamId::PARAM_VOICE_MODE:             return param_voice_mode_name(inValue);
    case ParamId::PARAM_VOICE_ALLOC_MODE:       return param_voice_alloc_name(inValue);
    case ParamId::PARAM_OSC_PHASE_SYNC:         return " OscPhaseSync";
    case ParamId::PARAM_PORTAMENTO_TIME:        return " Portamento";
    case ParamId::PARAM_SYNC_MODE:              return " Sync Mode";
    case ParamId::PARAM_PW_VALUE:               return " PW";
    case ParamId::PARAM_CHARACTER:              return " Character";

    // Drift
    case ParamId::PARAM_ANALOG_DRIFT_AMOUNT:    return " Analog Drift";
    case ParamId::PARAM_ANALOG_DRIFT_SPEED:     return " Analog Drift Speed";
    case ParamId::PARAM_ANALOG_DRIFT_SPREAD:    return " Analog Drift Spread";

    // Levels & Dynamics
    case ParamId::PARAM_OSC1_LEVEL:             return " OSC1 Level";
    case ParamId::PARAM_OSC2_LEVEL:             return " OSC2 Level";
    case ParamId::PARAM_OSC3_LEVEL:             return " OSC3 Level";
    case ParamId::PARAM_SUB_LEVEL:              return " SUB Level";
    case ParamId::PARAM_VCA_LEVEL:              
    case ParamId::PARAM_VCA_LEVEL_ALT:          return " VCA -> LEVEL";
    case ParamId::PARAM_ADSR1_TO_VCA:           return " ADSR1 -> VCA";
    case ParamId::PARAM_VELOCITY_TO_VCF:        return " Velocity -> VCF";
    case ParamId::PARAM_VELOCITY_TO_VCA:        return " Velocity -> VCA";

    // Filter & Mod
    case ParamId::PARAM_VCF_KEYTRACK:           return " VCF Keytrack";
    case ParamId::PARAM_UI_CUTOFF:              return " Cutoff";
    case ParamId::PARAM_UI_RESONANCE:           return " Resonance";
    case ParamId::PARAM_UI_ADSR2_TO_VCF:        return " ADSR2 -> VCF";
    case ParamId::PARAM_UI_LFO2_TO_VCF:         return " LFO2 -> VCF";
    case ParamId::PARAM_ADSR3_TO_PWM:           return " ADSR3 -> PWM";
    case ParamId::PARAM_ADSR3_TO_DETUNE1:       return " ADSR3 -> Pitch";
    case ParamId::PARAM_ADSR3_PITCH_MODE:       return " EnvDCO pitch centered";
    case ParamId::PARAM_ADSR3_RESTART:          return " ADSR3 Restart";

    // Panel Overrides
    case ParamId::PARAM_FUNCTION_KEY:           return " FUNCTION KEY";
    case ParamId::PARAM_FADERS_CONTROL_MANUAL:  return " MAN FADERS";
    case ParamId::PARAM_FADER_ROW1_CONTROL_MANUAL: return " MAN FADERS 1";
    case ParamId::PARAM_FADER_ROW2_CONTROL_MANUAL: return " MAN FADERS 2";
    case ParamId::PARAM_VCF_POTS_CONTROL_MANUAL: return " MANUAL VCF";
    case ParamId::PARAM_PWM_POTS_CONTROL_MANUAL: return " MANUAL PWM";
    case ParamId::PARAM_VCA_POTS_CONTROL_MANUAL: return " MANUAL VCA";
    case ParamId::PARAM_POTS_CONTROL_MANUAL:    return " MANUAL POTS";
    case ParamId::PARAM_ALL_CONTROLS_MANUAL:    return " ALL CONTROLS MANUAL";
    case ParamId::PARAM_ADSR3_ENABLED:          return " ADSR3 ENABLED";

    // Calibration
    case ParamId::PARAM_CALIBRATION_FLAG:       return " AUTO CALIBRATION";
    case ParamId::PARAM_MANUAL_CALIBRATION_FLAG:return " MANUAL CALIBRATION";
    case ParamId::PARAM_MANUAL_CALIBRATION_OFFSET: return " OFFSET";
    case ParamId::PARAM_AMP_COMP_440:           return " AMP";
    case ParamId::PARAM_CAL_PW_CENTER:          return " PW";
    case ParamId::PARAM_GAP_FROM_DCO:           return " GAP";
    case ParamId::PARAM_CALIBRATION_VALUE:      return " CALIBRATION VAL";

    default:
      return "";
  }
}

#endif // PARAM_META_H