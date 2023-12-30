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
      case 0x9:
        // 9XY0 if (Vx != Vy)
        OPCODE(9XY0);
        break;
      case 0x6:
        // 6XNN Vx = NN
        OPCODE(6XNN);
        break;
      case 0x7:
        // 7XNN Vx += NN, not affect VF
        OPCODE(7XNN);
        break;
      case 0xA:
        // ANNN I = NNN
        OPCODE(ANNN);
        break;
      case 0xD:
        // DXYN draw(Vx, Vy, N)
        // Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels
        // and a height of N pixels.
        OPCODE(DXYN);
        break;
      default:
        break;
    }
  }

  // test
  print_display();
  return 0;
}
