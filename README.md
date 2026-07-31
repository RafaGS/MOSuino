# kimuino — emulador multi-placa 6502 (KIM-1 / SYM-1 / AIM 65 / Junior / System 1)

Todas las placas se comunican **únicamente por Serial** (sin pantalla ni teclado
físico), igual que el proyecto original.

## Estructura

```
kimuino.ino              sketch principal (setup/loop, variables globales)
board_config.h           SELECTOR DE PLACA (ver mas abajo)
fake6502.h                nucleo de CPU 6502, generico, igual para todas las placas
memmap.h                  read6502()/write6502(), un bloque #if por placa
boards/
  board_kim1.h              mapa de memoria del KIM-1        (COMPLETO)
  board_sym1.h               mapa de memoria del SYM-1         (PENDIENTE)
  board_aim65.h               mapa de memoria del AIM 65         (PENDIENTE)
  board_junior.h                Elektor Junior Computer            (PENDIENTE)
  board_system1.h                Acorn System 1                      (PENDIENTE)
roms/
  kim1/                     ROMs y extensiones propias del KIM-1 (extraidas del .ino original)
  sym1/, aim65/, junior/, system1/    (vacias: aqui van los dumps que generes con bin2h.py)
tools/
  bin2h.py                  convierte un .bin de una ROM monitor en un .h PROGMEM
```

## Compilar con arduino-cli

Por defecto (sin pasar nada) se compila el **KIM-1**:

```bash
arduino-cli compile --fqbn arduino:avr:uno kimuino
```

Para elegir otra placa, sin tocar ningun fichero, se pasa el define por
`--build-property`:

```bash
# SYM-1
arduino-cli compile --fqbn arduino:avr:uno \
  --build-property "compiler.cpp.extra_flags=-DBOARD_SYM1" \
  kimuino

# AIM 65
arduino-cli compile --fqbn arduino:avr:uno \
  --build-property "compiler.cpp.extra_flags=-DBOARD_AIM65" \
  kimuino

# Elektor Junior Computer
arduino-cli compile --fqbn arduino:avr:uno \
  --build-property "compiler.cpp.extra_flags=-DBOARD_JUNIOR" \
  kimuino

# Acorn System 1
arduino-cli compile --fqbn arduino:avr:uno \
  --build-property "compiler.cpp.extra_flags=-DBOARD_SYSTEM1" \
  kimuino
```

(También puedes fijar la placa por defecto editando el `#define BOARD_KIM1` de
`board_config.h`, si prefieres no pasar nada por línea de comandos.)

## Añadir una placa nueva (paso a paso, con tu .bin ya en la mano)

1. Genera el `.h` a partir del dump con `tools/bin2h.py`:

   ```bash
   python3 tools/bin2h.py supermon.bin SYM1_MONITOR_ROM roms/sym1/sym1_monitor.h --base 0x8000
   ```

   (el `--base` es la dirección donde arranca esa ROM en el mapa de memoria
   real de la máquina — ese dato sí lo necesito yo para ajustarlo bien).

2. En `boards/board_sym1.h`, sustituye el placeholder por el include real y
   ajusta `RAM_SIZE`, `RIOT_BASE`/`RIOT_SIZE` (o lo que corresponda) al mapa
   de memoria real de la placa.

3. En `memmap.h`, rellena el bloque `#elif defined(BOARD_SYM1)` de
   `read6502()`/`write6502()` con las direcciones de E/S por Serial
   (equivalentes a las intercepciones OUTCH/GETCH del KIM-1).

Repetimos este mismo proceso, uno por placa, empezando por el SYM-1.

## Nota de compatibilidad

`fake6502.h` incluye ahora los prototipos de las funciones de direccionamiento
y de opcodes. Esto es necesario para compilar fuera del IDE de Arduino (que
normalmente los genera solo automáticamente); con `arduino-cli`/el IDE sigue
funcionando exactamente igual que antes.
