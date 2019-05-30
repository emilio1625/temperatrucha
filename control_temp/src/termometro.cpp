#define USAR_LCD
#ifdef USAR_LCD
#include "inc/pantalla.hpp"
extern LiquidCrystal_PCF8574 lcd;
#endif
#include "inc/termometro.hpp"

void _imprimirTemp(const float temps[], byte i)
{
    lcd.print(temps[i], 2);
    lcd.print(F("C"));
}

void _imprimirTemp1(const float temps[], byte i)
{
    lcd.print(F("Temp "));
    lcd.print(i + 1);
    lcd.print(F(": "));
    _imprimirTemp(temps, i);
}

void _configTermometros(DallasTemperature &termometros, byte numSensores)
{
    #ifdef DEPURAR
    Serial.println(F("Configurando termometros"));
    #endif
    termometros.begin();
    byte numTermometrosEncontrados = termometros.getDeviceCount();
    #ifdef DEPURAR
    Serial.print(F("Encontre "));
    Serial.print(numTermometrosEncontrados);
    Serial.println(F(" termometros"));
    #endif
    if (!numTermometrosEncontrados) {
        bu_error(ERROR_NO_TERMOMETROS);
    } else if (numTermometrosEncontrados < numSensores) {
        bu_error(ERROR_FALTAN_TERMOMETROS);
    }

    termometros.setResolution(RESOLUCION_TEMPERATURA);
    termometros.setWaitForConversion(true);
    termometros.setCheckForConversion(true);
}

void _revisarTemperatura(DallasTemperature &termometros,
                         float temps[],
                         byte numSensores)
{
    DeviceAddress termometroActual;
    termometros.requestTemperatures();
    for (byte i = 0, intentos = 0; i < numSensores && intentos < 10; i++) {
        termometros.getAddress(termometroActual, i);
        temps[i] = termometros.getTempC(termometroActual);
        if (temps[i] < 0) {
            intentos ++;
            i = 0;
            termometros.requestTemperatures();
            #ifdef DEPURAR
            Serial.print(F("Intento: "));
            Serial.print(intentos);
            Serial.print(F(", "));
            Serial.println(i);
            #endif
        }
    }
    #ifdef DEPURAR
    Serial.print(F("Teperaturas: "));
    Serial.print(temps[0], 2);
    Serial.print(F(" "));
    Serial.println(temps[1], 2);
    Serial.flush();
    #endif
}
