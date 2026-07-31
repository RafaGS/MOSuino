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

// char_pending / serial_mode se usan en el KIM-1 para emular el teclado
// hexadecimal via Serial (siempre "sin teclado fisico", solo por Serial)
extern char char_pending;
extern uint8_t serial_mode;

// ---------------------------------------------------------------------------
uint8_t read6502(uint16_t address) {

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

#elif defined(BOARD_SYM1)
    #warning "BOARD_SYM1: mapa de memoria pendiente de implementar (falta el dump de SUPERMON)"
    return 0;

#elif defined(BOARD_AIM65)
    #warning "BOARD_AIM65: mapa de memoria pendiente de implementar"
    return 0;

#elif defined(BOARD_JUNIOR)
    #warning "BOARD_JUNIOR: mapa de memoria pendiente de implementar"
    return 0;

#elif defined(BOARD_SYSTEM1)
    #warning "BOARD_SYSTEM1: mapa de memoria pendiente de implementar"
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
    return; // TODO

#elif defined(BOARD_AIM65)
    return; // TODO

#elif defined(BOARD_JUNIOR)
    return; // TODO

#elif defined(BOARD_SYSTEM1)
    return; // TODO
#endif
}
