#include "leds.h"

// Construtor
Leds::Leds(Adafruit_MCP23X17 &mcp_ref) : mcp_dev(mcp_ref)
{
    // inicializa todos os canais de pisca
    for (int i = 0; i < LED_MAX; i++) {
        pisca[i].ativo = false;
        pisca[i].estado = false;      // false = apagado
        pisca[i].intervalo = 500;
        pisca[i].ultimaTroca = 0;
    }

    modoFarolJunto = false;
    modoFarolAlternado = false;
    modoLanternaJunta = false;
    modoLanternaAlternada = false;
}

void Leds::begin()
{
    // Configura pinos como saída
    mcp_dev.pinMode(PIN_SETA_DIR, OUTPUT);
    mcp_dev.pinMode(PIN_SETA_ESQ, OUTPUT);
    mcp_dev.pinMode(PIN_FAROL_DIR, OUTPUT);
    mcp_dev.pinMode(PIN_FAROL_ESQ, OUTPUT);
    mcp_dev.pinMode(PIN_LANTERNA_DIR, OUTPUT);
    mcp_dev.pinMode(PIN_LANTERNA_ESQ, OUTPUT);

    desligarTodos();
}

// ===== helpers elétricos =====
// Agora: ATIVO EM HIGH
inline void Leds::writeOn(uint8_t pin) {
    // LED ligado = HIGH
    mcp_dev.digitalWrite(pin, HIGH);
}

inline void Leds::writeOff(uint8_t pin) {
    // LED desligado = LOW
    mcp_dev.digitalWrite(pin, LOW);
}

// mapear LedId -> pino físico
uint8_t Leds::pinOf(LedId id) const {
    switch (id) {
        case LED_SETA_DIR:      return PIN_SETA_DIR;
        case LED_SETA_ESQ:      return PIN_SETA_ESQ;
        case LED_FAROL_DIR:     return PIN_FAROL_DIR;
        case LED_FAROL_ESQ:     return PIN_FAROL_ESQ;
        case LED_LANTERNA_DIR:  return PIN_LANTERNA_DIR;
        case LED_LANTERNA_ESQ:  return PIN_LANTERNA_ESQ;
        default:                return 0xFF;
    }
}

// liga/desliga direto um LED e cancela pisca dele
void Leds::setLed(LedId id, bool ligado) {
    // parar pisca deste canal
    pisca[id].ativo = false;

    // se mexer em farol, mata modos farol
    if (id == LED_FAROL_DIR || id == LED_FAROL_ESQ) {
        modoFarolJunto = false;
        modoFarolAlternado = false;
    }

    // se mexer em lanterna, mata modos lanterna
    if (id == LED_LANTERNA_DIR || id == LED_LANTERNA_ESQ) {
        modoLanternaJunta = false;
        modoLanternaAlternada = false;
    }

    uint8_t p = pinOf(id);
    if (ligado) writeOn(p);
    else        writeOff(p);
}

// coloca LED individual pra piscar
void Leds::piscarLed(LedId id, uint16_t intervaloMs) {
    PiscaState &st = pisca[id];
    st.ativo = true;
    st.intervalo = intervaloMs;
    st.ultimaTroca = millis();
    // st.estado mantém o último estado lógico (true=aceso/HIGH)
}

// para pisca individual e apaga
void Leds::pararPiscarLed(LedId id) {
    pisca[id].ativo = false;
    writeOff(pinOf(id)); // HIGH=on, então off=LOW
}

// ===== API semântica / pública =====

void Leds::ligarSetaDireita()        { setLed(LED_SETA_DIR, true); }
void Leds::desligarSetaDireita()     { setLed(LED_SETA_DIR, false); }
void Leds::ligarSetaEsquerda()       { setLed(LED_SETA_ESQ, true); }
void Leds::desligarSetaEsquerda()    { setLed(LED_SETA_ESQ, false); }

void Leds::ligarFarolDireito()       { setLed(LED_FAROL_DIR, true); }
void Leds::desligarFarolDireito()    { setLed(LED_FAROL_DIR, false); }
void Leds::ligarFarolEsquerdo()      { setLed(LED_FAROL_ESQ, true); }
void Leds::desligarFarolEsquerdo()   { setLed(LED_FAROL_ESQ, false); }

void Leds::ligarLanternaDireita()    { setLed(LED_LANTERNA_DIR, true); }
void Leds::desligarLanternaDireita() { setLed(LED_LANTERNA_DIR, false); }
void Leds::ligarLanternaEsquerda()   { setLed(LED_LANTERNA_ESQ, true); }
void Leds::desligarLanternaEsquerda(){ setLed(LED_LANTERNA_ESQ, false); }

void Leds::piscarSetaDireita(uint16_t ms)        { piscarLed(LED_SETA_DIR, ms); }
void Leds::piscarSetaEsquerda(uint16_t ms)       { piscarLed(LED_SETA_ESQ, ms); }
void Leds::piscarFarolDireito(uint16_t ms)       { piscarLed(LED_FAROL_DIR, ms); }
void Leds::piscarFarolEsquerdo(uint16_t ms)      { piscarLed(LED_FAROL_ESQ, ms); }
void Leds::piscarLanternaDireita(uint16_t ms)    { piscarLed(LED_LANTERNA_DIR, ms); }
void Leds::piscarLanternaEsquerda(uint16_t ms)   { piscarLed(LED_LANTERNA_ESQ, ms); }

void Leds::pararPiscarSetaDireita()        { pararPiscarLed(LED_SETA_DIR); }
void Leds::pararPiscarSetaEsquerda()       { pararPiscarLed(LED_SETA_ESQ); }
void Leds::pararPiscarFarolDireito()       { pararPiscarLed(LED_FAROL_DIR); }
void Leds::pararPiscarFarolEsquerdo()      { pararPiscarLed(LED_FAROL_ESQ); }
void Leds::pararPiscarLanternaDireita()    { pararPiscarLed(LED_LANTERNA_DIR); }
void Leds::pararPiscarLanternaEsquerda()   { pararPiscarLed(LED_LANTERNA_ESQ); }

// ===== modos de grupo =====

// faróis piscando juntos (mesmo ritmo)
void Leds::piscarFaroisJuntos(uint16_t intervaloMs)
{
    modoFarolJunto = true;
    modoFarolAlternado = false;

    piscarLed(LED_FAROL_DIR, intervaloMs);
    piscarLed(LED_FAROL_ESQ, intervaloMs);
}

// faróis alternados (um aceso, outro apagado, revezando)
void Leds::piscarFaroisAlternados(uint16_t intervaloMs)
{
    modoFarolAlternado = true;
    modoFarolJunto = false;

    PiscaState &d = pisca[LED_FAROL_DIR];
    PiscaState &e = pisca[LED_FAROL_ESQ];

    unsigned long agora = millis();

    d.ativo = true;
    d.intervalo = intervaloMs;
    d.ultimaTroca = agora;
    d.estado = false; // começa apagado (LOW)

    e.ativo = true;
    e.intervalo = intervaloMs;
    e.ultimaTroca = agora;
    e.estado = true; // começa aceso (HIGH)

    writeOff(PIN_FAROL_DIR); // false -> off => LOW
    writeOn (PIN_FAROL_ESQ); // true  -> on  => HIGH
}

// lanternas piscando juntas
void Leds::piscarLanternasJuntas(uint16_t intervaloMs)
{
    modoLanternaJunta = true;
    modoLanternaAlternada = false;

    piscarLed(LED_LANTERNA_DIR, intervaloMs);
    piscarLed(LED_LANTERNA_ESQ, intervaloMs);
}

// lanternas alternadas
void Leds::piscarLanternasAlternadas(uint16_t intervaloMs)
{
    modoLanternaAlternada = true;
    modoLanternaJunta = false;

    PiscaState &d = pisca[LED_LANTERNA_DIR];
    PiscaState &e = pisca[LED_LANTERNA_ESQ];

    unsigned long agora = millis();

    d.ativo = true;
    d.intervalo = intervaloMs;
    d.ultimaTroca = agora;
    d.estado = false; // começa apagada (LOW)

    e.ativo = true;
    e.intervalo = intervaloMs;
    e.ultimaTroca = agora;
    e.estado = true; // começa acesa (HIGH)

    writeOff(PIN_LANTERNA_DIR); // false -> LOW
    writeOn (PIN_LANTERNA_ESQ); // true  -> HIGH
}

void Leds::pararPiscarFarois()
{
    modoFarolJunto = false;
    modoFarolAlternado = false;
    pararPiscarFarolDireito();
    pararPiscarFarolEsquerdo();
}

void Leds::pararPiscarLanternas()
{
    modoLanternaJunta = false;
    modoLanternaAlternada = false;
    pararPiscarLanternaDireita();
    pararPiscarLanternaEsquerda();
}

void Leds::pararPiscarTodos()
{
    pararPiscarSetaDireita();
    pararPiscarSetaEsquerda();
    pararPiscarFarois();
    pararPiscarLanternas();
}

// grupos estáticos ON/OFF
void Leds::ligarFarois()
{
    modoFarolJunto = false;
    modoFarolAlternado = false;

    pisca[LED_FAROL_DIR].ativo = false;
    pisca[LED_FAROL_ESQ].ativo = false;

    writeOn(PIN_FAROL_DIR);
    writeOn(PIN_FAROL_ESQ);
}

void Leds::desligarFarois()
{
    modoFarolJunto = false;
    modoFarolAlternado = false;

    pisca[LED_FAROL_DIR].ativo = false;
    pisca[LED_FAROL_ESQ].ativo = false;

    writeOff(PIN_FAROL_DIR);
    writeOff(PIN_FAROL_ESQ);
}

void Leds::ligarLanternas()
{
    modoLanternaJunta = false;
    modoLanternaAlternada = false;

    pisca[LED_LANTERNA_DIR].ativo = false;
    pisca[LED_LANTERNA_ESQ].ativo = false;

    writeOn(PIN_LANTERNA_DIR);
    writeOn(PIN_LANTERNA_ESQ);
}

void Leds::desligarLanternas()
{
    modoLanternaJunta = false;
    modoLanternaAlternada = false;

    pisca[LED_LANTERNA_DIR].ativo = false;
    pisca[LED_LANTERNA_ESQ].ativo = false;

    writeOff(PIN_LANTERNA_DIR);
    writeOff(PIN_LANTERNA_ESQ);
}

void Leds::ligarSetas()
{
    ligarSetaDireita();
    ligarSetaEsquerda();
}

void Leds::desligarSetas()
{
    desligarSetaDireita();
    desligarSetaEsquerda();
}

void Leds::desligarTodos()
{
    desligarSetas();
    desligarFarois();
    desligarLanternas();
}

// ===== loop de atualização =====
// deve ser chamado repetidamente no loop()

void Leds::atualizar()
{
    const unsigned long agora = millis();

    for (int id = 0; id < LED_MAX; id++) {
        PiscaState &st = pisca[id];
        if (!st.ativo) continue;

        if (agora - st.ultimaTroca >= st.intervalo) {
            st.ultimaTroca = agora;
            st.estado = !st.estado; // toggle true/false (true = aceso/HIGH)

            // Se estiver em modo alternado, quem manda é sincronizarAlternado()
            if (isFarol(id) && modoFarolAlternado) continue;
            if (isLanterna(id) && modoLanternaAlternada) continue;

            // Caso comum: individual ou "juntos"
            writeLedEstado((LedId)id, st.estado);
        }
    }

    // agora tratamos os modos alternados (mantém oposição)
    if (modoFarolAlternado) {
        sincronizarAlternado(LED_FAROL_DIR, LED_FAROL_ESQ);
    }
    if (modoLanternaAlternada) {
        sincronizarAlternado(LED_LANTERNA_DIR, LED_LANTERNA_ESQ);
    }
}

// escreve estado lógico de um LED (aceso/apagado)
void Leds::writeLedEstado(LedId id, bool aceso)
{
    if (aceso) writeOn(pinOf(id));   // true -> HIGH
    else       writeOff(pinOf(id));  // false -> LOW
}

// garante que direita/esquerda fiquem invertidos entre si
void Leds::sincronizarAlternado(LedId dirId, LedId esqId)
{
    bool estadoDir = pisca[dirId].estado;
    bool estadoEsq = !estadoDir;

    writeLedEstado(dirId, estadoDir);
    writeLedEstado(esqId, estadoEsq);

    // Mantém coerência interna
    pisca[esqId].estado = estadoEsq;
}

// helpers pra saber tipo
bool Leds::isFarol(int id) const {
    return (id == LED_FAROL_DIR || id == LED_FAROL_ESQ);
}
bool Leds::isLanterna(int id) const {
    return (id == LED_LANTERNA_DIR || id == LED_LANTERNA_ESQ);
}