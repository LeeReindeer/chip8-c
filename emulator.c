#include "emulator.h"

#include <sys/time.h>

#include "chip8.h"
#include "port.h"

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
  const char* rom_name;
  int frequency = CYCLE_FREQUENCY;
  if (argc == 3) {
    frequency = atoi(argv[1]);
    rom_name = argv[2];
  } else {
    printf("usage: ./emulator <frequency> <rom name>\n");
    return -1;
  }
  if (!chip8_load_rom(chip8, rom_name)) {
    return -1;
  }

  init_display("CHIP-8", 10, DISPLAY_WIDTH, DISPLAY_HEIGHT);

  long last_cycle_time = current_micros();
  long last_timer_time = current_micros();
  uint8_t flag = 1;
  while (flag) {
    // get any key pressed
    flag = handle_keypad(chip8->keys);
    long now = current_micros();
    if (now - last_cycle_time >= CYCLE_DELAY(frequency)) {
      last_cycle_time = now;
      chip8_cycle(chip8);
      // update display
      if (chip8->display_refresh_flag) {
        handle_display(chip8->display, sizeof(chip8->display[0]));
      }
    }
    if (now - last_timer_time >= TIMER_DELAY) {
      last_timer_time = now;
      chip8_timer(chip8);
    }
  }
  close_display();
  return 0;
}
