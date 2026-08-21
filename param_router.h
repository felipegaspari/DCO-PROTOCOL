/**
 * @file param_router.h
 * @brief O(1) Jump Table Dispatcher and Parameter Router Builder.
 * 
 * @details Provides a high-performance, jump-table-driven parameter dispatch engine 
 * designed for time-critical audio/CV microcontrollers.
 * 
 * Key Capabilities:
 *  - Builds a static O(1) function pointer jump table (`PARAM_ROUTER_JUMP_SIZE = 256`) 
 *    at startup from local descriptor arrays (`ParamDescriptorT`).
 *  - Decouples parameter identification (`ParamId`) from local hardware execution, allowing
 *    the DCO (Q24 pitch/drift), Mainboard (analog CV/DACs), and Input (RAM mirrors) to each
 *    bind unique static applier functions to identical wire IDs.
 *  - Provides linear fallback scanning for memory-constrained or non-contiguous environments.
 */

#ifndef PARAM_ROUTER_H
#define PARAM_ROUTER_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "params_def.h"

template<typename ValueT>
struct ParamDescriptorT {
  ParamId id;
  void (*apply)(ValueT value);
};

static constexpr uint16_t PARAM_ROUTER_JUMP_SIZE = 256;

// 1. Build Jump Table (DCO, Mainboard, Input)
template<typename ValueT>
inline void param_router_build_jump(
    void (*(&jump)[PARAM_ROUTER_JUMP_SIZE])(ValueT),
    const ParamDescriptorT<ValueT>* table,
    size_t tableSize)
{
  memset(jump, 0, sizeof(void*) * PARAM_ROUTER_JUMP_SIZE);
  for (size_t i = 0; i < tableSize; ++i) {
    uint16_t id = static_cast<uint16_t>(table[i].id);
    if (id < PARAM_ROUTER_JUMP_SIZE) {
      jump[id] = table[i].apply;
    }
  }
}

// 2. O(1) Jump Table Dispatch (3 arguments)
template<typename ValueT>
inline void param_router_apply(
    void (*const (&jump)[PARAM_ROUTER_JUMP_SIZE])(ValueT),
    uint16_t rawId,
    ValueT value)
{
  if (rawId < PARAM_ROUTER_JUMP_SIZE && jump[rawId]) {
    jump[rawId](value);
  }
}

// Backward compatibility alias for jump apply
template<typename ValueT>
inline void param_router_apply_jump(
    void (*const (&jump)[PARAM_ROUTER_JUMP_SIZE])(ValueT),
    uint16_t rawId,
    ValueT value)
{
  param_router_apply(jump, rawId, value);
}

// 3. Linear Scan Dispatch (4 arguments, used by Screen Controller)
template<typename ValueT>
inline void param_router_apply(
    const ParamDescriptorT<ValueT>* table,
    size_t tableSize,
    uint16_t rawId,
    ValueT value)
{
  ParamId id = static_cast<ParamId>(rawId);
  for (size_t i = 0; i < tableSize; ++i) {
    if (table[i].id == id) {
      table[i].apply(value);
      return;
    }
  }
}

void init_param_router();
void update_parameters(uint8_t id, int16_t value);

#endif // PARAM_ROUTER_H