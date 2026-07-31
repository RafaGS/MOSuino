#pragma once
// Vectores de interrupcion del KIM-1 (en vez de rellenar 0x1FFF a 0xFFFA con 0xFF)
#include <avr/pgmspace.h>
#include <stdint.h>

#define KIM1_IRQ_BASE 0xFFFA
#define KIM1_IRQ_END  0xFFFF
#define KIM1_IRQ_SIZE (KIM1_IRQ_END - KIM1_IRQ_BASE + 1)

// Interrupt requests (instead of filling memory from 1FFF to FFFA with 0xFF)
const uint8_t KIM1_IRQ_VECTORS[] PROGMEM = {
/* FFFA */                     //  ;       ** INTERRUPT VECTORS **
/* FFFA */                     //          .org  $1FFA
/* FFFA */ 0x00, 0x1C,         //  NMIENT  .WORD NMIT
/* FFFC */ 0x22, 0x1C,         //  RSTENT  .WORD RST
/* FFFE */ 0x1F, 0x1C          //  IRQENT  .WORD IRQT
};
