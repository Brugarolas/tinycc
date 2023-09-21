#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define _DEBUG_POXIM
#define _DEBUG_OUT

#ifdef _DEBUG_POXIM
#define printf(x, ...)
#define puts(x)
#endif

#define UNUSED 0x00
#define GLOBAL
#define MEM_SIZE 1024
#pragma warning(disable : 4996)
int32_t INSTRUCTION_COUNTER = 0;

typedef volatile uint32_t vu32;
typedef float f32;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;

/*===========================Structures=======================*/
GLOBAL enum OP : u8 {
  //:> Format Type 'U'
  mov = 0b000000,
  movs = 0b000001,
  add = 0b000010,
  sub = 0b000011,

  // ==================================
  // Becaful Operation with the same code
  mul = 0b000100,  // FREE bits op.L: 0b000
  muls = 0b000100, // FREE bits op.L: 0b010
  DIV = 0b000100,  // FREE bits op.L: 0b100
  divs = 0b000100, // FREE bits op.L: 0b110
  sll = 0b000100,  // FREE bits op.L: 0b001
  sla = 0b000100,  // FREE bits op.L: 0b011
  srl = 0b000100,  // FREE bits op.L: 0b101
  sra = 0b000100,  // FREE bits op.L: 0b111
                   //  ==================================
  CMP = 0b000101,
  AND = 0b000110,
  OR = 0b000111,
  NOT = 0b001000,
  XOR = 0b001001,
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
  INT = 0b111111,
  calli = 0b111001,

  reti = 0b100000,

  cbr = 0b100001, // equals last bit (0th bit) == 0
  sbr = 0b100001  // equals last bit (0th bit) == 1
};

GLOBAL struct Operands {
  u32 Z, X, Y, I, L;
};

GLOBAL struct Expression {
  char format;
  u32 expression;    // Whole expresion <OP> <Operands>;
  OP operation;      // Just the operand <OP>
  Operands operands; // Cluster of all operands in the expression <Operands>;

  Expression(u32 inst) {
    this->expression = inst;
    this->operation =
        (OP)((this->expression & 0b11111100000000000000000000000000) >> 26);

    if ((expression >> 31) == 0b1) // If the first bit is 1.
    {
      this->format = 'S';
    } else if ((expression >> 30) ==
               0b1) // if the first is a 0, and the second bit is a 1
    {
      this->format = 'F';
    } else {
      this->format = 'U';
    }

    // Catching Outliers
    if (operation == 0b100001) {
      this->format = 'F';
    }

    this->operands = _CreateOperands();
  }

private:
  Operands _CreateOperands() {
    Operands op{};
    switch (this->format) {
    case 'U': {
      op.Z =
          (i32)((this->expression & 0b00000011111000000000000000000000) >> 21);
      op.X =
          (i32)((this->expression & 0b00000000000111110000000000000000) >> 16);
      op.Y =
          (i32)((this->expression & 0b00000000000000001111100000000000) >> 11);
      op.L =
          (i32)((this->expression & 0b00000000000000000000011111111111) >> 0);
      op.I = UNUSED;
      break;
    }
    case 'F': {
      op.Z =
          (i32)((this->expression & 0b00000011111000000000000000000000) >> 21);
      op.X =
          (i32)((this->expression & 0b00000000000111110000000000000000) >> 16);
      op.Y = UNUSED;
      op.L = UNUSED;
      op.I =
          (i32)((this->expression & 0b00000000000000001111111111111111) >> 0);
      break;
    }
    case 'S': {
      op.Z = UNUSED;
      op.X = UNUSED;
      op.Y = UNUSED;
      op.L = UNUSED;
      op.I =
          (i32)((this->expression & 0b00000011111111111111111111111111) >> 0);
      break;
    }
    default:
      puts("[[WARNING]] Invalid Expression Format");
      break;
    }
    return op;
  }
};
/*=============================================================*/

/*===========================Funcitons
 * Start==============================================================================================*/

/*************************** Bit Manipulation Functions
 * ***********************************************************************************
 * These functions are zero indexed, meaning they consider 'end' as the last
 * bit, the bit in position '0' [right-to-left] and 'start' as the first bit,
 * ex: unsinged int (32bits) has the start bit a position '31' ex: 0b011111, the
 * 'end' is the position '0' bit, which is 1 in this case, again counting from
 * right-to-left in increasing order. if you want the 'start' to start at the
 * first bit , then you wanna choose 'start' := 5, because the bit you wanna set
 * is the 6th bit [right-to-left index] (the bit in position '5')
 */
void SetBit(u32 &number, u8 nth_bit, bool choice);
void SetBit(u64 &number, u8 nth_bit, bool choice);

u8 GetBit(const u32 &number, int nth_bit);
u8 GetBit(u64 &number, int nth_bit);

u32 GetBits(const u32 &number, u32 start, u32 end);
u32 GetBits(const u64 &number, u32 start, u32 end);

void FillBits(u32 &number, u8 start, u8 end, bool bit_choice);
void FillBits(u64 &number, u8 start, u8 end, bool bit_choice);

void ExtendBit(u32 &num, u8 bit_place);

u64 ConcatBits(const u32 &start, const u32 &end);
u32 ConcatBits(u8 first, u8 second, u8 third, u8 fourth);
u16 ConcatBits(u8 start, u8 end);

u32 SwitchEndianess(const u32 &number);

/****************************************************************************************************************************************/

void ExecuteCode(u32 instructions[]);
void ProcessExpression(Expression single_instruction);
// void StoreHexInstructions(const char* filename, std::vector<u32>&
// expression);
void StoreHexInstructions(const char *filename, u32 instructions[]);
void Branch(u32 imediate, bool is_signed);
u8 &AccessMemory(u32 index);

void PrintTerminal();

/****************************************************************************************************************************************/
bool isDevice(u32 index);
u32 &AccessDevice32(u32 index, OP op);
u16 &AccessDevice16(u32 index, OP op);
u8 &AccessDevice8(u32 index, OP op);
void Interrupt(const char type[26], Operands op = {});
void ISR();

void DefaultRoutines();
/****************************************************************************************************************************************/

void RegStr(char register_name[5], u8 id);
void ReadWriteStr(char result[256], const char *instruction, u8 bits_count,
                  Operands op);
void BranchStr(char dest[256], const char *instruction_name, u32 imediate);
void PushPopStr(char dest[500], const char *instruction_name, Operands op);
char *UpperStr(char *);
u32 HexToBin(const char *hex);
std::string ToHex(u32 hex);

void PrintOperation(OP op, Operands operand);

namespace inst {
void SetZN(bool value);
void SetZD(bool value);
void SetSN(bool value);
void SetOV(bool value);
void SetIV(bool value);
void SetCY(bool value);
void SetIE(bool value);

u8 GetZN();
u8 GetZD();
u8 GetSN();
u8 GetOV();
u8 GetIV();
u8 GetCY();
u8 GetIE();

//:>>/*===========================Format 'U' =======================*/
void mov(const Operands op);
void movs(const Operands op);
void add(const Operands op);
void sub(const Operands op);
void mul(const Operands op);
void muls(const Operands op);
void div(const Operands op);
void divs(const Operands op);
void sll(const Operands op);
void sla(const Operands op);
void srl(const Operands op);
void sra(const Operands op);

void CMP(const Operands op);
/*==========================BIT WISE OPERATIONS========================*/
void AND(const Operands op);
void OR(const Operands op);
void NOT(const Operands op);
void XOR(const Operands op);
/*====================================================================*/
void push(const Operands op);
void pop(const Operands op);

//:>>/*===========================Format 'F' =======================*/
void addi(Operands op);
void subi(Operands op);
void muli(Operands op);
void divi(Operands op);
void remi(Operands op);
void CMPI(Operands op);

void l8(Operands op);
void l16(Operands op);
void l32(Operands op);
void s8(Operands op);
void s16(Operands op);
void s32(Operands op);

void call(Operands op);
void ret(Operands op);

//:>>/*===========================Format 'S' =======================*/
void bae(Operands op);
void bat(Operands op);
void bbe(Operands op);
void bbt(Operands op);
void beq(Operands op);
void bge(Operands op);
void bgt(Operands op);
void biv(Operands op);
void ble(Operands op);
void blt(Operands op);
void bne(Operands op);
void bni(Operands op);
void bnz(Operands op);
void bun(Operands op);
void bzd(Operands op);
void INT(Operands op);
void calli(Operands op);

void reti(Operands op);
void sbr(Operands op);
void cbr(Operands op);

} // namespace inst
/*===========================Funcitons
 * End==============================================================================================*/

/*===========================GLOBAL VARIABLES=======================*/

GLOBAL bool running = true;
GLOBAL i8 int_flag = -1;
/*===========================Extern Devices=======================*/

class Terminal {
  std::queue<u8> storage{};

  u8 current_index = 0;
  bool used = false;

public:
  u32 TERMINAL_ADRESS = (u32)0x88888888;
  union {
    u32 IO32 = 0;
    u16 IO16[2];
    u8 IO8[4];
  };

  void AddIn() {
    this->used = true;
    storage.push(IO8[3]);
  }

  bool HasNext() {
    return (storage.size() > 0);
    if (current_index > (storage.size() - 1)) {
      return false;
    }
    if (current_index < 0) {
      return false;
    } else {
      return true;
    }
  }

  char Next() {
    // char next_char = storage[this->current_index];
    char next_char = storage.front();
    storage.pop();
    current_index++;
    return next_char;
  }
  bool Used() { return this->used; }
};

struct WatchDog {
  const u32 WATCHDOG_ADRESS = (u32)0x80808080;
  union {
    u32 WD32 = 0x0;
    u16 WD16[2];
    u8 WD8[4];
  };
};

struct FloatDevice {
  // Floating Operations : u5 actually
  enum class FOP : u8 {
    nop = 0b00000,   // NO operation
    add = 0b00001,   // Z = X + Y
    sub = 0b00010,   // Z = X - Y
    mul = 0b00011,   // Z = X * Y
    div = 0b00100,   // Z = X / Y
    atrXZ = 0b00101, // X = Z
    atrYZ = 0b00110, // Y = Z
    ceil = 0b00111,  // Teto(Z)
    floor = 0b01000, // Piso(Z)
    round = 0b01001, // Round(|Z|)

  };

  bool should_operate = false;
  i32 wait_cycles = -1;

  bool Y_overwritten = false;
  bool X_overwritten = false;

private:
  bool Xisfloat = false;
  bool Yisfloat = false;
  bool Zisfloat = false;

public:
  union {
    u32 FPU32[4];
    u16 FPU16[4 * 2];
    u8 FPU8[4 * 4];

    /*0x80808880, 0x80808884, 0x80808888*/ // 0x80808880
    // U is for holding OP and RO
    // Z is for the Result
    struct {
      u32 X, Y;
      f32 Z;
      u32 U;
    };
  };

  u32 code = 0x01EEE754;
  u8 OP;
  u8 ST;

  void ClearOP() {
    this->FPU8[0x0000000f] = 0;
    SetRO(ST);
  }

  void SetRO(u8 bit) {
    ST = bit;
    SetBit((u32 &)this->FPU8[0x0000000f], 5, bit);
    // SetBit(this->U, 5, bit);
  }

  void MakeInteger(std::string operand) {
    if (operand == "X") {
      this->Xisfloat = false;
      return;
    } else if (operand == "Y") {
      this->Yisfloat = false;
      return;
    }
  }
  u32 CicleTime() {
    FOP operation = (FOP)GetBits((u32)this->FPU8[0x0000000f], 7, 0);
    // FOP operation = this->previous_operator;
    switch (operation) {

    case FloatDevice::FOP::nop:
    case FloatDevice::FOP::atrXZ:
    case FloatDevice::FOP::atrYZ:
    case FloatDevice::FOP::ceil:
    case FloatDevice::FOP::floor:
    case FloatDevice::FOP::round: {
      return 1;
    }
    case FloatDevice::FOP::add:
    case FloatDevice::FOP::sub:
    case FloatDevice::FOP::mul:
    case FloatDevice::FOP::div: {
      break;
    }
    default: {
      return 1;
    }
    }

    i32 X_time, tempX;
    i32 Y_time, tempY;

    tempX = SwitchEndianess(this->X);
    tempY = SwitchEndianess(this->Y);

    if (Xisfloat) {
      X_time = GetBits(*(u32 *)(&this->X), 30, 23);

    } else {
      // Do I need to Switch Endianess here too?
      f32 X_as_float = (f32)tempX;
      X_time = GetBits(*(u32 *)&X_as_float, 30, 23);
    }

    if (Yisfloat) {
      Y_time = GetBits(*(u32 *)(&this->Y), 30, 23);
    } else {
      f32 Y_as_float = (f32)tempY;
      Y_time = GetBits(*(u32 *)&Y_as_float, 30, 23);
    }

    X_time = X_time - 127;
    Y_time = Y_time - 127;

    u32 base_time = abs(X_time - Y_time);
    return (base_time + 1);
  }

  bool ReadyToOperate() {
    if (should_operate == true) {
      if ((wait_cycles == 0)) {
        return true;
      } else if (wait_cycles > 0) {
        wait_cycles--;
        return false;
      } else {
        wait_cycles = -1;
        return false;
      }
    } else {
      wait_cycles = -1;
      return false;
    }
  }

  void Operate() {
    SetRO(0);

    FOP operation = (FOP)GetBits((u32)this->FPU8[0x0000000f], 4, 0);
    f32 X, Y, Z, result;
    u32 tempX = SwitchEndianess(this->X);
    u32 tempY = SwitchEndianess(this->Y);
    Z = this->Z;

    if (Xisfloat)
      X = *(f32 *)(&(this->X));
    else
      X = (f32)tempY;
    if (Yisfloat)
      Y = *(f32 *)(&(this->Y));
    else
      Y = (f32)tempY;

    if (wait_cycles == 0) {
      switch (operation) {
      case FloatDevice::FOP::nop: {
        break;
      }
      case FloatDevice::FOP::add: {

        this->Z = X + Y;
        Interrupt("op_var");
        break;
      }
      case FloatDevice::FOP::sub: {
        this->Z = X - Y;
        Interrupt("op_var");
        break;
      }
      case FloatDevice::FOP::mul: {
        this->Z = X * Y;
        Interrupt("op_var");
        break;
      }
      case FloatDevice::FOP::div: {
        if (Y == 0) {
          SetRO(1);
          void ClearOP();
          int_flag = 2;
          Interrupt("op_error");
          break;
        } else {
          this->Z = X / Y;
          Interrupt("op_var");
          break;
        }
      }

      case FloatDevice::FOP::atrXZ: {
        this->X = *(u32 *)&Z;
        Xisfloat = true;
        Interrupt("op_const");

        break;
      }
      case FloatDevice::FOP::atrYZ: {
        this->Y = *(u32 *)&Z;
        Yisfloat = true;
        Interrupt("op_const");

        break;
      }
      case FloatDevice::FOP::ceil: {
        this->Z = (u32)ceil(Z);
        Interrupt("op_const");
        break;
      }
      case FloatDevice::FOP::floor: {
        this->Z = (u32)floor(Z);
        Interrupt("op_const");
        break;
      }

      case FloatDevice::FOP::round: {
        this->Z = (u32)round(Z);
        Interrupt("op_const");
        break;
      }

      default: {
        SetRO(1);
        int_flag = 2;
        Interrupt("op_error");
        break;
      }
      }
    }
    this->should_operate = false;
    wait_cycles = -1;
  }
};

GLOBAL WatchDog W{};
GLOBAL Terminal T{};
GLOBAL FloatDevice F{};

GLOBAL u32 DEFAULT_DEVICE = (u32)0x0;

/*==========================Registers & RAM========================*/

typedef struct {
  union {
    GLOBAL u32 instructions[32 * MEM_SIZE / 4];
    GLOBAL u8 RAM[32 * MEM_SIZE];
    GLOBAL u16 RAM16[32 * MEM_SIZE / 2];
    GLOBAL u32 RAM32[32 * MEM_SIZE / 4];
  };
} Memory;

GLOBAL Memory MEM{};

GLOBAL u32 registers[32] = {};

// R0 must always be zero (0);
u32 &R0 = registers[0];

// SR => Status Register
u32 &SR = registers[31];

// SP => Stack Pointer
u32 &SP = registers[30];

// PC => Aponta para as instru��es < controla fluxo
u32 &PC = registers[29];
// Pseudo 'PC' to print correctly
u32 PPC = 0x0;
u32 SSP = 0x0;

// IR => Intruction Register < Armazena a intru��o a ser carregada
u32 &IR = registers[28];

u32 &IPC = registers[27];

u32 &CR = registers[26];

/*========================================================================*/
/*===========================IO GLOBALS=======================*/
GLOBAL std::ofstream assembly_out;

/*_______________________________________________________________________________________________________________________*/
/*===========================Start of
 * Program============================================================================*/

int main(int argc, char **argv) {
  const char *filename_in = argv[1];
  const char *filename_out = argv[2];
  if (filename_in == nullptr) {
    filename_in = ".in";
    filename_out = ".out";
  }
  StoreHexInstructions(filename_in, MEM.instructions);
  assembly_out.open(filename_out, std::ios::out);

  ExecuteCode(MEM.instructions);

  // END OF PROGRAM:
  // Closing IO
  assembly_out.close();
  return 0;
}
/*_______________________________________________________________________________________________________________________*/
/*=======================================================================================================================*/

void ExecuteCode(u32 instructions[]) {
  puts("[START OF SIMULATION]");
  assembly_out << "[START OF SIMULATION]\n";
  SP = 0x00007ffc;
  while (running) {
    // memory is indexed in 32 bits or 4bytes
    // we just need to devide by 4 to get our index,
    // in the instructions dynamic array
    INSTRUCTION_COUNTER++;

    u32 intruction_index = PC / 4ul;

    if (PC > MEM_SIZE * 4)
      break;
    if (intruction_index > 4096)
      break;

    // Seeting  the Next instruction
    IR = SwitchEndianess(instructions[intruction_index]);
    /*
    if (INSTRUCTION_COUNTER >= 15490)
    {
            IR = 0b111111'00000'00000'00000'00000'000000;
    }
    */
    Expression expression{IR};

    /*	char intruction_address[13] = { 0 };
            snprintf(intruction_address, sizeof(intruction_address),
       "0x%08X:\t", PC); assembly_out << intruction_address;
            printf(intruction_address);*/

    // Processing a isntruction
    ProcessExpression(expression);
    // Reset R0

    DefaultRoutines();
    bool interruption_occurred = (int_flag >= 1);

    u32 tempPC, tempSP;

    if (interruption_occurred) {
      tempPC = PC;
      tempSP = SP;
      PC = PPC;
      SP = SSP;
    }

    R0 = 0;
    PC += 4ul;
    // Print to File
    PrintOperation(expression.operation, expression.operands);

    if (interruption_occurred) {
      SP = tempSP;
      PC = tempPC + 4ul;
    }
  }

  if (T.Used()) {
    PrintTerminal();
  }

  puts("[END OF SIMULATION]");
  assembly_out << "[END OF SIMULATION]";
}

void PrintTerminal() {
  printf("[TERMINAL]\n");
  assembly_out << "[TERMINAL]\n";

  T.AddIn();
  while (T.HasNext()) {
    char c = T.Next();
    // terminal_string += std::to_string(c);
    if (c) {
      printf("%c", c);
      assembly_out << c;
    }
  }
  puts("");
  assembly_out << "\n";
}

void ProcessExpression(Expression ex) {
  // Printing PC
  //  Creating Adress Count

  char intruction_address[13] = {0};
  snprintf(intruction_address, sizeof(intruction_address), "0x%08X:\t", PC);

  bool is_valid_instruction = true;

  switch (ex.operation) {
  /*===========================Format 'U'=======================*/
  // OP = 000000 mov
  case OP::mov: {
    inst::mov(ex.operands);
    break;
  }
  // OP = 000001 movs
  case OP::movs: {
    inst::movs(ex.operands);
    break;
  }
  // OP = 000010 add
  case OP::add: {
    inst::add(ex.operands);
    break;
  }

  case OP::sub: {
    inst::sub(ex.operands);
    break;
  }

  // OP = 011111
  // NOTE(Everton): Gotta be caferul with this operation code
  // these operation have the same OP code = 0b000100
  case           /*:>the first 3 bits of operand.L are:*/
      OP::mul |  // mul	 = op.L: 0b000
      OP::muls | // muls = op.L: 0b010
      OP::DIV |  // div	 = op.L: 0b100
      OP::divs | // divs = op.L: 0b110
      OP::sll |  // sll	 = op.L: 0b001
      OP::sla |  // sla	 = op.L: 0b011
      OP::srl |  // srl	 = op.L: 0b101
      OP::sra:   // sra  = op.L: 0b111
  {
    switch (GetBits(ex.operands.L, 10, 8)) {
    case 0b000: // mul
    {
      inst::mul(ex.operands);
      break;
    }
    case 0b010: // muls
    {
      inst::muls(ex.operands);
      break;
    }
    case 0b100: // div
    {
      inst::div(ex.operands);
      break;
    }
    case 0b110: // divs
    {
      inst::divs(ex.operands);
      break;
    }
    case 0b001: // sll
    {
      inst::sll(ex.operands);
      break;
    }
    case 0b011: // sla
    {
      inst::sla(ex.operands);
      break;
    }
    case 0b101: // srl
    {
      inst::srl(ex.operands);
      break;
    }
    case 0b111: // sra
    {
      inst::sra(ex.operands);
      break;
    }
    }
    break;
  }
  case OP::CMP: {
    inst::CMP(ex.operands);
    break;
  }
  case OP::AND: {
    inst::AND(ex.operands);
    break;
  }
  case OP::OR: {
    inst::OR(ex.operands);
    break;
  }
  case OP::NOT: {
    inst::NOT(ex.operands);
    break;
  }
  case OP::XOR: {
    inst::XOR(ex.operands);
    break;
  }
  case OP::push: {
    inst::push(ex.operands);
    break;
  }
  case OP::pop: {
    inst::pop(ex.operands);
    break;
  }
    //:>>/*===========================Format 'F'=======================*/
    // OP = 010010 addi
  case OP::addi: {
    inst::addi(ex.operands);
    break;
  }
  case OP::subi: {
    inst::subi(ex.operands);
    break;
  }
  case OP::muli: {
    inst::muli(ex.operands);
    break;
  }
  case OP::divi: {
    inst::divi(ex.operands);
    break;
  }
  case OP::remi: {
    inst::remi(ex.operands);
    break;
  }
  case OP::cmpi: {
    inst::CMPI(ex.operands);
    break;
  }

  case OP::l8: {
    inst::l8(ex.operands);
    break;
  }
  case OP::l16: {
    inst::l16(ex.operands);
    break;
  }
  case OP::l32: {
    inst::l32(ex.operands);
    break;
  }
  case OP::s8: {
    inst::s8(ex.operands);
    break;
  }
  case OP::s16: {
    inst::s16(ex.operands);
    break;
  }
  case OP::s32: {
    inst::s32(ex.operands);
    break;
  }
  case OP::call: {
    inst::call(ex.operands);
    break;
  }
  case OP::ret: {
    inst::ret(ex.operands);
    break;
  }

    //:>>/*===========================Format 'S'=======================*/
  case OP::bae: {
    inst::bae(ex.operands);
    break;
  }
  case OP::bat: {
    inst::bat(ex.operands);
    break;
  }
  case OP::bbe: {
    inst::bbe(ex.operands);
    break;
  }
  case OP::bbt: {
    inst::bbt(ex.operands);
    break;
  }
  case OP::beq: {
    inst::beq(ex.operands);
    break;
  }
  case OP::bge: {
    inst::bge(ex.operands);
    break;
  }
  case OP::bgt: {
    inst::bgt(ex.operands);
    break;
  }
  case OP::biv: {
    inst::biv(ex.operands);
    break;
  }
  case OP::ble: {
    inst::ble(ex.operands);
    break;
  }
  case OP::blt: {
    inst::blt(ex.operands);
    break;
  }
  case OP::bne: {
    inst::bne(ex.operands);
    break;
  }
  case OP::bni: {
    inst::bni(ex.operands);
    break;
  }
  case OP::bnz: {
    inst::bnz(ex.operands);
    break;
  }
  case OP::bun: {
    inst::bun(ex.operands);
    break;
  }
  case OP::bzd: {
    inst::bzd(ex.operands);
    break;
  }
  case OP::INT: {
    inst::INT(ex.operands);
    break;
  }
  case OP::calli: {
    inst::calli(ex.operands);
    break;
  }

  /*===========================Interruptions=======================*/
  case OP::reti: {
    inst::reti(ex.operands);
    break;
  }

  // Type F
  // case OP::sbr: //Sames as the below
  case OP::cbr: {
    if (GetBit(ex.operands.I, 0) == 1) {
      inst::sbr(ex.operands);
    } else {
      inst::cbr(ex.operands);
    }
    break;
  }

  // OP = Invalido
  default: {

    is_valid_instruction = false;
    char invalid_str[256];

    snprintf(invalid_str, sizeof(invalid_str),
             "[INVALID INSTRUCTION @ 0x%08X]\n", PC);
    Interrupt("invalid", ex.operands);

    printf(invalid_str);
    printf("[SOFTWARE INTERRUPTION]\n");
    assembly_out << invalid_str << "[SOFTWARE INTERRUPTION]\n";
    // running = false; <- if Old Version
    break;
  }
  }
  if (is_valid_instruction) {
    assembly_out << intruction_address;
    printf(intruction_address);
  }
  // Bom lugar
}

/*===========================File IO=======================*/
void Write(const char *filename) {
  std::fstream output;
  output.open(filename, std::ios::out);

  if (output.is_open()) {
    output << "cock.";
  }
}

void Read(const char *filename) {
  std::fstream input;
  filename = (char *)"exemplo.hex";
  input.open(filename, std::ios::in);
  if (input.is_open()) {
  }
}

u32 HexToBin(const char *hex) {
  std::string hex_code = {hex};
  u32 number = std::stoul(hex_code, nullptr, 16);
  return number;
}
/*===========================================================*/

/*===========================Bit Manipulation=======================*/
void SetBit(u32 &number, u8 nth_bit, bool choice) {
  if (choice) {
    number |= (1UL << (nth_bit));
  } else {
    number = number & ~(1ul << (nth_bit));
  }
}

void SetBit(u64 &number, u8 nth_bit, bool choice) {
  if (choice) {
    number |= (1ULL << (nth_bit));
  } else {
    number = number & ~(1ULL << (nth_bit));
  }
}

u64 ConcatBits(const u32 &start, const u32 &end) {
  u64 bits = ((u64)start << 32) | (u64)end;
  return bits;
}

u16 ConcatBits(u8 start, u8 end) {
  u16 bits = ((u16)start << 8) | (u16)end;
  return bits;
}

u32 ConcatBits(u8 first, u8 second, u8 third, u8 fourth) {
  u32 bits = (((u32)first << 24) | ((u32)second << 16));
  bits = bits | (((u32)third << 8) | ((u32)fourth));
  return bits;
}

u32 SwitchEndianess(const u32 &number) {
  u32 byte_1th = GetBits((u32 &)number, 7, 0);
  u32 byte_2th = GetBits((u32 &)number, 15, 8);
  u32 byte_3th = GetBits((u32 &)number, 23, 16);
  u32 byte_4th = GetBits((u32 &)number, 31, 24);
  u32 switched_number = ConcatBits(byte_1th, byte_2th, byte_3th, byte_4th);
  return switched_number;
}

u8 GetBit(const u32 &number, int nth_bit) {
  u32 bit;
  bit = (number >> nth_bit) & 1U;

  return (u8)bit;
}

u8 GetBit(u64 &number, int nth_bit) {
  u32 bit;
  bit = (number >> nth_bit) & 1U;

  return (u8)bit;
}

u32 GetBits(const u32 &number, u32 start, u32 end) {
  u32 bits = (number >> end);
  if (!(start > 31)) {
    FillBits(bits, 31, start - end + 1, 0);
  }
  return bits;
}

u32 GetBits(const u64 &number, u32 start, u32 end) {
  u64 bits = (number >> end);
  if (!(start > 63)) {
    FillBits(bits, 63, start - end + 1, 0);
  }

  return (u32)bits;
}

void FillBits(u32 &number, u8 start, u8 end, bool bit_choice) {
  for (i8 i = start; i >= end; i--) {
    SetBit(number, i, bit_choice);
  }
  return;
}

void FillBits(u64 &number, u8 start, u8 end, bool bit_choice) {
  for (i8 i = start; i >= end; i--) {
    SetBit(number, i, bit_choice);
  }
  return;
}
/*===========================================================*/

namespace inst {
void SetZN(bool value) {
  // Changes the znbit
  //  Position-1
  //  NOTE(Everton): Consider this simplified case:
  //  0b0001 << 2   igual a: 0b100, depois inverta: 0b011 e depois & com o
  //  numero que vc queria:
  // SR = SR & ~(1ul << (6 - 1));
  SetBit(SR, 6, value);
}
void SetZD(bool value) { SetBit(SR, 5, value); }

void SetSN(bool value) { SetBit(SR, 4, value); }

void SetOV(bool value) { SetBit(SR, 3, value); }

void SetIV(bool value) { SetBit(SR, 2, value); }

void SetCY(bool value) { SetBit(SR, 0, value); }

void SetIE(bool value) { SetBit(SR, 1, value); }

u8 GetZN() { return GetBit(SR, 6); }
u8 GetZD() { return GetBit(SR, 5); }

u8 GetSN() { return GetBit(SR, 4); }

u8 GetOV() { return GetBit(SR, 3); }

u8 GetIV() { return GetBit(SR, 2); }

u8 GetCY() { return GetBit(SR, 0); }

u8 GetIE() { return GetBit(SR, 1); }

/*===========================All INSTRUCTIONS=======================*/

//:>>/*===========================Format Type 'U'=======================*/
// Imediate Move No sign Extension
void mov(Operands op) {
  u32 mask = 0b00000000000111111111111111111111ul;

  u32 partial = mask & ((op.X << 16) | (op.Y << 11) | (op.L << 0));
  registers[op.Z] = (u32)partial;
}

// Imediate Move with sign Extension
void movs(Operands op) {
  u32 mask = 0b00000000000111111111111111111111ull;

  u32 partial = mask & ((op.X << 16) | (op.Y << 11) | (op.L << 0));
  FillBits(partial, 31, 20, GetBit(op.X, 4));

  registers[op.Z] = (u32)partial;
}

// Adition with Registers
void add(const Operands op) {
  i64 partial = (i64)((i64)registers[op.X]) + (i64)((i64)registers[op.Y]);
  registers[op.Z] = (i32)partial;
  // AFFECTED CAMPS
  R0 = 0;
  SetZN(0);
  SetSN(0);
  SetOV(0);
  SetCY(0);

  if (registers[op.Z] == 0ul) { // set zero flag
    SetZN(1);
  }
  if ((registers[op.Z] >> 31) == 1ul) { // set negative sign flag
    SetSN(1);
  }
  if (GetBit(registers[op.X], 31) ==
      GetBit(registers[op.Y], 31)) { // set overflow flag
    if ((GetBit(registers[op.Z], 31) != GetBit(registers[op.X], 31))) {
      SetOV(1);
    }
  }
  if (GetBit((u64 &)partial, 32) == (u8)1) { // set overflow flag
    SetCY(1);
  }
}

// Subtraction with Registers
void sub(const Operands op) {
  // R[Z] = R[X] - R[Y]
  i64 partial = (i64)((i64)registers[op.X]) - (i64)((i64)registers[op.Y]);
  registers[op.Z] = (i32)partial;

  // AFFECTED CAMPS
  R0 = 0;
  SetZN(0);
  SetSN(0);
  SetOV(0);
  SetCY(0);
  if (registers[op.Z] == 0ul) { // set zero flag
    SetZN(1);
  }
  if (GetBit(registers[op.Z], 31) == (u8)1) { // set negative sign flag
    SetSN(1);
  }
  if ((GetBit(registers[op.X], 31) != GetBit(registers[op.Y], 31)) &&
      (GetBit(registers[op.Z], 31) != GetBit(registers[op.X], 31))) {
    SetOV(1);
  }
  if (GetBit((u64 &)partial, 32) == (u8)1) { // set overflow flag
    SetCY(1);
  }
}

// Multiplication with Registers
void mul(const Operands op) {
  // R[L4:0] : R[Z] = R[x] * R[Y] NO SIGN
  u32 r_index_L = GetBits(op.L, 4, 0);
  u64 partial = (u64)registers[op.X] * (u64)registers[op.Y];
  // The first 32 bits goes to R[Z]
  // The bits 64:32 goes to R[L4:0]
  registers[op.Z] = (u32)GetBits(partial, 31, 0);
  registers[r_index_L] = (u32)GetBits(partial, 63, 32);

  // AFFECTED CAMPS
  R0 = 0;
  SetZN(0);
  SetCY(0);
  if (partial == 0ul) { // set zero flag
    SetZN(1);
  }
  if (registers[r_index_L] != 0ul) { // set Carry flag
    SetCY(1);
  }
}

// RESEARCH(Everton): WHAT IS the diff about multiplying with sign
// in  a binary form.
// Multiplication with Registers
void muls(const Operands op) {
  // R[L4:0] : R[Z] = R[x] * R[Y] WITH SIGN
  u32 r_index_L = GetBits(op.L, 4, 0);
  u32 r_index_Z = op.Z;
  i64 partial = ((i64)(i32)registers[op.X]) * ((i64)(i32)registers[op.Y]);
  // The first 32 bits goes to R[Z]
  // The bits 64:32 goes to R[L4:0]
  registers[r_index_Z] = (i32)GetBits((u64 &)partial, 31, 0);
  registers[r_index_L] = (i32)GetBits((u64 &)partial, 63, 32);

  // AFFECTED CAMPS
  R0 = 0;
  SetZN(0);
  SetOV(0);
  if (partial == 0ul) { // set zero flag
    SetZN(1);
  }
  if (registers[r_index_L] != 0ul) { // set Carry flag
    SetOV(1);
  }
}

void div(const Operands op) {
  // R[L4:0] : R[Z] = R[x] * R[Y] WITH SIGN
  R0 = 0;
  if (registers[op.Y] == 0ul) { // Set Zero Division
    SetZD(1);
    Interrupt("zero", op);
    return;
  } else {
    SetZD(0);
  }
  u32 r_index_L = GetBits(op.L, 4, 0);
  u32 r_index_Z = op.Z;

  // u64 partial = (u64)registers[op.X] / (u64)registers[op.Y];
  //  The first 32 bits goes to R[Z]
  //  The bits 64:32 goes to R[L4:0]
  registers[r_index_Z] = registers[op.X] / registers[op.Y];
  registers[r_index_L] = registers[op.X] % registers[op.Y];

  // AFFECTED CAMPS
  R0 = 0;
  if (registers[op.Z] == 0ul) { // set zero flag
    SetZN(1);

  } else {
    SetZN(0);
  }
  if (registers[r_index_L] != 0ul) { // set Carry flag
    SetCY(1);
  } else {
    SetCY(0);
  }
}
void divs(const Operands op) {
  // R[L4:0] : R[Z] = R[x] * R[Y] WITH SIGN
  R0 = 0;
  if (registers[op.Y] == 0ul) { // set zero division flag
    SetZD(1);
    Interrupt("zero", op);
    return;
  } else {
    SetZD(0);
  }

  u32 r_index_L = GetBits(op.L, 4, 0);
  u32 r_index_Z = op.Z;
  // i64 partial = (i64)((i32)registers[op.X]) / (i64)((i32)registers[op.Y]);
  //  The first 32 bits goes to R[Z]
  //  The bits 64:32 goes to R[L4:0]
  registers[r_index_Z] = (i32)registers[op.X] / (i32)registers[op.Y];
  registers[r_index_L] = (i32)registers[op.X] % (i32)registers[op.Y];
  // AFFECTED CAMPS
  R0 = 0;
  if (registers[op.Z] == 0ul) { // set zero flag
    SetZN(1);
  } else {
    SetZN(0);
  }
  if (registers[r_index_L] != 0ul) { // set Carry flag
    SetCY(1);
  } else {
    SetCY(0);
  }
}
// no sign
void sll(const Operands op) {
  u32 L_4_0 = (u32)GetBits(op.L, 4, 0);
  u64 partial = ((u64)ConcatBits(registers[op.Z], registers[op.Y]))
                << (L_4_0 + 1);
  registers[op.Z] = GetBits(partial, 63, 32);
  registers[op.X] = GetBits(partial, 31, 0);
  R0 = 0;
  if (partial == 0ull) {
    SetZN(1);
  } else {
    SetZN(0);
  }
  if (registers[op.Z] != 0ul) {
    SetCY(1);
  } else {
    SetCY(0);
  }
}

// with sign
void sla(const Operands op) {
  u32 L_4_0 = (u32)GetBits(op.L, 4, 0);
  i64 concat_xy = (i64)ConcatBits(registers[op.Z], registers[op.Y]);
  i64 partial = (concat_xy) << (L_4_0 + 1);
  registers[op.Z] = (i32)GetBits((u64 &)partial, 63, 32);
  registers[op.X] = (i32)GetBits((u64 &)partial, 31, 0);
  R0 = 0;
  if (partial == 0ull) {
    SetZN(1);
  } else {
    SetZN(0);
  }
  if (registers[op.Z] != 0ul) {
    SetOV(1);
  } else {
    SetOV(0);
  }
}

// NO sign
void srl(const Operands op) {
  u32 L_4_0 = (u32)GetBits(op.L, 4, 0);
  u64 partial =
      ((u64)ConcatBits(registers[op.Z], registers[op.Y])) >> (L_4_0 + 1);
  registers[op.Z] = (u32)GetBits((u64 &)partial, 63, 32);
  registers[op.X] = (u32)GetBits((u64 &)partial, 31, 0);

  // AFFECTED CAMPS
  R0 = 0;
  if (partial == 0ull) {
    SetZN(1);
  } else {
    SetZN(0);
  }
  if (registers[op.Z] != 0ul) {
    SetCY(1);
  } else {
    SetCY(0);
  }
}

void sra(const Operands op) {
  u32 L_4_0 = (u32)GetBits(op.L, 4, 0);
  i64 partial =
      ((i64)ConcatBits(registers[op.Z], registers[op.Y])) >> (L_4_0 + 1);
  registers[op.Z] = (i32)GetBits((u64 &)partial, 63, 32);
  registers[op.X] = (i32)GetBits((u64 &)partial, 31, 0);
  // AFFECTED CAMPS
  R0 = 0;
  if (partial == 0ull) {
    SetZN(1);
  } else {
    SetZN(0);
  }
  if (registers[op.Z] != 0ul) {
    SetOV(1);
  } else {
    SetOV(0);
  }
}

void CMP(const Operands op) {
  // CMP = R[x] - R[Y]
  i64 cmp = (i64)registers[op.X] - (i64)registers[op.Y];
  // AFFECTED CAMPS
  R0 = 0;
  if (cmp == 0ull) { // set zero flag
    SetZN(1);
  } else {
    SetZN(0);
  }
  if (GetBit((u64 &)cmp, 31) == (u8)1) {
    SetSN(1);
  } else {
    SetSN(0);
  }
  if ((GetBit(registers[op.X], 31) != GetBit(registers[op.Y], 31)) &&
      (GetBit((u64 &)cmp, 31) != GetBit(registers[op.X], 31))) {
    SetOV(1);
  } else {
    SetOV(0);
  }
  if (GetBit((u64 &)cmp, 32) == (u8)1) {
    SetCY(1);
  } else {
    SetCY(0);
  }
}

/*==========================BIT WISE OPERATIONS========================*/
void AND(const Operands op) {
  // R[Z] = R[X] & R[Y]
  u64 AND = (u64)registers[op.X] & (u64)registers[op.Y];
  registers[op.Z] = (u32)registers[op.X] & (u32)registers[op.Y];
  // AFFECTED CAMPS
  R0 = 0;
  if ((u32)AND == 0ul) { // set zero flag
    SetZN(1);
  } else {
    SetZN(0);
  }
  if (GetBit(AND, 31) == (u8)1) {
    SetSN(1);
  } else {
    SetSN(0);
  }
}

void OR(const Operands op) {
  // R[Z] = R[X] & R[Y]
  u64 OR = (u64)registers[op.X] | (u64)registers[op.Y];
  registers[op.Z] = (u32)registers[op.X] | (u32)registers[op.Y];
  // AFFECTED CAMPS
  R0 = 0;
  if ((u32)OR == 0ul) { // set zero flag
    SetZN(1);
  } else {
    SetZN(0);
  }
  if (GetBit(OR, 31) == (u8)1) {
    SetSN(1);
  } else {
    SetSN(0);
  }
}

void NOT(const Operands op) {
  // R[Z] = R[X] & R[Y]
  u64 NOT = ~(u64)registers[op.X];
  registers[op.Z] = ~(u32)registers[op.X];

  // AFFECTED CAMPS
  R0 = 0;
  if ((u32)NOT == 0ul) { // set zero flag
    SetZN(1);
  } else {
    SetZN(0);
  }
  if (GetBit(registers[op.Z], 31) == (u8)1) {
    SetSN(1);
  } else {
    SetSN(0);
  }
}

void XOR(const Operands op) {
  // R[Z] = R[X] & R[Y]
  u64 XOR = (u64)registers[op.X] ^ (u64)registers[op.Y];
  registers[op.Z] = registers[op.X] ^ registers[op.Y];

  // AFFECTED CAMPS
  R0 = 0;
  if ((u32)XOR == 0ul) { // set zero flag
    SetZN(1);
  } else {
    SetZN(0);
  }
  if (GetBit(registers[op.Z], 31) == (u8)1) {
    SetSN(1);
  } else {
    SetSN(0);
  }
}
void push(const Operands op) {
  u32 X = op.X;
  u32 Y = op.Y;
  u32 Z = op.Z;

  u32 L = op.L;
  u32 W = GetBits(L, 4, 0);
  u32 V = GetBits(L, 10, 6);
  u32 registers_to_push[] = {V, W, X, Y, Z};
  if (V == 0)
    return;

  for (u32 &index : registers_to_push) {
    R0 = 0;
    if (index != 0) {
      MEM.RAM32[SP / 4] = SwitchEndianess(registers[index]);
      SP -= 4;
    } else {
      break;
    }
  }

  R0 = 0;
}
void pop(const Operands op) {
  u32 X = op.X;
  u32 Y = op.Y;
  u32 Z = op.Z;

  u32 L = op.L;
  u32 W = GetBits(L, 4, 0);
  u32 V = GetBits(L, 10, 6);
  u32 registers_to_push[] = {V, W, X, Y, Z};
  if (V == 0)
    return;

  for (u32 &index : registers_to_push) {
    R0 = 0;
    if (index != 0) {
      SP += 4;
      registers[index] = SwitchEndianess(MEM.RAM32[SP / 4]);
    } else {
      break;
    }
  }

  R0 = 0;
}

//:>>/*===========================Format Type 'F'=======================*/
// Adition with Registers
void addi(Operands op) {
  u32 imediate = op.I;
  u8 imediate_15_bit = GetBit(imediate, 15);

  // With Bit Extension 16th is extended;
  FillBits(imediate, 31, 16, imediate_15_bit);

  i64 partial = ((i64)registers[op.X]) + ((i64)imediate);
  registers[op.Z] = (i32)partial;
  // AFFECTED CAMPS
  R0 = 0;
  if (registers[op.Z] == 0ul) { // set zero flag
    SetZN(1);
  } else {
    SetZN(0);
  }
  if (GetBit(registers[op.Z], 31) == (u8)1) { // set negative sign flag
    SetSN(1);
  } else {
    SetSN(0);
  }
  if ((GetBit(registers[op.X], 31) == imediate_15_bit) &&
      (GetBit(registers[op.Z], 31) !=
       GetBit(registers[op.X], 31))) { // set overflow flag
    SetOV(1);
  } else {
    SetOV(0);
  }
  if (GetBit((u64 &)partial, 32) == 1) { // set Carry over flag
    SetCY(1);
  } else {
    SetCY(0);
  }
}

// Subtraction with imediates
void subi(Operands op) {
  u32 imediate = op.I;
  u8 imediate_15_bit = GetBit(imediate, 15);

  // With Bit Extension 16th is extended;
  FillBits(imediate, 31, 16, imediate_15_bit);

  i64 partial = ((i64)registers[op.X]) - ((i64)imediate);
  registers[op.Z] = (i32)partial;
  // AFFECTED CAMPS
  R0 = 0;
  if (registers[op.Z] == 0ul) { // set zero flag
    SetZN(1);
  } else {
    SetZN(0);
  }
  if (GetBit(registers[op.Z], 31) == (u8)1) { // set negative sign flag
    SetSN(1);
  } else {
    SetSN(0);
  }
  if ((GetBit(registers[op.X], 31) != imediate_15_bit) &&
      (GetBit(registers[op.Z], 31) != GetBit(registers[op.X], 31))) {
    SetOV(1);
  } else {
    SetOV(0);
  }
  if (GetBit((u64 &)partial, 32) == 1) { // set Carry over flag
    SetCY(1);
  } else {
    SetCY(0);
  }
}

void muli(Operands op) {
  u32 imediate = op.I;
  u8 imediate_15_bit = GetBit(imediate, 15);

  // With Bit Extension 16th is extended;
  FillBits(imediate, 31, 16, imediate_15_bit);

  u64 partial = ((i64)(i32)registers[op.X]) * ((i64)(i32)imediate);
  registers[op.Z] = (i32)partial;
  // AFFECTED CAMPS
  R0 = 0;
  if (registers[op.Z] == 0ul) { // set zero flag
    SetZN(1);
  } else {
    SetZN(0);
  }
  if (GetBits((u64 &)partial, 63, 32) != 0) {
    SetOV(1);
  } else {
    SetOV(0);
  }
}
void divi(Operands op) {
  u32 imediate = op.I;
  u8 imediate_15_bit = GetBit(imediate, 15);

  // AFFECTED CAMPS
  R0 = 0;
  if (imediate == 0ul) { // set zero division flag
    SetZD(1);
    Interrupt("zero", op);
    return;
  } else {
    SetZD(0);
  }

  FillBits(imediate, 31, 16, imediate_15_bit);

  i64 partial = ((i64)(i32)registers[op.X]) / ((i64)(i32)imediate);
  registers[op.Z] = (i32)partial;
  // AFFECTED CAMPS
  R0 = 0;
  if (registers[op.Z] == 0ul) { // set zero flag
    SetZN(1);
  } else {
    SetZN(0);
  }

  SetOV(0);
  // TODO(Everton): gotta implement overflow  flag
}

void remi(Operands op) {
  u32 imediate = op.I;
  u8 imediate_15_bit = GetBit(imediate, 15);

  FillBits(imediate, 31, 16, imediate_15_bit);
  // AFFECTED CAMPS
  R0 = 0;
  if (imediate == 0ul) { // set zero flag
    SetZD(1);
    return;
  } else {
    SetZD(0);
  }

  i64 remainder = ((i64)(i32)registers[op.X]) % ((i64)(i32)imediate);
  registers[op.Z] = (i32)remainder;
  /// AFFECTED CAMPS
  R0 = 0;
  if (registers[op.Z] == 0ul) { // set zero flag
    SetZN(1);
  } else {
    SetZN(0);
  }
  SetOV(0);
  // TODO(Everton): gotta implement overflow  flag
}

// compare imediate
void CMPI(Operands op) {
  ExtendBit(op.I, 15);

  i64 CMPI = (i64)(i32)registers[op.X] - (i64)(i32)op.I;
  // AFFECTED CAMPS
  R0 = 0;
  if (CMPI == 0ull) { // set zero flag
    SetZN(1);
  } else {
    SetZN(0);
  }
  if (GetBit((u32 &)CMPI, 31) == 1) {
    SetSN(1);
  } else {
    SetSN(0);
  }
  if ((GetBit(registers[op.X], 31) != GetBit(op.I, 15)) &&
      (GetBit((u32 &)CMPI, 31) !=
       GetBit(registers[op.X], 31))) { // set zero flag
    SetOV(1);
  } else {
    SetOV(0);
  }
  if (GetBit((u64 &)CMPI, 32) == 1) {
    SetCY(1);
  } else {
    SetCY(0);
  }
  // TODO(Everton): gotta implement overflow  flag
}

void l8(Operands op) {
  ExtendBit(op.I, 15);
  u32 mem_index = (registers[op.X] + op.I);

  if (isDevice(mem_index)) {
    u8 &device = AccessDevice8(mem_index, OP::l8);
    registers[op.Z] = device;
    return;
  }

  registers[op.Z] = MEM.RAM[mem_index];
}
void l16(Operands op) {
  ExtendBit(op.I, 15);
  u32 mem_index = ((registers[op.X] + op.I) << 1);

  if (isDevice(mem_index)) {
    u16 higher_parts = ((u16)AccessDevice8(mem_index, OP::l16)) << 8;
    u16 lower_parts = ((u16)AccessDevice8(mem_index + 1, OP::l16)) << 0;
    registers[op.Z] = higher_parts | lower_parts;
    return;
  }

  u16 higher_parts = ((u16)MEM.RAM[mem_index]) << 8;
  u16 lower_parts = ((u16)MEM.RAM[mem_index + 1]) << 0;
  registers[op.Z] = higher_parts | lower_parts;
}
void l32(Operands op) {
  ExtendBit(op.I, 15);
  u32 mem_index = ((registers[op.X] + op.I) << 2);

  if (isDevice(mem_index)) {

    u32 &device = AccessDevice32(mem_index, OP::l32);
    registers[op.Z] = SwitchEndianess(device);

    // u32 bit_1th = ((u32)AccessDevice8(mem_index,OP::l32)) << 24;
    // u32 bit_2th = ((u32)AccessDevice8(mem_index + 1, OP::l32)) << 16;
    // u32 bit_3th = ((u32)AccessDevice8(mem_index + 2, OP::l32)) << 8;
    // u32 bit_4th = ((u32)AccessDevice8(mem_index + 3, OP::l32)) << 0;

    // registers[op.Z] = bit_1th | bit_2th | bit_3th | bit_4th;
    return;
  }
  u32 bit_1th = ((u32)MEM.RAM[mem_index]) << 24;
  u32 bit_2th = ((u32)MEM.RAM[mem_index + 1]) << 16;
  u32 bit_3th = ((u32)MEM.RAM[mem_index + 2]) << 8;
  u32 bit_4th = ((u32)MEM.RAM[mem_index + 3]) << 0;

  registers[op.Z] = bit_1th | bit_2th | bit_3th | bit_4th;
}

void s8(Operands op) {
  ExtendBit(op.I, 15);
  u32 mem_index = ((registers[op.X] + op.I) << 0);

  if (isDevice(mem_index)) {
    u8 &device = AccessDevice8(mem_index, OP::s8);
    device = (u8)(0b00000000000000000000000011111111 & registers[op.Z]);
    return;
  }
  MEM.RAM[mem_index] =
      (u8)(0b00000000000000000000000011111111 & registers[op.Z]);
}

void s16(Operands op) {
  ExtendBit(op.I, 15);
  u32 mem_index = ((registers[op.X] + op.I) << 1);
  // MEM.RAM16[mem_index/2] = higher_parts | lower_parts;

  if (isDevice(mem_index)) {
    u8 &device1 = AccessDevice8(mem_index, OP::s16);
    u8 &device2 = AccessDevice8(mem_index + 1, OP::s16);

    device1 = (u8)(registers[op.Z] >> 8);
    device2 = (u8)registers[op.Z];
    return;
  }

  MEM.RAM[mem_index] = (u8)(registers[op.Z] >> 8);
  MEM.RAM[mem_index + 1] = (u8)registers[op.Z];
}

void s32(Operands op) {
  ExtendBit(op.I, 15);
  u32 mem_index = ((registers[op.X] + op.I) << 2);

  if (isDevice(mem_index)) {
    u32 &device = AccessDevice32(mem_index, OP::s32);
    device = SwitchEndianess(registers[op.Z]);
    return;
  }
  MEM.RAM32[mem_index / 4] = SwitchEndianess(registers[op.Z]);
}

void call(Operands op) {
  ExtendBit(op.I, 15);
  // TODO(Everton): CONFIRM that it doesnt need to increase 4 in these PC
  // expresions.
  MEM.RAM32[SP / 4] = SwitchEndianess(PC + 4);
  SP -= 4;

  // We -4 in here because the PC will auto increment, after each operation
  PC = ((registers[op.X] + (i32)op.I) << 2) - 4;
  R0 = 0;

  ExtendBit(op.I, 25);
  // TODO(Everton): CONFIRM that it doesnt need to increase 4 in these PC
  // expresions.
}

void ret(Operands op) {
  SP = SP + 4;
  // PC - 4 because we arlready increment +4 each operation
  PC = SwitchEndianess(MEM.RAM32[SP / 4]) - 4;
}

//:>>/*===========================Format Type 'S'=======================*/
void bae(Operands op) {
  if (GetCY() == 0) {
    Branch(op.I, false);
  }
}
void bat(Operands op) {
  if ((GetCY() == 0) && (GetZN() == 0)) {
    Branch(op.I, false);
  }
}
void bbe(Operands op) {
  if ((GetZN() == 1) || (GetCY() == 1)) {
    Branch(op.I, false);
  }
}
void bbt(Operands op) {
  if ((GetCY() == 1)) {
    Branch(op.I, false);
  }
}
void beq(Operands op) {
  if ((GetZN() == 1)) {
    Branch(op.I, false);
  }
}

void bge(Operands op) {
  if ((GetSN() == GetOV())) {
    Branch(op.I, true);
  }
}
void bgt(Operands op) {
  if ((GetZN() == 0) && (GetSN() == 0)) {
    Branch(op.I, true);
  }
}
void biv(Operands op) {
  if (GetIV() == 1) {
    Branch(op.I, false);
  }
}
void ble(Operands op) {
  if ((GetZN() == 1) || (GetSN() != GetOV())) {
    Branch(op.I, true);
  }
}
void blt(Operands op) {
  if (GetSN() != GetOV()) {
    Branch(op.I, true);
  }
}
void bne(Operands op) {
  if (GetZN() == 0) {
    Branch(op.I, false);
  }
}
void bni(Operands op) {
  if (GetIV() == 0) {
    Branch(op.I, false);
  }
}
void bnz(Operands op) {
  if (GetZD() == 0) {
    Branch(op.I, false);
  }
}
void bun(Operands op) { Branch(op.I, false); }

void bzd(Operands op) {
  if (GetZD() == 1) {
    Branch(op.I, false);
  }
}

void INT(Operands op) {
  if (op.I == 0) {
    PC = -4;
    CR = 0;
    running = false;
  } else {
    Interrupt("int", op);
  }
}

void calli(Operands op) {
  ExtendBit(op.I, 25);
  // TODO(Everton): CONFIRM that it doesnt need to increase 4 in these PC
  // expresions.
  MEM.RAM32[SP / 4] = SwitchEndianess(PC + 4);
  SP -= 4;
  PC = PC + (op.I << 2);
  R0 = 0;
}

void reti(Operands op) {
  SP = SP + 4;
  IPC = SwitchEndianess(MEM.RAM32[SP / 4]);
  SP = SP + 4;
  CR = MEM.RAM32[SP / 4];
  SP = SP + 4;
  PC = SwitchEndianess(MEM.RAM32[SP / 4]) - 4;
}

void cbr(Operands op) { SetBit(registers[op.Z], op.X, 0); }

void sbr(Operands op) { SetBit(registers[op.Z], op.X, 1); }

/*============================================================*/
/*============================================================*/
} // namespace inst

u8 &AccessMemory(u32 index) { return MEM.RAM[index]; }

bool isDevice(u32 index) {
  bool is = false;
  if (index > 0x80'000'000) {
    is = true;
  }
  return is;
}

void DefaultRoutines() {

  u32 watch = SwitchEndianess(W.WD32);
  bool watch_dog_enabled = GetBit((u32)watch, 31);
  if (watch_dog_enabled) {

    u32 counter = GetBits((u32)watch, 30, 0);
    if (counter == 0) {
      if (inst::GetIE() == 1) {
        Interrupt("watchdog");
        W.WD32 = 0x0;
        return;
      }
    }

    if (counter < 1) {
      return;
    }
    counter--;
    SetBit((u32 &)counter, 31, 1);
    W.WD32 = SwitchEndianess(counter);
  }

  if (F.wait_cycles == -2) {
    F.wait_cycles = F.CicleTime();
  }
  if (F.ReadyToOperate()) {
    F.Operate();
    F.ClearOP();
  }

  if (F.X_overwritten) {
    F.MakeInteger("X");
    F.X_overwritten = false;
  }
  if (F.Y_overwritten) {
    F.MakeInteger("Y");
    F.Y_overwritten = false;
  }
}

void Interrupt(const char type[26], Operands op) {
  SSP = SP;
  if (strcmp(type, "zero") == 0) {
    if (inst::GetIE() == 0) {
      return;
    }
    ISR();
    CR = 0;
    IPC = PC;
    PC = 0x00000008 - 4;

    // My interruption flag
    int_flag = 0;
  }

  if (strcmp(type, "int") == 0) {
    ISR();
    CR = op.I;
    IPC = PC;
    PC = 0x0000000C - 4;

    // My interruption flag
    int_flag = 0;
  }

  if (strcmp(type, "invalid") == 0) {
    ISR();
    inst::SetIV(1);
    CR = GetBits(IR, 31, 26);
    IPC = PC;
    PC = 0x00000004 - 4;
  }

  if (strcmp(type, "watchdog") == 0) {
    ISR();
    CR = 0xE1AC04DA;
    IPC = PC;
    PC = 0x0000010 - 0x4;
    // My interruption flag
    int_flag = 1;
  }

  if (strcmp(type, "op_error") == 0) {
    ISR();
    CR = 0x01EEE754;
    IPC = PC;
    PC = 0x0000014 - 0x4;
    // My interruption flag
    int_flag = 2;
  }

  if (strcmp(type, "op_var") == 0) {
    ISR();
    CR = 0x01EEE754;
    IPC = PC;
    PC = 0x0000018 - 0x4;
    // My interruption flag
    int_flag = 3;
  }

  if (strcmp(type, "op_const") == 0) {
    ISR();
    CR = 0x01EEE754;
    IPC = PC;
    PC = 0x000001C - 0x4;
    // My interruption flag
    int_flag = 4;
  }
}

u32 &AccessDevice32(u32 index, OP op) {
  F.SetRO(F.ST);
  if (index == W.WATCHDOG_ADRESS) {
    return W.WD32;
  }

  if (index == T.TERMINAL_ADRESS) {
    T.AddIn();
    return T.IO32;
  }

  // is FPU
  if (index >= 0x80'80'88'80 && index <= 0x80'80'88'8C) {
    if (op == OP::s32) {
      if (index == 0x80'80'88'80)
        F.X_overwritten = true;
      if (index == 0x80'80'88'84)
        F.Y_overwritten = true;
    }

    if (op == OP::s32 && index == 0x8080888C) {
      F.should_operate = true;
      F.wait_cycles = -2;
    }

    if (op == OP::l32 && index == 0x8080888C) {
      u32 l32index = (index - 0x80'80'88'80) / 4;
      if (SwitchEndianess(F.FPU32[l32index]) == 0x024) {
      }
    }

    index = (index - 0x80'80'88'80) / 4;
    return F.FPU32[index];
  }

  return DEFAULT_DEVICE;
}

u16 &AccessDevice16(u32 index, OP op) {
  if (index >= W.WATCHDOG_ADRESS && index <= W.WATCHDOG_ADRESS + 2) {
    u32 wd_index = (index - W.WATCHDOG_ADRESS) / 2;
    return W.WD16[wd_index];
  }

  if (index >= T.TERMINAL_ADRESS && index <= T.TERMINAL_ADRESS + 2) {
    u32 t_index = (index - T.TERMINAL_ADRESS) / 2;
    T.AddIn();
    return T.IO16[t_index];
  }

  // is FPU
  if (index >= 0x80'80'88'80 && index <= 0x80'80'88'8C) {
    index = (index - 0x80'80'88'80) / 2;
    return F.FPU16[index];
  }
}

u8 &AccessDevice8(u32 index, OP op) {
  if ((index >= W.WATCHDOG_ADRESS) && (index <= W.WATCHDOG_ADRESS + 4)) {
    u32 wd_index = index - W.WATCHDOG_ADRESS;
    return W.WD8[wd_index];
  }

  if (op == OP::l8 && index == 0x8888888A) {
    u32 t_index = (index - T.TERMINAL_ADRESS);
    T.IO8[t_index] = 48;
    return T.IO8[t_index];
  }
  if (index >= T.TERMINAL_ADRESS && index <= T.TERMINAL_ADRESS + 4) {
    u32 t_index = (index - T.TERMINAL_ADRESS);
#ifdef _DEBUG_OUT
#endif
    T.AddIn();
    return T.IO8[t_index];
  }

  // is FPU
  if (index >= 0x80'80'88'80 && index <= (0x80'80'88'8C + 4)) {

    if (op == OP::s8 && index == 0x8080888F) {
      F.should_operate = true;
      F.wait_cycles = -2;
    }
    index = (index - 0x80'80'88'80) / 1;
    return F.FPU8[index];
  }
}

// NOTE(Everton): Maybe you need to change  the Endianess
void ISR() {
  PPC = PC;
  MEM.RAM32[SP / 4] = SwitchEndianess(PC + 4);
  SP = SP - 4;
  MEM.RAM32[SP / 4] = SwitchEndianess(CR);
  SP = SP - 4;
  MEM.RAM32[SP / 4] = SwitchEndianess(IPC);
  SP = SP - 4;
}

void Branch(u32 imediate, bool is_signed) {
  u8 imediate_26th_bit = GetBit(imediate, 25);
  FillBits(imediate, 31, 24, imediate_26th_bit);
  if (is_signed) {
    PC = PC + ((i32)imediate << 2);
  } else {
    PC = PC + ((u32)imediate << 2);
  }
}

void StoreHexInstructions(const char *filename, u32 instructions[]) {
  std::ifstream hex_in;
  hex_in.open(filename, std::ios::in);
  if (hex_in.is_open()) {
    std::string instruction;
    u32 instruction_counter = 0;

    while (std::getline(hex_in, instruction)) {
      const char *inst = instruction.c_str();
      instructions[instruction_counter] = SwitchEndianess(HexToBin(inst));

      instruction_counter++;
    }
  }
  hex_in.close();
}

void ReadWriteStr(char result[256], const char *instruction, u8 bits_count,
                  Operands op) {
  ExtendBit(op.I, 15);

  char rz[5], rx[5];
  RegStr(rz, op.Z);
  RegStr(rx, op.X);

  char assembly_text[32];
  char bytes_accessed[12];

  u32 mem_index = 0;
  switch (bits_count) {
  case 32: {
    mem_index = ((registers[op.X] + op.I) << 2);
    snprintf(bytes_accessed, sizeof(bytes_accessed), "0x%08X", registers[op.Z]);
    break;
  }
  case 16: {
    mem_index = ((registers[op.X] + op.I) << 1);
    snprintf(bytes_accessed, sizeof(bytes_accessed), "0x%04X", registers[op.Z]);
    break;
  }
  case 8: {
    mem_index = ((registers[op.X] + op.I) << 0);
    snprintf(bytes_accessed, sizeof(bytes_accessed), "0x%02X", registers[op.Z]);
    break;
  }
  default:
    snprintf(bytes_accessed, sizeof(bytes_accessed), "0x%08X", registers[op.Z]);
    break;
  }

  // l32 r24,[r0+8]           	R24=MEM[0x00000020]=0x00323456
  if (instruction[0] == 'l') // Then its a read
  {
    snprintf(assembly_text, sizeof(assembly_text),
             "l%d "
             "%s"
             ","
             "[%s+%d]",
             bits_count, rz, rx, op.I);
    snprintf(result, 256,
             "%-24s\t"
             "%s=MEM[0x%08X]="
             "%s\n",
             assembly_text, UpperStr(rz), mem_index, bytes_accessed);
  }
  // s32 [r0+8],r24           	MEM[0x00000020]=R24=0x00323456
  else if (instruction[0] == 's') // Then its a Write
  {
    snprintf(assembly_text, sizeof(assembly_text),
             "s%d "
             "[%s+%d]"
             ","
             "%s",
             bits_count, rx, op.I, rz);
    snprintf(result, 256,
             "%-24s\t"
             "MEM[0x%08X]=%s="
             "%s\n",
             assembly_text, mem_index, UpperStr(rz), bytes_accessed);
  }
}

char *UpperStr(char *str) {
  char *start_of_str = str;
  while (*str) {
    *str = toupper(*str);
    str++;
  }
  return start_of_str;
}

// Extend the bit_place up to the start of the number
// bits after the bit_place remains untouched
void ExtendBit(u32 &num, u8 bit_place) {
  u8 bit = GetBit(num, bit_place);
  FillBits(num, 31, bit_place, bit);
}
/*===========================BITS MANIPULATION ENDS=======================*/

void RegStr(char register_name[5], u8 id) {
  if (id < 26) {
    sprintf(register_name, "r%d", id);
  } else if (id == 26) {
    sprintf(register_name, "cr");
  } else if (id == 27) {
    sprintf(register_name, "ipc");
  } else if (id == 28) {
    sprintf(register_name, "ir");
  } else if (id == 29) {
    sprintf(register_name, "pc");
  } else if (id == 30) {
    sprintf(register_name, "sp");
  } else if (id == 31) {
    sprintf(register_name, "sr");
  }
}

void BranchStr(char dest[256], const char *instruction_name,
               u32 imediate) { //'branch' 7		PC=0x00000020
  char assembly_text[32];
  ExtendBit(imediate, 25);
  snprintf(assembly_text, sizeof(assembly_text), "%s %d", instruction_name,
           (i32)imediate);
  snprintf(dest, 256,
           "%-24s\t"
           "PC=0x%08X\n",
           assembly_text, PC);
  // PC -= 4;
}

void PushPopStr(char dest[500], const char *instruction_name,
                Operands operand) {
  u32 X = operand.X;
  u32 Y = operand.Y;
  u32 Z = operand.Z;

  u32 L = operand.L;
  u32 W = GetBits(L, 4, 0);
  u32 V = GetBits(L, 10, 6);

  char reg[5];
  char assembly_text[32];
  char no_register_instruction_name[35];

  u32 registers_to_push[] = {V, W, X, Y, Z};
  // 0x00000104:  push -                     MEM[0x00007FF8]{}={}
  // push r21,r22,r23,r24,r25
  // MEM[0x00007FF8]{0x1EBCE8B4,0xDD23FFFF,0x00000000,0xFFFFFFFC,0xFFFFFFF6}={R21,R22,R23,R24,R25}
  std::string registers_string = "";
  std::string registers_values = "";
  u32 SP_before = SP;
  for (u32 &index : registers_to_push) {
    if (V == 0) {

      strcpy(no_register_instruction_name, instruction_name);
      strcat(no_register_instruction_name, " - ");
      instruction_name = no_register_instruction_name;
    }
    if (index != 0) {
      RegStr(reg, index);
      registers_string += std::string(reg) + ",";
      registers_values += std::string(ToHex(registers[index]) + ",");

      // SP +- 4 because  we're supposed to print when  the operation is
      // taken place, but considering we're printing after, we need to
      // "undo" certain operations;
      if (instruction_name[1] == 'o') {
        SP_before -= 4;
      } else {
        SP_before += 4;
      }
    } else {
      break;
    }
  }
  // Clear last comma from register representation in the assembly text
  if (!registers_string.empty()) {
    registers_string.pop_back();
  }
  // TODO(Everton):If it's empty we decorate with a '-'
  // Clear last comma from register values in the assembly text
  if (!registers_values.empty()) {
    registers_values.pop_back();
  }

  // Copied the Resgister String Representation to uppercase them
  char upper_registers_string[26];
  strcpy(upper_registers_string, registers_string.c_str());

  // Assemble all Text about the resgister  values and  representations
  snprintf(assembly_text, sizeof(assembly_text), "%s %s", instruction_name,
           registers_string.c_str());
  // SP + 4 because  we're supposed to print when  the operation is taken
  // place, but considering we're printing after, we need to "undo" certain
  // operations;
  if (instruction_name[1] == 'o') {
    snprintf(dest, 500,
             "%-24s\t"
             "{%s}=MEM[0x%08X]{%s}\n",
             assembly_text, UpperStr(upper_registers_string), SP_before,
             registers_values.c_str());
  } else {
    snprintf(dest, 500,
             "%-24s\t"
             "MEM[0x%08X]{%s}={%s}\n",
             assembly_text, SP_before, registers_values.c_str(),
             UpperStr(upper_registers_string));
  }
}

std::string ToHex(u32 hex) {
  char number[20];
  snprintf(number, 20, "0x%08X", hex);
  return std::string(number);
}

void PrintOperation(OP op, Operands operand) {
  char result[500] = {0};

  switch (op) {
  case OP::mov: {
    // mov r1,1193046           	R1=0x00123456
    char rz[5];
    RegStr(rz, operand.Z);
    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "mov %s,%u", rz,
             registers[operand.Z]);

    snprintf(result, 256,
             "%-24s\t"
             "%s="
             "0x%08X\n",
             assembly_text, UpperStr(rz), registers[operand.Z]);
    break;
  }
  case OP::movs: {
    // movs r2,-1048576         	R2=0xFFF00000
    char rz[5];
    RegStr(rz, operand.Z);
    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "movs %s,%d", rz,
             registers[operand.Z]);

    snprintf(result, 256,
             "%-24s\t"
             "%s="
             "0x%08X\n",
             assembly_text, UpperStr(rz), registers[operand.Z]);

    break;
  }
  case OP::add: {
    char rz[5], rx[5], ry[5];
    RegStr(rz, operand.Z);
    RegStr(rx, operand.X);
    RegStr(ry, operand.Y);
    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "add %s,%s,%s", rz, rx, ry);
    snprintf(result, 256,
             "%-24s\t"
             // NOTE(Everton):IS this  supposed to be %4s ?
             "%s=%s+%s="
             "0x%08X,"
             "SR=0x%08X\n",
             assembly_text, UpperStr(rz), UpperStr(rx), UpperStr(ry),
             registers[operand.Z], SR);

    break;
  }
  case OP::sub: {
    // sub r4, r2, r3             	R4 = R2 - R3 = 0x7FFDCBAA, SR =
    // 0x00000008
    char rz[5], rx[5], ry[5];
    RegStr(rz, operand.Z);
    RegStr(rx, operand.X);
    RegStr(ry, operand.Y);
    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "sub %s,%s,%s", rz, rx, ry);
    snprintf(result, 256,
             "%-24s\t"
             "%s=%s-%s="
             "0x%08X,"
             "SR=0x%08X\n",
             assembly_text, UpperStr(rz), UpperStr(rx), UpperStr(ry),
             registers[operand.Z], SR);

    break;
  }
  case           /*:>the first 3 bits of operand.L are:*/
      OP::mul |  // mul	 = op.L: 0b000
      OP::muls | // muls = op.L: 0b010
      OP::DIV |  // div	 = op.L: 0b100
      OP::divs | // divs = op.L: 0b110
      OP::sll |  // sll	 = op.L: 0b001
      OP::sla |  // sla	 = op.L: 0b011
      OP::srl |  // srl	 = op.L: 0b101
      OP::sra:   // sra  = op.L: 0b111
  {
    switch (GetBits(operand.L, 10, 8)) {
    case 0b000: // mul
    {
      // mul r0,r5,r4,r3
      // R0:R5=R4*R3=0x0000000023F4F31C,SR=0x00000008
      char rl[5], rz[5], rx[5], ry[5];
      u32 L_4_0 = (u32)GetBits(operand.L, 4, 0);
      RegStr(rl, L_4_0);
      RegStr(rz, operand.Z);
      RegStr(rx, operand.X);
      RegStr(ry, operand.Y);
      char assembly_text[32];
      snprintf(assembly_text, sizeof(assembly_text), "mul %s,%s,%s,%s", rl, rz,
               rx, ry);

      snprintf(result, sizeof(result),
               "%-24s\t"
               "%s:%s=%s*%s="
               "0x%08X%08X,"
               "SR=0x%08X\n",
               assembly_text, UpperStr(rl), UpperStr(rz), UpperStr(rx),
               UpperStr(ry), registers[L_4_0], registers[operand.Z], SR);

      ;
      break;
    }
    case 0b010: // muls
    {
      // muls r0,r7,r6,r5
      // R0:R7=R6*R5=0x0000000000000000,SR=0x00000040
      char rl[5], rz[5], rx[5], ry[5];
      u32 L_4_0 = (u32)GetBits(operand.L, 4, 0);
      RegStr(rl, L_4_0);
      RegStr(rz, operand.Z);
      RegStr(rx, operand.X);
      RegStr(ry, operand.Y);
      char assembly_text[32];
      snprintf(assembly_text, sizeof(assembly_text), "muls %s,%s,%s,%s", rl, rz,
               rx, ry);

      snprintf(result, sizeof(result),
               "%-24s\t"
               // NOTE(Everton):IS this  supposed to be %4s ?
               "%s:%s=%s*%s="
               "0x%08X%08X,"
               "SR=0x%08X\n",
               assembly_text, UpperStr(rl), UpperStr(rz), UpperStr(rx),
               UpperStr(ry), registers[L_4_0], registers[operand.Z], SR);

      break;
    }
    case 0b100: /*div */
    {
      // div r0,r9,r8,r7
      // R0=R8%R7=0x00000000,R9=R8/R7=0x00000000,SR=0x00000060
      char rz[5], rx[5], ry[5], rl[5];
      u32 r_index_L = GetBits(operand.L, 4, 0);
      RegStr(rz, operand.Z);
      RegStr(rx, operand.X);
      RegStr(ry, operand.Y);
      RegStr(rl, r_index_L);
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
               assembly_text, UpperStr(rl), UpperStr(rx), percent, UpperStr(ry),
               ((u32)registers[r_index_L]), UpperStr(rz), UpperStr(rx),
               UpperStr(ry), ((u32)registers[operand.Z]), SR);

      break;
    }
    case 0b110: /*divs*/
    {
      // divs r10,r11,r9,r8
      // R10=R9%R8=0x00000000,R11=R9/R8=0x00000000,SR=0x00000060
      char rz[5], rx[5], ry[5], rl[5];
      u32 r_index_L = GetBits(operand.L, 4, 0);
      RegStr(rz, operand.Z);
      RegStr(rx, operand.X);
      RegStr(ry, operand.Y);
      RegStr(rl, r_index_L);
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
               assembly_text, UpperStr(rl), UpperStr(rx), percent, UpperStr(ry),
               ((i32)registers[r_index_L]), UpperStr(rz), UpperStr(rx),
               UpperStr(ry), ((i32)registers[operand.Z]), SR);

      break;
    }
    case 0b001: /*sll */
    {
      // sll r6,r5,r5,0
      // R6:R5=R6:R5<<1=0x0000000047E9E638,SR=0x00000008
      char rz[5], rx[5], ry[5];
      u32 L_4_0 = (u32)GetBits(operand.L, 4, 0);
      RegStr(rz, operand.Z);
      RegStr(rx, operand.X);
      RegStr(ry, operand.Y);
      char assembly_text[32];

      snprintf(assembly_text, sizeof(assembly_text), "sll %s,%s,%s,%d", rz, rx,
               ry, L_4_0);

      snprintf(result, sizeof(result),
               "%-24s\t"
               "%s:%s="
               "%s:%s<<%d="
               "0x%08X%08X,"
               "SR=0x%08X\n",
               assembly_text, UpperStr(rz), UpperStr(rx), UpperStr(rz),
               UpperStr(ry), L_4_0 + 1, registers[operand.Z],
               registers[operand.Y], SR);

      break;
    }
    case 0b011: /*sla */
    {
      // sla r0,r2,r2,10
      // R0:R2=R0:R2<<11=0x0000000080000000,SR=0x00000001
      char rz[5], rx[5], ry[5];
      u32 L_4_0 = (u32)GetBits(operand.L, 4, 0);
      RegStr(rz, operand.Z);
      RegStr(rx, operand.X);
      RegStr(ry, operand.Y);

      char assembly_text[32];
      snprintf(assembly_text, sizeof(assembly_text), "sla %s,%s,%s,%u", rz, rx,
               ry, L_4_0);

      snprintf(result, sizeof(result),
               "%-24s\t"
               "%s:%s="
               "%s:%s<<%ld="
               "0x%08X%08X,"
               "SR=0x%08X\n",

               assembly_text, UpperStr(rz), UpperStr(rx), UpperStr(rz),
               UpperStr(ry), (L_4_0 + 1l), registers[operand.Z],
               registers[operand.X], ((u32)SR));

      break;
    }
    case 0b101: /*srl */
    {
      // srl r10,r9,r9,2
      // R10:R9=R10:R9>>3=0x0000000000000000,SR=0x00000060
      char rz[5], rx[5], ry[5];
      u32 L_4_0 = (u32)GetBits(operand.L, 4, 0);
      RegStr(rz, operand.Z);
      RegStr(rx, operand.X);
      RegStr(ry, operand.Y);
      char assembly_text[32];

      snprintf(assembly_text, sizeof(assembly_text), "srl %s,%s,%s,%d", rz, rx,
               ry, L_4_0);

      snprintf(result, sizeof(result),
               "%-24s\t"
               "%s:%s="
               "%s:%s>>%d="
               "0x%08X%08X,"
               "SR=0x%08X\n",
               assembly_text, UpperStr(rz), UpperStr(rx), UpperStr(rz),
               UpperStr(ry), L_4_0 + 1, registers[operand.Z],
               registers[operand.X], SR);

      break;
    }
    case 0b111: /*sra */
    {
      // sra r12,r10,r10,3
      // R12:R10=R12:R10>>4=0x0000000000000000,SR=0x0000
      char rz[5], rx[5], ry[5];
      u32 L_4_0 = (u32)GetBits(operand.L, 4, 0);
      RegStr(rz, operand.Z);
      RegStr(rx, operand.X);
      RegStr(ry, operand.Y);
      char assembly_text[32];

      snprintf(assembly_text, sizeof(assembly_text), "sra %s,%s,%s,%d", rz, rx,
               ry, L_4_0);

      snprintf(result, sizeof(result),
               "%-24s\t"
               "%s:%s="
               "%s:%s>>%d="
               "0x%08X%08X,"
               "SR=0x%08X\n",
               assembly_text, UpperStr(rz), UpperStr(rx), UpperStr(rz),
               UpperStr(ry), (L_4_0 + 1), registers[operand.Z],
               registers[operand.X], SR);

      break;
    }
    }
    break;
  }

  case OP::CMP: // NOTE(Everton): DONE
  {
    // cmp ir,pc                	SR=0x00000020
    char rx[5], ry[5];
    RegStr(rx, operand.X);
    RegStr(ry, operand.Y);

    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "cmp %s,%s", rx, ry);

    snprintf(result, sizeof(result),
             "%-24s\t"
             "SR=0x%08X\n",
             assembly_text, SR);

    break;
  }

  case OP::AND: {
    // and r13,r1,r5            	R13=R1&R5=0x00002410,SR=0x00000020
    char rz[5], rx[5], ry[5];
    RegStr(rz, operand.Z);
    RegStr(rx, operand.X);
    RegStr(ry, operand.Y);

    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "and %s,%s,%s", rz, rx, ry);

    snprintf(result, sizeof(result),
             "%-24s\t"
             "%s=%s&%s="
             "0x%08X,"
             "SR=0x%08X\n",
             assembly_text, UpperStr(rz), UpperStr(rx), UpperStr(ry),
             registers[operand.Z], SR);

    break;
  }
  case OP::OR: {
    // or r14,r2,r6             	R14=R2|R6=0x80000000,SR=0x00000030
    char rz[5], rx[5], ry[5];
    RegStr(rz, operand.Z);
    RegStr(rx, operand.X);
    RegStr(ry, operand.Y);

    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "or %s,%s,%s", rz, rx, ry);

    snprintf(result, sizeof(result),
             "%-24s\t"
             "%s=%s|%s="
             "0x%08X,"
             "SR=0x%08X\n",
             assembly_text, UpperStr(rz), UpperStr(rx), UpperStr(ry),
             registers[operand.Z], SR);
    break;
  }
  case OP::NOT: {
    // not r15,r7               	R15=~R7=0xFFFFFFFF,SR=0x00000030
    char rz[5], rx[5];
    RegStr(rz, operand.Z);
    RegStr(rx, operand.X);

    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "not %s,%s", rz, rx);

    snprintf(result, sizeof(result),
             "%-24s\t"
             "%s=~%s="
             "0x%08X,"
             "SR=0x%08X\n",

             assembly_text, UpperStr(rz), UpperStr(rx), registers[operand.Z],
             SR);
    break;
  }
  case OP::XOR: {
    // xor r16, r16, r8           	R16 = R16 ^ R8 = 0x00000000, SR =
    // 0x00000060
    char rz[5], rx[5], ry[5];
    RegStr(rz, operand.Z);
    RegStr(rx, operand.X);
    RegStr(ry, operand.Y);

    char assembly_text[32];

    snprintf(assembly_text, sizeof(assembly_text), "xor %s,%s,%s", rz, rx, ry);

    snprintf(result, sizeof(result),
             "%-24s\t"
             "%s=%s^%s="
             "0x%08X,"
             "SR=0x%08X\n",

             assembly_text, UpperStr(rz), UpperStr(rx), UpperStr(ry),
             registers[operand.Z], SR);
    break;
  }
  //:> Format Type 'F'
  case OP::addi: {
    // addi r1,r1,32771                R1 = R1 + 0x00008003 = 0xFFFF8003,SR
    // = 0x00000010
    char rz[5], rx[5];
    RegStr(rz, operand.Z);
    RegStr(rx, operand.X);

    u8 imediate_15_bit = GetBit(operand.I, 15);
    u32 imediate = (i32)operand.I;
    FillBits(imediate, 31, 16, imediate_15_bit);

    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "addi %s,%s,%d", rz, rx,
             (i32)imediate);

    snprintf(result, sizeof(result),
             "%-24s\t"
             "%s=%s+0x%08X="
             "0x%08X,"
             "SR=0x%08X\n",
             assembly_text, UpperStr(rz), UpperStr(rx), (i32)imediate,
             registers[operand.Z], SR);

    break;
  }

  case OP::subi: {
    // subi r18, r18, -1          	R18 = R18 - 0xFFFFFFFF = 0x00000001, SR
    // = 0x00000020
    char rz[5], rx[5];
    RegStr(rz, operand.Z);
    RegStr(rx, operand.X);

    u8 imediate_15_bit = GetBit(operand.I, 15);
    u32 imediate = (i32)operand.I;
    FillBits(imediate, 31, 16, imediate_15_bit);

    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "subi %s,%s,%d", rz, rx,
             (i32)imediate);
    snprintf(result, sizeof(result),
             "%-24s\t"
             "%s=%s-0x%08X="
             "0x%08X,"
             "SR=0x%08X\n",
             assembly_text, UpperStr(rz), UpperStr(rx), (i32)imediate,
             registers[operand.Z], SR);
    break;
  }
  case OP::muli: {
    // muli r19, r17, 2           	R19 = R17 * 0x00000002 = 0x00000002, SR
    // = 0x00000020
    char rz[5], rx[5];
    RegStr(rz, operand.Z);
    RegStr(rx, operand.X);

    u8 imediate_15_bit = GetBit(operand.I, 15);
    u32 imediate = (i32)operand.I;
    FillBits(imediate, 31, 16, imediate_15_bit);

    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "muli %s,%s,%d", rz, rx,
             (i32)imediate);
    snprintf(result, sizeof(result),
             "%-24s\t"
             "%s=%s*0x%08X="
             "0x%08X,"
             "SR=0x%08X\n",
             assembly_text, UpperStr(rz), UpperStr(rx), (i32)imediate,
             registers[operand.Z], SR);
    break;
  }
  case OP::divi: {
    // divi r20, r19, 2           	R20 = R19 / 0x00000002 = 0x00000001, SR
    // = 0x00000000
    char rz[5], rx[5];
    RegStr(rz, operand.Z);
    RegStr(rx, operand.X);

    u8 imediate_15_bit = GetBit(operand.I, 15);
    u32 imediate = (i32)operand.I;
    FillBits(imediate, 31, 16, imediate_15_bit);

    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "divi %s,%s,%d", rz, rx,
             (i32)imediate);
    snprintf(result, sizeof(result),
             "%-24s\t"
             "%s=%s/0x%08X="
             "0x%08X,"
             "SR=0x%08X\n",
             assembly_text, UpperStr(rz), UpperStr(rx), (i32)imediate,
             registers[operand.Z], SR);
    break;
  }
  case OP::remi: {
    // modi r20, r19, 2           	R20 = R19 mod  0x00000002 = 0x00000001,
    // SR = 0x00000000
    char rz[5], rx[5];
    RegStr(rz, operand.Z);
    RegStr(rx, operand.X);

    u8 imediate_15_bit = GetBit(operand.I, 15);
    u32 imediate = (i32)operand.I;
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
             assembly_text, UpperStr(rz), UpperStr(rx), percent, (i32)imediate,
             registers[operand.Z], SR);

    break;
  }

  case OP::cmpi: {
    // cmpi r20, r19, 2           	CMPI = R19 - 0x00000001 = 0x00000001, SR
    // = 0x00000000
    char rx[5];
    RegStr(rx, operand.X);

    ExtendBit(operand.I, 15);

    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "cmpi %s,%d", rx,
             (i32)operand.I);
    snprintf(result, sizeof(result),
             "%-24s\t"
             "SR=0x%08X\n",
             assembly_text, SR);
    break;
  }

  case OP::l8: {
    ReadWriteStr(result, "l8", 8, operand);
    break;
  }

  case OP::l16: {
    ReadWriteStr(result, "l16", 16, operand);
    break;
  }
  case OP::l32: {
    ReadWriteStr(result, "l32", 32, operand);
    break;
  }
  case OP::s8: {
    ReadWriteStr(result, "s8", 8, operand);
    break;
  }
  case OP::s16: {
    ReadWriteStr(result, "s8", 16, operand);
    break;
  }
  case OP::s32: {
    ReadWriteStr(result, "s8", 32, operand);
    break;
  }

  case OP::bae: {
    BranchStr(result, "bae", operand.I);

    break;
  }
  case OP::bat: {

    BranchStr(result, "bat", operand.I);

    break;
  }
  case OP::bbe: {
    BranchStr(result, "bbe", operand.I);
    break;
  }
  case OP::bbt: {

    BranchStr(result, "bbt", operand.I);

    break;
  }
  case OP::beq: {

    BranchStr(result, "beq", operand.I);

    break;
  }
  case OP::bge: {

    BranchStr(result, "bge", operand.I);

    break;
  }
  case OP::bgt: {

    BranchStr(result, "bgt", operand.I);

    break;
  }
  case OP::biv: {

    BranchStr(result, "biv", operand.I);

    break;
  }
  case OP::ble: {

    BranchStr(result, "ble", operand.I);

    break;
  }
  case OP::blt: {

    BranchStr(result, "blt", operand.I);

    break;
  }
  case OP::bne: {

    BranchStr(result, "bne", operand.I);

    break;
  }
  case OP::bni: {

    BranchStr(result, "bni", operand.I);

    break;
  }
  case OP::bnz: {

    BranchStr(result, "bnz", operand.I);

    break;
  }
  case OP::bun: {

    BranchStr(result, "bun", operand.I);
    break;
  }
  case OP::bzd: {
    BranchStr(result, "bzd", operand.I);

    break;
  }
  case OP::INT: {
    // 0x000000D4:	int 0 CR=0x00000000,PC=0x00000000
    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "%s %d", "int", operand.I);

    snprintf(result, 256,
             "%-24s\t"
             "CR=0x%08X,PC=0x%08X\n",
             assembly_text, CR, PC);
    break;
  }
    //::>> FLOW CONTROL:
  case OP::call: {
    ExtendBit(operand.I, 15);
    char rx[5];
    RegStr(rx, operand.X);

    // call[r0 + 16]             	PC=0x00000040,MEM[0x00007FFC]=0x000002C4
    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "%s [%s+%d]", "call", rx,
             (operand.I));

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
  case OP::calli: {
    // call[r0 + 16]             	PC=0x00000040,MEM[0x00007FFC]=0x000002C4
    ExtendBit(operand.I, 25);
    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "%s %d", "call", operand.I);
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
  case OP::ret: {
    // ret                      	PC=MEM[0x00007FFC]=0x000002C4
    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "%s", "ret");

    snprintf(result, 256,
             "%-24s\t"
             "PC=MEM[0x%08X]=0x%08X\n",
             assembly_text, SP, SwitchEndianess(MEM.RAM32[(SP) / 4]));
    break;
  }
  case OP::push: {
    PushPopStr(result, "push", operand);
    break;
  }
  case OP::pop: {
    PushPopStr(result, "pop", operand);
    break;
  }

  case OP::sbr: // cbr
  {
    // 0x00000034:	sbr sr[1]                	SR=0x00000002
    char instruction[5];
    if (GetBit(operand.I, 0) == 0) {
      sprintf(instruction, "%s", "cbr");
    } else {
      sprintf(instruction, "%s", "sbr");
    }

    // mov r1,1193046           	R1=0x00123456
    char rz[5];
    RegStr(rz, operand.Z);
    char assembly_text[32];
    snprintf(assembly_text, sizeof(assembly_text), "%s %s[%u]", instruction, rz,
             operand.X);

    snprintf(result, 256,
             "%-24s\t"
             "%s="
             "0x%08X\n",
             assembly_text, UpperStr(rz), registers[operand.Z]);
    break;
  }
  case OP::reti: {
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
