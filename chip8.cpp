#include "chip8.hpp"

chip8::chip8(){
  pc= 0x200;  // Program counter starts at 0x200
  opcode = 0;      // Reset current opcode	
  I      = 0;      // Reset index register
  sp     = 0;
}