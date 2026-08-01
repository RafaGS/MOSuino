# MOSuino — emulador multi-placa 6502 (KIM-1 / SYM-1 / AIM 65 / Junior / System 1)

Todas las placas se comunican **únicamente por Serial** (sin pantalla ni teclado
físico), igual que el proyecto original.

## Placas donde aplica

- [MOS KIM-1](https://minibots.wordpress.com/2026/06/10/emulador-de-mos-kim-1-con-arduino-uno-r3/)

- [Synertek SYM-1](https://minibots.wordpress.com/2026/08/01/emulador-de-synertek-sym-1-con-arduino-uno-r3/)

## Estructura

```
MOSuino.ino              sketch principal (setup/loop, variables globales)
board_config.h           SELECTOR DE PLACA (ver mas abajo)
fake6502.h                nucleo de CPU 6502, generico, igual para todas las placas
memmap.h                  read6502()/write6502(), un bloque #if por placa
boards/
  board_kim1.h              mapa de memoria del KIM-1        (COMPLETO)
  board_sym1.h               mapa de memoria del SYM-1         (COMPLETO — Supermon 1.1)
  board_aim65.h               mapa de memoria del AIM 65         (PENDIENTE)
  board_junior.h                Elektor Junior Computer            (PENDIENTE)
  board_system1.h                Acorn System 1                      (PENDIENTE)
roms/
  kim1/                     ROMs y extensiones propias del KIM-1 (extraidas del .ino original)
  sym1/                      ROM real de Supermon 1.1 (symon1_1.bin ya integrado)
  aim65/, junior/, system1/    (vacias: aqui van los dumps que generes con bin2h.py)
tools/
  bin2h.py                  convierte un .bin de una ROM monitor en un .h PROGMEM
```

## Compilar con arduino-cli

Por defecto (sin pasar nada) se compila el **KIM-1**:

```bash
arduino-cli compile --fqbn arduino:avr:uno MOSuino
```

Para elegir otra placa, sin tocar ningun fichero, se pasa el define por
`--build-property`:

```bash
# SYM-1
arduino-cli compile --fqbn arduino:avr:uno \
  --build-property "compiler.cpp.extra_flags=-DBOARD_SYM1" \
  MOSuino

# AIM 65
arduino-cli compile --fqbn arduino:avr:uno \
  --build-property "compiler.cpp.extra_flags=-DBOARD_AIM65" \
  MOSuino

# Elektor Junior Computer
arduino-cli compile --fqbn arduino:avr:uno \
  --build-property "compiler.cpp.extra_flags=-DBOARD_JUNIOR" \
  MOSuino

# Acorn System 1
arduino-cli compile --fqbn arduino:avr:uno \
  --build-property "compiler.cpp.extra_flags=-DBOARD_SYSTEM1" \
  MOSuino
```

(También se puede fijar la placa por defecto editando el `#define BOARD_KIM1` de `board_config.h`).

## Nota de compatibilidad

`fake6502.h` incluye ahora los prototipos de las funciones de direccionamiento y de opcodes. Esto es necesario para compilar fuera del IDE de Arduino (que normalmente los genera solo automáticamente); con `arduino-cli`/el IDE sigue funcionando exactamente igual que antes.

## Validación realizada

Ambas placas (KIM-1 y SYM-1) se han comprobado con una compilación de sintaxis real usando `avr-g++` (no solo revisión visual), con stubs mínimos de `Serial`/`EEPROM`. Compilan sin errores nuevos respecto al `.ino` original (solo avisos preexistentes propios del estilo de `fake6502.c`).

---

## Créditos

#### ARDUINO MOS 6502 SBC EMULATOR (MOSuino)

- Developed by RafaG

- Based on Code Monkey King's code
