#pragma once
/**********************************************\
 ==============================================

              SELECTOR DE PLACA

   Se selecciona UNA sola placa, de dos formas
   posibles (la segunda tiene prioridad si se
   usa, porque llega ya definida por el compilador):

   1) Editando el "#define BOARD_KIM1" de aqui
      abajo antes de compilar desde el IDE.

   2) Pasando el define desde arduino-cli, sin
      tocar este fichero, por ejemplo:

      arduino-cli compile --fqbn arduino:avr:uno \
        --build-property "compiler.cpp.extra_flags=-DBOARD_SYM1" \
        kimuino

 ==============================================
\**********************************************/

// Si NINGUNO de los defines de placa llega ya puesto por arduino-cli
// (-DBOARD_XXX), se usa KIM-1 como placa por defecto.
#if !defined(BOARD_KIM1)  && !defined(BOARD_SYM1)    && \
    !defined(BOARD_AIM65) && !defined(BOARD_JUNIOR)  && \
    !defined(BOARD_SYSTEM1)
    #define BOARD_KIM1
#endif

// Comprobacion de que solo hay UNA placa seleccionada
#if (defined(BOARD_KIM1)  + defined(BOARD_SYM1) + defined(BOARD_AIM65) + \
     defined(BOARD_JUNIOR) + defined(BOARD_SYSTEM1)) > 1
    #error "Solo se puede definir UNA placa a la vez (BOARD_KIM1 / BOARD_SYM1 / BOARD_AIM65 / BOARD_JUNIOR / BOARD_SYSTEM1)"
#endif

#if defined(BOARD_KIM1)
    #include "boards/board_kim1.h"
#elif defined(BOARD_SYM1)
    #include "boards/board_sym1.h"
#elif defined(BOARD_AIM65)
    #include "boards/board_aim65.h"
#elif defined(BOARD_JUNIOR)
    #include "boards/board_junior.h"
#elif defined(BOARD_SYSTEM1)
    #include "boards/board_system1.h"
#endif
