#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

// 4:3 = 640x480, 800x600, 1024x768 -  16:9 = 1280x720, 1920x1080

static const int CELL_SIZE = 20;
static const int ANIMATION_FRAME_DELAY = 30;
static const int INPUT_DELAY = 2;

typedef enum {
  tT, tS, tZ, tI, tJ, tL, tO, TETROMINO_TYPES
} TetrominoType;

typedef enum {
  r0, r90, r180, r270, rNONE
} Rotation;

typedef struct {
  TetrominoType type;
  Rotation rotation;
  int x;
  int y;
} Tetromino;


typedef struct {
  int width;
  int height;
  bool* blocks;
} Grid;

typedef struct {
  Grid grid;
  Tetromino* current;
  int gameTimer;
  int inputTimer;
} State;

static const unsigned int TETROMINOES[TETROMINO_TYPES][4] = {
  { 0x4640, 0x0E40, 0x4C40, 0x4E00 }, // 'T'
  { 0x8C40, 0x6C00, 0x8C40, 0x6C00 }, // 'S'
  { 0x4C80, 0xC600, 0x4C80, 0xC600 }, // 'Z'
  { 0x4444, 0x0F00, 0x4444, 0x0F00 }, // 'I'
  { 0x44C0, 0x8E00, 0xC880, 0xE200 }, // 'J'
  { 0x88C0, 0xE800, 0xC440, 0x2E00 }, // 'L'
  { 0xCC00, 0xCC00, 0xCC00, 0xCC00 }  // 'O'
};

typedef enum {
  dLEFT, dRIGHT, dUP, dDOWN, dNONE
} Direction;
static const int DIRECTIONS[5][2] = {
  {-1, 0}, {1, 0}, {0, -1}, {0, 1}, {0, 0}
};

Tetromino* newTetromino(TetrominoType type, int x, int y, Rotation r) {
  Tetromino* t = malloc(sizeof(Tetromino));
  t->type = type;
  t->rotation = r;
  t->x = x;
  t->y = y;

  return t;
}

Tetromino* randomTetromino() {
  srand(time(NULL));

  TetrominoType type = rand() % TETROMINO_TYPES;
  return newTetromino(type, 0, 0, r0);
}

bool isTetrominoBlockSet(Tetromino t, int off_x, int off_y) {
  return TETROMINOES[t.type][t.rotation] & (0x8000 >> (off_y * 4 + off_x));
}

void drawTetromino(Tetromino t) {
  // Sprite boundaries
  DrawRectangleLines(t.x * CELL_SIZE, t.y * CELL_SIZE, 4 * CELL_SIZE, 4 * CELL_SIZE, RED);

  for (int off_y = 0; off_y < 4; off_y++) {
    for (int off_x = 0; off_x < 4; off_x++) {
      bool set = isTetrominoBlockSet(t, off_x, off_y);
      DrawRectangleLines(
        (t.x + off_x) * CELL_SIZE, 
        (t.y + off_y) * CELL_SIZE, 
        CELL_SIZE, CELL_SIZE, set ? WHITE : BLANK
      );
    }
  }
}

Grid initGrid(int width, int height) {
  Grid grid;
  grid.width = width;
  grid.height = height;
  grid.blocks = calloc(width * height, sizeof(bool));

  return grid;
}

int getIdx(Grid grid, int x, int y) {
    return (x < 0 || x >= grid.width || y < 0 || y >= grid.height)
    ? -1 
    : grid.width * y + x;
}

void updateGrid(State s) {
  for (int off_y = 0; off_y < 4; off_y++) {
    for (int off_x = 0; off_x < 4; off_x++) {
      int idx = getIdx(s.grid, s.current->x + off_x, s.current->y + off_y);
      if (idx == -1) continue;

      s.grid.blocks[idx] = isTetrominoBlockSet(*s.current, off_x, off_y) ? true : s.grid.blocks[idx];
    }
  }
}

void drawGrid(Grid grid) {
  for (int y = 0; y < grid.height; y++) {
    for (int x = 0; x < grid.width; x++) {
      int idx = getIdx(grid, x, y);

      bool set = grid.blocks[idx];
      DrawRectangle(
        x * CELL_SIZE, y * CELL_SIZE,
        CELL_SIZE, CELL_SIZE, set ? WHITE : BLANK
      );
    }
  }
}

void drawLines(Grid grid) {
  for (int y = 0; y <= grid.height; y++) {
    DrawLine(0, y * CELL_SIZE, grid.width * CELL_SIZE, y * CELL_SIZE, GRAY);
  }

  for (int x = 0; x <= grid.width; x++) {
    DrawLine(x * CELL_SIZE, 0, x * CELL_SIZE, grid.height * CELL_SIZE, GRAY);
  }
}

bool isTetrominoColliding(State s, Direction d, Rotation r) {
  Tetromino* t = newTetromino(
    s.current->type, 
    s.current->x + DIRECTIONS[d][0],
    s.current->y + DIRECTIONS[d][1],
    r == rNONE ? s.current->rotation : r
  );
  
  for (int off_y = 0; off_y < 4; off_y++) {
    for (int off_x = 0; off_x < 4; off_x++) {
      bool set = isTetrominoBlockSet(*t, off_x, off_y);
      int idx = getIdx(s.grid, t->x + off_x, t->y + off_y);

      if (idx == -1 && set) return true;
      else if (idx != -1 && s.grid.blocks[idx] && set) return true;
    }
  }

  return false;
}

typedef void (*EventCallback)(State);
typedef bool (*EventHandler)(int);
typedef struct {
  int key;
  EventHandler handler;
  EventCallback callback;
} KeyEvent;

void moveLeft(State s)   { if (!isTetrominoColliding(s, dLEFT, rNONE)) s.current->x -= 1; }
void moveRight(State s)  { if (!isTetrominoColliding(s, dRIGHT, rNONE)) s.current->x += 1; }
void moveUp(State s)     { if (!isTetrominoColliding(s, dUP, rNONE)) s.current->y -= 1; }
void moveDown(State s)   { if (!isTetrominoColliding(s, dDOWN, rNONE)) s.current->y += 1; }
void rotate(State s)     { 
  Rotation next = (s.current->rotation + 1) % 4;
  if (!isTetrominoColliding(s, dNONE, next)) s.current->rotation = next;
}

typedef enum {
  kLEFT, kRIGHT, kUP, kDOWN, kROT, KEYS
} Keys;
static const KeyEvent KEY_EVENTS[KEYS] = {
  (KeyEvent) { .key = KEY_LEFT,   .handler = &IsKeyDown,    .callback = &moveLeft },
  (KeyEvent) { .key = KEY_RIGHT,  .handler = &IsKeyDown,    .callback = &moveRight },
  (KeyEvent) { .key = KEY_UP,     .handler = &IsKeyDown,    .callback = &moveUp },
  (KeyEvent) { .key = KEY_DOWN,   .handler = &IsKeyDown,    .callback = &moveDown },
  (KeyEvent) { .key = KEY_R,      .handler = &IsKeyPressed, .callback = &rotate },
};

void handleInputAndUpdateTetromino(State s) {
  for(int i=0; i<KEYS; i++) {
    KeyEvent k = KEY_EVENTS[i];
    
    if ( k.handler(k.key) &&
      ( k.handler == &IsKeyPressed || s.inputTimer >= INPUT_DELAY )
    ) {
      k.callback(s);
    }
  }
}

void handleTimers(State* s) {
    if (s->gameTimer >= ANIMATION_FRAME_DELAY) {
      s->gameTimer -= ANIMATION_FRAME_DELAY;
      moveDown(*s);
    }

    if (s->inputTimer >= INPUT_DELAY) {
      s->inputTimer -= INPUT_DELAY;
    }

    s->gameTimer++;
    s->inputTimer++;
}

int main() {
  InitWindow(800, 600, "Tetris");
  SetTargetFPS(30);

  Grid grid = initGrid(30, 20);
  Tetromino* current = newTetromino(tT, 0, 0, r0);

  State gameState = {grid, current, 0, 0};

  TraceLog(LOG_INFO, "LOGGING!");
  while(!WindowShouldClose()) {
    BeginDrawing();
      ClearBackground(BLACK);
      drawLines(gameState.grid);
      drawGrid(gameState.grid);
      drawTetromino(*gameState.current);

      handleInputAndUpdateTetromino(gameState);
      
      // For debugging
      if (IsKeyPressed(KEY_SPACE)) {
        updateGrid(gameState);
        gameState.current = randomTetromino();
      }

      handleTimers(&gameState);
    EndDrawing();
  }

  CloseWindow();
}