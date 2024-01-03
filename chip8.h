#ifndef __CHIP8_H__
#define __CHIP8_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// CHIP-8 run at 540 Hz
#define CYCLE_FREQUENCY 540
// delay around 1851 microseconds a cycle
#define CYCLE_DELAY (1000000/CYCLE_FREQUENCY)
// delay around 16666 microseconds to decrease timer
#define TIMER_DELAY (1000000/60)

#define MEM_SIZE 4096
#define MEM_START 0x200
#define DISPLAY_HEIGHT 32
#define DISPLAY_WIDTH 64

#define FONTSET_SIZE 80
#define FONTSET_MEM_START 0x50
extern uint8_t chip8_fontset[];

typedef uint8_t byte;

typedef struct chip8 {
  uint8_t mem[MEM_SIZE];
  uint8_t reg[16];
  uint16_t index_reg;
  uint16_t pc;      // keep trace of opcode address
  uint16_t opcode;  // keep trace of opcode
  uint16_t stack[16];
  uint8_t sp;  // keep trace of stack top
  uint8_t delay_timer;
  uint8_t sound_timer;
  uint8_t display[DISPLAY_HEIGHT][DISPLAY_WIDTH];
  uint8_t input_keys[16];  // pressed or not
} CHIP8;

CHIP8 *chip8_init();

uint8_t chip8_load_rom(CHIP8 *chip8, char *rom_name);

void chip8_cycle(CHIP8 *chip8);

void chip8_timer(CHIP8 *chip8);

#define _OPCODE (chip8->opcode)
#define X(opcode) (uint8_t)((0x0F00 & (opcode)) >> 8)
#define Y(opcode) (uint8_t)((0x00F0 & (opcode)) >> 4)
#define N(opcode) (uint8_t)(0x000F & (opcode))
#define NN(opcode) (uint8_t)(0x00FF & (opcode))
#define NNN(opcode) (uint16_t)(0x0FFF & (opcode))

#define VX(opcode) (chip8->reg[X(opcode)])
#define VY(opcode) (chip8->reg[Y(opcode)])
#define _VF (chip8->reg[0xF])
#define _I (chip8->index_reg)

typedef void (*opcode_func)(CHIP8 *chip);
#define OPCODE(N) opcode_##N(chip8)

void opcode_00E0(CHIP8 *chip);  // Clear the screen
void opcode_00EE(CHIP8 *chip);  // Return from a subroutine
void opcode_1NNN(CHIP8 *chip);  // Jump to address NNN
void opcode_2NNN(CHIP8 *chip);  // Call subroutine at NNN
void opcode_3XNN(CHIP8 *chip);  // Skip next instruction if VX equals NN
void opcode_4XNN(CHIP8 *chip);  // Skip next instruction if VX doesn't equal NN
void opcode_5XY0(CHIP8 *chip);  // Skip next instruction if VX equals VY
void opcode_6XNN(CHIP8 *chip);  // Set VX to NN
void opcode_7XNN(CHIP8 *chip);  // Add NN to VX
void opcode_8XY0(CHIP8 *chip);  // Set VX to the value of VY
void opcode_8XY1(CHIP8 *chip);  // Set VX to VX OR VY
void opcode_8XY2(CHIP8 *chip);  // Set VX to VX AND VY
void opcode_8XY3(CHIP8 *chip);  // Set VX to VX XOR VY
void opcode_8XY4(
    CHIP8 *chip);  // Add VY to VX; VF is set to 1 if there's a carry, else 0
void opcode_8XY5(CHIP8 *chip);  // Subtract VY from VX; VF is set to 0 if
                                // there's a borrow, else 1
void opcode_8XY6(CHIP8 *chip);  // Store the least significant bit of VX in VF
                                // and then shift VX to the right by 1
void opcode_8XY7(CHIP8 *chip);  // Set VX to VY minus VX; VF is set to 0 if
                                // there's a borrow, else 1
void opcode_8XYE(CHIP8 *chip);  // Store the most significant bit of VX in VF
                                // and then shift VX to the left by 1
void opcode_9XY0(CHIP8 *chip);  // Skip next instruction if VX doesn't equal VY
void opcode_ANNN(CHIP8 *chip);  // Set I to the address NNN
void opcode_BNNN(CHIP8 *chip);  // Jump to the address NNN plus V0
void opcode_CXNN(CHIP8 *chip);  // Set VX to the result of a bitwise AND
                                // operation on a random number and NN
void opcode_DXYN(CHIP8 *chip);  // Draw a sprite at coordinates VX, VY with a
                                // width of 8 pixels and a height of N pixels
void opcode_EX9E(
    CHIP8 *chip);  // Skip next instruction if the key stored in VX is pressed
void opcode_EXA1(CHIP8 *chip);  // Skip next instruction if the key stored in VX
                                // isn't pressed
void opcode_FX07(CHIP8 *chip);  // Set VX to the value of the delay timer
void opcode_FX0A(CHIP8 *chip);  // Wait for a key press and store the key in VX
void opcode_FX15(CHIP8 *chip);  // Set the delay timer to the value in VX
void opcode_FX18(CHIP8 *chip);  // Set the sound timer to the value in VX
void opcode_FX1E(CHIP8 *chip);  // Add the value stored in VX to I; VF is set to
                                // 1 if there's a range overflow, else 0
void opcode_FX29(CHIP8 *chip);  // Set I to the location of the sprite for the
                                // character in VX
void opcode_FX33(CHIP8 *chip);  // Store the binary-coded decimal representation
                                // of VX at address I, I+1, and I+2
void opcode_FX55(
    CHIP8 *chip);  // Store V0 to VX (inclusive) in memory starting at address I
void opcode_FX65(CHIP8 *chip);  // Fill V0 to VX (inclusive) with values from
                                // memory starting at address I

#endif