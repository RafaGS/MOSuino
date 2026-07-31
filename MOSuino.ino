/**********************************************\
 ==============================================

             ARDUINO 6502 SBC EMULATOR
        (KIM-1 / SYM-1 / AIM 65 / Junior /
                Acorn System 1)

                       by
                Code Monkey King

   Todas las placas se comunican unicamente por
   Serial (sin pantalla ni teclado fisico).

   Seleccion de placa: ver board_config.h
   (por defecto BOARD_KIM1 si no se pasa nada).

 ==============================================
\**********************************************/

#include <stdio.h>
#include <stdint.h>
#include <avr/pgmspace.h>
#include <EEPROM.h>

#include "board_config.h"   // define BOARD_XXX + incluye boards/board_XXX.h (ROMs y mapa de memoria)
#include "fake6502.h"        // nucleo de CPU 6502, generico e independiente de placa
#include "memmap.h"          // read6502()/write6502(), un bloque por placa

/**********************************************\
 ==============================================

                   VARIABLES

 ==============================================
\**********************************************/

char char_pending;
uint8_t serial_mode = 1;

/**********************************************\
 ==============================================

                      MAIN

 ==============================================
\**********************************************/

void setup()
{
    // arranque de la comunicacion serie (unica via de E/S en todas las placas)
    Serial.begin(9600);

    // reset de la CPU (lee el vector de RESET ya sea de ROM o de RIOT segun la placa)
    reset6502();

#if defined(BOARD_KIM1)
    // vectores NMI/IRQ del KIM-1 en RIOT (el vector de RESET esta en la propia ROM)
    write6502(0x17FA, 0x00);
    write6502(0x17FB, 0x1C);
    write6502(0x17FE, 0x00);
    write6502(0x17FF, 0x1C);

    // direcciones de pagina cero usadas por BASIC para el SAVE/LOAD en EEPROM
    RAM[0x98] = 0x8B;
    RAM[0x99] = 0x40;
    RAM[0xA6] = 0x60;
    RAM[0xA7] = 0x40;
#elif defined(BOARD_SYM1)
    // El SYM-1 no usa un vector de reset "normal" en $FFFC (en el hardware
    // real se carga con trucos de RAM protegida); arrancamos Supermon 1.1
    // saltando directamente a $8000, igual que hace SYMulator y otros
    // emuladores existentes.
    pc = SYM1_RESET_PC;
#elif defined(BOARD_AIM65)
    // TODO: inicializacion especifica del AIM 65
#elif defined(BOARD_JUNIOR)
    // TODO: inicializacion especifica del Elektor Junior Computer
#elif defined(BOARD_SYSTEM1)
    // TODO: inicializacion especifica del Acorn System 1
#endif
}

void loop()
{
    step6502();
}
