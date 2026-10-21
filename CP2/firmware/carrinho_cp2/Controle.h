#pragma once
#include <stdint.h>
#include <math.h>
#include "Config.h"

enum class Pedido : uint8_t { PARAR, FRENTE, TRAS, ESQUERDA, DIREITA };
enum class Classe : uint8_t { INDEFINIDA, PERTO, MEDIO, LONGE };

inline const char* nomeClasse(Classe c) {
  switch (c) {
    case Classe::PERTO: return "perto";
    case Classe::MEDIO: return "medio";
    case Classe::LONGE: return "longe";
    default: return "indefinida";
  }
}

struct Contexto {
  Pedido pedido;
  bool habilitado;
  bool comandoVigente;
  float distancia;
  uint32_t idadeSensor;
  bool modeloValido;
  uint32_t idadeModelo;
  Classe classe;
  float escore;
};

struct Acao {
  int motorA;  // Sinal = direcao; modulo = duty PWM.
  int motorB;
  const char* motivo;
};

inline Acao decidir(const Contexto& c) {
  if (!c.habilitado) return {0, 0, "COLETA: MOTORES DESABILITADOS"};
  if (c.pedido == Pedido::PARAR) return {0, 0, "PARADO PELO CONTROLE"};
  if (!c.comandoVigente) return {0, 0, "COMANDO EXPIRADO"};
  if (!isfinite(c.distancia) || c.distancia < cfg::DIST_MIN_CM ||
      c.distancia > cfg::DIST_MAX_CM || c.idadeSensor > cfg::SENSOR_MAX_IDADE_MS)
    return {0, 0, "SENSOR INVALIDO OU DESATUALIZADO"};

  // O sensor e frontal. Manobras permanecem manuais e exigem supervisao.
  if (c.pedido == Pedido::TRAS)
    return {-cfg::PWM_MANOBRA, -cfg::PWM_MANOBRA, "TRAS MANUAL"};
  if (c.pedido == Pedido::ESQUERDA)
    return {-cfg::PWM_MANOBRA, cfg::PWM_MANOBRA, "ESQUERDA MANUAL"};
  if (c.pedido == Pedido::DIREITA)
    return {cfg::PWM_MANOBRA, -cfg::PWM_MANOBRA, "DIREITA MANUAL"};

  // Protecao independente do ML; nao e a classificacao TinyML.
  if (c.distancia <= cfg::EMERGENCIA_CM)
    return {0, 0, "EMERGENCIA: DISTANCIA ATE 20 CM"};
  if (!c.modeloValido || c.idadeModelo > cfg::MODELO_MAX_IDADE_MS)
    return {0, 0, "AGUARDANDO INFERENCIA VALIDA"};
  if (!isfinite(c.escore) || c.escore < cfg::ESCORE_MIN || c.escore > 1.0f)
    return {0, 0, "ESCORE INSUFICIENTE"};

  // Aqui a acao depende EXCLUSIVAMENTE da classe retornada pelo modelo.
  // Nao ha if(distancia <= 35/80) para substituir o classificador.
  switch (c.classe) {
    case Classe::PERTO: return {0, 0, "ML PERTO: PARAR"};
    case Classe::MEDIO: return {cfg::PWM_LENTO, cfg::PWM_LENTO, "ML MEDIO: FRENTE LENTA"};
    case Classe::LONGE: return {cfg::PWM_NORMAL, cfg::PWM_NORMAL, "ML LONGE: FRENTE NORMAL"};
    default: return {0, 0, "CLASSE DESCONHECIDA"};
  }
}
