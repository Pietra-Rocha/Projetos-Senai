#pragma once
#include <Arduino.h>
#include <Adafruit_MCP23X17.h>

class Leds
{
public:
    explicit Leds(Adafruit_MCP23X17 &mcp_dev);

    void begin();

    // Funções para ligar LEDs individuais
    void ligarSetaDireita();
    void ligarSetaEsquerda();
    void ligarFarolDireito();
    void ligarFarolEsquerdo();
    void ligarLanternaDireita();
    void ligarLanternaEsquerda();

    // Funções para desligar LEDs individuais
    void desligarSetaDireita();
    void desligarSetaEsquerda();
    void desligarFarolDireito();
    void desligarFarolEsquerdo();
    void desligarLanternaDireita();
    void desligarLanternaEsquerda();

    // Funções para piscar LEDs individuais (não bloqueantes)
    void piscarSetaDireita(uint16_t intervalo = 500);
    void piscarSetaEsquerda(uint16_t intervalo = 500);
    void piscarFarolDireito(uint16_t intervalo = 500);
    void piscarFarolEsquerdo(uint16_t intervalo = 500);
    void piscarLanternaDireita(uint16_t intervalo = 500);
    void piscarLanternaEsquerda(uint16_t intervalo = 500);

    // Funções para piscar grupos de LEDs
    void piscarFaroisJuntos(uint16_t intervalo = 500);
    void piscarFaroisAlternados(uint16_t intervalo = 500);
    void piscarLanternasJuntas(uint16_t intervalo = 500);
    void piscarLanternasAlternadas(uint16_t intervalo = 500);

    // Atualiza o estado dos LEDs piscantes (deve ser chamado no loop)
    void atualizar();

    // Para de piscar um LED específico
    void pararPiscarSetaDireita();
    void pararPiscarSetaEsquerda();
    void pararPiscarFarolDireito();
    void pararPiscarFarolEsquerdo();
    void pararPiscarLanternaDireita();
    void pararPiscarLanternaEsquerda();
    void pararPiscarFarois();
    void pararPiscarLanternas();
    void pararPiscarTodos();

    // Funções para controlar grupos de LEDs
    void ligarFarois();
    void desligarFarois();
    void ligarLanternas();
    void desligarLanternas();
    void ligarSetas();
    void desligarSetas();
    void desligarTodos();

private:
    Adafruit_MCP23X17 &mcp_dev;

    // IDs lógicos dos LEDs
    enum LedId : uint8_t {
        LED_SETA_DIR = 0,
        LED_SETA_ESQ,
        LED_FAROL_DIR,
        LED_FAROL_ESQ,
        LED_LANTERNA_DIR,
        LED_LANTERNA_ESQ,
        LED_MAX
    };

    // Pinos físicos no MCP (LOW = ligado)
    static const uint8_t PIN_SETA_DIR      = 10;
    static const uint8_t PIN_SETA_ESQ      = 13;
    static const uint8_t PIN_FAROL_DIR     = 11;
    static const uint8_t PIN_FAROL_ESQ     = 12;
    static const uint8_t PIN_LANTERNA_DIR  = 15;
    static const uint8_t PIN_LANTERNA_ESQ  = 14;

    struct PiscaState {
        bool ativo;
        bool estado;               // true = aceso
        uint16_t intervalo;        // ms
        unsigned long ultimaTroca; // timestamp
    };

    PiscaState pisca[LED_MAX];

    bool modoFarolJunto;
    bool modoFarolAlternado;
    bool modoLanternaJunta;
    bool modoLanternaAlternada;

    // helpers usados no .cpp

    inline void writeOn(uint8_t pin);    // LOW
    inline void writeOff(uint8_t pin);   // HIGH

    uint8_t pinOf(LedId id) const;

    void setLed(LedId id, bool ligado);

    void piscarLed(LedId id, uint16_t intervaloMs);
    void pararPiscarLed(LedId id);

    void writeLedEstado(LedId id, bool aceso);
    void sincronizarAlternado(LedId dirId, LedId esqId);

    bool isFarol(int id) const;
    bool isLanterna(int id) const;
};