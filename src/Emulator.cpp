/*!
 * CHIP-8 Emulator.
 * Copyright © 2020 John Hancock. All rights reserved
 */

#include "Emulator.h"

#include "fmt/core.h"
#include <iostream>
#include <fstream>

CPU initEmulator(const std::string &rom) {
  CPU cpu;

  // Copy character sprites into memory at SPRITES_ADDRESS (0x100)
  memcpy(&cpu.memory[SPRITES_ADDRESS], SPRITES_TABLE, sizeof(SPRITES_TABLE));

  // Load rom from file
  std::ifstream file;
  file.open(rom, std::ios::out | std::ios::binary);

  if (!file) {
    fmt::print(stderr, "An error has occurred while loading rom from \"{}\"", rom);
    exit(-2);
  }

  // Copy rom into memory starting at INIT_ADDRESS (0x200)
  file.read(reinterpret_cast<char*>(&cpu.memory[INIT_ADDRESS]), 0x1000 - INIT_ADDRESS);

  return cpu;
}

void cpuCycle(CPU* cpu) {
  if (cpu->delayTimer > 0) { cpu->delayTimer--; }
  if (cpu->soundTimer > 0) { cpu->soundTimer--; }  // This should play a sound eventually

  unsigned short ins = (cpu->memory[cpu->pc] << 8) + cpu->memory[cpu->pc + 1];
  fmt::print("${:03X}: {:04X} = ", cpu->pc, ins);

  parseInstruction(cpu, ins);
}

/**
 * Sprite Drawing Routine.
 * Draws a sprite at coordinate (x, y) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is
 * read as bit-coded starting from the memory address stored in the I register.
 *
 * Register VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, 0 otherwise.
 */
void drawSprite(CPU* cpu, unsigned char x, unsigned char y, unsigned char height) {
  cpu->registers[0xF] = 0;

  for (int i = 0; i < height; i++) {
    unsigned char slice = cpu->memory[cpu->i + i];

    for (int j = 0; j < 8; j++) {
      unsigned char pixelX = (x+j) % 64, pixelY = (y+i) % 32;

      // The bit in slice indexed by j xor current bit value
      if (((slice >> (7 - j)) & 0x1) ^ (cpu->bitScreen[pixelY][pixelX] == COLOR_WHITE)) {
        cpu->bitScreen[pixelY][pixelX] = COLOR_WHITE;

      } else {
        if (cpu->bitScreen[pixelY][pixelX] == COLOR_WHITE) { cpu->registers[0xF] = 1; }
        cpu->bitScreen[pixelY][pixelX] = COLOR_BLACK;
      }
    }
  }
}
