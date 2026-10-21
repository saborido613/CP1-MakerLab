/*
 * CP2: HC-SR04 -> CSV real -> Edge Impulse -> ESP32 -> motores.
 * Comece com USAR_TINYML=0 em Config.h. Leia ../../README.md.
 * Nao existe modelo de exemplo nem classificacao simulada neste firmware.
 */
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <esp_system.h>
#include <math.h>
#include <string.h>
#include "Config.h"
#include "Controle.h"
#include "Pagina.h"

#if USAR_TINYML
// Troque SOMENTE este nome pelo include do exemplo static_buffer da SUA biblioteca.
#include <SEU_PROJETO_inferencing.h>
static_assert(EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME == 1, "O modelo deve ter apenas distancia_cm como eixo.");
static_assert(EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE == cfg::LEITURAS_JANELA, "Configure janela de 1000 ms a 10 Hz (10 valores).");
static_assert(EI_CLASSIFIER_LABEL_COUNT == 3, "O modelo deve ter perto, medio e longe.");
#endif

WebServer server(80);
Pedido pedido = Pedido::PARAR;
uint32_t ultimoComando = 0;
uint32_t proximaLeitura = 0;
uint32_t ultimaLeitura = 0;
float distanciaAtual = -1.0f;
Acao acaoAtual = {0, 0, "INICIANDO"};
int pwmAtualA = 0;
int pwmAtualB = 0;

float janela[cfg::LEITURAS_JANELA] = {};
int posicaoJanela = 0;
int tamanhoJanela = 0;
int novasLeituras = 0;
Classe classeAtual = Classe::INDEFINIDA;
float escoreAtual = 0;
float escores[3] = {};
bool modeloValido = false;
bool configuracaoModeloOk = false;
uint32_t ultimaEntradaModelo = 0;
uint32_t inferenciasOk = 0;
uint32_t duracaoInferencia = 0;

float coleta[cfg::LEITURAS_COLETA] = {};
int tamanhoColeta = 0;
bool coletando = false;
bool csvPronto = false;
String arquivoCsv;
String mensagemColeta = "Selecione a classe, o conjunto e a sessao; depois grave.";
uint32_t sequenciaColeta = 0;
uint32_t bootId = 0;

Classe classePorNome(const char* nome) {
  if (strcmp(nome, "perto") == 0) return Classe::PERTO;
  if (strcmp(nome, "medio") == 0) return Classe::MEDIO;
  if (strcmp(nome, "longe") == 0) return Classe::LONGE;
  return Classe::INDEFINIDA;
}

void limparJanela() {
  posicaoJanela = 0;
  tamanhoJanela = 0;
  novasLeituras = 0;
  modeloValido = false;
  classeAtual = Classe::INDEFINIDA;
  escoreAtual = 0;
  for (int i = 0; i < 3; ++i) escores[i] = 0;
}

float medirDistancia() {
  digitalWrite(cfg::TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(cfg::TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(cfg::TRIG, LOW);
  const unsigned long duracao = pulseIn(cfg::ECHO, HIGH, 25000);

  #if !USAR_TINYML
  if (duracao == 0) {
    Serial.println("SENSOR | ECHO=0 us | TIMEOUT: pulso nao medido");
  } else {
    Serial.printf("SENSOR | ECHO=%lu us | distancia bruta=%.1f cm\n",
                  duracao, duracao * 0.0343f / 2.0f);
  }
#endif
  if (duracao == 0) return -1;
  const float cm = duracao * 0.0343f / 2.0f;
  if (!isfinite(cm) || cm < cfg::DIST_MIN_CM || cm > cfg::DIST_MAX_CM) return -1;
  return cm;
}

void escreverMotor(int positivo, int negativo, int pwm) {
  // PWM nos INs do L298N; ENA/ENB devem estar habilitados (jumpers).
  // Primeiro desativa a entrada oposta para evitar troca direta de polaridade.
  if (pwm >= 0) {
    analogWrite(negativo, 0);
    analogWrite(positivo, pwm);
  } else {
    analogWrite(positivo, 0);
    analogWrite(negativo, -pwm);
  }
}

void aplicarAcao(const Acao& nova) {
  const bool mudou = nova.motorA != pwmAtualA || nova.motorB != pwmAtualB ||
                     strcmp(nova.motivo, acaoAtual.motivo) != 0;
  if (nova.motorA != pwmAtualA || nova.motorB != pwmAtualB) {
    escreverMotor(cfg::IN1, cfg::IN2, nova.motorA);
    escreverMotor(cfg::IN3, cfg::IN4, nova.motorB);
  }
  pwmAtualA = nova.motorA;
  pwmAtualB = nova.motorB;
  acaoAtual = nova;
  if (mudou) Serial.printf("ACAO: %s | PWM A=%d B=%d\n", nova.motivo, nova.motorA, nova.motorB);
}

void atualizarAcao() {
  if (USAR_TINYML && !configuracaoModeloOk) {
    aplicarAcao({0, 0, "MODELO COM CONFIGURACAO INVALIDA"});
    return;
  }
  const uint32_t agora = millis();
  Contexto c = {pedido, USAR_TINYML && configuracaoModeloOk,
               (uint32_t)(agora - ultimoComando) <= cfg::COMANDO_MAX_IDADE_MS,
               distanciaAtual, (uint32_t)(agora - ultimaLeitura),
               modeloValido && tamanhoJanela == cfg::LEITURAS_JANELA,
               (uint32_t)(agora - ultimaEntradaModelo), classeAtual, escoreAtual};
  aplicarAcao(decidir(c));
}

void abortarColeta(const char* motivo) {
  if (!coletando) return;
  coletando = false;
  csvPronto = false;
  tamanhoColeta = 0;
  mensagemColeta = motivo;
  Serial.printf("COLETA ABORTADA: %s\n", motivo);
}

void amostrarSensor() {
  const uint32_t agora = millis();
  const int32_t atraso = (int32_t)(agora - proximaLeitura);
  if (atraso < 0) return;
  if ((uint32_t)atraso > cfg::ATRASO_MAX_MS) {
    limparJanela();
    abortarColeta("Atraso de amostragem maior que 20 ms. Repita sem baixar arquivos durante a gravacao.");
    proximaLeitura = agora + cfg::PERIODO_MS;
  } else {
    proximaLeitura += cfg::PERIODO_MS;
  }
  distanciaAtual = medirDistancia();
  ultimaLeitura = agora;
  if (distanciaAtual < 0) {
    limparJanela();
    abortarColeta("Leitura invalida. Verifique alvo, fios, alimentacao e divisor do ECHO; repita.");
    return;
  }

  if (coletando) {
    coleta[tamanhoColeta++] = distanciaAtual;
    if (tamanhoColeta == cfg::LEITURAS_COLETA) {
      coletando = false;
      csvPronto = true;
      mensagemColeta = "CSV pronto. Baixe antes da proxima gravacao: " + arquivoCsv;
      Serial.printf("COLETA PRONTA: %s | 50 leituras reais\n", arquivoCsv.c_str());
    }
  }

#if USAR_TINYML
  janela[posicaoJanela] = distanciaAtual;
  posicaoJanela = (posicaoJanela + 1) % cfg::LEITURAS_JANELA;
  if (tamanhoJanela < cfg::LEITURAS_JANELA) ++tamanhoJanela;
  ++novasLeituras;
#endif
}

void executarInferencia() {
#if USAR_TINYML
  if (!configuracaoModeloOk || tamanhoJanela != cfg::LEITURAS_JANELA ||
      novasLeituras < cfg::PASSO_INFERENCIA) return;
  novasLeituras = 0;
  float entradas[cfg::LEITURAS_JANELA];
  for (int i = 0; i < cfg::LEITURAS_JANELA; ++i)
    entradas[i] = janela[(posicaoJanela + i) % cfg::LEITURAS_JANELA];

  // Centimetros brutos: a escala 0.01 configurada no Raw data e aplicada
  // pela biblioteca exportada. NAO dividir por 100 novamente aqui.
  signal_t sinal;
  if (ei::numpy::signal_from_buffer(entradas, cfg::LEITURAS_JANELA, &sinal) != 0) {
    limparJanela();
    Serial.println("ERRO criando o sinal de entrada.");
    return;
  }
  ei_impulse_result_t resultado = {};
  const uint32_t inicio = millis();
  const EI_IMPULSE_ERROR erro = run_classifier(&sinal, &resultado, false);
  duracaoInferencia = (uint32_t)(millis() - inicio);
  if (erro != EI_IMPULSE_OK) {
    limparJanela();
    Serial.printf("ERRO run_classifier: %d\n", (int)erro);
    return;
  }

  float maior = -1;
  Classe vencedora = Classe::INDEFINIDA;
  for (int i = 0; i < EI_CLASSIFIER_LABEL_COUNT; ++i) {
    const Classe c = classePorNome(resultado.classification[i].label);
    const float valor = resultado.classification[i].value;
    if (c == Classe::INDEFINIDA || !isfinite(valor) || valor < 0 || valor > 1) {
      limparJanela();
      Serial.println("ERRO: classe ou escore invalido na biblioteca.");
      return;
    }
    escores[(int)c - 1] = valor;
    if (valor > maior) { maior = valor; vencedora = c; }
  }
  classeAtual = vencedora;
  escoreAtual = maior;
  modeloValido = true;
  ultimaEntradaModelo = ultimaLeitura;  // Idade dos dados, nao do fim do processamento.
  ++inferenciasOk;
  Serial.printf("ML #%lu | cm=%.1f | classe=%s | escore=%.3f | perto=%.3f medio=%.3f longe=%.3f | inferencia=%lu ms\n",
                (unsigned long)inferenciasOk, distanciaAtual, nomeClasse(classeAtual), escoreAtual,
                escores[0], escores[1], escores[2], (unsigned long)duracaoInferencia);
#endif
}

bool sessaoValida(const String& texto) {
  if (texto.length() == 0 || texto.length() > 20) return false;
  for (unsigned int i = 0; i < texto.length(); ++i) {
    const char c = texto[i];
    if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
          (c >= '0' && c <= '9') || c == '_' || c == '-')) return false;
  }
  return true;
}

void iniciarColeta() {
  if (USAR_TINYML) { server.send(409, "text/plain", "Grave primeiro o firmware em modo COLETA (Config.h = 0)."); return; }
  if (coletando) { server.send(409, "text/plain", "Ja existe uma coleta em andamento."); return; }
  const String classe = server.arg("classe");
  const String conjunto = server.arg("conjunto");
  const String sessao = server.arg("sessao");
  if (classePorNome(classe.c_str()) == Classe::INDEFINIDA ||
      (conjunto != "treino" && conjunto != "teste") || !sessaoValida(sessao)) {
    server.send(400, "text/plain", "Classe/conjunto invalidos; sessao deve ter 1..20 letras, numeros, _ ou -.");
    return;
  }
  pedido = Pedido::PARAR;
  atualizarAcao();
  tamanhoColeta = 0;
  csvPronto = false;
  char id[40];
  snprintf(id, sizeof(id), "%08lx_%lu", (unsigned long)bootId, (unsigned long)++sequenciaColeta);
  arquivoCsv = classe + "." + conjunto + "_" + sessao + "_" + id + ".csv";
  mensagemColeta = "Gravando. Nao mova o alvo para outra classe.";
  coletando = true;
  server.send(202, "text/plain", mensagemColeta);
}

void baixarCsv() {
  if (!csvPronto || coletando) { server.send(409, "text/plain", "Nao ha CSV concluido para baixar."); return; }
  String csv;
  csv.reserve(1500);
  csv = "timestamp,distancia_cm\n";
  for (int i = 0; i < tamanhoColeta; ++i) {
    // Grade nominal de 100 ms; gravacoes com atraso >20 ms sao abortadas.
    csv += String((unsigned long)i * cfg::PERIODO_MS) + "," + String(coleta[i], 3) + "\n";
  }
  server.sendHeader("Content-Disposition", "attachment; filename=\"" + arquivoCsv + "\"");
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "text/csv; charset=utf-8", csv);
}

void enviarStatus() {
  String s;
  s.reserve(850);
  s = "{\"tinyml\":" + String(USAR_TINYML ? "true" : "false");
  s += ",\"distancia_cm\":" + (distanciaAtual >= 0 ? String(distanciaAtual, 3) : String("null"));
  s += ",\"classe\":\"" + String(nomeClasse(classeAtual)) + "\",\"escore\":" + String(escoreAtual, 4);
  s += ",\"escores\":{\"perto\":" + String(escores[0], 4) + ",\"medio\":" + String(escores[1], 4) + ",\"longe\":" + String(escores[2], 4) + "}";
  s += ",\"janela\":" + String(tamanhoJanela) + ",\"inferencias\":" + String((unsigned long)inferenciasOk);
  s += ",\"inferencia_ms\":" + String((unsigned long)duracaoInferencia);
  s += ",\"acao\":\"" + String(acaoAtual.motivo) + "\",\"pwm_a\":" + String(pwmAtualA) + ",\"pwm_b\":" + String(pwmAtualB);
  s += ",\"coletando\":" + String(coletando ? "true" : "false");
  s += ",\"csv_pronto\":" + String(csvPronto ? "true" : "false");
  s += ",\"coleta_leituras\":" + String(tamanhoColeta);
  s += ",\"coleta_mensagem\":\"" + mensagemColeta + "\"}";
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", s);
}

void receberComando(Pedido novo) {
  if (!USAR_TINYML && novo != Pedido::PARAR) {
    server.send(409, "text/plain", "Motores desabilitados no modo coleta."); return;
  }
  pedido = novo;
  ultimoComando = millis();
  atualizarAcao();
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "text/plain", acaoAtual.motivo);
}

void setup() {
  Serial.begin(115200);
  pinMode(cfg::IN1, OUTPUT); pinMode(cfg::IN2, OUTPUT);
  pinMode(cfg::IN3, OUTPUT); pinMode(cfg::IN4, OUTPUT);
  digitalWrite(cfg::IN1, LOW); digitalWrite(cfg::IN2, LOW);
  digitalWrite(cfg::IN3, LOW); digitalWrite(cfg::IN4, LOW);
  // Anexa os quatro canais PWM uma vez; nao alterna digitalWrite/PWM depois.
  analogWrite(cfg::IN1, 0); analogWrite(cfg::IN2, 0);
  analogWrite(cfg::IN3, 0); analogWrite(cfg::IN4, 0);
  pinMode(cfg::TRIG, OUTPUT); pinMode(cfg::ECHO, INPUT);
  digitalWrite(cfg::TRIG, LOW);
  bootId = esp_random();

#if USAR_TINYML
  unsigned int mascara = 0;
  for (int i = 0; i < EI_CLASSIFIER_LABEL_COUNT; ++i) {
    const Classe c = classePorNome(ei_classifier_inferencing_categories[i]);
    if (c != Classe::INDEFINIDA) mascara |= 1u << ((int)c - 1);
  }
  configuracaoModeloOk = mascara == 7u && fabsf(EI_CLASSIFIER_INTERVAL_MS - cfg::PERIODO_MS) < 0.1f;
  if (!configuracaoModeloOk) Serial.println("ERRO: confira rotulos perto/medio/longe e periodo de 100 ms no Edge Impulse.");
#endif

  WiFi.softAP(cfg::SSID, cfg::SENHA);
  server.on("/", []() { server.send_P(200, "text/html; charset=utf-8", PAGINA); });
  server.on("/status", enviarStatus);
  server.on("/coleta/iniciar", iniciarColeta);
  server.on("/coleta.csv", baixarCsv);
  server.on("/frente", []() { receberComando(Pedido::FRENTE); });
  server.on("/tras", []() { receberComando(Pedido::TRAS); });
  server.on("/esquerda", []() { receberComando(Pedido::ESQUERDA); });
  server.on("/direita", []() { receberComando(Pedido::DIREITA); });
  server.on("/parar", []() { receberComando(Pedido::PARAR); });
  server.begin();
  proximaLeitura = millis() + cfg::PERIODO_MS;
  atualizarAcao();
  Serial.printf("\nCP2 | modo=%s | rede=%s | senha=%s\n", USAR_TINYML ? "TINYML" : "COLETA", cfg::SSID, cfg::SENHA);
  Serial.print("Abra http://"); Serial.println(WiFi.softAPIP());
}

void loop() {
  server.handleClient();
  amostrarSensor();
  atualizarAcao();       // Emergencia usa a ultima leitura antes da inferencia.
  executarInferencia();
  atualizarAcao();       // A nova classe altera os motores aqui.
  delay(1);
}
