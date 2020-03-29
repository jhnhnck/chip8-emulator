/*!
 * CHIP-8 Emulator.
 * Copyright © 2020 John Hancock. All rights reserved
 */

#pragma once

#define WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#define WIN32_HIGH_RES_TIMING
#define GLFW_EXPOSE_NATIVE_WIN32

#include <stack>
#include <string>
#include <chrono>

/* --- Constants Declaration --- */

const std::string ROM_NAME = "./rom.ch8";

const short SPRITES_ADDRESS = 0x100;
const short INIT_ADDRESS = 0x200;

const int CPU_SPEED = 8 * 60;
const auto CYCLE_LENGTH = std::chrono::steady_clock::period::den / CPU_SPEED;  // (1000000000 / 480)

const unsigned char SPRITES_TABLE[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
    0x20, 0x60, 0x20, 0x20, 0x70,  // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
    0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
    0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
    0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
    0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
    0xF0, 0x80, 0xF0, 0x80, 0x80,  // F
};

const unsigned char KEY_BINDINGS[] = {
    'X', '1', '2', '3', 'Q', 'W', 'E', 'A', 'S', 'D', 'Z', 'C', '4', 'R', 'F', 'V'
};

const unsigned int COLOR_BLACK = 0xFF000000;
const unsigned int COLOR_WHITE = 0xFFFFFFFF;

/* --- Function Declaration --- */

typedef struct T_CPU {
  unsigned short i { 0 };   // Index Register
  unsigned short pc { INIT_ADDRESS };  // Program Counter
  unsigned char memory[0x1000] { 0 };
  unsigned char registers[0x10] { 0 };  // V1 - V16
  std::stack<short> stack;
  unsigned int delayTimer { 0 };  // Delay Timer
  unsigned int soundTimer { 0 };  // Sound Timer
  unsigned int bitScreen[32][64] { 0 };
  bool isSuper { false };
} CPU;

CPU initEmulator(const std::string &rom);
void cpuCycle(CPU* cpu);

void drawSprite(CPU* cpu, unsigned char x, unsigned char y, unsigned char height);
void parseInstruction(CPU* cpu, unsigned short code);

