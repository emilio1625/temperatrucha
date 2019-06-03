#ifndef ESTANQUE_HPP
#define ESTANQUE_HPP

#include "Arduino.h"
#include "error.hpp"
#include "termometro.hpp"

#ifndef NUM_TERMOMETROS
#define NUM_TERMOMETROS 2
#endif

/* Tiempo de espera para comenzar a calentar */
#ifndef INTERVALO_CUENTA_ATRAS
#define INTERVALO_CUENTA_ATRAS 1 /* minutos */ * 60000
#endif

/* Tiempo de espera para que suba la temperatura */
#ifndef INTERVALO_CALENTAMIENTO
#define INTERVALO_CALENTAMIENTO 5 /* minutos */ * 60000
#endif

/* Direccion en la que se guardan los umbrales */
#ifndef DIR_UMBRALES
#define DIR_UMBRALES (id - 1) * sizeof(float) * 2
#endif

enum ESTADO_TERMOMETROS {
    DENTRO,
    DEBAJO,
    ENCIMA,
};

enum ESTADO_MAQUINA {
    REPOSO,
    CUENTA_ATRAS,
    CALENTANDO,
    ERROR,
};

class Estanque
{
public:
    bool desactivado;              // Indica si el estanque esta desactivado
    byte id;                       // Numero de estanque
    float umbrales[2];             // Umbral minimo y maximo
    float temps[NUM_TERMOMETROS];  // Tantas mediciones como umbrales por tanque
    static byte totalEstanques;    // El numero de estanques que hay en total
    Estanque(byte pin_termometros, byte pin_electrovalvula);
    void begin();
    void apagarElectrovalvula();
    void encenderElectrovalvula();
    /* Solo revisa y guarda las temperaturas */
    void revisarTemperatura();
    /* Compara las temperaturas con los umbrales */
    enum ESTADO_TERMOMETROS revisarUmbrales();
    /* Almacena los umbrales de temperatura en la EEPROM */
    void guardarUmbrales();
    /* Restaura los umbrales de temperatura de la EEPROM */
    void restaurarUmbrales();
    /* Decide si debe encender o apagar la electrovalvula */
    void actualizarEstado();
    virtual ~Estanque();

private:
    byte _pin_eval;  // pin de la electrovalvula
    OneWire _busTermometros;
    DallasTemperature _termometros;
    enum ESTADO_MAQUINA _estadoActual, _estadoSiguiente;
    unsigned long int _inicioCuentaAtras, _inicioCalentamiento;
    void error(enum BU_ERRNO errno);
};

#endif /* end of include guard: ESTANQUE_HPP */
