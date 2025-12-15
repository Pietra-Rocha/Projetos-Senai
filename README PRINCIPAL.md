1) O carrinho é uma plataforma IoT inteligente com bateria reduzida para 5 V e LED azul indicador de funcionamento. Possui sistema de iluminação com LEDs de farol, lanterna e setas, além de um seguidor de linha na parte inferior para seguir trajetos no chão. Conta com sensores para medir a temperatura dos motores, um sensor de proximidade localizado na parte frontal e sensores para identificação RFID, movimento e cores. O controle do carrinho é feito por meio do MIT e de um joystick.


2) O grupo desenvolveu um carrinho inteligente com foco em IoT e automação. O projeto final integra alimentação segura em 5 V, sistema de iluminação com LEDs funcionais, seguidor de linha, sensor de proximidade frontal e sensores para identificação RFID, movimento, cores e temperatura dos motores. O carrinho pode ser controlado por meio do MIT e de um joystick, resultando em um protótipo funcional capaz de se locomover e seguir trajetos.


3) O projeto foi desenvolvido com uma arquitetura baseada em um módulo IoT central, responsável por processar os dados dos sensores e controlar os atuadores do carrinho. O hardware inclui uma placa principal com regulador de tensão para 5 V, sensores de proximidade, seguidor de linha, sensores de temperatura dos motores, RFID, sensor de movimento e cor, além do sistema de iluminação com LEDs.

A comunicação do carrinho foi realizada por meio do protocolo MQTT, permitindo a troca de mensagens entre o carrinho, o aplicativo desenvolvido no MIT App Inventor e o Node-RED. O MIT foi utilizado para enviar comandos de controle, como direção e funções, enquanto o Node-RED ficou responsável pelo monitoramento e organização dos dados recebidos dos sensores.

Os fluxos de funcionamento seguem a leitura contínua dos sensores, o envio e recebimento de mensagens via MQTT e a execução das ações correspondentes, como a movimentação do carrinho e acionamento dos LEDs. As telas do aplicativo foram projetadas de forma simples e intuitiva, com joystick, botões de controle e indicadores de estado, facilitando a interação do usuário com o sistema.

4) O grupo planejou desenvolver um sistema de logística otimizada utilizando mini carrinhos inteligentes organizados por setores identificados por cores. A proposta inicial era melhorar a organização do processo logístico, agilizar a separação das encomendas e reduzir falhas operacionais, utilizando os carrinhos como apoio no transporte e controle dos pedidos dentro de cada setor.

5) Faltou implementar o JSON no banco de dados para o armazenamento e organização das informações, realizar a junção dos códigos desenvolvidos ao longo do projeto e finalizar o sistema de seguir a linha andando de lado.



6) Sensor de proximidade, velocidade e banco de dados (o que não funcionou e causa provável)
O sensor de proximidade e o sensor de velocidade não foram exibidos corretamente na dashboard. Apesar de ambos estarem conectados ao sistema, os dados não apareciam na interface visual. A causa mais provável foi uma falha na integração entre o código do microcontrolador, o banco de dados e a dashboard, podendo estar relacionada a erros no envio das informações, formatação incorreta dos dados ou configuração inadequada dos campos responsáveis pela exibição. Além disso, houve dificuldade no entendimento da lógica de blocos utilizada para o recebimento das informações no MIT App Inventor.

Adicionalmente, o banco de dados não apresentou funcionamento adequado, o que pode ter impedido o armazenamento e a recuperação correta dos dados enviados pelos sensores. Essa falha comprometeu a visualização das informações em tempo real e afetou o funcionamento completo da dashboard. A ausência de testes individuais do banco de dados e dos sensores antes da integração final também contribuiu para a dificuldade em identificar com precisão a origem do problema.


7) O que precisa melhorar (melhorias técnicas + organização do projeto)
Tecnicamente, o projeto precisa melhorar principalmente na organização interna do código. Embora cada funcionalidade já esteja estruturada em sua própria função (void), muitas dessas funções concentram uma grande quantidade de informações, comandos e lógicas diferentes em um único bloco. Isso dificulta a leitura do código, o entendimento do funcionamento geral do sistema e a manutenção futura. Uma melhoria importante seria dividir essas funções em partes menores e mais específicas, tornando o código mais modular, organizado e fácil de testar individualmente.

Além disso, o funcionamento do banco de dados precisa ser aprimorado, pois ele não está operando corretamente. Essa falha compromete o armazenamento e a recuperação das informações enviadas pelo sistema, afetando diretamente a exibição dos dados na dashboard. Ajustes na integração entre o código, o banco de dados e a interface visual são necessários para garantir que as informações sejam enviadas, salvas e exibidas corretamente.

Quanto à organização do projeto, também é necessário melhorar o planejamento das etapas de desenvolvimento, a divisão de tarefas entre os integrantes e a documentação das alterações realizadas, o que facilitaria a identificação de erros e a implementação de melhorias futuras.




8. Como executar/testar (passo a passo para reproduzir)

Conectar corretamente todos os componentes do carrinho (motores, controlador, bateria e sensores, se houver).

Verificar se a alimentação elétrica está adequada e segura.

Carregar o código no microcontrolador utilizando o software apropriado.

Ligar o carrinho e aguardar a inicialização do sistema.

Enviar os comandos de controle (via aplicativo, controle remoto ou botão, conforme o projeto).



9. Integrantes e papéis (quem fez o quê)

PROJETO CARRINHO 

P.O  => Gui


Scrum Master => Pietra

PROGRAMAR:
- movimento MOTORES (pi e Madu Santos) OK
- seguidor de linha  (pi e Madu Santos) OK
- leds (setas, farol, lanterna) (pi e Madu Santos) ok
- sensor de temperatura (pi e Madu Santos) ok
- leitura de proximidade (pi e Madu Santos) OK

- display colorido + encoder + interface (pi e Madu Santos) 
- RFID  (Guilherme) 
- PID  (pi e pre) 



- aplicativo android MIT APP INVENTOR  (gui )
- nodered (dashboard)  (ju e riquelme)
- controle fisico (Maria Santos Juliana)
-  mecanica, montagem, concertos em gerais (Praxedes)

DEFINIR:
- formato JSON  (pi e pre) 
- IAAS EC2 (nodered, banco de dados, IOT core, certificados) (Riquelme, Juliana, Gui, Pietra)
- banner do projeto (Madu Santos)
- panfleto (Madu Santos)
- apresentacao (canvas ou pptx) (Riquelme, Pietra)
- desingend carrinho (Todos) 
 - sistemas de segurança, MIT e DashBoard (Gui, Juliana) 
 - Gráfico Gantt (Macro e Micro) (pi)
 - Easter Egg (Todos) 
 - QR para o download APK  (Gui) 
 - QR para o dashboard (Juliana) 