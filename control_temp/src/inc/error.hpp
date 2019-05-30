#ifndef ERROR_HPP
#define ERROR_HPP

#include "Arduino.h"

#define PIN_BUZZER 11

enum BU_ERRNO {
    // Error de la LCD
    ERROR_NO_LCD = 1,
    // Error en el calentador
    ERROR_CALENTADOR_FALLO,
    // Codigo de error de los termometros
    ERROR_NO_TERMOMETROS,
    ERROR_FALTAN_TERMOMETROS,
    // Error de programacion
    ERROR_PROGRAMACION,
};

void bu_error(enum BU_ERRNO errno);

#endif /* end of include guard: ERROR_HPP */
