#define USAR_LCD

#ifdef USAR_LCD
#include "inc/pantalla.hpp"
extern LiquidCrystal_PCF8574 lcd;
#endif
#include "inc/error.hpp"
#include "Arduino.h"


void bu_error(enum BU_ERRNO errno)
{
    Serial.print(F("Error "));
    Serial.println(errno);
    byte apagarPantalla = 0;

    /*
     * Cuando tenemos un error dejamos de hacer todo y nos quejamos bastante
     */
    while (1) {
#ifdef USAR_LCD
        switch (errno) {
        case ERROR_CALENTADOR_FALLO:
            lclear();
            lprintln(F("Error calentando"));
            lprintln(F("Revisa conexion"));
            break;
        case ERROR_NO_TERMOMETROS:
            lclear();
            lprintln(F("Error termometros"));
            lprintln(F("Revisa conexion"));
            break;
        case ERROR_FALTAN_TERMOMETROS:
            lclear();
            lprintln(F("Faltan termometros"));
            lprintln(F("Revisa conexion"));
            delay(20000);
            return;
            break;
        case ERROR_PROGRAMACION:
            lclear();
            lprintln(F("Error programa"));
            lprintln(F("Llama a Emilio"));
            break;
        case ERROR_NO_LCD:
        default:
            break;
        }
#endif
        for (byte i = 0; i < errno; i++) {
            tone(PIN_BUZZER, 1900);
            delay(300);
            noTone(PIN_BUZZER);
            delay(200);
        }
#ifdef USAR_LCD
        if (lcd.availableForWrite()) {
            lcd.setBacklight(apagarPantalla ^= 0xff);
        }
#endif
        delay(500);
    }
}
