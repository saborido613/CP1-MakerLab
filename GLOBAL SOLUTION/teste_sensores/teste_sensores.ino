// Teste dos sensores: LDR (analogico) + DHT (temperatura/umidade) no ESP32
// Bibliotecas (Gerenciador de Bibliotecas da Arduino IDE):
//   - "DHT sensor library" (Adafruit)
//   - "Adafruit Unified Sensor" (Adafruit)
// Abra o Monitor Serial em 115200.

#include <DHT.h>

#define LDR_PIN  34       // AO do modulo LDR (ADC1, funciona com Wi-Fi ligado)
#define DHT_PIN  4        // DATA do DHT
#define DHT_TYPE DHT11    // TROQUE para DHT22 se o seu sensor for DHT22

DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  Serial.println("Teste de sensores iniciado");
}

void loop() {
  int luz = analogRead(LDR_PIN);        // 0..4095
  float temp = dht.readTemperature();   // graus Celsius
  float umid = dht.readHumidity();      // %

  Serial.print("LDR: ");
  Serial.print(luz);

  if (isnan(temp) || isnan(umid)) {
    Serial.println(" | DHT: ERRO de leitura (confira ligacao e tipo DHT11/DHT22)");
  } else {
    Serial.print(" | Temp: ");
    Serial.print(temp);
    Serial.print(" C | Umidade: ");
    Serial.print(umid);
    Serial.println(" %");
  }

  delay(2000);  // o DHT precisa de pelo menos 1-2 s entre leituras
}
