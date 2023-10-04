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


const u32 swap_endianness32(u32 num);
const u64 swap_endianness64(u64 num);
const u16 swap_endianness16(u16 num);

const u8 bit_at(u32 number, int nth_bit);
const u32 bits_at(u32 number, u32 start, u32 end);
const u32 extend_bit_at(u32 num, u8 bit_place);
void set_bit(u32 *number, u8 nth_bit, u8 choice);
void fill_bits(u32 *number, u8 start, u8 end, u8 bit_choice);

/* pretty names for the registers */
enum { r0 = 0, r1, r2, r3, r4, rf, bp, bp2, rt, rt2, rret, rdiv, rmod, rmul, sp = 30 };

typedef enum  {
  //:> Format Type 'U'
  opcode_mov = 0b000000,
  opcode_movs = 0b000001,
  opcode_add = 0b000010,
  opcode_sub = 0b000011,
  // ==================================
  // Becaful Operation with the same code
  opcode_mul = 0b000100,  // FREE bits op.L: 0b000
  opcode_muls = 0b000100, // FREE bits op.L: 0b010
  opcode_div_ = 0b000100, // FREE bits op.L: 0b100
  opcode_divs = 0b000100, // FREE bits op.L: 0b110
  opcode_sll = 0b000100,  // FREE bits op.L: 0b001
  opcode_sla = 0b000100,  // FREE bits op.L: 0b011
  opcode_srl = 0b000100,  // FREE bits op.L: 0b101
  opcode_sra = 0b000100,  // FREE bits op.L: 0b111
  //  ==================================
  opcode_cmp = 0B000101,
  opcode_and = 0B000110,
  opcode_or = 0B000111,
  opcode_not = 0b001000,
  opcode_xor = 0b001001,
  /*push pop*/
  opcode_push = 0b001010,
  opcode_pop = 0b001011,
  //:> Format Type 'F'
  opcode_addi = 0b010010,
  opcode_subi = 0b010011,
  opcode_muli = 0b010100,
  opcode_divi = 0b010101,
  opcode_modi = 0b010110,
  opcode_remi = 0b010110,
  opcode_cmpi = 0b010111,
  /*Memory*/
  opcode_l8 = 0b011000,
  opcode_l16 = 0b011001,
  opcode_l32 = 0b011010,
  opcode_s8 = 0b011011,
  opcode_s16 = 0b011100,
  opcode_s32 = 0b011101,
  /*call return*/
  opcode_call = 0b011110,
  opcode_ret = 0b011111,
//:> Format Type 'S'
  opcode_bae    = 0b101010,
  opcode_bat    = 0b101011,
  opcode_bbe    = 0b101100,
  opcode_bbt    = 0b101101,
  opcode_beq    = 0b101110,
  opcode_bge    = 0b101111,
  opcode_bgt    = 0b110000,
  opcode_biv    = 0b110001,
  opcode_ble    = 0b110010,
  opcode_blt    = 0b110011,
  opcode_bne    = 0b110100,
  opcode_bni    = 0b110101,
  opcode_bnz    = 0b110110,
  opcode_bun    = 0b110111,
  opcode_bzd    = 0b111000,
  opcode_int_   = 0b111111,
  opcode_calli  = 0b111001,
  opcode_reti   = 0b100000,
  opcode_cbr    = 0b100001, // equals last bit (0th bit) == 0
  opcode_sbr    = 0b100001  // equals last bit (0th bit) == 1
} PoximOperation;

