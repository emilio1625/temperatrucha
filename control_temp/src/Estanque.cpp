#include "inc/Estanque.hpp"
#include "EEPROM.h"
#include "inc/electrovalvula.hpp"

byte Estanque::totalEstanques = 0;

Estanque::Estanque(byte pin_termometros, byte pin_electrovalvula)
    : _busTermometros(pin_termometros), _termometros(&_busTermometros)
{
    _pin_eval     = pin_electrovalvula;
    _estadoActual = _estadoSiguiente = REPOSO;
    desactivado = true;
}

void Estanque::begin()
{
    pinMode(_pin_eval, OUTPUT);
    apagarElectrovalvula();
    id = ++totalEstanques;
    _termometros.begin();
    _configTermometros(_termometros, NUM_TERMOMETROS);
    restaurarUmbrales();
    desactivado = false;
#ifdef DEPURAR
    Serial.print(F("Estanque "));
    Serial.print(id);
    Serial.println(" creado");
#endif
}

/* Nunca debemos llegar aqui, pero por si acaso */
Estanque::~Estanque()
{
    apagarElectrovalvula();
#ifdef DEPURAR
    Serial.print(F("Estanque "));
    Serial.print(id);
    Serial.println(F(" destruido"));
    Serial.flush();
#endif
}

void Estanque::apagarElectrovalvula()
{
    _apagarEval(_pin_eval);
}

void Estanque::encenderElectrovalvula()
{
    _encenderEval(_pin_eval);
}

void Estanque::revisarTemperatura()
{
    _revisarTemperatura(_termometros, temps, NUM_TERMOMETROS);
}

enum ESTADO_TERMOMETROS Estanque::revisarUmbrales()
{
    byte menor = 0;
    byte mayor = 0;
    for (int i = 0; i < NUM_TERMOMETROS; i++) {
        // Checamos si alguna las temperaturas son menores al umbral hacia abajo
        menor |= (temps[i] < umbrales[0]) << i;
        // Checamos si alguna temperatura es mayor que el umbral hacia arriba
        mayor |= (temps[i] > umbrales[1]) << i;
        // Cuando un sensor se desconecta su valor es -127.0
        // El rango del sensor es de -55C a 125C
        if (temps[i] <= -55.0 || temps[i] >= 125.0) {
            error(ERROR_NO_TERMOMETROS);
        }
    }

    if (menor) {
#ifdef DEPURAR
        Serial.println(F("Estado termometros: debajo"));
#endif
        return DEBAJO;
    } else if (mayor) {
#ifdef DEPURAR
        Serial.println(F("Estado termometros: encima"));
#endif
        return ENCIMA;
    } else {
#ifdef DEPURAR
        Serial.println(F("Estado termometros: dentro"));
#endif
        return DENTRO;
    }
}

void Estanque::actualizarEstado()
{
    enum ESTADO_TERMOMETROS entradas = revisarUmbrales();
    static unsigned long int ahora;

#ifdef DEPURAR
    Serial.print(F("Estanque "));
    Serial.println(id);
    Serial.print(F("Estado actual: "));
#endif

    if (desactivado == true) {
        _estadoActual = _estadoSiguiente = REPOSO;
        apagarElectrovalvula();
#ifdef DEPURAR
        Serial.println(F("desactivado"));
#endif
        return;
    }

    switch (_estadoActual) {
    case REPOSO:
#ifdef DEPURAR
        Serial.println(F("REPOSO"));
#endif
        apagarElectrovalvula();
        if (entradas == ENCIMA) {
            // Si las estradas estan encima, solo podemos esperar
            // a que el estanque se enfrie :c
            _estadoSiguiente = REPOSO;
        } else if (entradas == DEBAJO) {
            // Si bajo la temperatura esperamos un poco antes de
            // comenzar a calentar
            _estadoSiguiente   = CUENTA_ATRAS;
            _inicioCuentaAtras = millis();
        } else if (entradas == DENTRO) {
            // Si el estanque se encuentra bien, no debemos hacer nada
            _estadoSiguiente = REPOSO;
        } else {
            _estadoSiguiente = ERROR;  // nunca debemos llegar aqui
        }
        break;
    case CUENTA_ATRAS:
#ifdef DEPURAR
        Serial.println(F("CUENTA_ATRAS"));
#endif
        apagarElectrovalvula();
        switch (entradas) {
        case DEBAJO:
            // Si bajo un poco la temperatura, esperamos un intervalo
            // antes de comenzar a calentar
            ahora = millis();
#ifdef DEPURAR
            Serial.print(F("Cuenta atras "));
            Serial.println(ahora - _inicioCuentaAtras);
#endif
            if (ahora - _inicioCuentaAtras > INTERVALO_CUENTA_ATRAS) {
                // Comienza a contar el tiempo desde que comienza el
                // calentamiento y la temperatura al inicio de este
                encenderElectrovalvula();
                _inicioCalentamiento = millis();
                _estadoSiguiente     = CALENTANDO;
            } else {
                _estadoSiguiente = CUENTA_ATRAS;
            }
            break;
        case DENTRO:
        case ENCIMA:
            // Si mientras esperamos, las temperaturas se regulan,
            // regresamos al reposo
            _estadoSiguiente = REPOSO;
            break;
        default:
            _estadoSiguiente = ERROR;  // nunca debemos llegar aqui
            break;
        }
        break;
    case CALENTANDO:
#ifdef DEPURAR
        Serial.println(F("CALENTANDO"));
#endif
        encenderElectrovalvula();
        if (entradas == ENCIMA) {
            // Ya calentamos hasta el umbral hacia arriba
            _estadoSiguiente = REPOSO;
        } else if (entradas == DEBAJO) {
            // Si ha pasado un intervalo de tiempo y la temperatura no ha
            // aumentado, probablemente el calentador fallo y debemos
            // cerrar la valvula o se desbordara el tanque
            ahora = millis();
            if (ahora - _inicioCalentamiento < INTERVALO_CALENTAMIENTO) {
#ifdef DEPURAR
                Serial.print(F("Cuenta atras calentamiento "));
                Serial.println(ahora - _inicioCalentamiento);
#endif
                _estadoSiguiente = CALENTANDO;
            } else {
                error(ERROR_CALENTADOR_FALLO);
            }
        }
        break;
    case ERROR:
    default:
        error(ERROR_PROGRAMACION);
        break;
    }
    _estadoActual = _estadoSiguiente;
}

void Estanque::guardarUmbrales()
{
#ifdef DEPURAR
    Serial.print(F("Guardando Umbrales de Estanque "));
    Serial.println(id);
    Serial.print(F("umbrales[0]: "));
    Serial.println(umbrales[0]);
    Serial.print(F("umbrales[1]: "));
    Serial.println(umbrales[1]);
#endif
    EEPROM.put(DIR_UMBRALES, umbrales[0]);
    EEPROM.put(DIR_UMBRALES + sizeof(float), umbrales[1]);
#ifdef DEPURAR
    restaurarUmbrales();
#endif
}

void Estanque::restaurarUmbrales()
{
#ifdef DEPURAR
    Serial.print(F("Restaurando Umbrales de Estanque "));
    Serial.println(id);
#endif
    EEPROM.get<float>(DIR_UMBRALES, umbrales[0]);
    EEPROM.get<float>(DIR_UMBRALES + sizeof(float), umbrales[1]);
#ifdef DEPURAR
    Serial.print(F("umbrales[0]: "));
    Serial.println(umbrales[0]);
    Serial.print(F("umbrales[1]: "));
    Serial.println(umbrales[1]);
#endif
}

void Estanque::error(enum BU_ERRNO errno)
{
    apagarElectrovalvula();
    bu_error(errno);
}
