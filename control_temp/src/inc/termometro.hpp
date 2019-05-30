#ifndef TERMOMETRO_HPP
#define TERMOMETRO_HPP

#include "DallasTemperature.h"
#include "OneWire.h"

#ifdef USAR_LCD
#include "pantalla.hpp"
extern LiquidCrystal_PCF8574 lcd;
#endif

#include "error.hpp"

// Estas definiciones deben hacerse en el archivo del programa principal
#ifndef NUM_TERMOMETROS /* Termometros por estanque */
#define NUM_TERMOMETROS 2
#endif /* ifndef NUM_TERMOMETROS */

#ifndef RESOLUCION_TEMPERATURA
#define RESOLUCION_TEMPERATURA 11 // menor resolucion, mas rapido (9 - 12)
#endif /* ifndef RESOLUCION_TEMPERATURA */

#ifdef USAR_LCD
void _imprimirTemp(const float temps[], byte i);
void _imprimirTemp1(const float temps[], byte i);
#endif

/* 
 * Configura los termometros, debe llamarse desde el contructor de las clases
 * que usen el termometro
 */
void _configTermometros(DallasTemperature &termometros, byte numSensores);

/* 
 * Actualiza las temperaturas y las guarda en el arreglo temps. Si no encuentra
 * todos los termometros, rellena con la misma medicion que el ultimo termometro
 * que encuentre en el mismo pin, o con -127 cuando no encuentre ninguno
 */
void _revisarTemperatura(DallasTemperature &termometros,
                       float temps[],
                       byte numSensores);

#endif /* end of include guard: TERMOMETRO_HPP */
