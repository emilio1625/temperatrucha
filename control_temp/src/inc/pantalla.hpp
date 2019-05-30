#ifndef PANTALLA_HPP
#define PANTALLA_HPP

#include "Wire.h"
#include "LiquidCrystal_PCF8574.h"

/* Configuracion para la pantalla LCD */
#define DIR_LCD 0x27  // Tipo de LCD 16x2

// macros para imprimir en pantalla
#define lprint(str) ({ lcd.print(str); })
#define lprintln(str)        \
    ({                       \
        lcd.print(str);      \
        lcd.setCursor(0, 1); \
    })

// macro para limpiar la pantalla
#define lclear()     \
    ({               \
        lcd.clear(); \
        lcd.home();  \
    })

#define INICIAR_LCD(lcd)                            \
    ({                                              \
        Wire.begin();                               \
        Wire.beginTransmission(0x27);               \
        if (Wire.endTransmission() == 0) {          \
            Serial.println(F("LCD encontrada"));    \
        } else {                                    \
            Serial.println(F("LCD no encontrada")); \
        }                                           \
        lcd.begin(16, 2);                           \
        lcd.setBacklight(255);                      \
    })
#endif
