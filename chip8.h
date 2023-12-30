#ifndef __CHIP8_H__
#define __CHIP8_H__

#include <stdint.h>

#define MEM_SIZE 4096
#define DISPLAY_HEIGHT 32
#define DISPLAY_WIDTH 64

typedef uint8_t byte;

typedef struct chip8 {
  uint8_t mem[MEM_SIZE];
  uint8_t reg[16];
  uint16_t index_reg;
  uint16_t pc;  // keep trace of opcode
  uint16_t stack[12];
  uint8_t sp;  // keep trace of stack top
  uint8_t delay_timer;
  uint8_t sound_timer;
  uint8_t display[DISPLAY_HEIGHT][DISPLAY_WIDTH];
  uint8_t inputs[16];
} CHIP8;

#endif