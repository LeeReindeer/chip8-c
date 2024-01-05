#include "port.h"

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *texture;

/**
 * @brief init SDL2 windows
 * @note
 * @param  *title: window title
 * @param  scale: window scale of chip-9 origin width and height
 * @param  width: chip-8 screen width
 * @param  height: chip-8 screen height
 * @retval None
 */
void init_display(const char *title, int scale, int width, int height) {
  SDL_Init(SDL_INIT_VIDEO);
  window =
      SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       width * scale, height * scale, SDL_WINDOW_SHOWN);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                              SDL_TEXTUREACCESS_STREAMING, width, height);
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
uint8_t handle_keypad(uint8_t *keys) {
  uint8_t flag = 1;
  SDL_Event e;
  if (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      flag = 0;
    }
    if (e.type == SDL_KEYDOWN) {
      switch (e.key.keysym.sym) {
        case SDLK_ESCAPE:
          flag = 0;
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
          // printf("key up: %c\n", KEY_MAP[i]);
          keys[i] = 0;
        }
      }
    }
  }
  return flag;
}

void handle_sound(uint8_t beep) {}