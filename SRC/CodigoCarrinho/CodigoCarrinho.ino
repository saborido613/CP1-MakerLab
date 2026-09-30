#include <WiFi.h>
#include <WebServer.h>

// ---------- Pinos da ponte H (L298N) ----------
#define IN1 23
#define IN2 22
#define IN3 18
#define IN4 19
// #define ENA 5
// #define ENB 23   // GPIO6 NÃO pode ser usado no ESP32 (é ligado à memória flash)

// ---------- Rede Wi-Fi criada pelo ESP32 ----------
const char *SSID = "Carrinho-Grupo";
const char *SENHA = "12345678"; // mínimo de 8 caracteres

WebServer server(80);

int velocidade = 200; // 0 a 255
char estado = 'S';    // F, B, L, R ou S
unsigned long ultimoComando = 0;
const unsigned long TIMEOUT_MS = 600; // para o carro se o celular parar de mandar comandos

// ---------- Página do controle ----------
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
         display:flex; flex-direction:column; align-items:center; justify-content:center; min-height:100vh; }
  h1 { font-size:22px; margin:0 0 20px; }
  .grid { display:grid; grid-template-columns:repeat(3, 90px); grid-template-rows:repeat(3, 90px); gap:12px; }
  button { border:none; border-radius:18px; font-size:34px; background:#2d6cdf; color:#fff; touch-action:none; }
  button:active { background:#1b4aa0; }
  #S { background:#c0392b; font-size:22px; }
  .vazio { visibility:hidden; }
  .vel { margin-top:28px; width:300px; text-align:center; }
  input[type=range] { width:100%; }
</style>
</head>
<body>
  <h1>Carrinho Wi-Fi</h1>
  <div class="grid">
    <div class="vazio"></div><button data-d="F">&#9650;</button><div class="vazio"></div>
    <button data-d="L">&#9664;</button><button id="S">PARAR</button><button data-d="R">&#9654;</button>
    <div class="vazio"></div><button data-d="B">&#9660;</button><div class="vazio"></div>
  </div>
  <div class="vel">
    Velocidade: <span id="v">200</span>
    <input type="range" min="80" max="255" value="200" id="slider">
  </div>
<script>
  let timer = null;
  function enviar(d) { fetch('/cmd?d=' + d).catch(() => {}); }
  function iniciar(d) {
    parar(false);
    enviar(d);
    timer = setInterval(() => enviar(d), 200);   // repete enquanto o botão estiver pressionado
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
  const s = document.getElementById('slider');
  s.addEventListener('input', () => {
    document.getElementById('v').textContent = s.value;
    fetch('/vel?v=' + s.value).catch(() => {});
  });
</script>
</body>
</html>
)rawliteral";

// ---------- Motores ----------
// void aplicarVelocidade() {
//  analogWrite(ENA, velocidade);
// analogWrite(ENB, velocidade);
//}

void moveForward()
{
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
}

void moveBackward()
{
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
}

void turnLeft()
{
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
}

void turnRight()
{
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
}

void stopMotors()
{
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
}

void executar(char d)
{
    switch (d)
    {
    case 'F':
        moveForward();
        break;
    case 'B':
        moveBackward();
        break;
    case 'L':
        turnLeft();
        break;
    case 'R':
        turnRight();
        break;
    default:
        stopMotors();
        d = 'S';
        break;
    }
    estado = d;
}

// ---------- Rotas do servidor ----------
void handleRoot()
{
    server.send_P(200, "text/html", PAGINA);
}

void handleCmd()
{
    if (server.hasArg("d") && server.arg("d").length() > 0)
    {
        executar(server.arg("d")[0]);
        ultimoComando = millis();
    }
    server.send(200, "text/plain", "ok");
}

void handleVel()
{
    if (server.hasArg("v"))
    {
        velocidade = constrain(server.arg("v").toInt(), 0, 255);
        // aplicarVelocidade();
    }
    server.send(200, "text/plain", "ok");
}

// ---------- Setup e loop ----------
void setup()
{
    Serial.begin(115200);

    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    // pinMode(ENA, OUTPUT);
    // pinMode(ENB, OUTPUT);
    stopMotors();
    // aplicarVelocidade();

    WiFi.softAP(SSID, SENHA);
    Serial.print("Rede criada. Acesse: http://");
    Serial.println(WiFi.softAPIP()); // normalmente 192.168.4.1

    server.on("/", handleRoot);
    server.on("/cmd", handleCmd);
    server.on("/vel", handleVel);
    server.begin();
}

void loop()
{
    server.handleClient();

    // Segurança: se a conexão cair com o botão pressionado, o carro para sozinho
    if (estado != 'S' && millis() - ultimoComando > TIMEOUT_MS)
    {
        stopMotors();
        estado = 'S';
    }
}
