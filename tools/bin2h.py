#!/usr/bin/env python3
"""
bin2h.py - convierte un volcado .bin de una ROM en un array PROGMEM en C
para usar en el emulador kimuino.

Uso:
    python3 bin2h.py entrada.bin NOMBRE_ARRAY salida.h [--base 0x8000]

Ejemplo (SYM-1):
    python3 bin2h.py supermon.bin SYM1_MONITOR_ROM roms/sym1/sym1_monitor.h --base 0x8000

El fichero generado define:
    #define <NOMBRE_ARRAY>_BASE  <base>
    #define <NOMBRE_ARRAY>_SIZE  <tamano en bytes>
    const uint8_t <NOMBRE_ARRAY>[] PROGMEM = { ... };

que puedes usar tal cual en tu board_XXX.h:
    #include "../roms/sym1/sym1_monitor.h"
    #define MONITOR_ROM_BASE SYM1_MONITOR_ROM_BASE
    #define MONITOR_ROM_SIZE SYM1_MONITOR_ROM_SIZE
    #define MONITOR_ROM      SYM1_MONITOR_ROM
"""
import argparse
import os


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("input_bin", help="fichero .bin de entrada (el dump de la ROM)")
    parser.add_argument("array_name", help="nombre del array C a generar, ej: SYM1_MONITOR_ROM")
    parser.add_argument("output_h", help="fichero .h de salida")
    parser.add_argument("--base", default="0x0000", help="direccion base en hex donde arranca esta ROM (ej: 0x8000)")
    parser.add_argument("--bytes-per-line", type=int, default=16)
    args = parser.parse_args()

    with open(args.input_bin, "rb") as f:
        data = f.read()

    if len(data) == 0:
        raise SystemExit(f"Error: {args.input_bin} esta vacio")

    base = int(args.base, 16)
    name = args.array_name
    guard = os.path.basename(args.output_h).upper().replace(".", "_").replace("-", "_")

    with open(args.output_h, "w") as out:
        out.write("#pragma once\n")
        out.write(f"// Generado automaticamente por tools/bin2h.py desde {os.path.basename(args.input_bin)}\n")
        out.write(f"// {len(data)} bytes, base {hex(base)} - {hex(base + len(data) - 1)}\n")
        out.write("#include <avr/pgmspace.h>\n#include <stdint.h>\n\n")
        out.write(f"#define {name}_BASE {hex(base)}\n")
        out.write(f"#define {name}_END  {hex(base + len(data) - 1)}\n")
        out.write(f"#define {name}_SIZE {len(data)}\n\n")
        out.write(f"const uint8_t {name}[] PROGMEM = {{\n")

        for i in range(0, len(data), args.bytes_per_line):
            chunk = data[i:i + args.bytes_per_line]
            line = ", ".join(f"0x{b:02X}" for b in chunk)
            comment = f"// {hex(base + i)}"
            out.write(f"    {line},{' ' if i + args.bytes_per_line < len(data) else ' '}{comment}\n")

        # quitar la ultima coma sobrante es opcional en C, dejamos coma final (valido en C99+)
        out.write("};\n")

    print(f"OK: {args.output_h} generado ({len(data)} bytes, {name})")


if __name__ == "__main__":
    main()
