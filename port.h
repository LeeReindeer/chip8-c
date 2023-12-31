#ifndef __PORT_H__
#define __PORT_H__

// macos `brew install SDL2`
#include <SDL2/SDL.h>
#include "chip8.h"
/**
Keypad       Keyboard
+-+-+-+-+    +-+-+-+-+
|1|2|3|C|    |1|2|3|4|
+-+-+-+-+    +-+-+-+-+
|4|5|6|D|    |Q|W|E|R|
+-+-+-+-+ => +-+-+-+-+
|7|8|9|E|    |A|S|D|F|
+-+-+-+-+    +-+-+-+-+
|A|0|B|F|    |Z|X|C|V|
+-+-+-+-+    +-+-+-+-+
 */
const static uint8_t KEY_MAP[KEY_SIZE] = {
    SDLK_x,  // 0
    SDLK_1,  // 1
    SDLK_2,  // 2
    SDLK_3,  // 3
    SDLK_q,  // 4
    SDLK_w,  // 5
    SDLK_e,  // 6
    SDLK_a,  // 7
    SDLK_s,  // 8
    SDLK_d,  // 9
    SDLK_z,  // A
    SDLK_c,  // B
    SDLK_4,  // C
    SDLK_r,  // D
    SDLK_f,  // E
    SDLK_v   // F
};

uint8_t init_display(const char *title, int scale, int width, int height);

void close_display();

void handle_display(void *display, int pitch);

void handle_keypad(uint8_t *keys, CHIP8 *chip8);

void handle_sound();

#endif  //__PORT_H__