#ifndef SERIAL_DMA_TX_H
#define SERIAL_DMA_TX_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#if defined(ARDUINO_ARCH_RP2040)
  #include "hardware/dma.h"
  #include "hardware/uart.h"
  #include "pico/mutex.h"
#elif defined(ARDUINO_ARCH_STM32)
  #include <Arduino.h> 
#endif

// 512 bytes ping-pong buffer provides plenty of headroom for bursts
static constexpr uint16_t SERIAL_DMA_BUF_SIZE = 1024;

struct SerialDmaEngine {
#if defined(ARDUINO_ARCH_RP2040)
  uart_inst_t *uart;
  mutex_t mutex;
  int chan;
  dma_channel_config cfg;
#elif defined(ARDUINO_ARCH_STM32)
  USART_TypeDef *uart;
  DMA_HandleTypeDef hdma;
#endif

  alignas(32) uint8_t buf[2][SERIAL_DMA_BUF_SIZE];
  uint16_t len[2];
  uint8_t fill;
  bool sending;
};

// Universal inline global instance across all translation units
inline SerialDmaEngine serial_dma_eng[3] = {};

inline void serial_dma_poll_one(uint8_t i) {
  SerialDmaEngine &e = serial_dma_eng[i];
  if (!e.uart) return;

#if defined(ARDUINO_ARCH_RP2040)
  if (e.sending) {
    if (dma_channel_is_busy((uint)e.chan)) return;
    e.sending = false;
  }
  const uint16_t count = e.len[e.fill];
  if (count == 0) return;
  
  const uint8_t send = e.fill;
  e.len[send] = 0;
  e.fill ^= 1u;
  e.sending = true;
  
  dma_channel_set_write_addr((uint)e.chan, &uart_get_hw(e.uart)->dr, false);
  dma_channel_set_read_addr((uint)e.chan, e.buf[send], false);
  dma_channel_set_trans_count((uint)e.chan, count, true);

#elif defined(ARDUINO_ARCH_STM32)
  if (e.sending) {
    DMA_Stream_TypeDef *stream = reinterpret_cast<DMA_Stream_TypeDef *>(e.hdma.Instance);
    if ((stream->CR & DMA_SxCR_EN) != 0U) return;
    if (HAL_DMA_PollForTransfer(&e.hdma, HAL_DMA_FULL_TRANSFER, 0) != HAL_OK) {
      (void)HAL_DMA_Abort(&e.hdma);
    }
    e.sending = false;
  }
  const uint16_t count = e.len[e.fill];
  if (count == 0) return;
  
  const uint8_t send = e.fill;
  e.len[send] = 0;
  e.fill ^= 1u;
  
  const uint32_t n_clean = ((uint32_t)count + 31u) & ~31u;
  SCB_CleanDCache_by_Addr(reinterpret_cast<uint32_t *>(e.buf[send]), (int32_t)n_clean);
  __DSB();
  
  if (HAL_DMA_Start(&e.hdma, reinterpret_cast<uint32_t>(e.buf[send]),
                    reinterpret_cast<uint32_t>(&e.uart->TDR), count) != HAL_OK) {
    e.len[send] = count;
    e.fill = send;
    return;
  }
  e.sending = true;
#endif
}

struct UartDmaTx {
  uint8_t id;

  inline size_t write(const uint8_t *p, size_t n) {
    if (id >= 3 || n == 0 || n > SERIAL_DMA_BUF_SIZE || !p) return 0;
    SerialDmaEngine &e = serial_dma_eng[id];
    if (!e.uart) return 0;

  #if defined(ARDUINO_ARCH_RP2040)
    mutex_enter_blocking(&e.mutex);
  #endif

    serial_dma_poll_one(id);
    
    // Fast path: room in current fill buffer
    if ((size_t)e.len[e.fill] + n <= SERIAL_DMA_BUF_SIZE) {
      memcpy(e.buf[e.fill] + e.len[e.fill], p, n);
      e.len[e.fill] += n;
      serial_dma_poll_one(id);
  #if defined(ARDUINO_ARCH_RP2040)
      mutex_exit(&e.mutex);
  #endif
      return n;
    }

    // Second chance: poll once more to see if active DMA just completed
    serial_dma_poll_one(id);
    if ((size_t)e.len[e.fill] + n <= SERIAL_DMA_BUF_SIZE) {
      memcpy(e.buf[e.fill] + e.len[e.fill], p, n);
      e.len[e.fill] += n;
      serial_dma_poll_one(id);
  #if defined(ARDUINO_ARCH_RP2040)
      mutex_exit(&e.mutex);
  #endif
      return n;
    }

  #if defined(ARDUINO_ARCH_RP2040)
    mutex_exit(&e.mutex);
  #endif

    return 0;
  }

  inline size_t write_blocking(const uint8_t *p, size_t n, uint32_t timeout_ms = 2) {
    uint32_t start = millis();
    while ((millis() - start) <= timeout_ms) {
      size_t written = write(p, n);
      if (written == n) return n;
    }
    return 0;
  }

  inline size_t availableForWrite() {
    if (id >= 3) return 0;
    SerialDmaEngine &e = serial_dma_eng[id];
    if (!e.uart) return 0;

  #if defined(ARDUINO_ARCH_RP2040)
    mutex_enter_blocking(&e.mutex);
  #endif

    serial_dma_poll_one(id);
    size_t avail = (size_t)(SERIAL_DMA_BUF_SIZE - e.len[e.fill]);

  #if defined(ARDUINO_ARCH_RP2040)
    mutex_exit(&e.mutex);
  #endif

    return avail;
  }
};

#if defined(ARDUINO_ARCH_RP2040)
inline void serial_dma_init_rp2040(uint8_t id, uart_inst_t *uart) {
  SerialDmaEngine &e = serial_dma_eng[id];
  mutex_init(&e.mutex);
  e.uart = uart;
  e.chan = dma_claim_unused_channel(true);
  e.cfg = dma_channel_get_default_config((uint)e.chan);
  channel_config_set_transfer_data_size(&e.cfg, DMA_SIZE_8);
  channel_config_set_read_increment(&e.cfg, true);
  channel_config_set_write_increment(&e.cfg, false);
  channel_config_set_dreq(&e.cfg, uart_get_dreq(uart, true));
  dma_channel_configure((uint)e.chan, &e.cfg, &uart_get_hw(uart)->dr, nullptr, 0, false);
  e.len[0] = e.len[1] = e.fill = 0; e.sending = false;
}
#elif defined(ARDUINO_ARCH_STM32)
inline void serial_dma_init_stm32(uint8_t id, USART_TypeDef *uart, DMA_Stream_TypeDef *stream, uint32_t request) {
  SerialDmaEngine &e = serial_dma_eng[id];
  if (!uart || !stream) return;
  memset(&e.hdma, 0, sizeof(e.hdma));
  e.hdma.Instance = stream;
  e.hdma.Init.Request = request;
  e.hdma.Init.Direction = DMA_MEMORY_TO_PERIPH;
  e.hdma.Init.PeriphInc = DMA_PINC_DISABLE;
  e.hdma.Init.MemInc = DMA_MINC_ENABLE;
  e.hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  e.hdma.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  e.hdma.Init.Mode = DMA_NORMAL;
  e.hdma.Init.Priority = DMA_PRIORITY_LOW;
  if (HAL_DMA_Init(&e.hdma) != HAL_OK) return;
  
  CLEAR_BIT(uart->CR1, USART_CR1_TCIE);
#if defined(USART_CR1_TXEIE_TXFNFIE)
  CLEAR_BIT(uart->CR1, USART_CR1_TXEIE_TXFNFIE);
#elif defined(USART_CR1_TXEIE)
  CLEAR_BIT(uart->CR1, USART_CR1_TXEIE);
#endif
  SET_BIT(uart->CR3, USART_CR3_DMAT);
  e.uart = uart;
  e.len[0] = e.len[1] = e.fill = 0; e.sending = false;
}
#endif

#endif // SERIAL_DMA_TX_H