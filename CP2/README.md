# CP2 — TinyML no Carrinho 2WD (ESP32 + HC-SR04 + Edge Impulse)

Evolução do carrinho do CP1: o sensor ultrassônico do próprio carrinho coleta dados reais, um modelo é treinado no Edge Impulse e a classificação, executada no ESP32, muda o comportamento dos motores (parar ou ajustar a velocidade).

**Ciclo do projeto:** COLETAR → TREINAR → TESTAR → EMBARCAR → INFERIR → AGIR

> Os arquivos do CP1 (`SRC/CodigoCarrinho/`, documentação, `Hardware/`, `CAD/`, `Organização/`) foram mantidos sem alterações. Tudo do CP2 fica nesta pasta.

## Integrantes

| Integrante | RM |
|---|---|
| João Saborido | 98184 |
| Matheus Haruo | 97663 |
| Pedro Guerra | *preencher* |
| Lucca Alexandre | 99700 |
| Victor Wittner | 98667 |

## Base utilizada

Conforme orientação da disciplina, o firmware partiu do repositório [HeitorNF1/maker-lab](https://github.com/HeitorNF1/maker-lab) (commit `9f44058`). O histórico de commits mostra a cópia original e, em seguida, as adaptações feitas para o nosso carrinho:

| Item | Base | Nosso carrinho |
|---|---|---|
| Pinos da ponte H (IN1–IN4) | 18, 5, 12, 14 | 23, 22, 18, 19 (os mesmos do CP1) |
| Rede Wi-Fi | `Carrinho-ESP32` | `Carrinho-Grupo` (a mesma do CP1) |
| Pinos do HC-SR04 | TRIG 26 / ECHO 25 | **a confirmar** (ver abaixo) |

## Estrutura da pasta

```text
CP2/
├── README.md                      ← este guia
├── firmware/carrinho_cp2/
│   ├── carrinho_cp2.ino           ← inicialização, servidor web, sensor, coleta e inferência
│   ├── Config.h                   ← pinos, modo (coleta/TinyML), tempos e PWM
│   ├── Controle.h                 ← decide a ação dos motores (comando + proteções + classe)
│   └── Pagina.h                   ← página de controle e coleta aberta no celular
└── dados/
    ├── treino/                    ← CSVs de treino
    └── teste/                     ← CSVs de teste
```

Os quatro arquivos do firmware precisam ficar juntos na mesma pasta para a Arduino IDE abrir o projeto.

## Ligações

| Sinal | GPIO do ESP32 |
|---|---:|
| IN1 (ponte H) | 23 |
| IN2 (ponte H) | 22 |
| IN3 (ponte H) | 18 |
| IN4 (ponte H) | 19 |
| HC-SR04 TRIG | **a confirmar** (hoje 26 no código) |
| HC-SR04 ECHO | **a confirmar** (hoje 25 no código) |
| GND | terra comum entre ESP32, ponte H, sensor e bateria |

**Pendente:** o firmware espera que o HC-SR04 esteja ligado **diretamente no ESP32**. Depois de confirmar os pinos no carrinho, altere `TRIG` e `ECHO` em `Config.h`. O ECHO do HC-SR04 trabalha em 5 V e precisa de um divisor de tensão (por exemplo, 1 kΩ e 2 kΩ) antes de entrar no ESP32, que aceita 3,3 V.

O PWM é aplicado diretamente nos pinos IN da ponte H. Os jumpers de ENA e ENB da L298N devem ficar colocados.

## Como gravar o firmware

1. Na Arduino IDE, instale o pacote de placas **esp32 by Espressif** (Boards Manager). O código foi verificado com a versão 3.0.7.
2. Abra `CP2/firmware/carrinho_cp2/carrinho_cp2.ino`.
3. Selecione a placa **ESP32 Dev Module** e a porta USB.
4. Envie o código e abra o Monitor Serial em **115200 baud**.
5. No celular, conecte-se à rede **`Carrinho-Grupo`** (senha `12345678`) e abra **http://192.168.4.1**.

## Etapa 1 — Coleta de dados (modo COLETA)

Em `Config.h`, mantenha:

```cpp
#define USAR_TINYML 0
```

Nesse modo os **motores ficam desligados** e a página mostra a área de coleta.

1. Confira na página (e no Monitor Serial) se a distância muda ao aproximar e afastar um objeto do sensor.
2. Escolha a **classe**, o **conjunto** (`treino` ou `teste`) e a **sessão** (por exemplo, `A`, `B`, `C`).
3. Clique em **Gravar 5 segundos**. Cada gravação gera 50 leituras (10 por segundo).
4. Clique em **Baixar CSV** antes de fazer a próxima gravação, porque o arquivo fica só na memória da placa.
5. Salve o arquivo em `CP2/dados/treino/` ou `CP2/dados/teste/`.

Formato do CSV gerado:

```csv
timestamp,distancia_cm
0,42.310
100,42.275
...
```

O nome do arquivo já carrega a classe, que é como o Edge Impulse identifica o rótulo:

```text
{classe}.{conjunto}_{sessao}_{id}_{sequencia}.csv
```

### Classes

| Classe | Situação na coleta | Ação no modo TinyML (ao segurar "frente") |
|---|---|---|
| `perto` | obstáculo entre 5 e 35 cm | parar |
| `medio` | obstáculo entre 35 e 80 cm | andar devagar (PWM 150) |
| `longe` | obstáculo entre 80 e 200 cm (ou nenhum) | andar normal (PWM 190) |

As faixas servem só para orientar a coleta. No modo TinyML, quem decide a classe é o modelo; o código não compara a distância com esses limites.

### Protocolo sugerido

- Pelo menos **10 gravações por classe para treino** e **3 por classe para teste**, em sessões diferentes (dias, superfícies ou objetos diferentes).
- Variar o obstáculo (parede, caixa, mochila) e a posição dentro da faixa da classe.
- Não mudar o obstáculo de faixa durante uma gravação.

Registre aqui o que foi coletado:

| Classe | Gravações de treino | Gravações de teste | Total de amostras |
|---|---:|---:|---:|
| perto | | | |
| medio | | | |
| longe | | | |

## Etapa 2 — Treino no Edge Impulse

1. Crie o projeto e importe os CSVs (**Data acquisition → Upload data**), mandando os de `treino` para Training e os de `teste` para Test.
2. **Create impulse:** janela de **1000 ms**, frequência de **10 Hz**, eixo único `distancia_cm`.
3. Bloco de processamento **Raw data**, com escala `0.01`.
4. Bloco de aprendizado **Classification** com as classes `perto`, `medio` e `longe` (os nomes têm que ser exatamente esses).
5. Gere as features, treine e registre a acurácia e a matriz de confusão.
6. Rode o **Model testing** com os dados de teste.
7. Salve os prints em `CP2/docs/edge-impulse/` (dados importados, impulse, treino, matriz de confusão e teste).

## Etapa 3 — Embarcar e inferir (modo TINYML)

1. No Edge Impulse, vá em **Deployment → Arduino library** e baixe o ZIP.
2. Na Arduino IDE: **Sketch → Include Library → Add .ZIP Library**.
3. Em `carrinho_cp2.ino`, troque `#include <SEU_PROJETO_inferencing.h>` pelo nome do cabeçalho da biblioteca exportada.
4. Em `Config.h`, mude para `#define USAR_TINYML 1` e grave novamente.
5. No Monitor Serial aparecem linhas como:

```text
ML #12 | cm=28.4 | classe=perto | escore=0.973 | perto=0.973 medio=0.025 longe=0.002 | inferencia=1 ms
ACAO: ML PERTO: PARAR | PWM A=0 B=0
```

O carrinho só avança se houver leitura válida do sensor, inferência recente e escore mínimo de 0,80. Uma parada de emergência a 20 cm funciona independentemente do modelo. Ré e curvas continuam manuais.

**Demonstração sugerida para o vídeo:** segurar "frente" e aproximar o carrinho de uma parede. Ele deve reduzir a velocidade em `medio` e parar em `perto` por volta de 30 cm, antes do limite fixo de 20 cm, mostrando que a parada vem do modelo.

## Checklist de entrega

- [ ] Confirmar os pinos TRIG/ECHO e atualizar `Config.h`
- [ ] Coletar os CSVs reais e preencher a tabela de amostras
- [ ] Treinar e testar no Edge Impulse, com prints em `CP2/docs/edge-impulse/`
- [ ] Exportar a biblioteca, ajustar o `#include` e ativar `USAR_TINYML 1`
- [ ] Testar no carrinho a parada e a mudança de velocidade pela classe inferida
- [ ] Gravar o vídeo da demonstração e colocar o link aqui
- [ ] Preencher o RM do Pedro Guerra
