#pragma once
/**********************************************\
 ==============================================

           BOARD: Elektor Junior Computer  (PENDIENTE)

   Falta insertar el volcado real del monitor
   en roms/junior/junior_monitor.h

   Usa tools/bin2h.py para generarlo a partir
   de tu .bin:

     python3 tools/bin2h.py monitor.bin \
         JUNIOR_MONITOR_ROM roms/junior/junior_monitor.h

 ==============================================
\**********************************************/
#include <avr/pgmspace.h>
#include <stdint.h>

#define BOARD_NAME "Elektor Junior Computer"

// TODO: sustituir por el mapa de memoria real de Elektor Junior Computer cuando se aporte el dump
#define RAM_BASE   0x0000
#define RAM_SIZE   1024
#define RIOT_BASE  0x0000
#define RIOT_SIZE  256

// Placeholder: se sobreescribira al incluir el dump real
static const uint8_t JUNIOR_MONITOR_ROM[1] PROGMEM = { 0xEA };
#define MONITOR_ROM_BASE 0x8000
#define MONITOR_ROM_SIZE 1
#define MONITOR_ROM JUNIOR_MONITOR_ROM
