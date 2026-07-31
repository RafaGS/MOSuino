# MOSuino — emulador multi-placa 6502 (KIM-1 / SYM-1 / AIM 65 / Junior / System 1)

Todas las placas se comunican **únicamente por Serial** (sin pantalla ni teclado
físico), igual que el proyecto original.

## Placas donde aplica

- [MOS KIM-1](https://minibots.wordpress.com/2026/06/10/emulador-de-mos-kim-1-con-arduino-uno-r3/)

- [Synertek SYM-1]()

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

## Nota sobre el SYM-1

El SYM-1 arranca Supermon 1.1 saltando directamente a `$8000` (no usa un
vector de reset "normal" en `$FFFC`; en el hardware real esos vectores se
cargan con trucos de RAM protegida). La entrada/salida por Serial se
intercepta en los puntos de "vectored I/O" reales del monitor (`$8A41` para
GETCH, `$8A55` para OUTCH), localizados por disassembly del propio dump.

La RAM de usuario por defecto es de 1KB (`0000-03FF`), la configuración
"de fábrica" real del SYM-1 — apto para Arduino Uno/Nano. Si usas un Mega
(más SRAM), puedes subir `RAM_SIZE` en `boards/board_sym1.h` hasta 4096
(`0000-0FFF`, el máximo ampliable a bordo).

### BASIC 1.1 (ROM opcional, ya integrado)

El BASIC 1.1 de Synertek (8K, `$C000-$DFFF`) ya está integrado. Solo tiene
un punto de entrada de código real en `$C000` (COLD start); el resto de la
ROM a partir de `$C003` son datos/vectores internos, no código. BASIC llama
directamente a las rutinas del monitor `$8A1B` (GETCH) y `$8A47` (OUTCH) —
las mismas cuyo salto vectorizado ya interceptamos para Serial — así que
funciona sin ningún intercepto adicional.

Para entrar en BASIC, desde el monitor (por Serial) usa el comando de
ejecutar en una dirección (`G` + `C000`), igual que harías en el hardware
real. El tamaño de flash total con monitor+BASIC (~18.9KB de código+datos)
cabe sin problema en un Uno (32KB); la RAM (~1.3KB de 2KB en un Uno) va más
justa — funciona, pero un Mega da más margen si notas inestabilidad.

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
   python3 tools/bin2h.py monitor.bin AIM65_MONITOR_ROM roms/aim65/aim65_monitor.h --base 0xA000
   ```
   
   (el `--base` es la dirección donde arranca esa ROM en el mapa de memoria
   real de la máquina — ese dato sí lo necesito yo para ajustarlo bien).

2. En `boards/board_aim65.h`, sustituye el placeholder por el include real y
   ajusta `RAM_SIZE`, `RIOT_BASE`/`RIOT_SIZE` (o lo que corresponda) al mapa
   de memoria real de la placa.

3. En `memmap.h`, rellena el bloque `#elif defined(BOARD_AIM65)` de
   `read6502()`/`write6502()` con las direcciones de E/S por Serial
   (equivalentes a las intercepciones GETCH/OUTCH del KIM-1 y el SYM-1).

Repetimos este mismo proceso, uno por placa. Toca el turno del AIM 65.

## Nota de compatibilidad

`fake6502.h` incluye ahora los prototipos de las funciones de direccionamiento
y de opcodes. Esto es necesario para compilar fuera del IDE de Arduino (que
normalmente los genera solo automáticamente); con `arduino-cli`/el IDE sigue
funcionando exactamente igual que antes.

## Validación realizada

Ambas placas (KIM-1 y SYM-1) se han comprobado con una compilación de
sintaxis real usando `avr-g++` (no solo revisión visual), con stubs mínimos
de `Serial`/`EEPROM`. Compilan sin errores nuevos respecto al `.ino`
original (solo avisos preexistentes propios del estilo de `fake6502.c`).
