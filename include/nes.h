/* container for the various chips and bits of the Nintendo Entertainment System */

#pragma once

#ifndef _NES_H
#define _NES_H

#include "def.h"

struct _6502;
struct _2C02;
struct APU;
struct mapper;
struct ROM;

/*
  NES' page size is 256 bytes. Total of 256 pages available. (0xFFFF bytes)

  SADDR           SIZE    DESCRIPTION
  0x0000         0x00FF   Zero page
  0x0100         0x00FF   Stack
  0x0200         0x0600   Internal RAM
  0x0800         0x0800   Mirror of 0x0000-0x07FF
  0x1000         0x0800   Mirror of 0x0000-0x07FF
  0x1800         0x0800   Mirror of 0x0000-0x07FF
  0x2000         0x0008   NES PPU registers
  0x2008         0x1FF8   Mirrors of 0x2000 every 8 bytes
  0x4000         0x0020   Input/Output registers

  0x4020         0x1FE0   Expansion ROM
  0x6000         0x2000   Save RAM
  0x8000         0x4000   PRG-ROM

  0xFFFA         0x0002   Address of Non Maskable Interrupt (NMI) handler routine
  0xFFFC         0x0002   Address of Power on reset handler routine
  0xFFFE         0x0002   Address of Break (BRK instruction) handler routine

*/

// the various memory blocks of significance
struct memory {
  u8 lowmem[0x800];     // 2K internal RAM   (CPU)
  u8 ppureg[0x008];     // 8B PPU registers  (PPU)
  u8 apureg[0x018];     // 18B APU registers (APU)

  u32 rom_size;
  u8 *rom;              // pointer to PRG ROM

  u32 vrom_size;
  u8 *vrom;             // pointer to CHR ROM
};

struct NES {
  struct _6502* cpu;
  struct _2C02* ppu;
  struct APU*   apu;

  struct ROM* rom;
  struct mapper* map;

  struct memory mem;
};

// functions
struct NES*   nes_create(void);
void          nes_free(struct NES* nes);
void          nes_reset(struct NES* nes);
bool          nes_load_rom(struct NES* nes, FILE* fp);
void          nes_tick(struct NES* nes);
void          nes_inspect(struct NES* nes);
u8            nes_fetch_memory(struct NES* nes, u16 addr);
void          nes_set_memory(struct NES* nes, u16 addr, u8 val);
void          nes_set_rom(struct NES* nes, u8* rom, u32 size);
void          nes_set_vrom(struct NES* nes, u8* vrom, u32 size);


#endif /* _NES_H */
