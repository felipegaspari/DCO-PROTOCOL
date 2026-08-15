#ifndef SERIAL_FRAME_H
#define SERIAL_FRAME_H

#include <stdint.h>
#include <stddef.h>
#include "serial_input_protocol.h"

static constexpr uint8_t SERIAL_FRAME_DELIMITER = 0x00;

#ifndef SERIAL_INNER_MAX_PAYLOAD
#define SERIAL_INNER_MAX_PAYLOAD 8
#endif

static constexpr uint8_t SERIAL_STUFFED_MAX = (uint8_t)(1u + SERIAL_INNER_MAX_PAYLOAD + 2u);

// --- 1. CORE INNER PACKING (Always compiled) ---
static inline INPUT_ALWAYS_INLINE uint8_t serial_inner_pack(uint8_t* dst, uint8_t cmd, const uint8_t* payload, uint8_t payload_len) {
  dst[0] = cmd;
  for (uint8_t i = 0; i < payload_len; ++i) dst[1 + i] = payload[i];
  return (uint8_t)(1u + payload_len);
}

static inline INPUT_ALWAYS_INLINE bool serial_inner_unpack(const uint8_t* inner, uint8_t inner_len, uint8_t& cmd, const uint8_t*& payload, uint8_t& payload_len) {
  if (inner_len < 1) return false;
  cmd = inner[0];
  payload = inner + 1;
  payload_len = (uint8_t)(inner_len - 1u);
  return true;
}

// --- 2. COBS ENCODE/DECODE (Always compiled, ready for future SPI) ---
static inline INPUT_ALWAYS_INLINE int serial_cobs_encode(const uint8_t* src, uint8_t src_len, uint8_t* dst, uint8_t dst_cap) {
  if (dst_cap < 1) return -1;
  uint8_t dst_len = 0, code_idx = 0, code = 1;
  dst[dst_len++] = 0;
  for (uint8_t i = 0; i < src_len; ++i) {
    if (src[i] == 0) {
      dst[code_idx] = code;
      if (dst_len >= dst_cap) return -1;
      code_idx = dst_len;
      dst[dst_len++] = 0;
      code = 1;
    } else {
      if (dst_len >= dst_cap) return -1;
      dst[dst_len++] = src[i];
      code++;
      if (code == 0xFF) {
        dst[code_idx] = code;
        if (i + 1u < src_len) {
          if (dst_len >= dst_cap) return -1;
          code_idx = dst_len;
          dst[dst_len++] = 0;
          code = 1;
        }
      }
    }
  }
  dst[code_idx] = code;
  return (int)dst_len;
}

static inline INPUT_ALWAYS_INLINE int serial_cobs_decode(const uint8_t* src, uint8_t src_len, uint8_t* dst, uint8_t dst_cap) {
  uint8_t dst_len = 0, i = 0;
  while (i < src_len) {
    uint8_t code = src[i++];
    if (code == 0) return -1;
    uint8_t copy = (uint8_t)(code - 1u);
    if ((uint16_t)i + copy > src_len || (uint16_t)dst_len + copy > dst_cap) return -1;
    for (uint8_t j = 0; j < copy; ++j) dst[dst_len++] = src[i++];
    if (code != 0xFF && i < src_len) {
      if (dst_len >= dst_cap) return -1;
      dst[dst_len++] = 0;
    }
  }
  return (int)dst_len;
}

// --- 3. EXPLICIT PIPELINES ---

// RAW Pipeline
static inline INPUT_ALWAYS_INLINE int serial_frame_stuff_raw(uint8_t cmd, const uint8_t* payload, uint8_t payload_len, uint8_t* dst, uint8_t dst_cap) {
  uint8_t n = 1u + payload_len;
  if (n > dst_cap || payload_len > SERIAL_INNER_MAX_PAYLOAD) return -1;
  return serial_inner_pack(dst, cmd, payload, payload_len);
}

static inline INPUT_ALWAYS_INLINE bool serial_frame_unstuff_raw(const uint8_t* wire, uint8_t wire_len, uint8_t& cmd, uint8_t* payload_out, uint8_t& payload_len) {
  const uint8_t* pay = nullptr;
  if (!serial_inner_unpack(wire, wire_len, cmd, pay, payload_len) || payload_len > SERIAL_INNER_MAX_PAYLOAD) return false;
  for (uint8_t i = 0; i < payload_len; ++i) payload_out[i] = pay[i];
  return true;
}

// COBS Pipeline
static inline INPUT_ALWAYS_INLINE int serial_frame_stuff_cobs(uint8_t cmd, const uint8_t* payload, uint8_t payload_len, uint8_t* dst, uint8_t dst_cap) {
  if (payload_len > SERIAL_INNER_MAX_PAYLOAD || dst_cap < 2) return -1;
  uint8_t inner[1 + SERIAL_INNER_MAX_PAYLOAD];
  uint8_t n = serial_inner_pack(inner, cmd, payload, payload_len);
  int enc = serial_cobs_encode(inner, n, dst, (uint8_t)(dst_cap - 1u));
  if (enc < 0) return -1;
  dst[enc] = SERIAL_FRAME_DELIMITER;
  return enc + 1;
}

static inline INPUT_ALWAYS_INLINE bool serial_frame_unstuff_cobs(const uint8_t* wire, uint8_t wire_len, uint8_t& cmd, uint8_t* payload_out, uint8_t& payload_len) {
  if (wire_len < 1) return false;
  uint8_t stuffed_len = (wire[wire_len - 1u] == SERIAL_FRAME_DELIMITER) ? wire_len - 1 : wire_len;
  if (stuffed_len == 0) return false;
  
  uint8_t inner[1 + SERIAL_INNER_MAX_PAYLOAD];
  int decoded = serial_cobs_decode(wire, stuffed_len, inner, sizeof(inner));
  if (decoded < 1) return false;
  
  const uint8_t* pay = nullptr;
  if (!serial_inner_unpack(inner, (uint8_t)decoded, cmd, pay, payload_len) || payload_len > SERIAL_INNER_MAX_PAYLOAD) return false;
  for (uint8_t i = 0; i < payload_len; ++i) payload_out[i] = pay[i];
  return true;
}

// --- 4. ALIAS ASSIGNMENT ---
#ifdef SERIAL_FRAMING_COBS
#define serial_frame_stuff serial_frame_stuff_cobs
#define serial_frame_unstuff serial_frame_unstuff_cobs
#else
#define serial_frame_stuff serial_frame_stuff_raw
#define serial_frame_unstuff serial_frame_unstuff_raw
#endif

// Generic Sender
template<typename StreamT>
static inline INPUT_ALWAYS_INLINE void serial_frame_write(StreamT& stream, uint8_t cmd, const uint8_t* payload, uint8_t payload_len) {
  uint8_t buf[SERIAL_STUFFED_MAX];
  int n = serial_frame_stuff(cmd, payload, payload_len, buf, sizeof(buf));
  if (n > 0) stream.write(buf, (size_t)n);
}

#endif // SERIAL_FRAME_H