/*!
 * CHIP-8 Emulator.
 * Copyright © 2020 John Hancock. All rights reserved
 */

#include "Emulator.h"

#include <Windows.h>
#include <cstdlib>
#include <fmt/core.h>

void parseInstruction(CPU* cpu, unsigned short code) {
  unsigned char x, y, byte, nibble = 0;
  unsigned short addr = 0;
  cpu->pc += 2;

  /* --- No argument OP codes --- */
  if (code == 0x00E0) {  // 00E0 -> CLS
    fmt::print("cls\n");

    for (y = 0; y < 32; y++) {
      for (x = 0; x < 64; x++) {
        cpu->bitScreen[y][x] = COLOR_BLACK;
      }
    }

  } else if (code == 0x00EE) {  // 00EE -> RET
    fmt::print("ret\n");

    cpu->pc = cpu->stack.top();
    cpu->stack.pop();

  } else if ((code & 0xF000) == 0x1000) {  // 1NNN -> JP addr
    addr = code & 0xFFF;
    fmt::print("jp ${:03X}\n", addr);

    cpu->pc = addr;

  } else if ((code & 0xF000) == 0x2000) {  // 2NNN -> CALL addr
    addr = code & 0xFFF;
    fmt::print("call ${:03X}\n", addr);

    cpu->stack.push(cpu->pc);
    cpu->pc = addr;

  } else if ((code & 0xF000) == 0x3000) {  // 3xkk -> SE Vx, byte
    x = (code >> 8) & 0xF;
    byte = code & 0xFF;
    fmt::print("se V{:X} #{:02X}\n", x, byte);

    if (cpu->registers[x] == byte) { cpu->pc += 2; }

  } else if ((code & 0xF000) == 0x4000) {  // 4xkk -> SNE Vx, byte
    x = (code >> 8) & 0xF;
    byte = code & 0xFF;
    fmt::print("sne V{:X} #{:02X}\n", x, byte);

    if (cpu->registers[x] != byte) { cpu->pc += 2; }

  } else if ((code & 0xF00F) == 0x5000) {  // 5xy0 -> SE Vx, Vy
    x = (code >> 8) & 0xF;
    y = (code >> 4) & 0xF;
    fmt::print("se V{:X} V{:X}\n", x, y);

    if (cpu->registers[x] == cpu->registers[y]) { cpu->pc += 2; }

  } else if ((code & 0xF000) == 0x6000) {  // 6xkk -> LD Vx, byte
    x = (code >> 8) & 0xF;
    byte = code & 0xFF;
    fmt::print("ld V{:X} #{:02X}\n", x, byte);

    cpu->registers[x] = byte;

  } else if ((code & 0xF000) == 0x7000) {  // 7xkk -> ADD Vx, byte
    x = (code >> 8) & 0xF;
    byte = code & 0xFF;
    fmt::print("add V{:X} #{:02X}\n", x, byte);

    cpu->registers[0xF] = (cpu->registers[x] + byte) > 0xFF;
    cpu->registers[x] = (cpu->registers[x] + byte) & 0xFF;

  } else if ((code & 0xF00F) == 0x8000) {  // 8xy0 -> LD Vx, Vy
    x = (code >> 8) & 0xF;
    y = (code >> 4) & 0xF;
    fmt::print("ld V{:X} V{:X}\n", x, y);

    cpu->registers[x] = cpu->registers[y];

  } else if ((code & 0xF00F) == 0x8001) {  // 8xy1 -> OR Vx, Vy
    x = (code >> 8) & 0xF;
    y = (code >> 4) & 0xF;
    fmt::print("or V{:X} V{:X}\n", x, y);

    cpu->registers[x] = cpu->registers[x] | cpu->registers[y];

  } else if ((code & 0xF00F) == 0x8002) {  // 8xy2 -> AND Vx, Vy
    x = (code >> 8) & 0xF;
    y = (code >> 4) & 0xF;
    fmt::print("and V{:X} V{:X}\n", x, y);

    cpu->registers[x] = cpu->registers[x] & cpu->registers[y];

  } else if ((code & 0xF00F) == 0x8003) {  // 8xy3 -> XOR Vx, Vy
    x = (code >> 8) & 0xF;
    y = (code >> 4) & 0xF;
    fmt::print("xor V{:X} V{:X}\n", x, y);

    cpu->registers[x] = cpu->registers[x] ^ cpu->registers[y];

  } else if ((code & 0xF00F) == 0x8004) {  // 8xy4 -> ADD Vx, Vy
    x = (code >> 8) & 0xF;
    y = (code >> 4) & 0xF;
    fmt::print("add V{:X} V{:X}\n", x, y);

    cpu->registers[0xF] = (cpu->registers[x] + cpu->registers[y]) > 0xFF;
    cpu->registers[x] = (cpu->registers[x] + cpu->registers[y]) & 0xFF;

  } else if ((code & 0xF00F) == 0x8005) {  // 8xy5 -> SUB Vx, Vy
    x = (code >> 8) & 0xF;
    y = (code >> 4) & 0xF;
    fmt::print("sub V{:X} V{:X}\n", x, y);

    cpu->registers[0xF] = cpu->registers[x] > cpu->registers[y];
    cpu->registers[x] = (cpu->registers[x] - cpu->registers[y]) & 0xFF;

  } else if ((code & 0xF00F) == 0x8006) {  // 8xy6 -> SHR Vx {, Vy}
    x = (code >> 8) & 0xF;
    y = (code >> 4) & 0xF;
    fmt::print("shr V{:X} V{:X}\n", x, y);

    cpu->registers[0xF] = cpu->registers[y] & 1;
    cpu->registers[x] = cpu->registers[y] >> 1;

  } else if ((code & 0xF00F) == 0x8007) {  // 8xy7 -> SUBN Vx, Vy
    x = (code >> 8) & 0xF;
    y = (code >> 4) & 0xF;
    fmt::print("subn V{:X} V{:X}\n", x, y);

    cpu->registers[0xF] = cpu->registers[y] > cpu->registers[x];
    cpu->registers[x] = (cpu->registers[y] - cpu->registers[x]) & 0xFF;

  } else if ((code & 0xF00F) == 0x800E) {  // 8xyE -> SHL Vx {, Vy}
    x = (code >> 8) & 0xF;
    y = (code >> 4) & 0xF;
    fmt::print("shl V{:X} V{:X}\n", x, y);

    cpu->registers[0xF] = cpu->registers[y] & 0x80;
    cpu->registers[x] = (cpu->registers[y] << 1) & 0xFF;

  } else if ((code & 0xF00F) == 0x9000) {  // 9xy0 -> SNE Vx, Vy
    x = (code >> 8) & 0xF;
    y = (code >> 4) & 0xF;
    fmt::print("sne V{:X} V{:X}\n", x, y);

    if (cpu->registers[x] != cpu->registers[y]) { cpu->pc += 2; }

  } else if ((code & 0xF000) == 0xA000) {  // Annn -> LD I, addr
    addr = code & 0xFFF;
    fmt::print("ld I ${:03X}\n", addr);

    cpu->i = addr;

  } else if ((code & 0xF000) == 0xB000) {  // Bnnn -> JP V0, addr
    addr = code & 0xFFF;
    fmt::print("jp +${:03X}\n", addr);

    cpu->pc = cpu->registers[0] + addr;

  } else if ((code & 0xF000) == 0xC000) {  // Cxkk -> RND Vx, byte
    x = (code >> 8) & 0xF;
    byte = code & 0xFF;
    fmt::print("rnd V{:X} #{:02X}\n", x, byte);

    cpu->registers[x] = byte & std::rand() % 255;

  } else if ((code & 0xF000) == 0xD000) {  // Dxyn -> DRW Vx, Vy, nibble
    x = (code >> 8) & 0xF;
    y = (code >> 4) & 0xF;
    nibble = code & 0xF;
    fmt::print("drw V{:X} V{:X} #{:X}\n", x, y, nibble);

    drawSprite(cpu, cpu->registers[x], cpu->registers[y], nibble);

  } else if ((code & 0xF0FF) == 0xE09E) {  // Ex9E -> SKP Vx
    x = (code >> 8) & 0xF;
    unsigned char key = cpu->registers[x];
    fmt::print("skp V{:X}\n", x);

    if (!(GetKeyState(KEY_BINDINGS[key]) & 0x8000)) {
      cpu->pc += 2;
    }

  } else if ((code & 0xF0FF) == 0xE0A1) {  // ExA1 -> SKNP Vx
    x = (code >> 8) & 0xF;
    unsigned char key = cpu->registers[x];
    fmt::print("sknp V{:X}\n", x);

    if (GetKeyState(KEY_BINDINGS[key]) & 0x8000) {
      cpu->pc += 2;
    }

  } else if ((code & 0xF0FF) == 0xF007) {  // Fx07 -> LD Vx, DT
    x = (code >> 8) & 0xF;
    fmt::print("load V{:X} DT\n", x);

    cpu->registers[x] = cpu->delayTimer / 8;

  } else if ((code & 0xF0FF) == 0xF00A) {  // Fx0A -> LD Vx, K
    x = (code >> 8) & 0xF;
    fmt::print("ld V{:X} K\n", x);

    for (char key = 0; key < 16; key++) {
      if (!(GetKeyState(KEY_BINDINGS[key]) & 0x8000)) {
        cpu->registers[x] = key;
        return;
      }
    }

    cpu->pc -= 2;  // Repeat check on next tick

  } else if ((code & 0xF0FF) == 0xF015) {  // Fx15 -> LD DT, Vx
    x = (code >> 8) & 0xF;
    fmt::print("ld DT V{:X}\n", x);

    cpu->delayTimer = cpu->registers[x] * 8;

  } else if ((code & 0xF0FF) == 0xF018) {  // Fx18 -> LD ST, Vx
    x = (code >> 8) & 0xF;
    fmt::print("ld ST V{:X}\n", x);

    cpu->soundTimer = cpu->registers[x] * 8;

  } else if ((code & 0xF0FF) == 0xF01E) {  // Fx1E -> ADD I, Vx
    x = (code >> 8) & 0xF;
    fmt::print("add I V{:X}\n", x);

    unsigned short sum = cpu->i + cpu->registers[x];
    cpu->i = sum & 0xFFF;
    cpu->registers[0xF] = (sum > 0xFFF);

  } else if ((code & 0xF0FF) == 0xF029) {  // Fx29 -> LD F, Vx
    x = (code >> 8) & 0xF;
    fmt::print("ld F V{:X}\n", x);

    cpu->i = (SPRITES_ADDRESS + (cpu->registers[x] * 5));

  } else if ((code & 0xF0FF) == 0xF033) {  // Fx33 -> LD B, Vx
    x = (code >> 8) & 0xF;
    fmt::print("ld B V{:X}\n", x);

    cpu->memory[cpu->i] = cpu->registers[x] / 100;
    cpu->memory[cpu->i + 1] = (cpu->registers[x] % 100) / 10;
    cpu->memory[cpu->i + 2] = cpu->registers[x] % 10;

  } else if ((code & 0xF0FF) == 0xF055) {  // Fx55 -> LD [I] = Vx
    x = (code >> 8) & 0xF;
    fmt::print("ld [I] V{:X}\n", x);

    for (int offset = 0; offset < x; offset++) {
      cpu->memory[cpu->i + offset] = cpu->registers[offset];
    }

    cpu->i += x + 1;

  } else if ((code & 0xF0FF) == 0xF065) {  // Fx65 -> LD, Vx, [I]
    x = (code >> 8) & 0xF;
    fmt::print("ld V{:X} [I]\n", x);

    for (int offset = 0; offset < x; offset++) {
      cpu->registers[offset] = cpu->memory[cpu->i + offset];
    }

    cpu->i += x + 1;

  } else {
    fmt::print("nop\n");
  }
}