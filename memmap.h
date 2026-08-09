#pragma once
/**********************************************\
 ==============================================

     MAPA DE MEMORIA / DISPOSITIVOS DE E-S
     (read6502 / write6502), un bloque por placa

 ==============================================
\**********************************************/
#include <stdint.h>
#include <EEPROM.h>
#include "board_config.h"

// ---------------------------------------------------------------------------
// Memoria RAM y RIOT del sistema (tamanos definidos por cada board_XXX.h)
// ---------------------------------------------------------------------------
uint8_t RAM[RAM_SIZE];
#if defined(RIOT_SIZE)
uint8_t RIOT[RIOT_SIZE];
#endif
#if defined(BOARD_AIM65)
uint8_t SCRATCH[AIM65_SCRATCH_SIZE];
#endif

// char_pending / serial_mode se usan en el KIM-1 para emular el teclado
// hexadecimal via Serial (siempre "sin teclado fisico", solo por Serial)
extern char char_pending;
extern uint8_t serial_mode;

// ---------------------------------------------------------------------------
uint8_t read6502(uint16_t address) {
#if defined(BOARD_AIM65)
    // Contador de lecturas de $A800, para fingir la autodeteccion de
    // velocidad al arranque (ver bloque BOARD_AIM65 mas abajo).
    static uint16_t aim65_a800_reads = 0;
#endif

#if defined(BOARD_KIM1)
    // ---- KIM-1 ------------------------------------------------------------
    // Vectores IRQ/NMI/RST
    if (address >= KIM1_IRQ_BASE && address <= KIM1_IRQ_END)
        return pgm_read_byte_near(KIM1_IRQ_VECTORS + (address - KIM1_IRQ_BASE));

    // Extension SAVE/LOAD 512B en EEPROM
    if (address >= KIM1_LOAD512_BASE && address <= KIM1_LOAD512_END)
        return pgm_read_byte_near(KIM1_LOAD512_ROM + (address - KIM1_LOAD512_BASE));
    if (address >= KIM1_SAVE512_BASE && address <= KIM1_SAVE512_END)
        return pgm_read_byte_near(KIM1_SAVE512_ROM + (address - KIM1_SAVE512_BASE));
    if (address >= KIM1_MOVIT_BASE && address <= KIM1_MOVIT_END)
        return pgm_read_byte_near(KIM1_MOVIT_ROM + (address - KIM1_MOVIT_BASE));

    // EEPROM de datos (programas BASIC guardados)
    if (address >= 0x2900 && address <= 0x38FF)
        return (EEPROM.read(address - 0x2900));

    // Tiny BASIC ROM
    if (address >= TINYBASIC_ROM_BASE && address <= (TINYBASIC_ROM_BASE + TINYBASIC_ROM_SIZE - 1))
        return pgm_read_byte_near(TINYBASIC_ROM + (address - TINYBASIC_ROM_BASE));

    // Monitor ROM (con intercepciones de E/S por Serial)
    if (address >= MONITOR_ROM_BASE && address <= (MONITOR_ROM_BASE + MONITOR_ROM_SIZE - 1)) {
        // OUTCH: enviar caracter por Serial
        if (address == KIM1_ADDR_OUTCH) {
            extern uint8_t a; extern uint16_t pc;
            Serial.print((char)a);
            pc = KIM1_ADDR_OUTCH_RET;   // 0x1ED3
            return (0xEA);
        }
        // GETCH: leer caracter de Serial
        if (address == KIM1_ADDR_GETCH) {
            extern uint8_t a, x; extern uint16_t pc;
            a = Serial.read();
            if (a == 0xFF) a = 0x00;
            if (a == 0) { pc = KIM1_ADDR_GETCH_LOOP; return (0xEA); }
            Serial.print((char)a);
            x = RAM[0x00FD];
            pc = KIM1_ADDR_GETCH_RET;
            return (0xEA);
        }
        // DETCPS: simular deteccion de velocidad de linea TTY
        if (address == KIM1_ADDR_DETCPS) {
            RIOT[0x17F3-0x1700] = 1;
            RIOT[0x17F2-0x1700] = 1;
            extern uint16_t pc;
            pc = KIM1_ADDR_DETCPS_RET;
            return (0xEA);
        }
        return pgm_read_byte_near(MONITOR_ROM + (address - MONITOR_ROM_BASE));
    }

    // 6530 RIOT
    if (address >= RIOT_BASE && address <= (RIOT_BASE + RIOT_SIZE - 1)) {
        if (address == 0x1740) {
            // No hay teclado fisico (todo va por Serial); se preserva la logica
            // original del monitor tal cual, con char_pending siempre "sin tecla".
            uint8_t sv = (RIOT[0x42] >> 1) & 0xf;
            if (sv == 0) {
                if (char_pending > 6) return 0xff;
            } else if (sv == 1) {
                if (!(char_pending >= 7 && char_pending <= 13)) return 0xff;
            } else if (sv == 2) {
                if (!(char_pending >= 14 && char_pending <= 20)) return 0xff;
            } else if (sv == 3) {
                if (serial_mode) return 0;
                return 0xff;
            } else return 0x80;
        }
        return RIOT[address - RIOT_BASE];
    }

    // RAM
    if (address < 0x0400) return RAM[address];

    return 0;
#endif

#if defined(BOARD_SYM1)
    // ---- SYM-1 (Supermon 1.1) ---------------------------------------------
    // Interceptamos DESPUES de que el JSR que nos trajo aqui ya empujo su
    // direccion de retorno a la pila real del 6502 emulado. Por eso NO
    // podemos fijar pc a mano (dejariamos esa direccion sin desapilar, y
    // un RTS posterior la recogeria y saltaria a cualquier sitio). En vez
    // de eso, hacemos nosotros mismos el equivalente exacto de un RTS:
    // desapilar 16 bits y sumar 1 (mismo calculo que usa rts() en fake6502.h).
    if (address == SYM1_ADDR_GETCH) {
        extern uint8_t a; extern uint16_t pc; extern uint16_t pull16();
        // El monitor real cuenta con que el teletipo (ASR-33) hace "eco
        // local" mecanico de lo que se teclea (por eso el ROM, tras leer
        // un digito, solo imprime explicitamente un espacio fijo via
        // $8342, nunca el caracter en si). Un terminal serie moderno no
        // hace ese eco local, asi que lo compensamos aqui.
        while (Serial.available() == 0) { /* esperar byte */ }
        a = Serial.read();
        Serial.print((char)a);
        pc = pull16() + 1;
        return (0xEA);
    }
    // OUTCH vectorizado: caracter a imprimir ya esta en A
    if (address == SYM1_ADDR_OUTCH) {
        extern uint8_t a; extern uint16_t pc; extern uint16_t pull16();
        Serial.print((char)a);
        pc = pull16() + 1;
        return (0xEA);
    }

    if (address >= MONITOR_ROM_BASE && address <= (MONITOR_ROM_BASE + MONITOR_ROM_SIZE - 1))
        return pgm_read_byte_near(MONITOR_ROM + (address - MONITOR_ROM_BASE));

#if defined(BASIC_ROM_BASE)
    if (address >= BASIC_ROM_BASE && address <= (BASIC_ROM_BASE + BASIC_ROM_SIZE - 1))
        return pgm_read_byte_near(BASIC_ROM + (address - BASIC_ROM_BASE));
#endif

    if (address >= SYM1_SYSRAM_BASE && address <= (SYM1_SYSRAM_BASE + SYM1_SYSRAM_SIZE - 1))
        return RIOT[address - SYM1_SYSRAM_BASE];   // reutilizamos el array RIOT[] como RAM de sistema

    if (address < RAM_SIZE) return RAM[address];

    return 0;
#endif

#if defined(BOARD_AIM65)
    // ---- AIM 65 -------------------------------------------------------
    // GETCH real (autocontenida, segura de interceptar en su entrada)
    if (address == AIM65_ADDR_GETCH) {
        extern uint8_t a; extern uint16_t pc; extern uint16_t pull16();
        while (Serial.available() == 0) { /* esperar byte */ }
        a = Serial.read();
        pc = pull16() + 1;
        return (0xEA);
    }
    // OUTCH: tap no intrusivo en $EEAC (ver nota extensa en board_aim65.h
    // sobre por que el intercept anterior en $EBB6 imprimia basura). Solo
    // efecto lateral: NO tocamos pc, dejamos que la instruccion real (STA
    // $A427) se ejecute con total normalidad -- por eso NO hacemos return
    // aqui, cae al lookup normal de MONITOR_E000 unas lineas mas abajo.
    if (address == AIM65_ADDR_OUTCH_TAP) {
        extern uint8_t a;
        Serial.print((char)(a & 0x7F));
    }

    // Vector VIA fingido: Timer1 "siempre expirado" para no bloquear
    // esperas de temporizacion que no emulamos.
    if (address == 0xA80D) return 0x20;
    // $A800: fingimos "modo serie, autobaud instantaneo" (bit3=0 en la
    // primera lectura -> evita rama teclado; bit6=1 en las siguientes ->
    // saca de la espera de autodeteccion).
    if (address == 0xA800) {
        aim65_a800_reads++;
        return (aim65_a800_reads == 1) ? 0x00 : 0x40;
    }
    // $A482: flag de "temporizador de impresora/display listo". En
    // hardware real lo actualiza una ISR periodica disparada por el
    // Timer2 del VIA de impresora/display (un PIA/VIA separado del que
    // gestiona el TTY en $A800); como no emulamos interrupciones reales,
    // sin esto BASIC se queda esperando aqui para siempre en cuanto hace
    // el primer PRINT (confirmado: rutina de espera en $ECEF-$ECFB,
    // usada por BASIC para temporizar la salida hacia el display/
    // impresora fisicos). Fingimos que el temporizador esta siempre
    // listo, igual que con $A80D.
    if (address == 0xA482) return 0xFF;

    // BASIC ROM 1 ($B000-$BFFF, bytes de R3226-11.BIN -- ver nota en
    // board_aim65.h sobre el intercambio de chips B000/C000).
    if (address >= BASIC1_BASE &&
            address <= (BASIC1_BASE + BASIC1_SIZE - 1))
        return pgm_read_byte_near(BASIC1_ROM + (address - BASIC1_BASE));

    // BASIC ROM 2 ($C000-$CFFF, bytes de R3225-11.BIN).
    if (address >= BASIC2_BASE &&
            address <= (BASIC2_BASE + BASIC2_SIZE - 1))
        return pgm_read_byte_near(BASIC2_ROM + (address - BASIC2_BASE));

    // ROM Assembler ($D000-$DFFF).
    if (address >= ASSEMBLER_BASE &&
            address <= (ASSEMBLER_BASE + ASSEMBLER_SIZE - 1))
        return pgm_read_byte_near(ASSEMBLER_ROM + (address - ASSEMBLER_BASE));

    if (address >= MONITOR_E000_BASE && address <= (MONITOR_E000_BASE + MONITOR_E000_SIZE - 1))
        return pgm_read_byte_near(MONITOR_E000 + (address - MONITOR_E000_BASE));
    if (address >= MONITOR_F000_BASE && address <= (MONITOR_F000_BASE + MONITOR_F000_SIZE - 1))
        return pgm_read_byte_near(MONITOR_F000 + (address - MONITOR_F000_BASE));

    if (address >= AIM65_SCRATCH_BASE && address <= (AIM65_SCRATCH_BASE + AIM65_SCRATCH_SIZE - 1))
        return SCRATCH[address - AIM65_SCRATCH_BASE];

    if (address < RAM_SIZE) return RAM[address];

    return 0;
#endif

}

// ---------------------------------------------------------------------------
void write6502(uint16_t address, uint8_t value) {

#if defined(BOARD_KIM1)
    if (address >= 0x2900 && address <= 0x38FF) {
        EEPROM.update((address - 0x2900), value);
        return;
    }
    if (address >= RIOT_BASE && address <= (RIOT_BASE + RIOT_SIZE - 1)) {
        RIOT[address - RIOT_BASE] = value;
        return;
    }
    if (address < 0x0400) { RAM[address] = value; return; }
    return;

#elif defined(BOARD_SYM1)
    if (address >= SYM1_SYSRAM_BASE && address <= (SYM1_SYSRAM_BASE + SYM1_SYSRAM_SIZE - 1)) {
        RIOT[address - SYM1_SYSRAM_BASE] = value;
        return;
    }
    if (address < RAM_SIZE) { RAM[address] = value; return; }
    return;

#elif defined(BOARD_AIM65)
    if (address == 0xA800 || address == 0xA80D) return;  // registros VIA fingidos
    if (address >= AIM65_SCRATCH_BASE && address <= (AIM65_SCRATCH_BASE + AIM65_SCRATCH_SIZE - 1)) {
        SCRATCH[address - AIM65_SCRATCH_BASE] = value;
        return;
    }
    if (address < RAM_SIZE) { RAM[address] = value; return; }
    return;
#endif
}
