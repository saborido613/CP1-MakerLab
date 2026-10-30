# Software

Código-fonte completo: [`SRC/CodigoCarrinho/CodigoCarrinho.ino`](../SRC/CodigoCarrinho/CodigoCarrinho.ino) (C++/Arduino para ESP32).

**Requisitos para compilar:** Arduino IDE com o pacote **esp32** da Espressif, placa **ESP32 Dev Module**. As bibliotecas `WiFi.h` e `WebServer.h` já vêm com o pacote.

## 1. Visão geral

```text
 Celular (navegador)                      ESP32                         L298N        Motores
 ───────────────────                      ─────                         ─────        ───────
 toca na seta  ──► GET /cmd?d=F ──►  handleCmd() ──► executar('F') ──► IN1..IN4 ──► giram
 segura o botão ─► repete a cada 200 ms   guarda ultimoComando
 solta o botão ──► GET /cmd?d=S ──►  stopMotors()
 conexão cai    ──► (nada chega)    loop(): 600 ms sem comando ──► stopMotors()
```

## 2. Estrutura do código

| Parte | O que faz |
|---|---|
| Definições de pinos | IN1=23, IN2=22, IN3=18, IN4=19 |
| Configuração Wi-Fi | SSID `Carrinho-Grupo`, senha `12345678` |
| `PAGINA` | Página HTML/CSS/JS do controle, guardada na flash (`PROGMEM`) |
| `moveForward()`, `moveBackward()`, `turnLeft()`, `turnRight()`, `stopMotors()` | Combinações dos pinos IN para cada movimento |
| `executar(char d)` | Converte a letra recebida (F, B, L, R, S) em movimento |
| `handleRoot()`, `handleCmd()`, `handleVel()` | Rotas do servidor web |
| `setup()` | Configura os pinos, cria a rede Wi-Fi e inicia o servidor |
| `loop()` | Atende o celular e aplica a parada de segurança |

## 3. Controle dos motores

Cada motor usa dois pinos da ponte H. Os níveis lógicos definem o sentido:

| Comando | IN1 | IN2 | IN3 | IN4 | Resultado |
|---|:---:|:---:|:---:|:---:|---|
| `F` frente | H | L | H | L | duas rodas para frente |
| `B` trás | L | H | L | H | duas rodas para trás |
| `L` esquerda | L | H | H | L | esquerda para trás, direita para frente |
| `R` direita | H | L | L | H | esquerda para frente, direita para trás |
| `S` parar | L | L | L | L | motores desligados |

A velocidade é fixa (máxima) nesta versão: ENA/ENB estão com jumper na L298N. O valor do slider é recebido em `/vel`, mas não é aplicado (decisão D5). No CP2 a velocidade passou a ser controlada por PWM nos próprios pinos IN.

## 4. Comunicação sem fio

1. `WiFi.softAP(SSID, SENHA)` faz o ESP32 criar a própria rede; o IP dele é `192.168.4.1`.
2. `WebServer server(80)` atende requisições HTTP:
   - `/` → envia a página de controle;
   - `/cmd?d=X` → executa o movimento `X` e registra o horário em `ultimoComando`;
   - `/vel?v=N` → guarda a velocidade (0–255).
3. Na página, cada botão usa eventos `pointerdown` / `pointerup`, que funcionam com toque no iPhone e no Android. Enquanto o dedo está no botão, um `setInterval` reenvia o comando a cada 200 ms. Ao soltar, a página manda `S`.
4. O CSS desativa seleção de texto, menu de toque longo e zoom (`user-select: none`, `-webkit-touch-callout: none`, `user-scalable=no`), que atrapalhavam segurar os botões no iPhone.

## 5. Parada de segurança

```cpp
if (estado != 'S' && millis() - ultimoComando > TIMEOUT_MS) {
    stopMotors();
    estado = 'S';
}
```

Se nenhum comando chegar em 600 ms (3 envios perdidos seguidos), o carrinho para. Isso cobre conexão caindo, celular bloqueando a tela ou saindo do alcance.

## 6. Utilização do sensor

O HC-SR04 está montado na frente do carrinho. No código do CP1 ele ainda não é lido: o MVP era só controle remoto (seção 3 de [requisitos-e-planejamento.md](requisitos-e-planejamento.md#3-mvp-minimum-viable-product)) e o uso do sensor foi feito na evolução do CP2.

No CP2 (em andamento, [`CP2/firmware/carrinho_cp2/`](../CP2/firmware/carrinho_cp2/)) o firmware já está escrito para:
- disparar o TRIG, medir o tempo do pulso em ECHO e converter em distância (cm), 10 vezes por segundo;
- gravar as leituras em CSV para treinar um modelo no Edge Impulse (classes `perto`, `medio`, `longe`);
- rodar o modelo no próprio ESP32 e decidir se o carrinho anda normal, devagar ou para;
- parar sempre abaixo de 20 cm, independentemente do modelo.

Detalhes em [`CP2/README.md`](../CP2/README.md).
