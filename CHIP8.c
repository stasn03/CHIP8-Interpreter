#include "CHIP8.h"
#include <stdbool.h>
#include <stdio.h>

static const uint8_t fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void reset() {
  pc = 0x200;
  opcode = 0;
  X = 0;
  Y = 0;
  sp = 0;
  delay_timer = 0;
  sound_timer = 0;

  for (int i = 0; i < 4096; i++) {
    memory[i] = 0;
  }

  for (int i = 0; i < 2046; i++) {
    graphics[i] = 0;
  }

  for (int i = 0; i < 16; i++) {
    V[i] = 0;
    stack[i] = 0;
    key[i] = 0;
  }
}

bool load(const char *filepath) {
  reset();

  FILE *fptr = fopen(filepath, "rb");
  if (!fptr)
    return false;

  fseek(fptr, 0, SEEK_END);
  long rom_size = ftell(fptr);
  fread(&memory[pc], sizeof(uint16_t), rom_size, fptr);

  fclose(fptr);

  return true;
}

void decode() {
  opcode = memory[pc] << 8 | memory[pc + 1];
  pc += 2;

  uint8_t nibble = opcode >> 12 & 0xF;
  switch (nibble) {
  case 0x0:
    if (opcode == 0x00E0) { // clear screen
      for (int i = 0; i < 2048; i++)
        graphics[i] = 0;

    } else if (opcode == 0x00EE) { // return from subrutine
      if (sp > 0) {
        pc = stack[sp];
        sp--;
      }
    }
    break;
  // 1NNN - jump to NNN
  case 0x1:
    pc = opcode & 0x0FFF;
    break;
  // 2NNN - calls subrutine at NNN
  case 0x2:
    stack[sp] = pc;
    sp++;
    pc = opcode & 0x0FFF;

  // 3XNN - skips if V[X] == NN
  case 0x3:
    X = (opcode >> 8) & 0x000F;
    if (V[X] == (opcode & 0x00FF)) {
      pc += 2;
    }
    break;
    // 4XNN - skips if V[X] != NN
  case 0x4:
    X = (opcode >> 8) & 0x000F;
    if (V[X] != (opcode & 0x00FF)) {
      pc += 2;
    }
    break;
  // 5XY0 - skips if V[X] == V[Y]
  case 0x5:
    X = (opcode >> 8) & 0x000F;
    Y = (opcode >> 4) & 0x000F;

    if (V[X] == V[Y]) {
      pc += 2;
    }
    break;
  // 6XNN - sets V[X] = NN
  case 0x6:
    X = (opcode >> 8) & 0x000F;
    V[X] = (opcode & 0x00FF);
    break;
  // 7XNN - adds V[X] += NN
  case 0x7:
    X = (opcode >> 8) & 0x000F;
    V[X] += (opcode & 0x00FF);
    break;
  }
}
