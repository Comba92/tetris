#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// 4:3 = 640x480, 800x600, 1024x768 -  16:9 = 1280x720, 1920x1080

static const int CELL_SIZE = 20;

typedef struct {
  int width;
  int height;
  bool* blocks;
} Grid;

typedef enum {
  tT, tS, tZ, tI, tJ, tL, tO
} TetrominoType;

typedef enum {
  r0, r90, r180, r270
} Rotation;

typedef struct {
  TetrominoType type;
  Rotation rotation;
  int x;
  int y;
} Tetromino;

static const unsigned int TETROMINOES[7][4] = {
  { 0x4640, 0x0E40, 0x4C40, 0x4E00 }, // 'T'
  { 0x8C40, 0x6C00, 0x8C40, 0x6C00 }, // 'S'
  { 0x4C80, 0xC600, 0x4C80, 0xC600 }, // 'Z'
  { 0x4444, 0x0F00, 0x4444, 0x0F00 }, // 'I'
  { 0x44C0, 0x8E00, 0xC880, 0xE200 }, // 'J'
  { 0x88C0, 0xE800, 0xC440, 0x2E00 }, // 'L'
  { 0xCC00, 0xCC00, 0xCC00, 0xCC00 }  // 'O'
};

typedef void (*KeyCallback)(Tetromino* t);
typedef struct {
  int key;
  KeyCallback callback;
} KeyCommand;

void moveLeft(Tetromino* t)   { t->x -= 1; }
void moveRight(Tetromino* t)  { t->x += 1; }
void moveUp(Tetromino* t)     { t->y -= 1; }
void moveDown(Tetromino* t)   { t->y += 1; }

typedef enum {
  LEFT, RIGHT, UP, DOWN
} Keys;
static const KeyCommand KEY_COMMANDS[4] = {
  (KeyCommand) { .key = KEY_LEFT,   .callback = &moveLeft },
  (KeyCommand) { .key = KEY_RIGHT,  .callback = &moveRight },
  (KeyCommand) { .key = KEY_UP,     .callback = &moveUp },
  (KeyCommand) { .key = KEY_DOWN,   .callback = &moveDown },
};

void handleInputAndUpdateTetromino(Tetromino* t) {
  for(int i=0; i<4; i++) {
    KeyCommand k = KEY_COMMANDS[i];
    if (IsKeyDown(k.key) && k.callback) k.callback(t);
  }
}

Grid initGrid(int width, int height) {
  Grid grid;
  grid.width = width;
  grid.height = height;
  grid.blocks = malloc(width * height * sizeof(bool));

  return grid;
}

Grid updateGrid(Grid grid, Tetromino t) {

}

void drawLines(Grid grid) {
  for (int y = 0; y <= grid.height; y++) {
    DrawLine(0, y * CELL_SIZE, grid.width * CELL_SIZE, y * CELL_SIZE, GRAY);
  }

    for (int x = 0; x <= grid.width; x++) {
    DrawLine(x * CELL_SIZE, 0, x * CELL_SIZE, grid.height * CELL_SIZE, GRAY);
  }
}

void drawSprite(Tetromino t) {
  // Sprite boundaries
  DrawRectangleLines(t.x * CELL_SIZE, t.y * CELL_SIZE, 4 * CELL_SIZE, 4 * CELL_SIZE, RED);

  for (int off_y = 0; off_y < 4; off_y++) {
    for (int off_x = 0; off_x < 4; off_x++) {
      // TODO: extract this condition to function
      if (TETROMINOES[t.type][t.rotation] & (0x8000 >> (off_y * 4 + off_x))) {
        DrawRectangleLines(
          (t.x + off_x) * CELL_SIZE, 
          (t.y + off_y) * CELL_SIZE, 
          CELL_SIZE, CELL_SIZE, WHITE
        );
      }
    }
  }
}

Tetromino newTetromino(TetrominoType type, int x, int y) {
  Tetromino t;
  t.type = type;
  t.rotation = r0;
  t.x = x;
  t.y = y;

  return t;
}

int main() {
  InitWindow(800, 600, "Tetris");
  SetTargetFPS(30);

  Grid grid = initGrid(30, 60);
  Tetromino current = newTetromino(tT, 0, 0);

  while(!WindowShouldClose()) {
    BeginDrawing();
      ClearBackground(BLACK);
      drawLines(grid);
      drawSprite(current);

      handleInputAndUpdateTetromino(&current);
    EndDrawing();
  }

  CloseWindow();
}