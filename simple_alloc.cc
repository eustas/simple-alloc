/*
 * Copyright (c) 2020 Eugene Kliuchnikov
 * Distributed under MIT license.
 * See file LICENSE for detail or copy at https://opensource.org/licenses/MIT
 */

#include <cstddef>
#include <cstdint>

#if defined(__wasm32__)

extern "C" {

namespace {

static uint32_t freeHeads[32] = {0};
static uint32_t freePages = 0;
static uint32_t freeStart = 0;

}  // namespace

void* malloc(size_t size) {
  size_t storageSize = (size < 65535) ? size : (size + 4);
  uint32_t bucket = (__builtin_clz(storageSize) ^ 31) + 1;
  if (bucket < 3) bucket = 3;
  if (freeHeads[bucket] == 0) {
    uint32_t wantPages = (bucket <= 16) ? 1 : (1 << (bucket - 16));
    if (freePages < wantPages) {
      uint32_t currentPages = __builtin_wasm_memory_size(0);
      if (freePages == 0) freeStart = currentPages << 16;
      uint32_t plusPages = currentPages;
      if (plusPages < 32) plusPages = 32;
      if (plusPages > 256) plusPages = 256;
      if (plusPages < wantPages - freePages) plusPages = wantPages - freePages;
      if (__builtin_wasm_memory_grow(0, plusPages) == -1) __builtin_trap();
      freePages += plusPages;
    }
    if (bucket <= 16) {
      size_t step = 1 << bucket;
      size_t count = 1 << (16 - bucket);
      size_t address = freeStart;
      for (size_t i = 0; i < count - 1; ++i) {
        reinterpret_cast<uint32_t*>(address)[0] = address + step;
        address += step;
      }
      freeHeads[bucket] = freeStart;
      reinterpret_cast<uint32_t*>(freeStart + (count - 1) * step)[0] = 0;
      reinterpret_cast<uint8_t*>(freeStart | 0xFFFF)[0] = bucket;
    } else {
      freeHeads[bucket] = freeStart + 4;
      reinterpret_cast<uint8_t*>(freeStart + 3)[0] = bucket;
    }
    freeStart += wantPages << 16;
    freePages -= wantPages;
  }
  uint32_t result = freeHeads[bucket];
  freeHeads[bucket] = reinterpret_cast<uint32_t*>(result)[0];
  return reinterpret_cast<void*>(result);
}

void free(void* ptr) {
  uint32_t p = reinterpret_cast<uint32_t>(ptr);
  size_t bucket;
  if ((p & 0xFFFF) != 4) {
    bucket = reinterpret_cast<uint8_t*>(p | 0xFFFF)[0];
  } else {
    bucket = reinterpret_cast<uint8_t*>(p - 1)[0];
  }
  reinterpret_cast<uint32_t*>(p)[0] = freeHeads[bucket];
  freeHeads[bucket] = p;
}

}  // extern "C"

#endif  // __wasm32__