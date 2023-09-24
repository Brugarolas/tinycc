#include "poxim.h"

const u32 swap_endianness32(u32 num) {
  return ((num >> 24) & 0xFF) | ((num >> 8) & 0xFF00) |
         ((num << 8) & 0xFF0000) | ((num << 24) & 0xFF000000);
}

const u64 swap_endianness64(u64 num) {
  return ((num & 0x00000000000000FF) << 56) |
         ((num & 0x000000000000FF00) << 40) |
         ((num & 0x0000000000FF0000) << 24) |
         ((num & 0x00000000FF000000) << 8 ) |
				 ((num & 0x000000FF00000000) >> 8 ) |
         ((num & 0x0000FF0000000000) >> 24) |
         ((num & 0x00FF000000000000) >> 40) |
         ((num & 0xFF00000000000000) >> 56);
}

const u16 swap_endianness16(u16 num) {
  return ((num >> 8) & 0xFF) | ((num << 8) & 0xFF00);
}

