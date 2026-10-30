# Requisitos, planejamento e evolução

Este documento reúne o que foi produzido nas aulas, na ordem em que aconteceu: a proposta inicial, o MVP, a priorização, o backlog e, no fim, as decisões que mudaram o projeto até a versão final.

> **Leitura rápida:** o projeto foi planejado com **Arduino + Bluetooth** (seções 1 a 6) e entregue com **ESP32 + Wi-Fi**. O motivo e o impacto da troca estão na [seção 7](#7-decisões-de-projeto).

---

## 1. Ficha de requisitos (proposta inicial)

**Nome do projeto:** Carrinho 2WD controlado por Arduino e Bluetooth *(nome da proposta inicial; ver seção 7)*

**Objetivo:** desenvolver um carrinho de duas rodas motrizes (2WD), controlado por uma placa Arduino, utilizando comunicação Bluetooth e equipado com um sensor ultrassônico para detecção de obstáculos.

### 1.1 Requisitos físicos

O carrinho deverá possuir:

- chassi em acrílico;
- tração 2WD, com 2 motores DC e 2 rodas motrizes;
- 1 roda de apoio (roda boba);
- espaço para acomodar os componentes eletrônicos e estrutura que suporte o peso deles;
- distribuição dos componentes que mantenha o equilíbrio.

### 1.2 Requisitos eletrônicos

- 1 microcontrolador (proposta: Arduino);
- 1 ponte H;
- 2 motores DC;
- 1 módulo de comunicação sem fio (proposta: Bluetooth);
- 1 sensor ultrassônico;
- bateria, fios e jumpers.

**Alimentação planejada:** a bateria alimenta dois ramais — lógica (microcontrolador, comunicação e sensor) e potência (ponte H → motores). O microcontrolador envia só o sinal de controle à ponte H; a corrente dos motores não passa pelos seus pinos.

### 1.3 Requisitos funcionais

O carrinho deverá:

1. receber comandos sem fio;
2. processar os comandos no microcontrolador;
3. controlar os dois motores por meio da ponte H;
4. andar para frente;
5. andar para trás;
6. fazer curvas para a esquerda e para a direita;
7. usar o sensor ultrassônico para detectar obstáculos;
8. funcionar com bateria.

### 1.4 Resumo dos requisitos

| Categoria | Proposta inicial | Versão final |
|---|---|---|
| Controle | Arduino | **ESP32 DevKit V1** |
| Comunicação | Bluetooth | **Wi-Fi (ESP32 em modo AP + página web)** |
| Tração | 2WD | 2WD |
| Motores | 2 motores DC | 2 motores DC TT com redução |
| Controle dos motores | Ponte H | Ponte H L298N |
| Detecção | Sensor ultrassônico | HC-SR04 (usado no CP2) |
| Alimentação | Bateria | 2× 18650 para os motores + power bank USB para o ESP32 |
| Estrutura | Chassi de acrílico | Kit chassi 2WD de acrílico (comercial) |
| Apoio | Roda boba | Roda boba |
| Movimento | Frente, trás, esquerda, direita | Frente, trás, esquerda, direita + parada de segurança |

---

## 2. Esboço inicial

### 2.1 Disposição planejada dos componentes

- **Motores:** um em cada lateral do chassi, com as rodas acopladas.
- **Roda boba:** ponto de apoio e equilíbrio.
- **Controlador:** região central/superior.
- **Ponte H:** perto dos motores, para encurtar os fios de potência.
- **Bateria:** no centro, para o equilíbrio.
- **Sensor ultrassônico:** na frente, apontado para a direção de movimento.
- **Cabos:** canaleta central atrás do controlador, da bateria e da ponte H; abraçadeiras a cada ~5 cm no chicote ponte H → motores; cabos de sinal separados dos de potência.

### 2.2 Croqui com dimensões (proposta)

Medidas de referência da proposta (comprimento 20 cm, largura 15 cm, acrílico de 3 mm):

```text
                 ←──────── 20 cm ────────→
             ┌────────────────────────────┐   ↑
             │          SENSOR            │   │
             │        ULTRASSÔNICO        │   │
             │       ┌──────────┐         │   │
             │       │  ARDUINO │         │   │
             │       └──────────┘         │  15 cm
             │       ┌──────────┐         │   │
             │       │  BATERIA │         │   │
             │       └──────────┘         │   │
             │       ┌──────────┐         │   │
             │       │  PONTE H │         │   │
             │       └──────────┘         │   │
             └────────────────────────────┘   ↓
               ◯                      ◯
             MOTOR                  MOTOR
                         ●
                     RODA BOBA
```

Diagramas da proposta inicial: [layout físico v1](../Hardware/v1_proposta_inicial/layout_fisico_v1.png) e [alimentação v1](../Hardware/v1_proposta_inicial/diagrama_alimentacao_v1.png). A versão final está em [`Hardware/`](../Hardware/).

---

## 3. MVP (Minimum Viable Product)

**MVP:** o carrinho anda para frente, para trás e faz curvas por comando sem fio, usando a ponte H para controlar os dois motores. Sem sensor e sem desvio automático — só controle remoto básico.

**Critério de "minimamente funcional"** — ✅ atingido

- [x] Microcontrolador recebe comando sem fio *(Wi-Fi, ver seção 7)*
- [x] Motores respondem a pelo menos 4 comandos (frente, trás, esquerda, direita)
- [x] Carrinho anda em superfície plana sem cair peça nem desconectar fio

**Critério de "concluído"**

- [x] Chassi montado e cabeamento organizado
- [x] Parada automática de segurança quando a conexão cai *(adicionada durante os testes)*
- [x] Testes finais de movimento ([testes.md](testes.md))
- [ ] Sensor ultrassônico integrado e detectando obstáculo — **em andamento no CP2** ([CP2/](../CP2/)): sensor montado no carrinho e firmware de leitura pronto
- [ ] Parada/redução de velocidade automática ao detectar obstáculo — **em andamento no CP2** (será decidida pelo modelo TinyML)
- [ ] Autonomia da bateria medida — ver teste T8 em [testes.md](testes.md#1-testes-realizados)

---

## 4. MoSCoW

| Prioridade | Item | Resultado |
|---|---|---|
| **Must** | Controle sem fio (frente/trás/esquerda/direita) | ✅ Feito (Wi-Fi) |
| **Must** | Montagem mecânica do chassi (motores, rodas, roda boba) | ✅ Feito |
| **Must** | Alimentação estável (bateria → controlador e ponte H) | ✅ Feito (fontes separadas) |
| **Should** | Detecção de obstáculo com sensor ultrassônico | 🔄 Sensor montado; leitura em desenvolvimento no CP2 |
| **Should** | Parada automática ao detectar obstáculo próximo | 🔄 Em desenvolvimento no CP2 (TinyML) |
| **Could** | Desvio automático (contornar o obstáculo) | ❌ Não feito |
| **Could** | LED indicador de status | ❌ Não feito |
| **Won't** (nesta entrega) | App próprio / interface gráfica custom | ⚠️ Mudou: a página web do ESP32 virou a interface (ver decisão D1) |
| **Won't** (nesta entrega) | Mapeamento ou navegação autônoma completa | — Fora do escopo |

---

## 5. Backlog

| # | Tarefa | Responsável | Status |
|---|---|---|---|
| 1 | Montar chassi (motores, rodas, roda boba) | Matheus Haruo | ✅ Concluído |
| 2 | Ligar ponte H aos motores e à bateria | Matheus Haruo | ✅ Concluído |
| 3 | Ligar o controlador à ponte H (sinal de controle) e à alimentação | Pedro Guerra | ✅ Concluído |
| 4 | ~~Configurar módulo Bluetooth e parear com celular~~ → Configurar o Wi-Fi do ESP32 (modo AP) | João Saborido | ✅ Concluído (substituído, ver D1) |
| 5 | Programar comandos básicos de movimento | João Saborido | ✅ Concluído |
| 6 | Testar movimento sem sensor (MVP) | Victor Wittner | ✅ Concluído |
| 7 | Montar e ligar sensor ultrassônico ao ESP32 | Pedro Guerra | 🔄 Sensor montado; pinos sendo confirmados (CP2) |
| 8 | Programar leitura de distância do sensor | João Saborido | 🔄 Firmware escrito, em teste (CP2) |
| 9 | Programar lógica de parada por obstáculo | Matheus Haruo | 🔄 Em andamento no CP2 (depende do modelo TinyML) |
| 10 | Organizar cabeamento | Pedro Guerra | ✅ Concluído |
| 11 | Testes finais | Victor Wittner | ✅ Concluído |
| 12 | Documentação final (README, fotos, vídeo) | Victor Wittner | ✅ Concluído |
| 13 | *Novo:* parada de segurança por timeout | João Saborido | ✅ Concluído |
| 14 | *Novo:* página de controle compatível com toque no iPhone | João Saborido | ✅ Concluído |

> Todas as etapas foram feitas em conjunto nas aulas; o "responsável" é quem conduziu a tarefa.

---

## 6. Dependências e Kanban

### 6.1 Dependências

```
1. Montar chassi
      ↓
2. Ponte H ↔ motores/bateria ──► 3. Controlador ↔ ponte H/alimentação
      ↓                                   ↓
                4. Comunicação sem fio (Wi-Fi)
                          ↓
             5. Programar comandos de movimento
                          ↓
             6. Testar movimento (MVP concluído)
                          ↓
      7. Sensor ──► 8. Leitura de distância ──► 9. Parada por obstáculo   (CP2)
                          ↓
      10. Organizar cabeamento (em paralelo com 6–9)
                          ↓
             11. Testes finais → 12. Documentação
```

- As tarefas 2 e 3 (elétrica) precisavam estar prontas antes de qualquer teste de software.
- A tarefa 6 é o marco entre "minimamente funcional" e "concluído".

### 6.2 Kanban — quadro inicial

| A fazer | Em andamento | Concluído |
|---|---|---|
| Configurar Bluetooth | Montar chassi | — |
| Programar comandos de movimento | | |
| Ligar sensor ultrassônico | | |
| Programar leitura de distância | | |
| Lógica de parada/desvio | | |
| Organizar cabeamento | | |
| Testes finais | | |
| Documentação final | | |

### 6.3 Kanban — quadro final

| A fazer | Em andamento | Concluído |
|---|---|---|
| — | Sensor, leitura e parada por obstáculo (CP2) | Montar chassi |
| | | Ligações elétricas |
| | | ~~Configurar Bluetooth~~ → Wi-Fi do ESP32 |
| | | Programar comandos de movimento |
| | | Parada de segurança (timeout) |
| | | Organizar cabeamento |
| | | Testes finais |
| | | Documentação final |

---

## 7. Decisões de projeto

| # | Decisão | Motivo | Consequência |
|---|---|---|---|
| D1 | **Trocar Arduino + Bluetooth por ESP32 com Wi-Fi** | Todos os integrantes usam iPhone, que não se conecta a módulos Bluetooth clássicos (perfil serial). Sem isso, ninguém do grupo conseguiria dirigir o carrinho. | O ESP32 já tem Wi-Fi: dispensa o módulo extra e o controle passou a ser uma página web, que funciona em qualquer celular sem instalar app. |
| D2 | ESP32 em **modo Access Point** (cria a própria rede) | Não depende do Wi-Fi da faculdade nem de roteador. | O celular conecta direto na rede `Carrinho-Grupo` e o endereço é sempre `192.168.4.1`. |
| D3 | **Enviar o comando repetidamente** (a cada 200 ms) enquanto o botão está pressionado | Requisições HTTP isoladas podem se perder; com repetição, o carrinho só anda enquanto recebe comandos. | Permitiu a parada de segurança (D4). |
| D4 | **Parada automática após 600 ms sem comando** | Nos testes, quando a conexão caía com o botão pressionado, o carrinho continuava andando. | Se o celular sair da rede ou travar, o carrinho para sozinho. |
| D5 | **Sem PWM (ENA/ENB) nesta entrega** | O pino planejado para o ENB era o GPIO6, ligado à memória flash do ESP32 e proibido para uso. | Motores sempre em velocidade máxima; o slider da página ficou sem efeito. O PWM foi retomado no CP2, aplicado direto nos pinos IN. |
| D6 | **Alimentação separada**: 2× 18650 para a ponte H e power bank USB para o ESP32 | Os motores causam quedas de tensão ao arrancar; separar evita que o ESP32 reinicie e derrube o Wi-Fi. | GND comum entre ESP32 e ponte H para o sinal de controle funcionar. |
| D7 | **Chassi comercial** em vez de modelar um próprio | O kit 2WD já vem com motores, rodas e roda boba compatíveis. | Esforço concentrado na eletrônica e no software. Adaptações em [CAD/](../CAD/README.md). |
| D8 | **Uso do sensor ultrassônico levado para o CP2** | O CP2 pede justamente um sensor do carrinho gerando dados para TinyML; fazer a lógica de obstáculo duas vezes seria retrabalho. | O CP1 entrega o controle remoto com o sensor montado; o CP2 usa o HC-SR04 para classificar a distância e mudar o comportamento dos motores. |

## 8. Evolução do projeto

| Etapa | O que mudou |
|---|---|
| 1. Proposta inicial | Requisitos, croqui, MVP, MoSCoW e backlog com Arduino + Bluetooth (seções 1 a 6). |
| 2. Revisão de comunicação | Troca para ESP32 com Wi-Fi (D1, D2). Planilha de custos refeita com ESP32 ([Organização/](../Organização/)). |
| 3. Montagem (21/08) | ESP32, L298N, baterias e motores ligados ([foto](../Hardware/foto_montagem_21-08.jpeg)). |
| 4. Programação e MVP | Página de controle, comandos de movimento e testes do MVP. |
| 5. Ajustes após testes | Repetição de comandos, parada de segurança, página adaptada ao toque no iPhone, desativação do PWM (D3–D5). |
| 6. Versão final do CP1 | Carrinho controlado pelo celular, documentação e evidências. |
| 7. CP2 (em andamento) | Firmware de coleta e inferência com o HC-SR04 pronto; próximos passos: coletar CSVs, treinar no Edge Impulse e embarcar ([CP2/](../CP2/)). |
