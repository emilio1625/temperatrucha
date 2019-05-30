#include "Arduino.h"
#include "inc/electrovalvula.hpp"

#ifdef USAR_DIRECTIO
void _apagaEval(const byte pin)
{
    OutputPin pinEval(pin);
    pinEval = OFF;
    #ifdef DEPURAR
    Serial.print(F("Electrovalvula "));
    Serial.print(pin);
    Serial.println(F(" apagada"));
    #endif
}
void _enciendeEval(const byte pin)
{
    OutputPin pinEval(pin);
    pinEval = ON;
    #ifdef DEPURAR
    Serial.print(F("Electrovalvula "));
    Serial.print(pin);
    Serial.println(F(" encendida"));
    #endif
}
#else
void _apagarEval(const byte pin)
{
    digitalWrite(pin, OFF);
    #ifdef DEPURAR
    Serial.print(F("Electrovalvula "));
    Serial.print(pin);
    Serial.println(F(" apagada"));
    #endif
}
void _encenderEval(const byte pin)
{
    digitalWrite(pin, ON);
    #ifdef DEPURAR
    Serial.print(F("Electrovalvula "));
    Serial.print(pin);
    Serial.println(F(" encendida"));
    #endif
}
#endif  // USAR_DIRECTIO

