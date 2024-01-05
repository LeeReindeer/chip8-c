#include "chip8.h"

#include <time.h>

#include "port.h"

uint8_t chip8_fontset[FONTSET_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
    0x20, 0x60, 0x20, 0x20, 0x70,  // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
    0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
    0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
    0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
    0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
    0xF0, 0x80, 0xF0, 0x80, 0x80   // F
};

long get_file_size(FILE *rom_file) {
  if (rom_file == NULL) {
    printf("open rom error\n");
    return 1;
  }
  fseek(rom_file, 0L, SEEK_END);
  long file_size = ftell(rom_file);
  rewind(rom_file);
  return file_size;
}

void print_hex(uint8_t *buffer, size_t size) {
  for (int i = 0; i < size; i++) {
    printf("%02X ", buffer[i]);
    if ((i + 1) % 16 == 0) {
      printf("\n");
    }
  }
  printf("\n");
}

CHIP8 *chip8_init() {
  srand((unsigned)time(NULL));
  CHIP8 *chip8 = malloc(sizeof(CHIP8));
  memset(chip8, 0, sizeof(CHIP8));
  chip8->pc = MEM_START;
  // init font
  for (int i = 0; i < FONTSET_SIZE; i++) {
    chip8->mem[FONTSET_MEM_START + i] = chip8_fontset[i];
  }
  // init timers
  return chip8;
}

uint8_t chip8_load_rom(CHIP8 *chip8, const char *rom_name) {
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
  // printf("rom size: %ld\n", file_size);
  // print_hex(rom, file_size);
  fclose(rom_file);
  return 1;
}

void chip8_cycle(CHIP8 *chip8) {
  // fetch and execute
  uint16_t opcode =
      (0xFF00 & (chip8->mem[chip8->pc] << 8)) | chip8->mem[chip8->pc + 1];
  chip8->opcode = opcode;
  chip8->pc += 2;
  byte type = (0xF000 & opcode) >> 12;
  switch (type) {
    case 0x0:
      if (opcode == 0x00E0) {
        OPCODE(00E0);
      } else if (opcode == 0x00EE) {
        // 00EE return;
        OPCODE(00EE);
      }
      break;
    case 0x1:
      // 1NNN goto NNN;
      OPCODE(1NNN);
      break;
    case 0x2:
      // 2NNN *(0xNNN)();
      OPCODE(2NNN);
      break;
    case 0x3:
      // 3XNN if (Vx == NN)
      OPCODE(3XNN);
      break;
    case 0x4:
      // 4XNN if (Vx != NN)
      OPCODE(4XNN);
      break;
    case 0x5:
      // 5XY0 if (Vx == Vy)
      OPCODE(5XY0);
      break;
    case 0x6:
      // 6XNN Vx = NN
      OPCODE(6XNN);
      break;
    case 0x7:
      // 7XNN Vx += NN, not affect VF
      OPCODE(7XNN);
      break;
    case 0x8:
      switch (N(_OPCODE)) {
        case 0x0:
          OPCODE(8XY0);
          break;
        case 0x1:
          OPCODE(8XY1);
          break;
        case 0x2:
          OPCODE(8XY2);
          break;
        case 0x3:
          OPCODE(8XY3);
          break;
        case 0x4:
          OPCODE(8XY4);
          break;
        case 0x5:
          OPCODE(8XY5);
          break;
        case 0x6:
          OPCODE(8XY6);
          break;
        case 0x7:
          OPCODE(8XY7);
          break;
        case 0xE:
          OPCODE(8XYE);
          break;
      }
      break;
    case 0x9:
      // 9XY0 if (Vx != Vy)
      OPCODE(9XY0);
      break;
    case 0xA:
      // ANNN I = NNN
      OPCODE(ANNN);
      break;
    case 0xB:
      OPCODE(BNNN);
      break;
    case 0xC:
      OPCODE(CXNN);
      break;
    case 0xD:
      // DXYN draw(Vx, Vy, N)
      // Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels
      // and a height of N pixels.
      OPCODE(DXYN);
      break;
    case 0xE:
      if (NN(_OPCODE) == 0x9E) {
        OPCODE(EX9E);
      } else if (NN(_OPCODE) == 0xA1) {
        OPCODE(EXA1);
      }
      break;
    case 0xF:
      switch (NN(_OPCODE)) {
        case 0x07:
          OPCODE(FX07);
          break;
        case 0x0A:
          // FX0A Vx = get_key()
          OPCODE(FX0A);
          break;
        case 0x15:
          OPCODE(FX15);
          break;
        case 0x18:
          OPCODE(FX18);
          break;
        case 0x1E:
          OPCODE(FX1E);
          break;
        case 0x29:
          OPCODE(FX29);
          break;
        case 0x33:
          OPCODE(FX33);
          break;
        case 0x55:
          OPCODE(FX55);
          break;
        case 0x65:
          OPCODE(FX65);
          break;
      }
      break;
    default:
      break;
  }
}

void chip8_timer(CHIP8 *chip8) {
  // update timers
  if (chip8->delay_timer > 0) {
    chip8->delay_timer--;
  }
  if (chip8->sound_timer > 0) {
    chip8->sound_timer--;
  }
}

/**
 * Clear the screen
 */
void opcode_00E0(CHIP8 *chip8) {
  memset(chip8->display, 0, sizeof(chip8->display));
}

/**
 * Return from a subroutine
 */
void opcode_00EE(CHIP8 *chip8) {
  // attention please: decrease sp first
  chip8->pc = chip8->stack[--(chip8->sp)];
}

/**
 * Jump to address NNN
 */
void opcode_1NNN(CHIP8 *chip8) {
  chip8->pc = NNN(_OPCODE);
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
  if (VX(_OPCODE) == NN(_OPCODE)) {
    chip8->pc += 2;
  }
}

/**
 * Skip next instruction if VX doesn't equal NN
 */
void opcode_4XNN(CHIP8 *chip8) {
  if (VX(_OPCODE) != NN(_OPCODE)) {
    chip8->pc += 2;
  }
}

/**
 * Skip next instruction if VX equals VY
 */
void opcode_5XY0(CHIP8 *chip8) {
  if (VX(_OPCODE) == VY(_OPCODE)) {
    chip8->pc += 2;
  }
}

/**
 * Set VX to NN
 */
void opcode_6XNN(CHIP8 *chip8) {
  VX(_OPCODE) = NN(_OPCODE);
  // printf("set V%d: %02X\n", X(_OPCODE), NN(_OPCODE));
}

/**
 * Add NN to VX
 */
void opcode_7XNN(CHIP8 *chip8) {
  VX(_OPCODE) += NN(_OPCODE);
  // printf("add V%d: %02X\n", X(_OPCODE), NN(_OPCODE));
}

/**
 * Set VX to the value of VY
 */
void opcode_8XY0(CHIP8 *chip8) { VX(_OPCODE) = VY(_OPCODE); }

/**
 * Set VX to VX OR VY
 */
void opcode_8XY1(CHIP8 *chip8) { VX(_OPCODE) |= VY(_OPCODE); }

/**
 * Set VX to VX AND VY
 */
void opcode_8XY2(CHIP8 *chip8) { VX(_OPCODE) &= VY(_OPCODE); }

/**
 * Set VX to VX XOR VY
 */
void opcode_8XY3(CHIP8 *chip8) { VX(_OPCODE) ^= VY(_OPCODE); }

/**
 * Add VY to VX; VF is set to 1 if there's a carry, else 0
 */
void opcode_8XY4(CHIP8 *chip8) {
  uint16_t add = (uint16_t)VX(_OPCODE) + (uint16_t)VY(_OPCODE);
  VX(_OPCODE) = add & 0xFF;
  // set to 1 if there's a carry
  _VF = (add & 0x0100) >> 8;
}

/**
 * Subtract VY from VX; VF is set to 0 if there's a borrow, else 1
 */
void opcode_8XY5(CHIP8 *chip8) {
  if (VX(_OPCODE) > VY(_OPCODE)) {
    _VF = 1;
  } else {
    _VF = 0;
  }
  VX(_OPCODE) -= VY(_OPCODE);
}

/**
 * Store the least significant bit of VX in VF and then shift VX to the right by
 * 1
 */
void opcode_8XY6(CHIP8 *chip8) {
  _VF = VX(_OPCODE) & 0x01;
  VX(_OPCODE) >>= 1;
}

/**
 * Set VX to VY minus VX; VF is set to 0 if there's a borrow, else 1
 */
void opcode_8XY7(CHIP8 *chip8) {
  if (VX(_OPCODE) < VY(_OPCODE)) {
    _VF = 1;
  } else {
    _VF = 0;
  }
  VX(_OPCODE) = VY(_OPCODE) - VX(_OPCODE);
}

/**
 * Store the most significant bit of VX in VF and then shift VX to the left by 1
 */
void opcode_8XYE(CHIP8 *chip8) {
  _VF = (VX(_OPCODE) * 0x80) >> 7;
  VX(_OPCODE) <<= 1;
}

/**
 * Skip next instruction if VX doesn't equal VY
 */
void opcode_9XY0(CHIP8 *chip8) {
  if (VX(_OPCODE) != VY(_OPCODE)) {
    chip8->pc += 2;
  }
}

/**
 * Set I to the address NNN
 */
void opcode_ANNN(CHIP8 *chip8) {
  chip8->index_reg = NNN(_OPCODE);
  // printf("set I: %04X\n", NNN(_OPCODE));
}

/**
 * Jump to the address NNN plus V0
 */
void opcode_BNNN(CHIP8 *chip8) { chip8->pc = NNN(_OPCODE) + chip8->reg[0]; }

/**
 * Set VX to the result of a bitwise AND operation on a random number and NN
 */
void opcode_CXNN(CHIP8 *chip8) {
  VX(_OPCODE) = NN(_OPCODE) & (uint8_t)(rand() % 256);
}

/**
 * Draw a sprite at coordinates VX, VY with a width of 8 pixels and a height of
 * N pixels
 */
void opcode_DXYN(CHIP8 *chip8) {
  chip8->reg[0xF] = 0;
  int x = X(_OPCODE);
  int y = Y(_OPCODE);
  int n = N(_OPCODE);
  // printf("draw(%d,%d,%d) %04X\n", chip8->reg[x] & (DISPLAY_WIDTH - 1),
  //  chip8->reg[y] & (DISPLAY_HEIGHT - 1), n, chip8->index_reg);
  // The starting position of the sprite will wrap
  byte start_x = chip8->reg[x] & (DISPLAY_WIDTH - 1);
  byte start_y = chip8->reg[y] & (DISPLAY_HEIGHT - 1);
  uint16_t index = chip8->index_reg;
  for (byte height = 0; height < n; height++) {
    for (byte width = 0; width < 8; width++) {
      // draw every byte
      byte pixel = (chip8->mem[index] & (0x80 >> width));
      // clip sprite out of edge
      byte cur_x = start_x + width;
      byte cur_y = start_y + height;
      if (cur_x >= DISPLAY_WIDTH || cur_y >= DISPLAY_HEIGHT) {
        continue;
      }
      // VF is set to 1 if any screen pixels are flipped from set(white) to
      // unset(black) when the sprite is drawn
      // XOR
      if (pixel) {
        if (chip8->display[cur_y][cur_x] == DISPLAY_WHITE) {
          chip8->reg[0xF] = 1;
          chip8->display[cur_y][cur_x] = DISPLAY_BLACK;
        } else {
          chip8->display[cur_y][cur_x] = DISPLAY_WHITE;
        }
        chip8->display_refresh_flag = 1;
      }
    }
    index++;
  }
}

/**
 * Skip next instruction if the key stored in VX is pressed
 */
void opcode_EX9E(CHIP8 *chip8) {
  if (VX(_OPCODE) >= 16) {
    return;
  }
  if (chip8->keys[VX(_OPCODE)]) {
    chip8->pc += 2;
  }
}

/**
 * Skip next instruction if the key stored in VX isn't pressed
 */
void opcode_EXA1(CHIP8 *chip8) {
  if (VX(_OPCODE) >= 16) {
    return;
  }
  if (!(chip8->keys[VX(_OPCODE)])) {
    chip8->pc += 2;
  }
}

/**
 * Set VX to the value of the delay timer
 */
void opcode_FX07(CHIP8 *chip8) { VX(_OPCODE) = chip8->delay_timer; }

/**
 * Block instruction
 *
 * Wait for a key press and store the key in VX
 * Timers continues counting
 */
void opcode_FX0A(CHIP8 *chip8) {
  byte key_pressed = 0;
  for (byte i = 0; i < 0xF; i++) {
    if (chip8->keys[i]) {
      key_pressed = 1;
      VX(_OPCODE) = i;
    }
  }
  if (!key_pressed) {
    // loop forever to wait
    chip8->pc -= 2;
  }
}

/**
 * Set the delay timer to the value in VX
 */
void opcode_FX15(CHIP8 *chip8) { chip8->delay_timer = VX(_OPCODE); }

/**
 * Set the sound timer to the value in VX
 */
void opcode_FX18(CHIP8 *chip8) { chip8->sound_timer = VX(_OPCODE); }

/**
 * Add the value stored in VX to I;
 */
void opcode_FX1E(CHIP8 *chip8) { _I += VX(_OPCODE); }

/**
 * Set I to the location of the sprite for the character in VX
 * Characters 0-F (in hexadecimal) are represented by a 8x5 font.
 */
void opcode_FX29(CHIP8 *chip8) {
  _I = chip8->mem[FONTSET_MEM_START + 5 * VX(_OPCODE)];
}

/**
 * Store the binary-coded decimal representation of VX at address I, I+1, and
 * I+2
 */
void opcode_FX33(CHIP8 *chip8) {
  int x = VX(_OPCODE);
  byte one = x % 10u;
  byte ten = x / 10u % 10u;
  byte hund = x / 100u % 10u;
  chip8->mem[_I] = one;
  chip8->mem[_I + 1] = ten;
  chip8->mem[_I + 2] = hund;
}

/**
 * Store V0 to VX (inclusive) in memory starting at address I
 */
void opcode_FX55(CHIP8 *chip8) {
  for (int i = 0; i <= X(_OPCODE); i++) {
    chip8->mem[_I + i] = chip8->reg[i];
  }
}

/**
 * Fill V0 to VX (inclusive) with values from memory starting at address I
 */
void opcode_FX65(CHIP8 *chip8) {
  for (int i = 0; i <= X(_OPCODE); i++) {
    chip8->reg[i] = chip8->mem[_I + i];
  }
}
