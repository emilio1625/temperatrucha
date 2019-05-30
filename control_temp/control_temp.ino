/* Temperatrucha - Control de temperatura para un estanque de peces
 *  Copyright © 2019 Emilio Cabrera <Emilio Cabrera>
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included
 *  in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 *  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 *  OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/* Temperatrucha
 * Este programa controla la temperatura de estanques de peces
 * Usa una maquina de estados para controlar la temperatura
 * El Arduino esta conectado a 2 más termometros y una electrovalvula por
 * estanque que permite la entrada de agua caliente, aunque puede ser
 * adaptado al uso de un calentador electrico.
 * Ademas, para mostrar la temperatura de los estanque y configurar la
 * temperatura a la que deben de estar, usa una LCD de 16x2 y un encoder
 */

#define USAR_LCD
//#define DEPURAR
#define BRATE 250000

#include "Arduino.h"
// Biblioteca para el planeador
#include "TaskScheduler.h"
// Bibliotecas para el estanque
#include "src/inc/Estanque.hpp"
// Bibliotecas para la pantalla
#include "src/inc/pantalla.hpp"
// Bibliotecas para el menu
#include "ClickEncoder.h"
#include "TimerOne.h"
#include "menu.h"
#include "menuIO/PCF8574Out.h"
#include "menuIO/chainStream.h"
#include "menuIO/clickEncoderIn.h"



////////////////////////////////////////////////////////////////////////////////
///////                          Estanques                               ///////
////////////////////////////////////////////////////////////////////////////////
/* Definicion de pines para los estanques */
// Este arduino controlara 3 estanques
#define NUM_ESTANQUES 3
// Pines de los termometros
#define PIN_TERMOMETROS_1 5
#define PIN_TERMOMETROS_2 6
#define PIN_TERMOMETROS_3 7
// Pines de las electrovalvulas
#define PIN_EVAL_1 8
#define PIN_EVAL_2 9
#define PIN_EVAL_3 10
Estanque estanques[3] = {
    Estanque(PIN_TERMOMETROS_1, PIN_EVAL_1),
    Estanque(PIN_TERMOMETROS_2, PIN_EVAL_2),
    Estanque(PIN_TERMOMETROS_3, PIN_EVAL_3),
};

// Solo se revisa un estanque a la vez, esta variable
// contienen la referencia al estanque actual
Estanque *estanqueActual = &estanques[0];

////////////////////////////////////////////////////////////////////////////////
///////                             Tareas                               ///////
////////////////////////////////////////////////////////////////////////////////

/* Lista de tareas a realizar */
void revisarTemperatura()
{
    estanqueActual->revisarTemperatura();
}

void actualizarEstado()
{
    estanqueActual->actualizarEstado();
}
// Indica si el menu esta usando la pantalla
bool menuOcupado = false;

void imprimirTemperatura()
{
    static byte i;
    static Estanque *estanque;
    estanque = &estanques[i++ % NUM_ESTANQUES];
    if (menuOcupado == false) {
        lclear();
        lprint(F("Estanque "));
        lprint(estanque->id);
        lprintln(F(": "));
        lprint(F("T: "));
        _imprimirTemp(estanque->temps, 0);
        lprint(F(" "));
        _imprimirTemp(estanque->temps, 1);
    }
}
/* Planeador - elige la siguiente tarea a realizar */
Scheduler planeador;

/* Tareas a realizar */
Task checarTemp(TASK_IMMEDIATE, TASK_FOREVER, &revisarTemperatura);
Task checarEstanque(TASK_IMMEDIATE, TASK_FOREVER, &actualizarEstado);
Task mostrarTemp(2 * TASK_SECOND, TASK_FOREVER, &imprimirTemperatura);

////////////////////////////////////////////////////////////////////////////////
///////                              Menu                                ///////
////////////////////////////////////////////////////////////////////////////////
using namespace Menu;
#define MAX_DEPTH 2

/* Encoder */
// Define los pines usados por el encoder
#define PIN_CLK 2
#define PIN_DT 3
#define PIN_SW 4


ClickEncoder clickEncoder(PIN_CLK, PIN_DT, PIN_SW, 4);
ClickEncoderStream encStream(clickEncoder, 1);
MENU_INPUTS(in, &encStream);
void timerIsr()
{
    clickEncoder.service();
}

LiquidCrystal_PCF8574 lcd(DIR_LCD);
MENU_OUTPUTS(out, MAX_DEPTH, LCD_OUT(lcd, {0, 0, 16, 2}), NONE);

void estanque1GuardaUmbrales() {estanques[0].guardarUmbrales();}
void estanque2GuardaUmbrales() {estanques[1].guardarUmbrales();}
void estanque3GuardaUmbrales() {estanques[2].guardarUmbrales();}

MENU(Estanque1,
     "Estanque 1",
     doNothing,
     anyEvent,
     noStyle,
     FIELD(estanques[0].umbrales[0],
           "Temp Min",
           "C",
           20,
           40,
           0.5,
           0.1,
           estanque1GuardaUmbrales,
           exitEvent,
           noStyle),
     FIELD(estanques[0].umbrales[1],
           "Temp Max",
           "C",
           0,
           40,
           0.5,
           0.1,
           estanque1GuardaUmbrales,
           exitEvent,
           noStyle),
     EXIT("atras"));

MENU(Estanque2,
     "Estanque 2",
     doNothing,
     anyEvent,
     noStyle,
     FIELD(estanques[1].umbrales[0],
           "Temp Min",
           "C",
           0,
           40,
           0.5,
           0.1,
           estanque2GuardaUmbrales,
           exitEvent,
           noStyle),
     FIELD(estanques[1].umbrales[1],
           "Temp Max",
           "C",
           0,
           40,
           0.5,
           0.1,
           estanque2GuardaUmbrales,
           exitEvent,
           noStyle),
     EXIT("atras"));

MENU(Estanque3,
     "Estanque 3",
     doNothing,
     anyEvent,
     noStyle,
     FIELD(estanques[2].umbrales[0],
           "Temp Min",
           "C",
           0,
           40,
           0.5,
           0.1,
           estanque3GuardaUmbrales,
           exitEvent,
           noStyle),
     FIELD(estanques[2].umbrales[1],
           "Temp Max",
           "C",
           0,
           40,
           0.5,
           0.1,
           estanque3GuardaUmbrales,
           exitEvent,
           noStyle),
     EXIT("atras"));

MENU(mainMenu,
     "Main menu",
     doNothing,
     noEvent,
     wrapStyle,
     SUBMENU(Estanque1),
     SUBMENU(Estanque2),
     SUBMENU(Estanque3),
     EXIT("atras"));

NAVROOT(MenuEstanque,
        mainMenu,
        MAX_DEPTH,
        in,
        out);  // the navigation root object

////////////////////////////////////////////////////////////////////////////////
////////////////                    Preparacion                   //////////////
////////////////////////////////////////////////////////////////////////////////

void setup()
{
    #ifdef DEPURAR
    Serial.begin(BRATE);
    #endif
    INICIAR_LCD(lcd);
    lprintln(F("Iniciando..."));
    // Configuramos el estanque
    for (byte i = 0; i < NUM_ESTANQUES; ++i, estanqueActual = &estanques[i]) {
        estanqueActual->begin();
    }
    lclear();
    lprint(F("Bienvenid@"));
    // Le indicamos al planeador las tareas a realizar
    planeador.init();
    planeador.addTask(checarTemp);
    planeador.addTask(checarEstanque);
    planeador.addTask(mostrarTemp);
    planeador.enableAll();
    // Configuramos el menu
    MenuEstanque.showTitle = false;
    MenuEstanque.doNav(escCmd);
    MenuEstanque.timeOut = 30;
    Timer1.initialize(1000);
    Timer1.attachInterrupt(timerIsr);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////            Programa principal         /////////////////////
////////////////////////////////////////////////////////////////////////////////

void loop()
{
    for (byte i = 0;; i = (i + 1) % NUM_ESTANQUES) {
        estanqueActual = &estanques[i]; // cambiamos de estanque
#ifdef DEPURAR
        Serial.print(F("Estanque "));
        Serial.println(estanqueActual->id);
#endif
        planeador.execute(); // realizamos las tareas necesarias
        MenuEstanque.poll(); // revisamos si estan usando el menu
        if (MenuEstanque.sleepTask) {
            menuOcupado = false;
        } else {
            menuOcupado = true;
        }
    }
}
