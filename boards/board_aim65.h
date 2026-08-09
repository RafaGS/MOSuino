#pragma once
/**********************************************\
 ==============================================

                  BOARD: AIM 65
        (Rockwell AIM 65, monitor R3222/R3223-11)

        ESTADO: arranque del monitor Y de BASIC
        verificados de extremo a extremo (simulacion
        + hardware real), incluyendo PRINT, aritmetica
        y bucles FOR/NEXT. Tres bugs encontrados y
        corregidos:
        1) Texto basura en vez del mensaje de
           bienvenida -> ver AIM65_ADDR_OUTCH_TAP
           mas abajo.
        2) BASIC (tecla "5"/"6") no arrancaba,
           acababa en un opcode ilegal -> las ROMs
           R3225-11.BIN y R3226-11.BIN estaban
           CAMBIADAS DE SITIO: R3226-11 va en $B000
           (empieza con "JMP $CEA3", el vector real
           de cold-start) y R3225-11 va en $C000 --
           justo al reves de como estaban puestas.
           Confirmado byte a byte contra el dump de
           referencia de github.com/mist64/msbasic
           (proyecto que reconstruye MS BASIC 6502
           desde fuente, incluyendo AIM-65) y contra
           las checksums oficiales de MAME
           (r3225.z25 CRC32 d7b42d2a, r3226.z26 CRC32
           36a61f39 -- las ROMs del usuario coinciden
           exactamente, no hay corrupcion). Ya
           corregido en aim65_basic_b000.h /
           aim65_basic_c000.h (regenerados con el
           fichero fuente correcto cada uno).
           Mismo patron "contraintuitivo" que ya se
           dio con R3222/R3223-11 del monitor: el
           sufijo numerico del chip NO se corresponde
           con el orden de direcciones de memoria.
        3) BASIC se colgaba para siempre en el primer
           PRINT -> esperaba en un bucle ($ECEF) a que
           $A482 cambiase, flag que en hardware real
           actualiza una ISR de timer del VIA de
           impresora/display (no emulamos IRQs reales).
           Fingido como "siempre listo", igual que ya
           se hacia con $A80D. Ver comentario junto a
           la lectura de $A482 en memmap.h.

 ==============================================
\**********************************************/
#include "../roms/aim65/aim65_monitor_e000.h"
#include "../roms/aim65/aim65_monitor_f000.h"
// Opcionales (comenta estas lineas si quieres ahorrar flash y no las usas):
#include "../roms/aim65/aim65_assembler.h"
#include "../roms/aim65/aim65_basic_b000.h"
#include "../roms/aim65/aim65_basic_c000.h"

#define BOARD_NAME "AIM 65"

// ---- Mapa de memoria (oficial, documentado por Rockwell) ------------------
#define RAM_BASE   0x0000
#define RAM_SIZE   4096          // 4K "de fabrica" en los AIM 65 Rockwell tempranos

// RAM de trabajo del monitor ("I/O scratchpad"): el monitor usa esta zona
// no solo para registros de E/S sino como RAM de variables normal
// (vectores de NMI/IRQ en $A402/$A404, buffers, etc.) -- confirmado por
// disassembly: sin esto como RAM real, los vectores nunca se inicializan
// y el sistema acaba saltando a basura.
#define AIM65_SCRATCH_BASE 0xA000
#define AIM65_SCRATCH_SIZE 0x0800   // $A000-$A7FF

#define MONITOR_E000_BASE AIM65_MONITOR_E000_ROM_BASE
#define MONITOR_E000_SIZE AIM65_MONITOR_E000_ROM_SIZE
#define MONITOR_E000       AIM65_MONITOR_E000_ROM
#define MONITOR_F000_BASE AIM65_MONITOR_F000_ROM_BASE
#define MONITOR_F000_SIZE AIM65_MONITOR_F000_ROM_SIZE
#define MONITOR_F000       AIM65_MONITOR_F000_ROM

// ROM Assembler ($D000-$DFFF)
#define ASSEMBLER_BASE  AIM65_ASSEMBLER_ROM_BASE
#define ASSEMBLER_SIZE  AIM65_ASSEMBLER_ROM_SIZE
#define ASSEMBLER_ROM   AIM65_ASSEMBLER_ROM

// BASIC ROM 1 ($B000-$BFFF): OJO, contiene los bytes de R3226-11.BIN, NO
// R3225-11.BIN -- ver nota de mas arriba sobre el intercambio de chips.
#define BASIC1_BASE     AIM65_BASIC_B000_ROM_BASE
#define BASIC1_SIZE     AIM65_BASIC_B000_ROM_SIZE
#define BASIC1_ROM      AIM65_BASIC_B000_ROM

// BASIC ROM 2 ($C000-$CFFF): contiene los bytes de R3225-11.BIN.
#define BASIC2_BASE     AIM65_BASIC_C000_ROM_BASE
#define BASIC2_SIZE     AIM65_BASIC_C000_ROM_SIZE
#define BASIC2_ROM      AIM65_BASIC_C000_ROM

// Entrada tras RESET: vector real del propio ROM ($FFFC/$FFFD -> $E0BF)
#define AIM65_RESET_PC 0xE0BF

// ---- E/S por Serial (bit-banging TTY de 20mA vía VIA, confirmado por
// disassembly) --------------------------------------------------------------
// $A800 = registro de datos del VIA que hace de puerto serie. $A80D = IFR
// (flags de interrupcion), su bit 5 indica "Timer 1 ha expirado" y se usa
// como temporizador de cada bit. No emulamos el VIA real: fingimos que
// el Timer 1 SIEMPRE ha expirado (bit5=1) para no bloquear la espera.
//
// GETCH real: $EBDB. Rutina autocontenida (guarda/restaura X con su propio
// PHA/PLA, termina en un RTS real en $EC0E) que muestrea 8 bits via $A800
// con temporizacion por Timer1. Es SEGURA de interceptar directamente.
#define AIM65_ADDR_GETCH 0xEBDB
//
// OUTCH -- RESUELTO (causa raiz encontrada por simulacion instruccion a
// instruccion desde el RESET, ver AIM65_notas_tecnicas.md).
//
// $EBAC/$EBB6 NO es una rutina de OUTCH dedicada: es la misma utilidad
// generica de "restaurar X/Y y devolver" que empareja con $EB9E (el truco
// de prestamo de pila), y se llama desde NUEVE sitios distintos de la ROM
// con propositos variados. El caracter real a transmitir NO esta fiable
// en $A42D en el momento en que $EBB6 hace "LDA $A42D; RTS": en la
// llamada real de transmision (JSR $EBAC en $EEEC, dentro del motor de
// bit-banging por Timer2 en $EEA8-$EEFB) lo ultimo que ha pasado por A
// antes de esa llamada es el valor de retorno de la espera de Timer2
// ($A80D AND #$20 = siempre 0x20 con nuestro VIA fingido) -- por eso el
// intercept antiguo en $EBB6 imprimia sistematicamente 0x20 (espacio) en
// vez del caracter real: encaja exactamente con el sintoma observado.
//
// El caracter real SI esta garantizado en A un poco antes: en $EEAC
// ("STA $A427", justo al entrar en el motor de bit-banging), A todavia
// tiene el valor original sin tocar. Interceptamos ahi, pero SOLO como
// un "tap" no intrusivo: imprimimos (A & 0x7F) como efecto lateral y NO
// tocamos pc/sp -- dejamos que la CPU seguida ejecutando la instruccion
// real con total normalidad (el bit-banging que sigue es inofensivo
// porque fingimos que el Timer2 esta siempre listo). Esto evita tener
// que reimplementar a mano el control de flujo posterior (que a veces
// hace RTS y a veces salta a $EF05 para seguir leyendo teclado), y de
// paso cubre gratis el eco de teclado por GETLIN, que reutiliza este
// mismo motor de transmision (unico JMP real a esta zona: $E98C).
//
// Confirmado por simulacion completa: con este unico cambio se reconstruye
// exactamente "\r\n\xFF  ROCKWELL AIM 65\r\n\xFF\r\n\xFF\r\n\xFF<" antes de
// llegar de forma estable a la espera de GETCH -- el mensaje de bienvenida
// real y el prompt "<" documentado.
#define AIM65_ADDR_OUTCH_TAP 0xEEAC
