# Documentação Técnica — Carrinho 2WD

## 1. Ficha de Requisitos

### 1.1 Nome do projeto

**Carrinho 2WD controlado por Arduino e Bluetooth**

### 1.2 Objetivo

Desenvolver um carrinho de duas rodas motrizes (2WD), controlado por uma placa Arduino, utilizando comunicação Bluetooth e equipado com um sensor ultrassônico para detecção de obstáculos.

---

## 2. Componentes

| Componente | Quantidade | Função |
|---|---:|---|
| Arduino | 1 | Controlador principal do projeto |
| Ponte H | 1 | Controle da direção e velocidade dos motores |
| Motor DC | 2 | Movimentação do carrinho |
| Roda | 2 | Transmissão do movimento dos motores |
| Roda boba/castor | 1 | Apoio e equilíbrio do chassi |
| Módulo Bluetooth | 1 | Recebimento dos comandos sem fio |
| Sensor ultrassônico | 1 | Detecção de obstáculos |
| Bateria | 1 | Alimentação do sistema |
| Chassi de acrílico | 1 | Estrutura física do carrinho |
| Jumpers/fios | Conforme necessário | Conexão dos componentes |

> **Observação:** Os modelos específicos dos componentes deverão ser adicionados posteriormente, caso necessário.

---

## 3. Dimensões do Chassi

As dimensões abaixo são uma proposta inicial e devem ser substituídas pelas medidas reais do chassi utilizado no projeto.

- **Comprimento:** 20 cm
- **Largura:** 15 cm
- **Espessura do acrílico:** 3 mm

---

## 4. Disposição dos Componentes

Os componentes deverão ser posicionados de maneira que o carrinho tenha boa distribuição de peso e organização dos fios.

- **Motores:** um em cada lateral do chassi.
- **Rodas:** acopladas aos respectivos motores.
- **Roda boba:** utilizada como ponto de apoio e equilíbrio.
- **Arduino:** posicionado na região central/superior do chassi.
- **Ponte H:** posicionada próxima aos motores, facilitando as conexões.
- **Bateria:** posicionada preferencialmente na região central para melhorar o equilíbrio.
- **Módulo Bluetooth:** posicionado na parte superior do chassi.
- **Sensor ultrassônico:** instalado na parte frontal do carrinho, direcionado para a área de movimentação.

> **Organização dos cabos:** recomenda-se uma canaleta central passando por trás do Arduino, da bateria e da ponte H para esconder a fiação de alimentação, e abraçadeiras a cada ~5 cm no chicote entre a ponte H e os motores (trecho de maior corrente). Os cabos de sinal (Bluetooth e sensor) devem ser roteados separados dos cabos de potência (bateria/motores) para evitar interferência.

---

# 5. Requisitos Físicos

O carrinho deverá possuir:

- Chassi construído em acrílico;
- Sistema de tração 2WD;
- 2 motores DC;
- 2 rodas motrizes;
- 1 roda de apoio;
- Espaço suficiente para acomodar os componentes eletrônicos;
- Estrutura capaz de suportar o peso dos componentes;
- Distribuição adequada dos componentes para manter o equilíbrio.

---

# 6. Requisitos Eletrônicos

O sistema deverá utilizar:

- 1 placa Arduino;
- 1 ponte H;
- 2 motores DC;
- 1 módulo Bluetooth;
- 1 sensor ultrassônico;
- 1 bateria;
- Fios e jumpers para as conexões elétricas.

**Alimentação:** a bateria alimenta dois ramais — um de lógica (Arduino, que por sua vez alimenta o módulo Bluetooth e o sensor ultrassônico em 5V) e um de potência (ponte H, que alimenta diretamente os dois motores DC). O Arduino envia apenas o sinal de controle para a ponte H; a corrente dos motores não passa pelos pinos do Arduino.

---

# 7. Requisitos Funcionais

O carrinho deverá ser capaz de:

1. Receber comandos através do módulo Bluetooth;
2. Processar os comandos utilizando o Arduino;
3. Controlar os dois motores por meio da ponte H;
4. Movimentar-se para frente;
5. Movimentar-se para trás;
6. Realizar curvas para a esquerda e para a direita;
7. Utilizar o sensor ultrassônico para detectar obstáculos;
8. Utilizar a bateria como fonte de alimentação do sistema.

---

# 8. Croqui do Chassi

## 8.1 Vista superior

```text
                         FRENTE
                            ↑
                            │
                  ┌───────────────────┐
                  │                   │
                  │     SENSOR        │
                  │    ULTRASSÔNICO   │
                  │                   │
                  │    ┌─────────┐    │
                  │    │ ARDUINO │    │
                  │    └─────────┘    │
                  │                   │
                  │    ┌─────────┐    │
                  │    │ BATERIA │    │
                  │    └─────────┘    │
                  │                   │
                  │    ┌─────────┐    │
                  │    │ PONTE H │    │
                  │    └─────────┘    │
                  │                   │
                  └───────────────────┘
                    ◯               ◯
                   RODA            RODA
                  MOTOR            MOTOR

                            ●
                       RODA BOBA
```
## 8.2 Croqui com Dimensões

```text
                 ←──────── 20 cm ────────→

             ┌────────────────────────────┐
             │                            │
             │          SENSOR            │
             │        ULTRASSÔNICO        │
             │                            │
             │       ┌──────────┐         │
             │       │  ARDUINO │         │
             │       └──────────┘         │
             │                            │
             │       ┌──────────┐         │
             │       │  BATERIA │         │
             │       └──────────┘         │
             │                            │
             │       ┌──────────┐         │
             │       │  PONTE H │         │
             │       └──────────┘         │
             │                            │
             └────────────────────────────┘
               ◯                      ◯
             MOTOR                  MOTOR

                       ↑
                       │
                     15 cm
                       │
                       ↓
```

# 9. Descrição do Projeto

O projeto consiste na construção de um carrinho 2WD utilizando Arduino como unidade de controle. O sistema será composto por dois motores DC responsáveis pela movimentação do carrinho, sendo controlados por uma ponte H.

A comunicação entre o usuário e o carrinho será realizada por meio de um módulo Bluetooth, permitindo o envio de comandos para controlar seus movimentos.

Além disso, será utilizado um sensor ultrassônico na parte frontal do carrinho para identificar possíveis obstáculos durante sua movimentação.

Todos os componentes serão instalados sobre um chassi de acrílico, buscando uma distribuição adequada dos elementos para garantir estabilidade, organização e funcionamento correto do sistema.

---

# 10. Resumo dos Requisitos

| Categoria | Requisito |
|---|---|
| **Controle** | Arduino |
| **Comunicação** | Bluetooth |
| **Tração** | 2WD |
| **Motores** | 2 motores DC |
| **Controle dos motores** | Ponte H |
| **Detecção** | Sensor ultrassônico |
| **Alimentação** | Bateria |
| **Estrutura** | Chassi de acrílico |
| **Apoio** | Roda boba |
| **Controle de movimento** | Frente, trás, esquerda e direita |

---

# 11. MVP (Minimum Viable Product)

**MVP = minimamente funcional:**
O carrinho se move para frente/trás e para os lados via comando Bluetooth, usando a ponte H para controlar os dois motores DC. Sem sensor, sem desvio automático — só controle remoto básico.

**Critério de "minimamente funcional":**
- [ ] Arduino recebe comando via Bluetooth
- [ ] Motores respondem a pelo menos 4 comandos (frente, trás, esquerda, direita)
- [ ] Carrinho anda em uma superfície plana sem cair peça nem desconectar fio

**Critério de "concluído":**
- [ ] Sensor ultrassônico integrado e detectando obstáculo antes de bater
- [ ] Comportamento de desvio ou parada automática ao detectar obstáculo
- [ ] Chassi montado e cabeamento organizado (canaleta + abraçadeiras)
- [ ] Bateria com autonomia testada (tempo mínimo de uso contínuo)
- [ ] Testes finais em pelo menos 2 ambientes diferentes (chão liso, chão com tapete/obstáculo)

---

# 12. MoSCoW

| Prioridade | Item |
|---|---|
| **Must** | Controle via Bluetooth (frente/trás/esquerda/direita) |
| **Must** | Montagem mecânica do chassi (motores, rodas, roda boba) |
| **Must** | Alimentação estável (bateria → Arduino e ponte H) |
| **Should** | Detecção de obstáculo com sensor ultrassônico |
| **Should** | Parada automática ao detectar obstáculo próximo |
| **Could** | Desvio automático (contorna o obstáculo sozinho) |
| **Could** | Indicador visual (LED) de status (ligado/obstáculo detectado) |
| **Won't** (nesta entrega) | Controle por app próprio / interface gráfica custom |
| **Won't** (nesta entrega) | Mapeamento de ambiente ou navegação autônoma completa |

---

# 13. Backlog

| # | Tarefa | Responsável | Status |
|---|---|---|---|
| 1 | Montar chassi (motores, rodas, roda boba) | *definir* | A fazer |
| 2 | Ligar ponte H aos motores e à bateria | *definir* | A fazer |
| 3 | Ligar Arduino à bateria e à ponte H (sinal de controle) | *definir* | A fazer |
| 4 | Configurar módulo Bluetooth e parear com celular | *definir* | A fazer |
| 5 | Programar comandos básicos de movimento | *definir* | A fazer |
| 6 | Testar movimento sem sensor (MVP) | *definir* | A fazer |
| 7 | Ligar sensor ultrassônico ao Arduino | *definir* | A fazer |
| 8 | Programar leitura de distância do sensor | *definir* | A fazer |
| 9 | Programar lógica de parada/desvio por obstáculo | *definir* | A fazer |
| 10 | Organizar cabeamento (canaleta + abraçadeiras) | *definir* | A fazer |
| 11 | Testes finais em diferentes ambientes | *definir* | A fazer |
| 12 | Documentação final (README, fotos, vídeo) | *definir* | A fazer |

> Preencher "Responsável" com quem do grupo vai tocar cada tarefa.

---

# 14. Dependências

```
1. Montar chassi
      ↓
2. Ligar ponte H aos motores/bateria ──► 3. Ligar Arduino à bateria/ponte H
      ↓                                         ↓
                    4. Configurar Bluetooth
                              ↓
                 5. Programar comandos de movimento
                              ↓
                 6. Testar movimento (MVP concluído)
                              ↓
                 7. Ligar sensor ultrassônico
                              ↓
                 8. Programar leitura de distância
                              ↓
                 9. Lógica de parada/desvio
                              ↓
        10. Organizar cabeamento (pode ser feito em paralelo com 6–9)
                              ↓
                 11. Testes finais
                              ↓
                 12. Documentação
```

**Pontos de atenção:**
- As tarefas 2 e 3 (elétrica) precisam estar prontas **antes** de qualquer teste de software.
- A tarefa 6 (MVP funcionando) é o marco que separa "minimamente funcional" de "concluído".
- A tarefa 10 (organização de cabos) não bloqueia nada, mas fica mais fácil fazer **depois** que toda a fiação estiver definitiva.

---

# 15. Kanban (quadro inicial)

| A Fazer | Em Andamento | Concluído |
|---|---|---|
| Configurar Bluetooth | Montar chassi | — |
| Programar comandos de movimento | | |
| Ligar sensor ultrassônico | | |
| Programar leitura de distância | | |
| Lógica de parada/desvio | | |
| Organizar cabeamento | | |
| Testes finais | | |
| Documentação final | | |

> Sugestão de ferramenta: Trello, Notion ou GitHub Projects (linkando as tarefas aos commits do repositório).

---

# 16. Integrantes

- João Saborido | RM98184
- Matheus Haruo | RM97663
- Pedro Guerra | ?
- Lucca Alexandre | RM99700
- Victor Wittner | RM98667