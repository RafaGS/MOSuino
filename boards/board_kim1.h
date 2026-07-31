#pragma once
/**********************************************\
 ==============================================

                  BOARD: KIM-1

 ==============================================
\**********************************************/
#include "../roms/kim1/kim1_monitor.h"
#include "../roms/kim1/kim1_tinybasic.h"
#include "../roms/kim1/kim1_eeprom_ext.h"
#include "../roms/kim1/kim1_irq.h"

#define BOARD_NAME "KIM-1"

// ---- Mapa de memoria del KIM-1 --------------------------------------------
#define RAM_BASE        0x0000
#define RAM_SIZE        1024          // 0x0000-0x03FF (con margen de RIOT sin usar)
#define RIOT_BASE       0x1700
#define RIOT_SIZE       0x100         // 0x1700-0x17FF  (6530 RIOT #1 y #2)

#define MONITOR_ROM_BASE  KIM1_MONITOR_BASE
#define MONITOR_ROM_SIZE  KIM1_MONITOR_SIZE
#define MONITOR_ROM       KIM1_MONITOR_ROM

#define TINYBASIC_ROM_BASE KIM1_TINYBASIC_BASE
#define TINYBASIC_ROM_SIZE KIM1_TINYBASIC_SIZE
#define TINYBASIC_ROM      KIM1_TINYBASIC_ROM

// direcciones que el monitor original de MOS intercepta para TTY (usadas en memmap.h)
#define KIM1_ADDR_OUTCH     0x1EA0
#define KIM1_ADDR_OUTCH_RET 0x1ED3
#define KIM1_ADDR_GETCH   0x1E65
#define KIM1_ADDR_GETCH_LOOP 0x1E60
#define KIM1_ADDR_GETCH_RET  0x1E87
#define KIM1_ADDR_DETCPS  0x1C2A
#define KIM1_ADDR_DETCPS_RET 0x1C4F

// vector de arranque (RESET) que setup() debe programar en RIOT
#define KIM1_RST_VECTOR_LO 0x1C
#define KIM1_RST_VECTOR_HI 0x00   // NMIV=0x1C00, IRQV=0x1C00 (ver setup() en el .ino)
