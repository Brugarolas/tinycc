#pragma once
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

#include <math.h>

// Macros to set and bits
#define bf_set_bit(bitfield, pos) ((bitfield) |= (1 << (pos)))
#define bf_clear_bit(bitfield, pos) ((bitfield) &= ~(1 << (pos)))
#define bf_toggle_bit(bitfield, pos) ((bitfield) ^= (1 << (pos)))
#define bf_read_bit(bitfield, pos) (((bitfield) >> (pos)) & 0x01)
// Force inline and no-inline macros
#if defined(__GNUC__) || defined(__GNUG__)
#define force_inline inline __attribute__((always_inline))
#define force_noinline __attribute__((noinline))
#elif defined(_MSC_VER)
#if _MSC_VER < 1300
#define force_inline
#else
#define force_inline __forceinline
#define force_noinline __declspec(noinline)
#endif
#endif
#define internal static
#define UNUSED 0x00
// I saw this typing in rust and casey's stream, i really like it
typedef float f32;
typedef double f64;

typedef __uint128_t u128;
typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef uint8_t b8;
typedef uint8_t byte;

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;
typedef size_t usize;
typedef ptrdiff_t isize;

// Why would a signed sizeof be more useful?
#ifndef size_of
#define size_of(x) (isize)(sizeof(x))
#endif

#ifndef count_of
#define count_of(x)                                                            \
  ((size_of(x) / size_of(0 [x])) / ((isize)(!(size_of(x) % size_of(0 [x])))))
#endif

#ifndef offset_of
#define offset_of(Type, element) ((isize) & (((Type *)0)->element))
#endif

#define timeit(msg, code)                                                      \
  do {                                                                         \
    clock_t start_time = clock();                                              \
    code;                                                                      \
    clock_t end_time = clock();                                                \
    printf(                                                                    \
        "\n[timeit] %s %lu us\n", msg,                                         \
        (unsigned long)(1000000 * (end_time - start_time) / CLOCKS_PER_SEC));  \
  } while (0)

internal void reg2cstr(char register_name[5], u8 id) {
  u8 len = 5;
  if (id < 26) {
    snprintf(register_name, len, "r%d", id);
  } else if (id == 26) {
    snprintf(register_name, len, "cr");
  } else if (id == 27) {
    snprintf(register_name, len, "ipc");
  } else if (id == 28) {
    snprintf(register_name, len, "ir");
  } else if (id == 29) {
    snprintf(register_name, len, "pc");
  } else if (id == 30) {
    snprintf(register_name, len, "sp");
  } else if (id == 31) {
    snprintf(register_name, len, "sr");
  }
}

typedef enum : u8 {
  //:> Format Type 'U'
  mov = 0b000000,
  movs = 0b000001,
  add = 0b000010,
  sub = 0b000011,

  // ==================================
  // Becaful Operation with the same code
  mul = 0b000100,  // FREE bits op.L: 0b000
  muls = 0b000100, // FREE bits op.L: 0b010
  div_ = 0b000100, // FREE bits op.L: 0b100
  divs = 0b000100, // FREE bits op.L: 0b110
  sll = 0b000100,  // FREE bits op.L: 0b001
  sla = 0b000100,  // FREE bits op.L: 0b011
  srl = 0b000100,  // FREE bits op.L: 0b101
  sra = 0b000100,  // FREE bits op.L: 0b111
                   //  ==================================
  cmp = 0B000101,
      and = 0B000110,
  or = 0B000111,
  not = 0b001000,
  xor = 0b001001,
  /*push pop*/
  push = 0b001010,
  pop = 0b001011,

  //:> Format Type 'F'
  addi = 0b010010,
  subi = 0b010011,
  muli = 0b010100,
  divi = 0b010101,
  remi = 0b010110,
  cmpi = 0b010111,
  /*Memory*/
  l8 = 0b011000,
  l16 = 0b011001,
  l32 = 0b011010,
  s8 = 0b011011,
  s16 = 0b011100,
  s32 = 0b011101,
  /*call return*/
  call = 0b011110,
  ret = 0b011111,

  //:> Format Type 'S'
  bae = 0b101010,
  bat = 0b101011,
  bbe = 0b101100,
  bbt = 0b101101,
  beq = 0b101110,
  bge = 0b101111,
  bgt = 0b110000,
  biv = 0b110001,
  ble = 0b110010,
  blt = 0b110011,
  bne = 0b110100,
  bni = 0b110101,
  bnz = 0b110110,
  bun = 0b110111,
  bzd = 0b111000,
  int_ = 0b111111,
  calli = 0b111001,

  reti = 0b100000,

  cbr = 0b100001, // equals last bit (0th bit) == 0
  sbr = 0b100001  // equals last bit (0th bit) == 1
} PoximOperation;

typedef struct {
  PoximOperation O;
  u32 Z, X, Y, L, I;
  char format;
} PoximInstruction;

void create_operands(PoximInstruction *this, u32 inst) {
  switch (this->format) {
  case 'U': {
    this->Z = (i32)((inst & 0b00000011111000000000000000000000) >> 21);
    this->X = (i32)((inst & 0b00000000000111110000000000000000) >> 16);
    this->Y = (i32)((inst & 0b00000000000000001111100000000000) >> 11);
    this->L = (i32)((inst & 0b00000000000000000000011111111111) >> 0);
    this->I = UNUSED;
    break;
  }
  case 'F': {
    this->Z = (i32)((inst & 0b00000011111000000000000000000000) >> 21);
    this->X = (i32)((inst & 0b00000000000111110000000000000000) >> 16);
    this->Y = UNUSED;
    this->L = UNUSED;
    this->I = (i32)((inst & 0b00000000000000001111111111111111) >> 0);
    break;
  }
  case 'S': {
    this->Z = UNUSED;
    this->X = UNUSED;
    this->Y = UNUSED;
    this->L = UNUSED;
    this->I = (i32)((inst & 0b00000011111111111111111111111111) >> 0);
    break;
  }
  default: {

    puts("[[WARNING]] Invalid Expression Format");
    break;
  }
  }
}

internal PoximInstruction parse_instruction(u32 inst) {
  PoximInstruction result = {0};
  result.O = ((inst & 0b11111100000000000000000000000000) >> 26);
  // If the first bit is 1.
  if ((inst >> 31) == 0b1) {
    result.format = 'S';
    // if the first is a 0, and the second bit is a 1
  } else if ((inst >> 30) == 0b1) {
    result.format = 'F';
  } else {
    result.format = 'U';
  }

  // Catching Outliers
  if (result.O == 0b100001) {
    result.format = 'F';
  }

  create_operands(&result, inst);

	return result;
}
