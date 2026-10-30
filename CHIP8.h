#include <stdint.h>
#ifndef CHIP8_H
#define CHIP8_H

extern uint16_t opcode;        // current opcode
extern char memory[4096];      // total memory
extern char V[16];             // general purpuse registers
extern uint16_t I;             // points at a location in memory
extern uint16_t pc;            // points at the current instruction in memory
extern char graphics[64 * 32]; // 2046 pixels -> 64 x 32
extern uint8_t delay_timer;    // sound register delay
extern uint8_t sound_timer;    // sound register timer
extern uint16_t stack[16];     // stack
extern uint16_t sp;            // stack pointer
extern char key[16];

#endif
