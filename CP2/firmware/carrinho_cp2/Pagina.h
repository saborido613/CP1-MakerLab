#pragma once
#include <Arduino.h>

const char PAGINA[] PROGMEM = R"HTML(
<!doctype html>
<html lang="pt-BR"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Carrinho CP2 TinyML</title>
<style>
*{box-sizing:border-box}[hidden]{display:none!important}body{font:16px Arial;background:#111827;color:#f3f4f6;margin:20px auto;max-width:700px;padding:0 14px}
h1{font-size:25px}.box{background:#1f2937;border-radius:16px;padding:16px;margin:14px 0}
button,select,input,a.download{font:inherit;border:0;border-radius:10px;padding:12px;margin:5px;background:#374151;color:white}
input{max-width:150px}button{cursor:pointer}button:disabled{opacity:.4;cursor:default}
.drive{width:85px;height:65px;font-size:27px;touch-action:none;user-select:none}.stop{background:#b91c1c}
.pad{text-align:center}pre{white-space:pre-wrap;font-size:14px}a{color:#93c5fd}.muted{color:#cbd5e1;font-size:14px}
.download{display:inline-block;text-decoration:none;background:#047857!important}
</style></head><body>
<h1>Carrinho ESP32 · CP2 TinyML</h1>
<div class="box"><b id="mode">Conectando...</b><pre id="status">Aguardando ESP32</pre></div>
<div class="box" id="collection">
<h2>Coletar dados reais</h2>
<p class="muted">Mantenha um alvo na mesma classe durante toda a gravacao. Baixe cada CSV antes de iniciar o proximo.</p>
<label>Classe <select id="label"><option>perto</option><option>medio</option><option>longe</option></select></label>
<label>Conjunto <select id="subset"><option>treino</option><option>teste</option></select></label>
<label>Sessao <input id="session" value="A" maxlength="20" pattern="[A-Za-z0-9_-]+"></label><br>
<button id="start" onclick="startCapture()">Gravar 5 segundos</button>
<a class="download" id="download" href="/coleta.csv" hidden>Baixar CSV</a>
<p id="captureMessage" class="muted"></p>
</div>
<div class="box" id="control" hidden>
<h2>Controle assistido pelo modelo</h2>
<p class="muted">Segure a seta para mover. Soltar para. Se o obstaculo sair enquanto a seta estiver pressionada, o carrinho pode retomar. O sensor observa apenas a frente.</p>
<div class="pad"><button class="drive" data-cmd="frente">▲</button><br>
<button class="drive" data-cmd="esquerda">◀</button>
<button class="drive stop" onclick="release()">■</button>
<button class="drive" data-cmd="direita">▶</button><br>
<button class="drive" data-cmd="tras">▼</button></div>
</div>
<p class="muted">Inferencia local no ESP32. O celular so fornece comandos e exibe a telemetria.</p>
<script>
let held='',inflight=false,sequence=0;
const byId=id=>document.getElementById(id);
async function sendHeld(){
  if(!held||inflight)return;
  const thisSequence=sequence,cmd=held;inflight=true;
  try{
    const r=await fetch('/'+cmd,{cache:'no-store'});
    if(!r.ok)throw new Error(await r.text());
  }catch(e){byId('status').textContent='Comando interrompido: '+e.message;held='';}
  finally{inflight=false;if(thisSequence!==sequence)fetch('/parar',{cache:'no-store'}).catch(()=>{});}
}
function release(){held='';sequence++;fetch('/parar',{cache:'no-store',keepalive:true}).catch(()=>{});}
document.querySelectorAll('[data-cmd]').forEach(b=>{
  b.addEventListener('pointerdown',e=>{e.preventDefault();sequence++;held=b.dataset.cmd;b.setPointerCapture(e.pointerId);sendHeld();});
  b.addEventListener('pointerup',release);b.addEventListener('pointercancel',release);b.addEventListener('lostpointercapture',release);
});
window.addEventListener('blur',release);
document.addEventListener('visibilitychange',()=>{if(document.hidden)release();});
setInterval(sendHeld,180);
async function startCapture(){
  const p=new URLSearchParams({classe:byId('label').value,conjunto:byId('subset').value,sessao:byId('session').value});
  byId('start').disabled=true;
  try{const r=await fetch('/coleta/iniciar?'+p,{cache:'no-store'});const t=await r.text();if(!r.ok)throw new Error(t);byId('captureMessage').textContent=t;}
  catch(e){byId('captureMessage').textContent=e.message;byId('start').disabled=false;}
}
async function poll(){
  try{
    const r=await fetch('/status',{cache:'no-store'});if(!r.ok)throw new Error('HTTP '+r.status);const s=await r.json();
    byId('mode').textContent=s.tinyml?'MODO TINYML':'MODO COLETA — motores desabilitados';
    byId('collection').hidden=s.tinyml;byId('control').hidden=!s.tinyml;
    byId('status').textContent='Distancia: '+(s.distancia_cm===null?'sem leitura valida':s.distancia_cm.toFixed(1)+' cm')+
      '\nClasse: '+s.classe+' | escore: '+s.escore.toFixed(3)+
      '\nJanela: '+s.janela+'/10 | inferencias OK: '+s.inferencias+
      '\nUltima inferencia: '+s.inferencia_ms+' ms'+
      '\nAcao: '+s.acao+' | PWM A/B: '+s.pwm_a+'/'+s.pwm_b+
      '\nEscores: '+JSON.stringify(s.escores);
    byId('start').disabled=s.coletando;
    byId('download').hidden=!s.csv_pronto;
    byId('captureMessage').textContent=s.coletando?('Gravando: '+s.coleta_leituras+'/50'):s.coleta_mensagem;
  }catch(e){byId('status').textContent='Sem comunicacao. Verifique o Wi-Fi e aguarde. '+e.message;}
  finally{setTimeout(poll,500);}
}
poll();
</script></body></html>
)HTML";
