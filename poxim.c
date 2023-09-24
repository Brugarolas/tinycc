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


const u8 bit_at(u32 number, int nth_bit) {
  u32 bit;
  bit = (number >> nth_bit) & 1U;

  return (u8)bit;
}

void set_bit(u32 *number, u8 nth_bit, u8 choice) {
  if (choice) {
    *number |= (1UL << (nth_bit));
  } else {
    *number = *number & ~(1ul << (nth_bit));
  }
}

void fill_bits(u32 *number, u8 start, u8 end, u8 bit_choice) {
  for (i8 i = start; i >= end; i--) {
    set_bit(number, i, bit_choice);
  }
  return;
}

const u32 bits_at(u32 number, u32 start, u32 end) {
  u32 bits = (number >> end);
  if (!(start > 31)) {
    fill_bits(&bits, 31, start - end + 1, 0);
  }
  return bits;
}

const u32 extend_bit_at(u32 num, u8 bit_place) {
  u8 bit = bit_at(num, bit_place);
  fill_bits(&num, 31, bit_place, bit);
  return num;
}
