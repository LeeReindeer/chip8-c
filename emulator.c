#include "emulator.h"

#include "chip8.h"

CHIP8* chip8;

void print_display() {
  for (byte row = 0; row < DISPLAY_HEIGHT; row++) {
    for (byte col = 0; col < DISPLAY_WIDTH; col++) {
      if (chip8->display[row][col]) {
        printf("*");
      } else {
        printf(" ");
      }
    }
    printf("\n");
  }
}

int main(int argc, char const* argv[]) {
  chip8 = chip8_init();
  if (!chip8) {
    return -1;
  }
  // 1. load ROM file
  char* rom_name;
  if (argc == 2) {
    rom_name = argv[1];
  } else {
    rom_name = "roms/IBM Logo.ch8";
  }
  if (!chip8_load_rom(chip8, rom_name)) {
    return -1;
  }
  // 2. read opcode(16 bits)
  // 3. execute opcode
  int i = 100;
  while (i--) {
    uint16_t opcode =
        (0xFF00 & (chip8->mem[chip8->pc] << 8)) | chip8->mem[chip8->pc + 1];
    chip8->opcode = opcode;
    // printf("pc: 0x%04X\n", chip8->pc);
    chip8->pc += 2;
    // printf("opcode: 0x%04X\n", opcode);
    byte type = (0xF000 & opcode) >> 12;
    // printf("type: %02x\n", type);
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

  // test
  print_display();
  return 0;
}
