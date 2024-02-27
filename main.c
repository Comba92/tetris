#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// 4:3 = 640x480, 800x600, 1024x768 -  16:9 = 1280x720, 1920x1080

typedef struct {
  int width;
  int height;
  bool* blocks;
} Grid;

typedef enum {
  tT, tS, tZ, tI, tJ, tL, tO
} Tetromino;

static const unsigned int tetrominoes[7][4] = {
  { 0x4640, 0x0E40, 0x4C40, 0x4E00 }, // 'T'
  { 0x8C40, 0x6C00, 0x8C40, 0x6C00 }, // 'S'
  { 0x4C80, 0xC600, 0x4C80, 0xC600 }, // 'Z'
  { 0x4444, 0x0F00, 0x4444, 0x0F00 }, // 'I'
  { 0x44C0, 0x8E00, 0xC880, 0xE200 }, // 'J'
  { 0x88C0, 0xE800, 0xC440, 0x2E00 }, // 'L'
  { 0xCC00, 0xCC00, 0xCC00, 0xCC00 }  // 'O'
};

int main() {
  InitWindow(800, 600, "Tetris");
  SetTargetFPS(30);

  while(!WindowShouldClose()) {
    BeginDrawing();
      ClearBackground(BLACK);

      for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
          if (tetrominoes[tT][0] & (0x8000 >> (y * 4 + x))) {
            DrawRectangleLines(x * 20, y * 20, 20, 20, WHITE);
          }
        }
      }
    EndDrawing();
  }

  CloseWindow();
}