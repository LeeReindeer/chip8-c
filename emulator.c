#include <stdio.h>
#include <stdlib.h>

#include "chip8.h"

CHIP8 chip8 = {0};

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

void print_display() {
  for (byte row = 0; row < DISPLAY_HEIGHT; row++) {
    for (byte col = 0; col < DISPLAY_WIDTH; col++) {
      if (chip8.display[row][col]) {
        printf("*");
      } else {
        printf(" ");
      }
    }
    printf("\n");
  }
}

int main(int argc, char const* argv[]) {
  // 1. load ROM file
  printf("argc:%d\n", argc);
  char* rom_name;
  if (argc == 2) {
    rom_name = argv[1];
  } else {
    rom_name = "roms/IBM Logo.ch8";
  }

  FILE* rom_file = fopen(rom_name, "r");
  long file_size = get_file_size(rom_file);
  if (file_size >= MEM_SIZE) {
    printf("memory overflow");
    return 1;
  }

  uint8_t* rom = chip8.mem + 0x200;  // 512
  size_t result = fread(rom, 1, file_size, rom_file);
  if (result != file_size) {
    printf("read rom error\n");
    return 1;
  }
  printf("rom size: %ld\n", file_size);
  print_hex(rom, file_size);
  fclose(rom_file);

  // 2. read opcode(16 bits)
  // 3. execute opcode
  chip8.pc = 0x200;
  int i = 100;
  while (i--) {
    uint16_t opcode =
        (0xFF00 & (chip8.mem[chip8.pc] << 8)) | chip8.mem[chip8.pc + 1];
    // printf("pc: 0x%04X\n", chip8.pc);
    chip8.pc += 2;
    // printf("opcode: 0x%04X\n", opcode);
    byte type = (0xF000 & opcode) >> 12;
    // printf("type: %02x\n", type);
    byte x = (0x0F00 & opcode) >> 8;
    byte y = (0x00F0 & opcode) >> 4;
    byte n = 0x000F & opcode;
    byte nn = 0x00FF & opcode;
    uint16_t nnn = 0x0FFF & opcode;
    switch (type) {
      case 0x0:
        if (opcode == 0x00E0) {
          printf("clear \n");
        } else if (opcode == 0x00EE) {
          // 00EE return;
          byte pc = chip8.stack[chip8.sp--];
          chip8.pc = pc;
        }
        break;
      case 0x1:
        // 1NNN goto NNN;
        chip8.pc = nnn;
        printf("goto %04X\n", nnn);
        break;
      case 0x2:
        // 2NNN *(0xNNN)();
        chip8.stack[chip8.sp++] = chip8.pc;
        chip8.pc = nnn;
        break;
      case 0x3:
        // 3XNN if (Vx == NN)
        if (chip8.reg[x] == nn) {
          chip8.pc += 2;
        }
        break;
      case 0x4:
        // 4XNN if (Vx != NN)
        if (chip8.reg[x] != nn) {
          chip8.pc += 2;
        }
        break;
      case 0x5:
        // 5XY0 if (Vx == Vy)
        if (chip8.reg[x] == chip8.reg[y]) {
          chip8.pc += 2;
        }
        break;
      case 0x9:
        // 9XY0 if (Vx != Vy)
        if (chip8.reg[x] != chip8.reg[y]) {
          chip8.pc += 2;
        }
        break;
      case 0x6:
        // 6XNN Vx = NN
        chip8.reg[x] = nn;
        printf("set V%d: %02X\n", x, nn);
        break;
      case 0x7:
        // 7XNN Vx += NN, not affect VF
        chip8.reg[x] += nn;
        printf("add V%d: %02X\n", x, nn);
        break;
      case 0xA:
        // ANNN I = NNN
        chip8.index_reg = nnn;
        printf("set I: %04X\n", nnn);
        break;
      case 0xD:
        // DXYN draw(Vx, Vy, N)
        // Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels
        // and a height of N pixels.
        printf("draw(%d,%d,%d) %04X\n", chip8.reg[x] & (DISPLAY_WIDTH - 1),
               chip8.reg[y] & (DISPLAY_HEIGHT - 1), n, chip8.index_reg);
        // The starting position of the sprite will wrap
        byte start_x = chip8.reg[x] & (DISPLAY_WIDTH - 1);
        byte start_y = chip8.reg[y] & (DISPLAY_HEIGHT - 1);
        uint16_t index = chip8.index_reg;
        for (byte height = 0; height < n; height++) {
          for (byte width = 0; width < 8; width++) {
            // draw every byte
            byte mask = 1 << (7 - width);
            byte pixel = (chip8.mem[index] & mask) >> (7 - width);
            // clip sprite out of edge
            if (start_x + width >= DISPLAY_WIDTH ||
                start_y + height >= DISPLAY_HEIGHT) {
              continue;
            }
            chip8.display[start_y + height][start_x + width] = pixel;
          }
          index++;
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
