#include "gui.h"
#include "cpu.h"
#include "memory.h"
#include "raygui.h"
#include "style_terminal.h"
#include <raylib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>

uint32_t getAddr(const char *str) {
  uint32_t addr = 0;
  uint32_t t = 0;
  for (uint32_t i = 0; i < strlen(str); i++) {
    if (str[i] >= '0' && str[i] <= '9') {
      t = str[i] - '0';
    } else if (str[i] >= 'a' && str[i] <= 'f') {
      t = str[i] - 'a' + 10;
    } else if (str[i] >= 'A' && str[i] <= 'F') {
      t = str[i] - 'A' + 10;
    }
    addr = addr * 16 + t;
  }
  return addr;
}

void GuiWindowStart(Cpu *cpu) {
  InitWindow(1000, 800, "risvc simuator");
  SetTargetFPS(10);
  Font font = LoadFont("res/JetBrainsMono-Regular.ttf");
  GuiLoadStyleCyber();
  GuiSetFont(font);

  int instCount = 0;
  char regVal[100] = {0};
  char instCountText[100] = {0};
  char instInHex[100] = {0};
  int code = 0;
  bool closeWindow = false;
  Color bgColor = GetColor(0x024658ff);
  uint32_t BreakPointsAddr[] = {0x100a4};
  float fontSize = 24;
  char memIn[100] = {0};
  bool mouseOnText;
  char MemText[100];
  int letterCount = 0;
  bool firstEnter = false;
  uint32_t addr;

  while (!WindowShouldClose() && !closeWindow) {
    Vector2 regsLineSpace =
        MeasureTextEx(font, "x1 (sp/fp) - FFFFFFFF", fontSize, 1);
    // int windowWidth = GetScreenWidth();
    // int windowHeight = GetScreenHeight();

    BeginDrawing();
    ClearBackground(bgColor);

    if (IsKeyReleased(KEY_N)) {
      code = CpuStep(cpu);
      // printf("%d %s\n", code, InstName[code]);
      if (code == -1)
        break;
      instCount++;
    }
    if (IsKeyReleased(KEY_R)) {
      for (int i = 0; i < (sizeof(BreakPointsAddr) / sizeof(uint32_t)); i++) {
        while (BreakPointsAddr[i] != cpu->pc) {
          code = CpuStep(cpu);
          if (code == -1)
            closeWindow = true;
          instCount++;
        }
      }
    }
    if (IsKeyReleased(KEY_EQUAL))
      fontSize++;
    if (IsKeyReleased(KEY_MINUS))
      fontSize--;
    for (int i = 0; i < 32; i++) {
      sprintf(regVal, "%s - %X", regNames[i], cpu->regs[i]);
      DrawTextEx(font, regVal, (Vector2){10, 10 + (regsLineSpace.y * i)},
                 fontSize, 1, RAYWHITE);
    }
    uint32_t *ptr = (uint32_t *)&cpu->csrRegs;
    for (int i = 0; i < NCSRS; i++) {
      sprintf(regVal, "%s - %X", csrNames[i], *ptr++);
      DrawTextEx(font, regVal,
                 (Vector2){regsLineSpace.x, 10 + (regsLineSpace.y * i)},
                 fontSize, 1, RAYWHITE);
    }
    sprintf(instCountText, "PC - %X", cpu->pc);
    DrawTextEx(font, instCountText, (Vector2){10, 10 + (regsLineSpace.y * 32)},
               fontSize, 1, RAYWHITE);
    sprintf(instCountText, "inst count - %d", instCount);
    DrawTextEx(font, instCountText, (Vector2){2 * regsLineSpace.x, 10},
               fontSize, 1, RAYWHITE);

    DrawTextEx(font, InstName[code],
               (Vector2){2 * regsLineSpace.x, 10 + regsLineSpace.y}, fontSize,
               1, RAYWHITE);

    sprintf(instInHex, "%X", cpu->inst);
    DrawTextEx(font, instInHex,
               (Vector2){2 * regsLineSpace.x, 10 + regsLineSpace.y * 2},
               fontSize, 1, RAYWHITE);
    int bx = 2 * regsLineSpace.x;
    int by = regsLineSpace.y * 4;
    // int bw = windowWidth - regsLineSpace.x;
    // int bh = windowHeight - regsLineSpace.y * 4;

    Rectangle textBox = (Rectangle){bx, by, 200, 20};
    if (CheckCollisionPointRec(GetMousePosition(), textBox))
      mouseOnText = true;
    else
      mouseOnText = false;
    if (mouseOnText) {
      SetMouseCursor(MOUSE_CURSOR_IBEAM);
      int key = GetCharPressed();
      while (key > 0) {
        if (((key >= 48 && key <= 57) || (key >= 65 && key <= 70) ||
             (key >= 97 && key <= 102)) &&
            (letterCount < 8)) {
          memIn[letterCount] = (char)key;
          memIn[letterCount + 1] = '\0';
          letterCount++;
        }
        key = GetCharPressed();
      }
      if (IsKeyPressed(KEY_BACKSPACE)) {
        letterCount--;
        if (letterCount < 0)
          letterCount = 0;
        memIn[letterCount] = '\0';
      }
    } else
      SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    DrawRectangleRec(textBox, DARKGRAY);
    DrawTextEx(font, memIn, (Vector2){textBox.x, textBox.y}, 24, 1, RAYWHITE);
    if (IsKeyReleased(KEY_ENTER)) {
      firstEnter = true;
      addr = getAddr(memIn);
    }
    if (firstEnter) {
      uint32_t val = MemoryGetDataU32(cpu->mem, addr);
      sprintf(MemText, "%x - %x", addr, val);
      DrawTextEx(font, MemText, (Vector2){bx, by + 25}, fontSize, 1, RAYWHITE);
    }
    EndDrawing();
  }
  CloseWindow();
}
