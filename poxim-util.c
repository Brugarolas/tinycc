#pragma once
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

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

char *str_upper(char *str) {
  char *start_of_str = str;
  while (*str) {
    *str = toupper(*str);
    str++;
  }
  return start_of_str;
}

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

internal void reg2str(char register_name[5], u8 id) {
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

// GLOBAL u32 registers[32] = {};
//
// // R0 must always be zero (0);
// u32& R0 = registers[0];
//
// // SR => Status Register
// u32& SR = registers[31];
//
// // SP => Stack Pointer
// u32& SP = registers[30];
//
// // PC => Aponta para as instru∩┐╜∩┐╜es < controla fluxo
// u32& PC = registers[29];
// // Pseudo 'PC' to print correctly
// u32  PPC = 0x0;
// u32  SSP = 0x0;
//
// // IR => Intruction Register < Armazena a intru∩┐╜∩┐╜o a ser carregada
// u32& IR = registers[28];
//
// u32& IPC = registers[27];
//
// u32& CR = registers[26];

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
    // if the first is a 0, and the second bit 1
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

internal void print_instruction(char *src, PoximInstruction inst) {
  char result[500] = {0};

  switch (inst.O) {
  case mov: {
    // mov r1,1193046           	R1=0x00123456
    char rz[5];
		char assembly_text[32];
    reg2str(rz, inst.Z);
    snprintf(assembly_text, sizeof(assembly_text), "mov %s,%u", rz, inst.I);

    snprintf(result, 256,
             "%-24s\t"
             "%s="
             "0x%08X\n",
             assembly_text, str_upper(rz), registers[inst.Z]);
    break;
  }
  case movs: {
    // movs r2,-1048576         	R2=0xFFF00000
    char rz[5];
    reg2str(rz, inst.Z);
    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "movs %s,%d", rz,
             registers[inst.Z]);

    snprintf(result, 256,
             "%-24s\t"
             "%s="
             "0x%08X\n",
             assembly_text, str_upper(rz), registers[inst.Z]);

    break;
  }
  case add: {
    char rz[5], rx[5], ry[5];
    reg2str(rz, inst.Z);
    reg2str(rx, inst.X);
    reg2str(ry, inst.Y);
    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "add %s,%s,%s", rz, rx, ry);
    snprintf(result, 256,
             "%-24s\t"
             // NOTE(Everton):IS this  supposed to be %4s ?
             "%s=%s+%s="
             "0x%08X,"
             "SR=0x%08X\n",
             assembly_text, str_upper(rz), str_upper(rx), str_upper(ry),
             registers[inst.Z], SR);

    break;
  }
  case sub: {
    // sub r4, r2, r3             	R4 = R2 - R3 = 0x7FFDCBAA, SR =
    // 0x00000008
    char rz[5], rx[5], ry[5];
    reg2str(rz, inst.Z);
    reg2str(rx, inst.X);
    reg2str(ry, inst.Y);
    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "sub %s,%s,%s", rz, rx, ry);
    snprintf(result, 256,
             "%-24s\t"
             "%s=%s-%s="
             "0x%08X,"
             "SR=0x%08X\n",
             assembly_text, str_upper(rz), str_upper(rx), str_upper(ry),
             registers[inst.Z], SR);

    break;
  }
  case       /*:>the first 3 bits of inst.L are:*/
      mul |  // mul	 = op.L: 0b000
      muls | // muls = op.L: 0b010
      DIV |  // div	 = op.L: 0b100
      divs | // divs = op.L: 0b110
      sll |  // sll	 = op.L: 0b001
      sla |  // sla	 = op.L: 0b011
      srl |  // srl	 = op.L: 0b101
      sra:   // sra  = op.L: 0b111
  {
    switch (GetBits(inst.L, 10, 8)) {
    case 0b000: // mul
    {
      // mul r0,r5,r4,r3
      // R0:R5=R4*R3=0x0000000023F4F31C,SR=0x00000008
      char rl[5], rz[5], rx[5], ry[5];
      u32 L_4_0 = (u32)GetBits(inst.L, 4, 0);
      reg2str(rl, L_4_0);
      reg2str(rz, inst.Z);
      reg2str(rx, inst.X);
      reg2str(ry, inst.Y);
      char assembly_text[32];
      snprintf(assembly_text, sizeof(assembly_text), "mul %s,%s,%s,%s", rl, rz,
               rx, ry);

      snprintf(result, sizeof(result),
               "%-24s\t"
               "%s:%s=%s*%s="
               "0x%08X%08X,"
               "SR=0x%08X\n",
               assembly_text, str_upper(rl), str_upper(rz), str_upper(rx),
               str_upper(ry), registers[L_4_0], registers[inst.Z], SR);

      ;
      break;
    }
    case 0b010: // muls
    {
      // muls r0,r7,r6,r5
      // R0:R7=R6*R5=0x0000000000000000,SR=0x00000040
      char rl[5], rz[5], rx[5], ry[5];
      u32 L_4_0 = (u32)GetBits(inst.L, 4, 0);
      reg2str(rl, L_4_0);
      reg2str(rz, inst.Z);
      reg2str(rx, inst.X);
      reg2str(ry, inst.Y);
      char assembly_text[32];
      snprintf(assembly_text, sizeof(assembly_text), "muls %s,%s,%s,%s", rl, rz,
               rx, ry);

      snprintf(result, sizeof(result),
               "%-24s\t"
               // NOTE(Everton):IS this  supposed to be %4s ?
               "%s:%s=%s*%s="
               "0x%08X%08X,"
               "SR=0x%08X\n",
               assembly_text, str_upper(rl), str_upper(rz), str_upper(rx),
               str_upper(ry), registers[L_4_0], registers[inst.Z], SR);

      break;
    }
    case 0b100: /*div */
    {
      // div r0,r9,r8,r7
      // R0=R8%R7=0x00000000,R9=R8/R7=0x00000000,SR=0x00000060
      char rz[5], rx[5], ry[5], rl[5];
      u32 r_index_L = GetBits(inst.L, 4, 0);
      reg2str(rz, inst.Z);
      reg2str(rx, inst.X);
      reg2str(ry, inst.Y);
      reg2str(rl, r_index_L);
      char assembly_text[32];

      snprintf(assembly_text, sizeof(assembly_text), "div %s,%s,%s,%s", rl, rz,
               rx, ry);

      char percent[3] = "%";
      snprintf(result, sizeof(result),
               "%-24s\t"
               "%s=%s"
               "%s"
               "%s="
               "0x%08X,"
               "%s=%s/%s="
               "0x%08X,"
               "SR=0x%08X\n",
               assembly_text, str_upper(rl), str_upper(rx), percent,
               str_upper(ry), ((u32)registers[r_index_L]), str_upper(rz),
               str_upper(rx), str_upper(ry), ((u32)registers[inst.Z]), SR);

      break;
    }
    case 0b110: /*divs*/
    {
      // divs r10,r11,r9,r8
      // R10=R9%R8=0x00000000,R11=R9/R8=0x00000000,SR=0x00000060
      char rz[5], rx[5], ry[5], rl[5];
      u32 r_index_L = GetBits(inst.L, 4, 0);
      reg2str(rz, inst.Z);
      reg2str(rx, inst.X);
      reg2str(ry, inst.Y);
      reg2str(rl, r_index_L);
      char assembly_text[32];

      snprintf(assembly_text, sizeof(assembly_text), "divs %s,%s,%s,%s", rl, rz,
               rx, ry);

      char percent[3] = "%";
      snprintf(result, sizeof(result),
               "%-24s\t"
               "%s=%s"
               "%s"
               "%s="
               "0x%08X,"
               "%s=%s/%s="
               "0x%08X,"
               "SR=0x%08X\n",
               assembly_text, str_upper(rl), str_upper(rx), percent,
               str_upper(ry), ((i32)registers[r_index_L]), str_upper(rz),
               str_upper(rx), str_upper(ry), ((i32)registers[inst.Z]), SR);

      break;
    }
    case 0b001: /*sll */
    {
      // sll r6,r5,r5,0
      // R6:R5=R6:R5<<1=0x0000000047E9E638,SR=0x00000008
      char rz[5], rx[5], ry[5];
      u32 L_4_0 = (u32)GetBits(inst.L, 4, 0);
      reg2str(rz, inst.Z);
      reg2str(rx, inst.X);
      reg2str(ry, inst.Y);
      char assembly_text[32];

      snprintf(assembly_text, sizeof(assembly_text), "sll %s,%s,%s,%d", rz, rx,
               ry, L_4_0);

      snprintf(result, sizeof(result),
               "%-24s\t"
               "%s:%s="
               "%s:%s<<%d="
               "0x%08X%08X,"
               "SR=0x%08X\n",
               assembly_text, str_upper(rz), str_upper(rx), str_upper(rz),
               str_upper(ry), L_4_0 + 1, registers[inst.Z], registers[inst.Y],
               SR);

      break;
    }
    case 0b011: /*sla */
    {
      // sla r0,r2,r2,10
      // R0:R2=R0:R2<<11=0x0000000080000000,SR=0x00000001
      char rz[5], rx[5], ry[5];
      u32 L_4_0 = (u32)GetBits(inst.L, 4, 0);
      reg2str(rz, inst.Z);
      reg2str(rx, inst.X);
      reg2str(ry, inst.Y);

      char assembly_text[32];
      snprintf(assembly_text, sizeof(assembly_text), "sla %s,%s,%s,%u", rz, rx,
               ry, L_4_0);

      snprintf(result, sizeof(result),
               "%-24s\t"
               "%s:%s="
               "%s:%s<<%ld="
               "0x%08X%08X,"
               "SR=0x%08X\n",

               assembly_text, str_upper(rz), str_upper(rx), str_upper(rz),
               str_upper(ry), (L_4_0 + 1l), registers[inst.Z],
               registers[inst.X], ((u32)SR));

      break;
    }
    case 0b101: /*srl */
    {
      // srl r10,r9,r9,2
      // R10:R9=R10:R9>>3=0x0000000000000000,SR=0x00000060
      char rz[5], rx[5], ry[5];
      u32 L_4_0 = (u32)GetBits(inst.L, 4, 0);
      reg2str(rz, inst.Z);
      reg2str(rx, inst.X);
      reg2str(ry, inst.Y);
      char assembly_text[32];

      snprintf(assembly_text, sizeof(assembly_text), "srl %s,%s,%s,%d", rz, rx,
               ry, L_4_0);

      snprintf(result, sizeof(result),
               "%-24s\t"
               "%s:%s="
               "%s:%s>>%d="
               "0x%08X%08X,"
               "SR=0x%08X\n",
               assembly_text, str_upper(rz), str_upper(rx), str_upper(rz),
               str_upper(ry), L_4_0 + 1, registers[inst.Z], registers[inst.X],
               SR);

      break;
    }
    case 0b111: /*sra */
    {
      // sra r12,r10,r10,3
      // R12:R10=R12:R10>>4=0x0000000000000000,SR=0x0000
      char rz[5], rx[5], ry[5];
      u32 L_4_0 = (u32)GetBits(inst.L, 4, 0);
      reg2str(rz, inst.Z);
      reg2str(rx, inst.X);
      reg2str(ry, inst.Y);
      char assembly_text[32];

      snprintf(assembly_text, sizeof(assembly_text), "sra %s,%s,%s,%d", rz, rx,
               ry, L_4_0);

      snprintf(result, sizeof(result),
               "%-24s\t"
               "%s:%s="
               "%s:%s>>%d="
               "0x%08X%08X,"
               "SR=0x%08X\n",
               assembly_text, str_upper(rz), str_upper(rx), str_upper(rz),
               str_upper(ry), (L_4_0 + 1), registers[inst.Z], registers[inst.X],
               SR);

      break;
    }
    }
    break;
  }

  case CMP: // NOTE(Everton): DONE
  {
    // cmp ir,pc                	SR=0x00000020
    char rx[5], ry[5];
    reg2str(rx, inst.X);
    reg2str(ry, inst.Y);

    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "cmp %s,%s", rx, ry);

    snprintf(result, sizeof(result),
             "%-24s\t"
             "SR=0x%08X\n",
             assembly_text, SR);

    break;
  }

  case AND: {
    // and r13,r1,r5            	R13=R1&R5=0x00002410,SR=0x00000020
    char rz[5], rx[5], ry[5];
    reg2str(rz, inst.Z);
    reg2str(rx, inst.X);
    reg2str(ry, inst.Y);

    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "and %s,%s,%s", rz, rx, ry);

    snprintf(result, sizeof(result),
             "%-24s\t"
             "%s=%s&%s="
             "0x%08X,"
             "SR=0x%08X\n",
             assembly_text, str_upper(rz), str_upper(rx), str_upper(ry),
             registers[inst.Z], SR);

    break;
  }
  case OR: {
    // or r14,r2,r6             	R14=R2|R6=0x80000000,SR=0x00000030
    char rz[5], rx[5], ry[5];
    reg2str(rz, inst.Z);
    reg2str(rx, inst.X);
    reg2str(ry, inst.Y);

    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "or %s,%s,%s", rz, rx, ry);

    snprintf(result, sizeof(result),
             "%-24s\t"
             "%s=%s|%s="
             "0x%08X,"
             "SR=0x%08X\n",
             assembly_text, str_upper(rz), str_upper(rx), str_upper(ry),
             registers[inst.Z], SR);
    break;
  }
  case NOT: {
    // not r15,r7               	R15=~R7=0xFFFFFFFF,SR=0x00000030
    char rz[5], rx[5];
    reg2str(rz, inst.Z);
    reg2str(rx, inst.X);

    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "not %s,%s", rz, rx);

    snprintf(result, sizeof(result),
             "%-24s\t"
             "%s=~%s="
             "0x%08X,"
             "SR=0x%08X\n",

             assembly_text, str_upper(rz), str_upper(rx), registers[inst.Z],
             SR);
    break;
  }
  case XOR: {
    // xor r16, r16, r8           	R16 = R16 ^ R8 = 0x00000000, SR =
    // 0x00000060
    char rz[5], rx[5], ry[5];
    reg2str(rz, inst.Z);
    reg2str(rx, inst.X);
    reg2str(ry, inst.Y);

    char assembly_text[32];

    snprintf(assembly_text, sizeof(assembly_text), "xor %s,%s,%s", rz, rx, ry);

    snprintf(result, sizeof(result),
             "%-24s\t"
             "%s=%s^%s="
             "0x%08X,"
             "SR=0x%08X\n",

             assembly_text, str_upper(rz), str_upper(rx), str_upper(ry),
             registers[inst.Z], SR);
    break;
  }
  //:> Format Type 'F'
  case addi: {
    // addi r1,r1,32771                R1 = R1 + 0x00008003 = 0xFFFF8003,SR
    // = 0x00000010
    char rz[5], rx[5];
    reg2str(rz, inst.Z);
    reg2str(rx, inst.X);

    u8 imediate_15_bit = GetBit(inst.I, 15);
    u32 imediate = (i32)inst.I;
    FillBits(imediate, 31, 16, imediate_15_bit);

    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "addi %s,%s,%d", rz, rx,
             (i32)imediate);

    snprintf(result, sizeof(result),
             "%-24s\t"
             "%s=%s+0x%08X="
             "0x%08X,"
             "SR=0x%08X\n",
             assembly_text, str_upper(rz), str_upper(rx), (i32)imediate,
             registers[inst.Z], SR);

    break;
  }

  case subi: {
    // subi r18, r18, -1          	R18 = R18 - 0xFFFFFFFF = 0x00000001, SR
    // = 0x00000020
    char rz[5], rx[5];
    reg2str(rz, inst.Z);
    reg2str(rx, inst.X);

    u8 imediate_15_bit = GetBit(inst.I, 15);
    u32 imediate = (i32)inst.I;
    FillBits(imediate, 31, 16, imediate_15_bit);

    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "subi %s,%s,%d", rz, rx,
             (i32)imediate);
    snprintf(result, sizeof(result),
             "%-24s\t"
             "%s=%s-0x%08X="
             "0x%08X,"
             "SR=0x%08X\n",
             assembly_text, str_upper(rz), str_upper(rx), (i32)imediate,
             registers[inst.Z], SR);
    break;
  }
  case muli: {
    // muli r19, r17, 2           	R19 = R17 * 0x00000002 = 0x00000002, SR
    // = 0x00000020
    char rz[5], rx[5];
    reg2str(rz, inst.Z);
    reg2str(rx, inst.X);

    u8 imediate_15_bit = GetBit(inst.I, 15);
    u32 imediate = (i32)inst.I;
    FillBits(imediate, 31, 16, imediate_15_bit);

    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "muli %s,%s,%d", rz, rx,
             (i32)imediate);
    snprintf(result, sizeof(result),
             "%-24s\t"
             "%s=%s*0x%08X="
             "0x%08X,"
             "SR=0x%08X\n",
             assembly_text, str_upper(rz), str_upper(rx), (i32)imediate,
             registers[inst.Z], SR);
    break;
  }
  case divi: {
    // divi r20, r19, 2           	R20 = R19 / 0x00000002 = 0x00000001, SR
    // = 0x00000000
    char rz[5], rx[5];
    reg2str(rz, inst.Z);
    reg2str(rx, inst.X);

    u8 imediate_15_bit = GetBit(inst.I, 15);
    u32 imediate = (i32)inst.I;
    FillBits(imediate, 31, 16, imediate_15_bit);

    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "divi %s,%s,%d", rz, rx,
             (i32)imediate);
    snprintf(result, sizeof(result),
             "%-24s\t"
             "%s=%s/0x%08X="
             "0x%08X,"
             "SR=0x%08X\n",
             assembly_text, str_upper(rz), str_upper(rx), (i32)imediate,
             registers[inst.Z], SR);
    break;
  }
  case remi: {
    // modi r20, r19, 2           	R20 = R19 mod  0x00000002 = 0x00000001,
    // SR = 0x00000000
    char rz[5], rx[5];
    reg2str(rz, inst.Z);
    reg2str(rx, inst.X);

    u8 imediate_15_bit = GetBit(inst.I, 15);
    u32 imediate = (i32)inst.I;
    FillBits(imediate, 31, 16, imediate_15_bit);

    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "modi %s,%s,%d", rz, rx,
             (i32)imediate);
    char percent[3] = "%";

    snprintf(result, sizeof(result),
             "%-24s\t"
             "%s=%s%s0x%08X="
             "0x%08X,"
             "SR=0x%08X\n",
             assembly_text, str_upper(rz), str_upper(rx), percent,
             (i32)imediate, registers[inst.Z], SR);

    break;
  }

  case cmpi: {
    // cmpi r20, r19, 2           	CMPI = R19 - 0x00000001 = 0x00000001, SR
    // = 0x00000000
    char rx[5];
    reg2str(rx, inst.X);

    ExtendBit(inst.I, 15);

    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "cmpi %s,%d", rx,
             (i32)inst.I);
    snprintf(result, sizeof(result),
             "%-24s\t"
             "SR=0x%08X\n",
             assembly_text, SR);
    break;
  }

  case l8: {
    ReadWriteStr(result, "l8", 8, inst);
    break;
  }

  case l16: {
    ReadWriteStr(result, "l16", 16, inst);
    break;
  }
  case l32: {
    ReadWriteStr(result, "l32", 32, inst);
    break;
  }
  case s8: {
    ReadWriteStr(result, "s8", 8, inst);
    break;
  }
  case s16: {
    ReadWriteStr(result, "s8", 16, inst);
    break;
  }
  case s32: {
    ReadWriteStr(result, "s8", 32, inst);
    break;
  }

  case bae: {
    BranchStr(result, "bae", inst.I);

    break;
  }
  case bat: {

    BranchStr(result, "bat", inst.I);

    break;
  }
  case bbe: {
    BranchStr(result, "bbe", inst.I);
    break;
  }
  case bbt: {

    BranchStr(result, "bbt", inst.I);

    break;
  }
  case beq: {

    BranchStr(result, "beq", inst.I);

    break;
  }
  case bge: {

    BranchStr(result, "bge", inst.I);

    break;
  }
  case bgt: {

    BranchStr(result, "bgt", inst.I);

    break;
  }
  case biv: {

    BranchStr(result, "biv", inst.I);

    break;
  }
  case ble: {

    BranchStr(result, "ble", inst.I);

    break;
  }
  case blt: {

    BranchStr(result, "blt", inst.I);

    break;
  }
  case bne: {

    BranchStr(result, "bne", inst.I);

    break;
  }
  case bni: {

    BranchStr(result, "bni", inst.I);

    break;
  }
  case bnz: {

    BranchStr(result, "bnz", inst.I);

    break;
  }
  case bun: {

    BranchStr(result, "bun", inst.I);
    break;
  }
  case bzd: {
    BranchStr(result, "bzd", inst.I);

    break;
  }
  case INT: {
    // 0x000000D4:	int 0 CR=0x00000000,PC=0x00000000
    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "%s %d", "int", inst.I);

    snprintf(result, 256,
             "%-24s\t"
             "CR=0x%08X,PC=0x%08X\n",
             assembly_text, CR, PC);
    break;
  }
  //::>> FLOW CONTROL:
  case call: {
    ExtendBit(inst.I, 15);
    char rx[5];
    reg2str(rx, inst.X);

    // call[r0 + 16]             	PC=0x00000040,MEM[0x00007FFC]=0x000002C4
    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "%s [%s+%d]", "call", rx,
             (inst.I));

    // SP + 4 because  we're supposed to print when  the operation is taken
    // place, but considering we're printing after, we need to "undo"
    // certain operations;
    snprintf(result, 256,
             "%-24s\t"
             "PC=0x%08X,MEM[0x%08X]=0x%08X\n",
             assembly_text, PC, SP + 4,
             SwitchEndianess(MEM.RAM32[(SP + 4) / 4]));
    break;
  }
  case calli: {
    // call[r0 + 16]             	PC=0x00000040,MEM[0x00007FFC]=0x000002C4
    ExtendBit(inst.I, 25);
    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "%s %d", "call", inst.I);
    // SP + 4 because  we're supposed to print when  the operation is taken
    // place, but considering we're printing after, we need to "undo"
    // certain operations;
    snprintf(result, 256,
             "%-24s\t"
             "PC=0x%08X,MEM[0x%08X]=0x%08X\n",
             assembly_text, PC, SP + 4,
             SwitchEndianess(MEM.RAM32[(SP + 4) / 4]));
    break;
  }
  case ret: {
    // ret                      	PC=MEM[0x00007FFC]=0x000002C4
    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "%s", "ret");

    snprintf(result, 256,
             "%-24s\t"
             "PC=MEM[0x%08X]=0x%08X\n",
             assembly_text, SP, SwitchEndianess(MEM.RAM32[(SP) / 4]));
    break;
  }
  case push: {
    PushPopStr(result, "push", inst);
    break;
  }
  case pop: {
    PushPopStr(result, "pop", inst);
    break;
  }

  case sbr: // cbr
  {
    // 0x00000034:	sbr sr[1]                	SR=0x00000002
    char instruction[5];
    if (GetBit(inst.I, 0) == 0) {
      sprintf(instruction, "%s", "cbr");
    } else {
      sprintf(instruction, "%s", "sbr");
    }

    // mov r1,1193046           	R1=0x00123456
    char rz[5];
    reg2str(rz, inst.Z);
    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "%s %s[%u]", instruction, rz,
             inst.X);

    snprintf(result, 256,
             "%-24s\t"
             "%s="
             "0x%08X\n",
             assembly_text, str_upper(rz), registers[inst.Z]);
    break;
  }
  case reti: {
    // reti
    // IPC=MEM[0x????????]=0x????????,CR=MEM[0x????????]=0x????????,PC=MEM[0x????????]=0x????????
    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "%s", "reti");

    snprintf(result, 256,
             "%-24s\t"
             "IPC=MEM[0x%08X]=0x%08X,"
             "CR=MEM[0x%08X]=0x%08X,"
             "PC=MEM[0x%08X]=0x%08X\n",
             assembly_text, SP - 2 * 4,
             SwitchEndianess(MEM.RAM32[(SP - 2 * 4) / 4]), SP - 1 * 4,
             SwitchEndianess(MEM.RAM32[(SP - 1 * 4) / 4]), SP - 0 * 4,
             SwitchEndianess(MEM.RAM32[(SP - 0 * 4) / 4]));
    break;
  }

  default: {
    return;
  }
  }
  printf(result);
  assembly_out << result;

  if (int_flag != -1) {
    if (int_flag == 0) {
      printf("[SOFTWARE INTERRUPTION]\n");
      assembly_out << "[SOFTWARE INTERRUPTION]\n";
    }

    if (int_flag > 0) {
      printf("[HARDWARE INTERRUPTION %d]\n", int_flag);
      assembly_out << "[HARDWARE INTERRUPTION " << std::to_string(int_flag)
                   << "]\n";
    }

    int_flag = -1;
  }
}
