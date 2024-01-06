#include "emulator.h"

#include <sys/time.h>
#include <unistd.h>

#include "chip8.h"
#include "port.h"

static CHIP8* chip8;

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

  if (!init_display("CHIP-8", 10, DISPLAY_WIDTH, DISPLAY_HEIGHT)) {
    return -1;
  }

  long last_cycle_time = current_micros();
  long last_timer_time = current_micros();
  // 2. emulator loop
  while (chip8->state) {
    // 2.0 handle user input
    do {
      handle_keypad(chip8->keys, chip8);
      // pause system if you press spacebar, press again to continue
    } while (chip8->state == SYS_PAUSE);

    long now = current_micros();
    // 2.1 update timer
    if (now - last_timer_time >= TIMER_DELAY) {
      last_timer_time = now;
      chip8_timer(chip8);
    }
    if (now - last_cycle_time >= CYCLE_DELAY(frequency)) {
      last_cycle_time = now;
      // 2.2 fetch/decode/execute instruction
      chip8_cycle(chip8);
      // 2.3 refresh display
      if (chip8->display_refresh_flag) {
        handle_display(chip8->display, sizeof(chip8->display[0]));
      }
      if (chip8->sound_timer > 0) {
        handle_sound();
      }
    }
    // not busy running
    usleep(100);
  }
  close_display();
  free(chip8);
  return 0;
}
