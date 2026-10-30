# Testes e resultados

Os testes foram feitos com o carrinho completo (ESP32 + L298N + motores no chassi), controlado pelo navegador do iPhone na rede `Carrinho-Grupo`.

Legenda: ✅ aprovado · ⚠️ aprovado com ressalva · ⬜ a registrar

## 1. Testes realizados

| ID | Teste | Como foi feito | Esperado | Resultado |
|---|---|---|---|---|
| T1 | Rede Wi-Fi | Ligar o ESP32 e procurar a rede no celular | Rede `Carrinho-Grupo` aparece e aceita a senha | ✅ |
| T2 | Página de controle | Abrir `http://192.168.4.1` no Safari | Página com setas, PARAR e slider | ✅ |
| T3 | Frente e trás | Segurar ▲ e ▼ | As duas rodas giram no mesmo sentido | ✅ |
| T4 | Curvas | Segurar ◀ e ▶ | Carrinho gira no próprio eixo para o lado certo | ✅ |
| T5 | Soltar o botão | Soltar a seta durante o movimento | Carrinho para | ✅ |
| T6 | Botão PARAR | Tocar em PARAR com o carrinho andando | Carrinho para na hora | ✅ |
| T7 | Queda de conexão | Segurar ▲ e desligar o Wi-Fi do celular | Carrinho para sozinho em até ~0,6 s | ✅ (após correção P3) |
| T8 | Autonomia da bateria | Usar o carrinho continuamente até perder força | Tempo de uso registrado | ⬜ *tempo:* ___ min |
| T9 | Superfícies | Dirigir em piso liso e em piso com tapete/obstáculo baixo | Anda e faz curvas nos dois | ⬜ |
| T10 | Alcance | Afastar com o carrinho parado e testar os comandos | Distância máxima com resposta | ⬜ *alcance:* ___ m |
| T11 | Slider de velocidade | Mudar o slider e andar para frente | Velocidade muda | ⚠️ Sem efeito nesta versão (P4) |

## 2. Problemas encontrados e correções

| # | Problema | Causa | Correção |
|---|---|---|---|
| P1 | Não era possível controlar o carrinho pelos celulares do grupo com o módulo Bluetooth previsto | iPhone não se comunica com módulos Bluetooth clássicos (perfil serial) | Troca para ESP32 com Wi-Fi e página web de controle (decisão D1) |
| P2 | Pino planejado para o ENB não podia ser usado | GPIO6 é ligado à memória flash do ESP32 (GPIO 6–11 são proibidos) | Pinos IN movidos para 23/22/18/19; ENA/ENB ficaram com jumper |
| P3 | Com o botão pressionado e a conexão caindo, o carrinho podia continuar andando | O ESP32 só parava ao receber o comando `S` | A página reenvia o comando a cada 200 ms e o ESP32 para após 600 ms sem comando |
| P4 | Slider de velocidade sem efeito | Sem ENA/ENB ligados não há PWM | Limitação aceita no CP1; PWM retomado no CP2 direto nos pinos IN |
| P5 | No iPhone, segurar o botão selecionava texto, abria menu ou dava zoom | Comportamento padrão do Safari no toque longo | CSS `user-select: none`, `-webkit-touch-callout: none`, `user-scalable=no` e eventos `pointer` com `setPointerCapture` |

## 3. Resultado final

O carrinho atende ao MVP e aos itens **Must** do MoSCoW:

- é controlado sem fio pelo celular de qualquer integrante, sem instalar app;
- anda para frente, para trás e faz curvas para os dois lados;
- para ao soltar o botão, no botão PARAR e automaticamente quando a conexão cai;
- usa alimentação separada para lógica e motores, com terra comum.

Pendências assumidas: velocidade variável (resolvida no CP2) e reação a obstáculos com o sensor (em andamento no CP2).

Vídeos do funcionamento: [controle pelo celular](../Evidencias/video_controle_celular.mp4) e [carrinho andando](../Evidencias/video_funcionamento.mp4). Foto do carrinho final e demais evidências: [`Evidencias/`](../Evidencias/).
