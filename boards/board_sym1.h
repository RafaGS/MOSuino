#pragma once
/**********************************************\
 ==============================================

           BOARD: SYM-1  (PENDIENTE)

   Falta insertar el volcado real de SUPERMON
   (monitor ROM del Synertek SYM-1) en
   roms/sym1/sym1_monitor.h

   Usa tools/bin2h.py para generar ese fichero
   a partir de tu .bin:

     python3 tools/bin2h.py supermon.bin \
         SYM1_MONITOR_ROM roms/sym1/sym1_monitor.h

 ==============================================
\**********************************************/
#include <avr/pgmspace.h>
#include <stdint.h>

#define BOARD_NAME "SYM-1"

// TODO: sustituir por el mapa de memoria real del SYM-1 cuando se aporte el dump
#define RAM_BASE   0x0000
#define RAM_SIZE   1024
#define RIOT_BASE  0x0000
#define RIOT_SIZE  256

// Placeholder: se sobreescribira al incluir roms/sym1/sym1_monitor.h real
static const uint8_t SYM1_MONITOR_ROM[1] PROGMEM = { 0xEA };
#define MONITOR_ROM_BASE 0x8000
#define MONITOR_ROM_SIZE 1
#define MONITOR_ROM SYM1_MONITOR_ROM
