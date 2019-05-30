#ifndef ELECTROVALVULA_HPP
#define ELECTROVALVULA_HPP

#include "Arduino.h"

#ifdef USAR_DIRECTIO
#include <DirectIO.h>
#endif

#define USAR_ELECTROVALVULA

// Define si la valvula enciende con el pin en alto o bajo
#ifdef INVIERTE_ELECTROVALVULA
#define ON 1
#define OFF 0
#else
#define ON 0
#define OFF 1
#endif  // INVIERTE_ELECTROVALVULA

void _apagarEval(const byte pin);
void _encenderEval(const byte pin);

#endif /* end of include guard: ELECTROVALVULA_HPP */
