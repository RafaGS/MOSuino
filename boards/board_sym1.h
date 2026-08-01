#pragma once
/**********************************************\
 ==============================================

                  BOARD: SYM-1
        (Synertek SYM-1, monitor Supermon 1.1)

 ==============================================
\**********************************************/
#include "../roms/sym1/sym1_monitor.h"
#include "../roms/sym1/sym1_basic.h"

#define BOARD_NAME "SYM-1"

// ---- Mapa de memoria del SYM-1 --------------------------------------------
// RAM de usuario: el SYM-1 "de fabrica" trae 1KB (0000-03FF), ampliable a
// bordo hasta 4KB (0000-0FFF). Un Arduino Uno/Nano (2KB SRAM totales) solo
// puede permitirse la config de 1KB; para 4KB hace falta un Mega o similar.
#define RAM_BASE   0x0000
#define RAM_SIZE   1024        // subir a 4096 solo en placas con mas SRAM (Mega)

// RAM de sistema propia del monitor (variables internas de Supermon,
// puntero de "vectored I/O", etc.), confirmada por disassembly del dump real
#define SYM1_SYSRAM_BASE 0xA600
#define SYM1_SYSRAM_SIZE 0x100   // 0xA600-0xA6FF
// alias para que memmap.h reserve el array generico (nombrado RIOT[] por
// herencia del KIM-1, aqui se usa como RAM de sistema del monitor)
#define RIOT_BASE SYM1_SYSRAM_BASE
#define RIOT_SIZE SYM1_SYSRAM_SIZE

#define MONITOR_ROM_BASE SYM1_MONITOR_ROM_BASE
#define MONITOR_ROM_SIZE SYM1_MONITOR_ROM_SIZE
#define MONITOR_ROM       SYM1_MONITOR_ROM

// BASIC 1.1 (ROM opcional): $C000-$DFFF (8K). Un unico punto de entrada de
// codigo real, en $C000 (COLD start: pide "MEMORY SIZE?" y arranca BASIC).
// El resto de la ROM a partir de $C003 son datos/vectores internos, no
// codigo (confirmado por disassembly: intentar desensamblar ahi da basura).
// BASIC llama directamente a $8A1B (GETCH) y $8A47 (OUTCH) del monitor -
// exactamente las rutinas cuyo salto vectorizado ya interceptamos para
// Serial, asi que no hace falta ningun intercepto nuevo para que funcione.
#define BASIC_ROM_BASE SYM1_BASIC_ROM_BASE
#define BASIC_ROM_SIZE SYM1_BASIC_ROM_SIZE
#define BASIC_ROM       SYM1_BASIC_ROM
#define SYM1_BASIC_COLD_ENTRY 0xC000

// Direccion de entrada tras RESET: el propio SYM-1 (y los emuladores
// existentes, p.ej. SYMulator) arrancan Supermon 1.1 saltando directamente
// a $8000, sin pasar por un vector de reset "normal" en $FFFC (en el
// hardware real los vectores se cargan con trucos de RAM protegida).
#define SYM1_RESET_PC 0x8000

// Puntos de intercepcion para E/S por Serial (confirmados por disassembly
// del dump real symon1_1.bin). El retorno real NO se fija a mano: en
// memmap.h se hace un pull16()+1 (el mismo calculo que un RTS) para
// desapilar correctamente la direccion que el JSR que nos trajo aqui ya
// dejo en la pila. Estas direcciones _RET se dejan solo como referencia
// documental (deberian coincidir con lo que pull16()+1 calcula).
//   GETCH vectorizado: $8A1B llama a $8A41 = "JMP ($A661)"
#define SYM1_ADDR_GETCH     0x8A41
#define SYM1_ADDR_GETCH_RET 0x8A21   // (informativo) = pull16()+1 esperado

//   OUTCH vectorizado: $8A47 llama a $8A55 = "JMP ($A664)" (con el caracter
//   a imprimir ya en A)
#define SYM1_ADDR_OUTCH     0x8A55
#define SYM1_ADDR_OUTCH_RET 0x8A52   // (informativo) = pull16()+1 esperado
