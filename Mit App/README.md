
# App de Controle de Carrinho via MQTT

Uma breve descrição sobre o que esse projeto faz e para quem ele é



O que o app faz

Este aplicativo foi desenvolvido no MIT App Inventor para controlar um carrinho robótico por
meio de comunicação MQTT. O app possui uma tela de login, essa tela é uma tela principal, dentro dela temos outras 4 "escondidas". Nossas telas ficam no modo invisível, assim fica tudo em uma único lugar, facilitando a conexão do MQTT e o funcionamento do app.  Os comandos enviados
incluem movimentação (frente, ré, esquerda, direita e parar) e estados de operação (Tipo de carga por exemplo). O aplicativo
também recebe mensagens do carrinho, como status de conexão e confirmação de comandos.


Como testar o app

1. Instalar o aplicativo em um celular Android (Através da chave APK).
2. Abrir o app e realizar o login.
3. Verificar se o status de conexão MQTT aparece como conectado.
4. Utilizar os botões ou controles do app para movimentar o carrinho.
5. Observar as mensagens de retorno do carrinho recebidas via MQTT (Se quiser).


Dados enviados e recebidos

Dados enviados (MQTT Publish):
• Comandos de movimento do carrinho (frente, ré, esquerda, direita e parar).
• Estados de controle, como modo manual ou automático.
• Mensagens estruturadas, podendo utilizar o formato JSON.




Dados recebidos (MQTT Subscribe):

• Status de conexão do carrinho.
• Confirmação de comandos executados.
• Informações de estado do sistema.



Tecnologias utilizadas

• MIT App Inventor.
• Protocolo MQTT.
• Formato JSON.
• Broker MQTT.
