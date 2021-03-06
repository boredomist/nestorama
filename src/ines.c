/*
 * This file is part of Nestorama.
 *
 * Nestorama is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * Nestorama is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with Nestorama.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "ines.h"
#include "nes.h"
#include "mapper.h"
#include "rom.h"

#include <string.h>

struct ROM* ines_rom_load_file(FILE* fp, struct ROM* rom)
{

  struct iNES_ROM_header header;

  char hdr[16] = {0};
  long hdrsz = fread(hdr, 1, 16, fp);

  if(hdrsz != 16 || memcmp(hdr, INES_HEADER, 4)) {
    LOGF("Given magic 0x%X 0x%X 0x%X 0x%X, expected 0x4E 0x45 0x53 0x1A, is this an iNES ROM?",
         hdr[0], hdr[1], hdr[2], hdr[3]);
    goto fail;
  }

  header.prg_rom_count = hdr[4];
  header.chr_rom_count = hdr[5];
  header.flags6        = hdr[6];
  header.flags7        = hdr[7];
  header.prg_ram_count = hdr[8];
  header.format        = hdr[9];

  u8 mapper_num  = header.flags7 | (header.flags6 >> 4);

  u32 rom_size = header.prg_rom_count * 0x4000;
  u32 vrom_size = header.chr_rom_count * 0x2000;

  LOGF("Loading 0x%X bytes of PRG ROM and 0x%X bytes of VROM", rom_size, vrom_size);

  unsigned read;

  rom->nes->mem->rom_size = rom_size;
  rom->nes->mem->vrom_size = vrom_size;

  rom->nes->mem->rom = malloc(rom_size);
  rom->nes->mem->vrom = malloc(vrom_size);

  if((read = fread(rom->nes->mem->rom, 1, rom_size, fp)) != rom_size) {
    LOGF("Unexpected EOF in ROM (wanted 0x%X bytes, only saw 0x%X)", rom_size, read);
    goto fail;
  }

  if((read = fread(rom->nes->mem->vrom, 1, vrom_size, fp)) != vrom_size)  {
    LOGF("Unexpected EOF in VROM (wanted 0x%X bytes, only saw 0x%X)", vrom_size, read);
    goto fail;
  }

  rom->hdr.type = INES;
  rom->hdr.format = header.format ? PAL : NTSC;
  rom->hdr.prg_rom_count = header.prg_rom_count;
  rom->hdr.chr_rom_count = header.chr_rom_count;
  rom->hdr.has_prg_ram = (header.prg_ram_count != 0);
  rom->hdr.mapper = mapper_num;

  return rom;

 fail:
  LOGF("ROM load failed, aborting...");
  return NULL;
}
