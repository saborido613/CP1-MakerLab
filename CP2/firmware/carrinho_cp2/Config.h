#pragma once

// PRIMEIRA ETAPA: 0 = coletar CSVs, motores desabilitados, sem biblioteca ML.
// APOS TREINAR: 1 = carregar a biblioteca real e controlar o carrinho.
#ifndef USAR_TINYML
#define USAR_TINYML 0
#endif

namespace cfg {
constexpr int IN1 = 18;
constexpr int IN2 = 5;
constexpr int IN3 = 12;
constexpr int IN4 = 14;
constexpr int TRIG = 26;
constexpr int ECHO = 25;

constexpr unsigned long PERIODO_MS = 100;  // 10 Hz, igual ao Edge Impulse.
constexpr unsigned long ATRASO_MAX_MS = 20;
constexpr int LEITURAS_COLETA = 50;        // Uma gravacao de 5 s.
constexpr int LEITURAS_JANELA = 10;        // Janela de 1 s.
constexpr int PASSO_INFERENCIA = 2;        // Reavaliar a cada 200 ms.
constexpr float DIST_MIN_CM = 2.0f;
constexpr float DIST_MAX_CM = 300.0f;
constexpr float EMERGENCIA_CM = 20.0f;
constexpr float ESCORE_MIN = 0.80f;
constexpr unsigned long SENSOR_MAX_IDADE_MS = 250;
constexpr unsigned long MODELO_MAX_IDADE_MS = 600;
constexpr unsigned long COMANDO_MAX_IDADE_MS = 700;

// PWM: 0..255. Ajustar no carrinho real: o motor deve partir sem travar.
// PWM nao e velocidade medida em cm/s e nao varia linearmente com ela.
constexpr int PWM_LENTO = 150;
constexpr int PWM_NORMAL = 190;
constexpr int PWM_MANOBRA = 150;

constexpr const char* SSID = "Carrinho-ESP32";
constexpr const char* SENHA = "12345678";
}
