#include "chip8.h"

long get_file_size(FILE* rom_file) {
  if (rom_file == NULL) {
    printf("open rom error\n");
    return 1;
  }
  fseek(rom_file, 0L, SEEK_END);
  long file_size = ftell(rom_file);
  rewind(rom_file);
  return file_size;
}

void print_hex(uint8_t* buffer, size_t size) {
  for (int i = 0; i < size; i++) {
    printf("%02X ", buffer[i]);
    if ((i + 1) % 16 == 0) {
      printf("\n");
    }
  }
  printf("\n");
}

CHIP8 *chip8_init() {
  CHIP8 *chip8 = malloc(sizeof(CHIP8));
  memset(chip8, 0, sizeof(CHIP8));
  chip8->pc = MEM_START;
  return chip8;
}

uint8_t chip8_load_rom(CHIP8 *chip8, char *rom_name) {
  FILE *rom_file = fopen(rom_name, "r");
  long file_size = get_file_size(rom_file);
  if (file_size >= MEM_SIZE - MEM_START) {
    printf("memory overflow");
    return 0;
  }

  uint8_t *rom = chip8->mem + MEM_START;  // 512
  size_t result = fread(rom, 1, file_size, rom_file);
  if (result != file_size) {
    printf("read rom error\n");
    return 0;
  }
  printf("rom size: %ld\n", file_size);
  print_hex(rom, file_size);
  fclose(rom_file);
  return 1;
}

/**
 * Clear the screen
 */
void opcode_00E0(CHIP8 *chip8) { printf("clear \n"); }

/**
 * Return from a subroutine
 */
void opcode_00EE(CHIP8 *chip8) {
  byte pc = chip8->stack[chip8->sp--];
  chip8->pc = pc;
}

/**
 * Jump to address NNN
 */
void opcode_1NNN(CHIP8 *chip8) {
  chip8->pc = NNN(_OPCODE);
  printf("goto %04X\n", NNN(_OPCODE));
}

/**
 * Call subroutine at NNN
 */
void opcode_2NNN(CHIP8 *chip8) {
  chip8->stack[chip8->sp++] = chip8->pc;
  chip8->pc = NNN(_OPCODE);
}

/**
 * Skip next instruction if VX equals NN
 */
void opcode_3XNN(CHIP8 *chip8) {
  if (chip8->reg[X(_OPCODE)] == NN(_OPCODE)) {
    chip8->pc += 2;
  }
}

/**
 * Skip next instruction if VX doesn't equal NN
 */
void opcode_4XNN(CHIP8 *chip8) {
  if (chip8->reg[X(_OPCODE)] != NN(_OPCODE)) {
    chip8->pc += 2;
  }
}

/**
 * Skip next instruction if VX equals VY
 */
void opcode_5XY0(CHIP8 *chip8) {
  if (chip8->reg[X(_OPCODE)] == chip8->reg[Y(_OPCODE)]) {
    chip8->pc += 2;
  }
}

/**
 * Set VX to NN
 */
void opcode_6XNN(CHIP8 *chip8) {
  chip8->reg[X(_OPCODE)] = NN(_OPCODE);
  printf("set V%d: %02X\n", X(_OPCODE), NN(_OPCODE));
}

/**
 * Add NN to VX
 */
void opcode_7XNN(CHIP8 *chip8) {
  chip8->reg[X(_OPCODE)] += NN(_OPCODE);
  printf("add V%d: %02X\n", X(_OPCODE), NN(_OPCODE));
}

/**
 * Set VX to the value of VY
 */
void opcode_8XY0(CHIP8 *chip8) {}

/**
 * Set VX to VX OR VY
 */
void opcode_8XY1(CHIP8 *chip8) {}

/**
 * Set VX to VX AND VY
 */
void opcode_8XY2(CHIP8 *chip8) {}

/**
 * Set VX to VX XOR VY
 */
void opcode_8XY3(CHIP8 *chip8) {}

/**
 * Add VY to VX; VF is set to 1 if there's a carry, else 0
 */
void opcode_8XY4(CHIP8 *chip8) {}

/**
 * Subtract VY from VX; VF is set to 0 if there's a borrow, else 1
 */
void opcode_8XY5(CHIP8 *chip8) {}

/**
 * Store the least significant bit of VX in VF and then shift VX to the right by
 * 1
 */
void opcode_8XY6(CHIP8 *chip8) {}

/**
 * Set VX to VY minus VX; VF is set to 0 if there's a borrow, else 1
 */
void opcode_8XY7(CHIP8 *chip8) {}

/**
 * Store the most significant bit of VX in VF and then shift VX to the left by 1
 */
void opcode_8XYE(CHIP8 *chip8) {}

/**
 * Skip next instruction if VX doesn't equal VY
 */
void opcode_9XY0(CHIP8 *chip8) {
  if (chip8->reg[X(_OPCODE)] != chip8->reg[Y(_OPCODE)]) {
    chip8->pc += 2;
  }
}

/**
 * Set I to the address NNN
 */
void opcode_ANNN(CHIP8 *chip8) {
  chip8->index_reg = NNN(_OPCODE);
  printf("set I: %04X\n", NNN(_OPCODE));
}

/**
 * Jump to the address NNN plus V0
 */
void opcode_BNNN(CHIP8 *chip8) {}

/**
 * Set VX to the result of a bitwise AND operation on a random number and NN
 */
void opcode_CXNN(CHIP8 *chip8) {}

/**
 * Draw a sprite at coordinates VX, VY with a width of 8 pixels and a height of
 * N pixels
 */
void opcode_DXYN(CHIP8 *chip8) {
  chip8->reg[0xF] = 0;
  int x = X(_OPCODE);
  int y = Y(_OPCODE);
  int n = N(_OPCODE);
  printf("draw(%d,%d,%d) %04X\n", chip8->reg[x] & (DISPLAY_WIDTH - 1),
         chip8->reg[y] & (DISPLAY_HEIGHT - 1), n, chip8->index_reg);
  // The starting position of the sprite will wrap
  byte start_x = chip8->reg[x] & (DISPLAY_WIDTH - 1);
  byte start_y = chip8->reg[y] & (DISPLAY_HEIGHT - 1);
  uint16_t index = chip8->index_reg;
  for (byte height = 0; height < n; height++) {
    for (byte width = 0; width < 8; width++) {
      // draw every byte
      byte mask = 1 << (7 - width);
      byte pixel = (chip8->mem[index] & mask) >> (7 - width);
      // clip sprite out of edge
      byte cur_x = start_x + width;
      byte cur_y = start_y + height;
      if (cur_x >= DISPLAY_WIDTH || cur_y >= DISPLAY_HEIGHT) {
        continue;
      }
      // VF is set to 1 if any screen pixels are flipped from set to unset
      // when the sprite is drawn
      if (chip8->display[cur_y][cur_x] && pixel) {
        chip8->reg[0xF] = 0;
      }
      chip8->display[cur_y][cur_x] ^= pixel;
    }
    index++;
  }
}

/**
 * Skip next instruction if the key stored in VX is pressed
 */
void opcode_EX9E(CHIP8 *chip8) {}

/**
 * Skip next instruction if the key stored in VX isn't pressed
 */
void opcode_EXA1(CHIP8 *chip8) {}

/**
 * Set VX to the value of the delay timer
 */
void opcode_FX07(CHIP8 *chip8) {}

/**
 * Wait for a key press and store the key in VX
 */
void opcode_FX0A(CHIP8 *chip8) {}

/**
 * Set the delay timer to the value in VX
 */
void opcode_FX15(CHIP8 *chip8) {}

/**
 * Set the sound timer to the value in VX
 */
void opcode_FX18(CHIP8 *chip8) {}

/**
 * Add the value stored in VX to I; VF is set to 1 if there's a range overflow,
 * else 0
 */
void opcode_FX1E(CHIP8 *chip8) {}

/**
 * Set I to the location of the sprite for the character in VX
 */
void opcode_FX29(CHIP8 *chip8) {}

/**
 * Store the binary-coded decimal representation of VX at address I, I+1, and
 * I+2
 */
void opcode_FX33(CHIP8 *chip8) {}

/**
 * Store V0 to VX (inclusive) in memory starting at address I
 */
void opcode_FX55(CHIP8 *chip8) {}

/**
 * Fill V0 to VX (inclusive) with values from memory starting at address I
 */
void opcode_FX65(CHIP8 *chip8) {}
