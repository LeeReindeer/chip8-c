#include "emulator.h"
#include "chip8.h"
#include <sys/time.h>

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

long current_millis() {
  struct timeval time;
  gettimeofday(&time, NULL);
  return time.tv_sec * 1000 + time.tv_usec / 1000;
}

long current_micros() {
  struct timeval time;
  gettimeofday(&time, NULL);
  return time.tv_sec * 1000000 + time.tv_usec;
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
  long last_cycle_time = current_micros();
  long last_timer_time = current_micros();
  while (1) {
    long now = current_micros();
    // todo get any key pressed
    if (now - last_cycle_time >= CYCLE_DELAY) {
      last_cycle_time = now;
      chip8_cycle(chip8);
      // todo update display
    }
    if (now - last_timer_time >= TIMER_DELAY) {
      chip8_timer(chip8);
    }
    print_display();
  }
  return 0;
}
