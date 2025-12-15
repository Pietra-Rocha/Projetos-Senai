DOCUMENTAÇÃO TÉCNICA – FLUXO NODE-RED COM MQTT
=============================================

1. VISÃO GERAL DO SISTEMA
------------------------
Este projeto utiliza o Node-RED como núcleo de processamento para controlar um
carrinho/robô por meio do protocolo MQTT.

O sistema é composto por três partes principais:
- Interface gráfica (Node-RED Dashboard)
- Comunicação (Broker MQTT)
- Dispositivo embarcado (ex: ESP32)

O Node-RED recebe comandos do Dashboard, processa esses comandos por meio de nós
function e publica mensagens estruturadas em um tópico MQTT específico.

------------------------------------------------------------

2. COMO IMPORTAR O FLUXO NO NODE-RED
-----------------------------------
1. Abra o Node-RED no navegador (ex: http://localhost:1880)
2. Clique no menu (☰) no canto superior direito
3. Selecione: Import → Clipboard
4. Cole o JSON do fluxo
5. Clique em Import
6. Finalize clicando em Deploy

------------------------------------------------------------

3. NÓS E PACOTES NECESSÁRIOS
---------------------------

3.1 node-red-dashboard
Utilizado para criar a interface gráfica de controle.
Componentes usados no fluxo:
- Botões de direção (Frente, Trás, Esquerda, Direita)
- Chaves (Farol, Lanterna, Setas)
- Sliders de velocidade

Instalação:
npm install node-red-dashboard

----------------------------

3.2 MQTT (mqtt in / mqtt out)
Responsável pela comunicação entre o Node-RED e o ESP32.
Permite enviar e receber mensagens através de um broker MQTT.

----------------------------

3.3 Nós padrão do Node-RED
- function: tratamento lógico dos comandos
- json: conversão e padronização das mensagens
- debug: visualização e testes

------------------------------------------------------------

4. TÓPICOS MQTT UTILIZADOS
-------------------------
Tópico principal:

senai134/comandos

Este tópico é utilizado para transportar todos os comandos do sistema, incluindo:
- Movimento do carrinho
- Direção
- Controle individual de motores
- Velocidade (PWM)
- Farol, lanterna e setas

------------------------------------------------------------

5. DESCRIÇÃO DOS COMPONENTES DO FLUXO
------------------------------------

5.1 MQTT IN (Entrada)
Recebe mensagens externas ou retornos de estado do dispositivo embarcado.

----------------------------

5.2 Nós JSON
Responsáveis por interpretar e organizar os dados recebidos, garantindo
compatibilidade com o processamento lógico.

----------------------------

5.3 Nós Function – Movimento
Processam comandos de direção:
- Frente
- Trás
- Esquerda
- Direita

Definem quais motores devem ser acionados e em qual sentido.

----------------------------

5.4 Nós Function – Motores
Controlam individualmente os motores:
- Motor 01
- Motor 02
- Motor 03
- Motor 04

Permitem ligar, desligar ou configurar o estado de cada motor.

----------------------------

5.5 Nós Function – Velocidade
Responsáveis por definir o valor de velocidade (PWM) aplicado
a cada motor individualmente.

----------------------------

5.6 Dashboard (Interface Gráfica)
Interface onde o usuário interage com o sistema em tempo real.
Os comandos gerados são convertidos em mensagens MQTT.

----------------------------

5.7 MQTT OUT (Saída)
Publica os comandos tratados no tópico MQTT para que o ESP32
execute as ações correspondentes.

------------------------------------------------------------

6. DIAGRAMA DO FLUXO
-------------------
O diagrama representa visualmente a sequência lógica:
Dashboard → Processamento (Function / JSON) → MQTT → ESP32

Essa organização facilita a compreensão, manutenção e expansão do sistema.

------------------------------------------------------------

FIM DA DOCUMENTAÇÃO
