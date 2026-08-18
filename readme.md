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

# 11. Integrantes

- João Saborido | RM98184
- Matheus Haruo | RM97663
- Pedro Guerra | ?
- Lucca Alexandre | RM99700
- Victor Wittner | RM98667