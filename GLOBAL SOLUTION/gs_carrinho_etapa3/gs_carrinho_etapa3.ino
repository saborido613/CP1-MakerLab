// GLOBAL SOLUTION - CitizenTech - Etapa 3 (IA)
// Carrinho 2WD (ESP32 + L298N) que monitora a iluminacao de espacos publicos.
//  - Controle manual e modo PATRULHA pelo celular (pagina web)
//  - Le LDR (GPIO 34) e DHT (GPIO 4) e classifica a iluminacao: adequada / baixa / critica
//  - Ao entrar em zona escura: o carrinho PARA, registra a ocorrencia e pede um ALERTA a IA (Gemini)
//  - Botao "GERAR RELATORIO (IA)": resumo da patrulha para a equipe de manutencao
//  - O celular mostra o texto da IA e pode FALAR o alerta em voz alta
//
// IMPORTANTE:
//  - Deixe os jumpers ENA/ENB colocados na L298N (sem PWM neste codigo).
//  - Crie o arquivo secrets.h (copie o secrets_exemplo.h) com Wi-Fi 2,4 GHz e a chave do Gemini.
//    NUNCA suba o secrets.h para o GitHub.
// Bibliotecas: "DHT sensor library" + "Adafruit Unified Sensor" (Adafruit) e "ArduinoJson" (Benoit Blanchon).

#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include "secrets.h"

// ---------- Pinos da ponte H (L298N) ----------
#define IN1 23
#define IN2 22
#define IN3 18
#define IN4 19

// ---------- Sensores ----------
#define LDR_PIN  34
#define DHT_PIN  4
#define DHT_TYPE DHT11   // troque para DHT22 se for o caso
DHT dht(DHT_PIN, DHT_TYPE);

// ---------- Limites do LDR (leitura bruta 0..4095) ----------
// Neste modulo a leitura e INVERTIDA: maior numero = mais escuro.
const int LIMITE_BAIXA   = 2500;
const int LIMITE_CRITICA = 3200;
const int HISTERESE      = 300;

// ---------- Rede propria (plano B, sem internet/IA) ----------
const char *AP_SSID  = "Carrinho-Grupo";
const char *AP_SENHA = "12345678";

WebServer server(80);

// ---------- Estado ----------
char estado = 'S';                 // F, B, L, R ou S
bool modoAuto = false;
bool zonaEscura = false;
unsigned long ultimoComando = 0;
const unsigned long TIMEOUT_MS = 600;

int luzBruta = 0;
int luzPct = 0;                    // 0..100 (100 = muito claro)
String classe = "adequada";
float temp = NAN;
float umid = NAN;
unsigned long tLuz = 0;
unsigned long tDht = 0;

// ---------- Ocorrencias de zona escura (ultimas 5) ----------
struct Ocorrencia {
  unsigned long seg;
  int pct;
  float temp;
  float umid;
};
const int MAX_OC = 5;
Ocorrencia ocs[MAX_OC];
int nOc = 0;                       // total de ocorrencias desde que ligou

// ---------- IA ----------
char pedirIA = 0;                  // 'A' = alerta, 'R' = relatorio, 0 = nada
unsigned long ultimaIA = 0;
String ultimoAlerta = "";
int alertaId = 0;

// ---------- Pagina do controle ----------
const char PAGINA[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no">
<title>Carrinho</title>
<style>
  * { -webkit-user-select:none; user-select:none; -webkit-touch-callout:none; box-sizing:border-box; }
  body { margin:0; font-family:-apple-system, sans-serif; background:#111; color:#eee;
         display:flex; flex-direction:column; align-items:center; justify-content:center; min-height:100vh; padding:12px; }
  h1 { font-size:20px; margin:0 0 12px; }
  .painel { background:#1c1c1c; border-radius:14px; padding:12px 16px; margin-bottom:12px; width:300px; text-align:center; }
  #classe { font-size:24px; font-weight:bold; }
  .adequada { color:#2ecc71; } .baixa { color:#f1c40f; } .critica { color:#e74c3c; }
  #aviso { min-height:20px; margin-top:6px; font-size:14px; color:#e74c3c; }
  #alerta { background:#1c1c1c; border-left:4px solid #2d6cdf; border-radius:8px; padding:10px 12px; margin-bottom:12px;
            width:300px; font-size:14px; line-height:1.4; min-height:48px; -webkit-user-select:text; user-select:text; }
  .grid { display:grid; grid-template-columns:repeat(3, 84px); grid-template-rows:repeat(3, 84px); gap:10px; }
  button { border:none; border-radius:16px; font-size:32px; background:#2d6cdf; color:#fff; touch-action:none; }
  button:active { background:#1b4aa0; }
  #S { background:#c0392b; font-size:18px; }
  .larga { margin-top:12px; width:300px; height:48px; font-size:16px; touch-action:auto; }
  #btnAuto { background:#27ae60; }
  #btnRel { background:#8e44ad; }
  #btnVoz { background:#555; }
  .vazio { visibility:hidden; }
</style>
</head>
<body>
  <h1>Carrinho CitizenTech</h1>
  <div class="painel">
    Iluminação: <span id="classe" class="adequada">--</span><br>
    Luz: <span id="pct">--</span>% | <span id="temp">--</span> &deg;C | <span id="umid">--</span>% umid.
    <div id="aviso"></div>
  </div>
  <div id="alerta">Alerta da IA aparece aqui.</div>
  <div class="grid">
    <div class="vazio"></div><button data-d="F">&#9650;</button><div class="vazio"></div>
    <button data-d="L">&#9664;</button><button id="S">PARAR</button><button data-d="R">&#9654;</button>
    <div class="vazio"></div><button data-d="B">&#9660;</button><div class="vazio"></div>
  </div>
  <button id="btnAuto" class="larga">PATRULHAR</button>
  <button id="btnRel" class="larga">GERAR RELATÓRIO (IA)</button>
  <button id="btnVoz" class="larga">VOZ: DESLIGADA</button>
<script>
  let timer = null, autoOn = false, ultimoId = 0, vozOn = false;
  function enviar(d) { fetch('/cmd?d=' + d).catch(() => {}); }
  function iniciar(d) {
    parar(false);
    enviar(d);
    timer = setInterval(() => enviar(d), 200);
  }
  function parar(mandarS = true) {
    if (timer) { clearInterval(timer); timer = null; }
    if (mandarS) enviar('S');
  }
  document.querySelectorAll('[data-d]').forEach(b => {
    b.addEventListener('pointerdown', e => { e.preventDefault(); b.setPointerCapture(e.pointerId); iniciar(b.dataset.d); });
    b.addEventListener('pointerup', () => parar());
    b.addEventListener('pointercancel', () => parar());
  });
  document.getElementById('S').addEventListener('pointerdown', () => parar());
  document.getElementById('btnAuto').addEventListener('click', () => {
    fetch('/auto?on=' + (autoOn ? 0 : 1)).catch(() => {});
  });
  document.getElementById('btnRel').addEventListener('click', () => {
    document.getElementById('alerta').textContent = 'Gerando relatório com IA...';
    fetch('/relatorio').catch(() => {});
  });

  function falar(t) {
    if (!vozOn || !('speechSynthesis' in window)) return;
    speechSynthesis.cancel();
    const u = new SpeechSynthesisUtterance(t);
    u.lang = 'pt-BR';
    speechSynthesis.speak(u);
  }
  document.getElementById('btnVoz').addEventListener('click', () => {
    vozOn = !vozOn;
    document.getElementById('btnVoz').textContent = vozOn ? 'VOZ: LIGADA' : 'VOZ: DESLIGADA';
    if (vozOn) falar('Voz ativada'); else if ('speechSynthesis' in window) speechSynthesis.cancel();
  });

  const nomes = { adequada: 'ADEQUADA', baixa: 'BAIXA', critica: 'CRÍTICA' };
  async function atualizar() {
    try {
      const r = await fetch('/status');
      const s = await r.json();
      const c = document.getElementById('classe');
      c.textContent = nomes[s.classe] || '--';
      c.className = s.classe;
      document.getElementById('pct').textContent = s.pct;
      document.getElementById('temp').textContent = (s.temp === null) ? '--' : s.temp;
      document.getElementById('umid').textContent = (s.umid === null) ? '--' : s.umid;
      document.getElementById('aviso').textContent =
        s.escura ? 'ZONA ESCURA detectada - carrinho parado' : (s.auto ? 'Modo patrulha ativo' : '');
      autoOn = s.auto;
      document.getElementById('btnAuto').textContent = autoOn ? 'PARAR PATRULHA' : 'PATRULHAR';
      if (s.alertaId !== ultimoId) {
        ultimoId = s.alertaId;
        if (s.alerta) {
          document.getElementById('alerta').textContent = s.alerta;
          falar(s.alerta);
        }
      }
    } catch (e) {}
  }
  setInterval(atualizar, 1000);
  atualizar();
</script>
</body>
</html>
)rawliteral";

// ---------- Motores ----------
void moveForward()  { digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);  }
void moveBackward() { digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH); digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH); }
void turnLeft()     { digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH); digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);  }
void turnRight()    { digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH); }
void stopMotors()   { digitalWrite(IN1, LOW);  digitalWrite(IN2, LOW);  digitalWrite(IN3, LOW);  digitalWrite(IN4, LOW);  }

void executar(char d) {
  switch (d) {
    case 'F': moveForward();  break;
    case 'B': moveBackward(); break;
    case 'L': turnLeft();     break;
    case 'R': turnRight();    break;
    default:  stopMotors(); d = 'S'; break;
  }
  estado = d;
}

// ---------- Sensores ----------
int lerLDR() {
  long soma = 0;
  for (int i = 0; i < 20; i++) {   // media de 20 leituras: o ADC do ESP32 e ruidoso
    soma += analogRead(LDR_PIN);
    delay(2);
  }
  return soma / 20;
}

String classificar(int v) {
  if (v >= LIMITE_CRITICA) return "critica";
  if (v >= LIMITE_BAIXA)   return "baixa";
  return "adequada";
}

void registrarOcorrencia() {
  Ocorrencia &o = ocs[nOc % MAX_OC];
  o.seg  = millis() / 1000;
  o.pct  = luzPct;
  o.temp = temp;
  o.umid = umid;
  nOc++;
}

// Decide sozinho: entrou em zona escura -> para, registra e pede alerta a IA
void verificarZonaEscura() {
  if (luzBruta >= LIMITE_CRITICA && !zonaEscura) {
    zonaEscura = true;
    modoAuto = false;
    executar('S');
    registrarOcorrencia();
    pedirIA = 'A';
    Serial.println(">>> ZONA ESCURA detectada: carrinho parado");
  } else if (luzBruta < LIMITE_CRITICA - HISTERESE && zonaEscura) {
    zonaEscura = false;
    Serial.println(">>> Iluminacao normalizada");
  }
}

// ---------- IA (Gemini) ----------
void definirAlerta(const String &texto) {
  ultimoAlerta = texto;
  alertaId++;
}

String montarPrompt(char tipo) {
  String p = "Voce e o assistente de um robo que monitora a iluminacao de espacos publicos (projeto CitizenTech). ";
  p += "Responda em portugues do Brasil, sem markdown e sem emojis, ";
  if (tipo == 'A') {
    p += "com um alerta imediato de no maximo 3 frases curtas para a equipe de manutencao da iluminacao publica, incluindo UMA acao recomendada.\n";
  } else {
    p += "com um relatorio resumido da patrulha de no maximo 5 frases curtas para a equipe de manutencao, priorizando as acoes recomendadas.\n";
  }
  p += "Classificacao da luz: adequada, baixa ou critica (critica significa trecho muito escuro, com risco para pedestres).\n";
  p += "Leitura atual: luz " + String(luzPct) + "% (" + classe + ")";
  if (!isnan(temp)) p += ", temperatura " + String(temp, 1) + " C";
  if (!isnan(umid)) p += ", umidade " + String(umid, 0) + "%";
  p += ".\nOcorrencias de zona escura nesta patrulha: " + String(nOc) + ".\n";
  int ini = (nOc > MAX_OC) ? nOc - MAX_OC : 0;
  for (int i = ini; i < nOc; i++) {
    Ocorrencia &o = ocs[i % MAX_OC];
    p += "- Ocorrencia " + String(i + 1) + " aos " + String(o.seg) + " s: luz " + String(o.pct) + "%";
    if (!isnan(o.temp)) p += ", " + String(o.temp, 1) + " C";
    p += ".\n";
  }
  return p;
}

void consultarIA(char tipo) {
  if (WiFi.status() != WL_CONNECTED) {
    definirAlerta("Sem internet: conecte o ESP32 a uma rede Wi-Fi 2,4 GHz com internet (veja o secrets.h).");
    return;
  }

  JsonDocument req;
  req["contents"][0]["parts"][0]["text"] = montarPrompt(tipo);
  String body;
  serializeJson(req, body);

  WiFiClientSecure client;
  client.setInsecure();   // simplificacao para o projeto escolar (nao valida o certificado)
  HTTPClient http;
  String url = String("https://generativelanguage.googleapis.com/v1beta/models/") + GEMINI_MODEL + ":generateContent";
  http.setTimeout(30000);
  if (!http.begin(client, url)) {
    definirAlerta("Falha ao iniciar a conexao com a IA.");
    return;
  }
  http.addHeader("Content-Type", "application/json");
  http.addHeader("x-goog-api-key", GEMINI_API_KEY);

  Serial.println("Consultando a IA...");
  int code = http.POST(body);
  String resp = http.getString();
  http.end();
  Serial.printf("IA respondeu: HTTP %d\n", code);

  if (code != 200) {
    Serial.println(resp);   // mostra o motivo do erro (modelo, chave, limite...)
    definirAlerta("Erro ao consultar a IA (codigo " + String(code) + "). Veja o Monitor Serial.");
    return;
  }

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, resp);
  if (err) {
    Serial.println(err.c_str());
    definirAlerta("Nao consegui ler a resposta da IA. Veja o Monitor Serial.");
    return;
  }

  String texto;
  JsonArray parts = doc["candidates"][0]["content"]["parts"].as<JsonArray>();
  for (JsonObject part : parts) {
    if (part["text"].is<const char *>()) texto += part["text"].as<const char *>();
  }
  texto.trim();
  if (texto.length() == 0) {
    Serial.println(resp);
    definirAlerta("A IA nao devolveu texto. Veja o Monitor Serial.");
    return;
  }
  Serial.println("IA: " + texto);
  definirAlerta(texto);
}

// ---------- Rotas do servidor ----------
void handleRoot() {
  server.send_P(200, "text/html", PAGINA);
}

void handleCmd() {
  if (server.hasArg("d") && server.arg("d").length() > 0) {
    char c = server.arg("d")[0];
    modoAuto = false;                       // comando manual cancela a patrulha
    if (zonaEscura && c == 'F') c = 'S';    // nao avanca para dentro da zona escura
    executar(c);
    ultimoComando = millis();
  }
  server.send(200, "text/plain", "ok");
}

void handleAuto() {
  bool ligar = server.hasArg("on") && server.arg("on") == "1";
  if (ligar && !zonaEscura) {
    modoAuto = true;
    executar('F');
  } else {
    modoAuto = false;
    executar('S');
  }
  server.send(200, "text/plain", "ok");
}

void handleRelatorio() {
  pedirIA = 'R';   // a chamada a IA e feita no loop(), com o carrinho parado
  server.send(200, "text/plain", "ok");
}

void handleStatus() {
  JsonDocument doc;
  doc["luz"] = luzBruta;
  doc["pct"] = luzPct;
  doc["classe"] = classe;
  doc["auto"] = modoAuto;
  doc["escura"] = zonaEscura;
  if (isnan(temp)) doc["temp"] = nullptr; else doc["temp"] = temp;
  if (isnan(umid)) doc["umid"] = nullptr; else doc["umid"] = umid;
  doc["internet"] = (WiFi.status() == WL_CONNECTED);
  doc["ocorrencias"] = nOc;
  doc["alertaId"] = alertaId;
  doc["alerta"] = ultimoAlerta;
  String j;
  serializeJson(doc, j);
  server.send(200, "application/json; charset=utf-8", j);
}

// ---------- Rede ----------
void iniciarRede() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Conectando ao Wi-Fi");
  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 15000) {
    delay(400);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Conectado! Acesse no celular (mesma rede): http://");
    Serial.println(WiFi.localIP());
    if (MDNS.begin("carrinho")) Serial.println("Ou tente: http://carrinho.local");
  } else {
    Serial.println("Nao conectou. Criando rede propria (a IA fica indisponivel).");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_SENHA);
    Serial.print("Rede criada. Acesse: http://");
    Serial.println(WiFi.softAPIP());   // normalmente 192.168.4.1
  }
}

// ---------- Setup e loop ----------
void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  stopMotors();

  dht.begin();
  iniciarRede();

  server.on("/", handleRoot);
  server.on("/cmd", handleCmd);
  server.on("/auto", handleAuto);
  server.on("/relatorio", handleRelatorio);
  server.on("/status", handleStatus);
  server.begin();
}

void loop() {
  server.handleClient();
  unsigned long agora = millis();

  // Luz: a cada 500 ms
  if (agora - tLuz >= 500) {
    tLuz = agora;
    luzBruta = lerLDR();
    luzPct = constrain(map(luzBruta, 0, 4095, 100, 0), 0, 100);
    classe = classificar(luzBruta);
    verificarZonaEscura();
    Serial.printf("LDR: %d | luz: %d%% | %s\n", luzBruta, luzPct, classe.c_str());
  }

  // DHT: a cada 2 s
  if (agora - tDht >= 2000) {
    tDht = agora;
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    if (!isnan(t)) temp = t;
    if (!isnan(h)) umid = h;
  }

  // IA: com o carrinho parado (a chamada demora alguns segundos)
  if (pedirIA != 0) {
    char tipo = pedirIA;
    pedirIA = 0;
    if (ultimaIA != 0 && millis() - ultimaIA < 8000) {
      definirAlerta("Aguarde alguns segundos antes de pedir outro alerta ou relatorio.");
    } else {
      modoAuto = false;
      executar('S');
      consultarIA(tipo);
      ultimaIA = millis();
    }
  }

  // Seguranca (so no modo manual): se o celular parar de mandar comandos, o carro para
  if (!modoAuto && estado != 'S' && millis() - ultimoComando > TIMEOUT_MS) {
    executar('S');
  }
}
