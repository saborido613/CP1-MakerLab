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