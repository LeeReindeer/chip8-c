#include "port.h"

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *texture;

/**
 * @brief init SDL2 windows
 * @note
 * @param  *title: window title
 * @param  scale: window scale of chip-8 origin width and height
 * @param  width: chip-8 screen width
 * @param  height: chip-8 screen height
 * @retval None
 */
uint8_t init_display(const char *title, int scale, int width, int height) {
  SDL_Init(SDL_INIT_VIDEO);
  window =
      SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       width * scale, height * scale, SDL_WINDOW_SHOWN);
  if (!window) {
    return 0;
  }
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    return 0;
  }
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                              SDL_TEXTUREACCESS_STREAMING, width, height);
  if (!texture) {
    return 0;
  }
  return 1;
}

/**
 * @brief destory SDL2 window
 * @note
 * @retval None
 */
void close_display() {
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyTexture(texture);
  SDL_Quit();
}

/**
 * @brief update display
 * @note
 * @param  *display: dispaly martrix
 * @param  pitch: the number of bytes in a row of pixel data
 * @retval None
 */
void handle_display(void *display, int pitch) {
  SDL_UpdateTexture(texture, NULL, display, pitch);
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);
}

/**
 * @brief handle modern computer keyboard
 * @note
 * @param  *keys: key array
 * @retval 1 for continue, 0 for quit
 */
void handle_keypad(uint8_t *keys, CHIP8 *chip8) {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      chip8->state = SYS_QUIT;
    }
    if (e.type == SDL_KEYDOWN) {
      switch (e.key.keysym.sym) {
        case SDLK_ESCAPE:
          chip8->state = SYS_QUIT;
          break;
        case SDLK_SPACE:
          if (chip8->state == SYS_PAUSE) {
            chip8->state = SYS_RUNNING;
          } else {
            chip8->state = SYS_PAUSE;
          }
          break;
        default:
          for (int i = 0; i < KEY_SIZE; i++) {
            if (KEY_MAP[i] == e.key.keysym.sym) {
              // printf("key press: %c\n", KEY_MAP[i]);
              keys[i] = 1;
            }
          }
          break;
      }
    } else if (e.type == SDL_KEYUP) {
      for (int i = 0; i < KEY_SIZE; i++) {
        if (KEY_MAP[i] == e.key.keysym.sym) {
          keys[i] = 0;
        }
      }
    }
  }
}

void handle_sound() {}