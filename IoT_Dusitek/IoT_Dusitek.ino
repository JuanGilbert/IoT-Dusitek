#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <Preferences.h>
#include <TFT_eSPI.h>
#include "images.h"
#include <ArduinoOTA.h>

// ==========================================
// PIN CONFIG
// ==========================================
#define RELAY_PIN   26
#define IR_TX_PIN   25
#define IR_RX_PIN   34
#define BUZZER_PIN  17

#define RELAY_ON    LOW
#define RELAY_OFF   HIGH

// ==========================================
// OBJECTS
// ==========================================
TFT_eSPI tft = TFT_eSPI();

IRsend irsend(IR_TX_PIN);
IRrecv irrecv(IR_RX_PIN);
decode_results results;

Preferences preferences;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// ==========================================
// SYSTEM VARIABLES
// ==========================================
bool isAPMode = false;

bool isCountingDown = false;
bool isCountingUp = false;
bool lastModeCountdown = true;

bool isLearningMode = false;

bool isBuzzing = false;
bool buzzerState = false;

bool pendingRelayShutdown = false;
unsigned long relayShutdownMillis = 0;

unsigned long learningStartMillis = 0;
unsigned long previousMillis = 0;
unsigned long saveMillis = 0;
unsigned long buzzerMillis = 0;
unsigned long statusMillis = 0;
unsigned long wifiLostMillis = 0;
bool wifiLostProcess = false;
unsigned long wifiCheckMillis = 0;

long timeRemaining = 0;
long timeElapsed = 0;
long lastDisplayedTime = -1;

volatile long pendingAddTime = 0;

int beepCount = 0;

// ==========================================
// IR VARIABLES
// ==========================================
uint32_t savedCode = 0;
decode_type_t savedProtocol = UNKNOWN;
uint16_t savedBits = 0;

// ==========================================
// HTML
// ==========================================
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">

<title>IoT Counter</title>

<style>

:root{
  --bg:#121212;
  --card:#1e1e1e;
  --primary:#00adb5;
  --danger:#e23e57;
  --text:#ffffff;
}

body{
  background:var(--bg);
  color:var(--text);
  font-family:Arial;
  text-align:center;
  margin:0;
  padding:20px;
}

.card{
  background:var(--card);
  max-width:420px;
  margin:auto;
  padding:20px;
  border-radius:15px;
}

#disp{
  font-size:3rem;
  margin:20px 0;
  color:var(--primary);
  font-family:monospace;
}

input{
  width:70px;
  padding:10px;
  margin:5px;
  text-align:center;
  border:none;
  border-radius:8px;
}

button{
  padding:12px 16px;
  margin:5px;
  border:none;
  border-radius:8px;
  font-weight:bold;
}

.primary{
  background:var(--primary);
}

.danger{
  background:var(--danger);
  color:white;
}

</style>
</head>

<body>

<div class="card">

<h2>IoT Counter</h2>

<div id="disp">00:00:00</div>

<div>
<input type="number" id="h" value="0" min="0">
<input type="number" id="m" value="0" min="0">
<input type="number" id="s" value="0" min="0">
</div>

<br>

<button class="primary" onclick="startCountdown()">Countdown</button>

<button class="primary" onclick="startCountup()">Countup</button>

<hr>

<h3>Preset Timer</h3>

<button class="primary" onclick="presetTimer(1800)">30 MIN</button>

<button class="primary" onclick="presetTimer(3600)">1 HOUR</button>

<button class="primary" onclick="presetTimer(7200)">2 HOURS</button>

<button class="primary" onclick="presetTimer(10800)">3 HOURS</button> 

<br><br>

<button class="primary" onclick="resumeTimer()">RESUME</button>

<button class="primary" onclick="pauseTimer()">PAUSE</button>

<button class="danger" onclick="resetTimer()">RESET</button>

<hr>

<h3>Add Time</h3>

<div>
<input type="number" id="add_h" value="0" min="0">
<input type="number" id="add_m" value="0" min="0">
<input type="number" id="add_s" value="0" min="0">
</div>

<button class="primary" onclick="addTime()">Add Time</button>

<hr>

<button onclick="learnIR()">Learn Remote</button>

<br><br>

<button onclick="window.location.href='/wifi'">WiFi Status</button>

</div>

<script>

var s = new WebSocket(`ws://${window.location.hostname}/ws`);

function getSeconds(){

  let h = parseInt(document.getElementById("h").value) || 0;
  let m = parseInt(document.getElementById("m").value) || 0;
  let sec = parseInt(document.getElementById("s").value) || 0;

  return (h*3600)+(m*60)+sec;
}

function getAddSeconds(){

  let h = parseInt(document.getElementById("add_h").value) || 0;
  let m = parseInt(document.getElementById("add_m").value) || 0;
  let sec = parseInt(document.getElementById("add_s").value) || 0;

  return (h*3600)+(m*60)+sec;
}

function startCountdown(){

  let total = getSeconds();

  if(total > 0){
    s.send("countdown:"+total);
  }
}

function startCountup(){
  s.send("countup");
}

function presetTimer(sec){

  s.send("countdown:"+sec);
}

function pauseTimer(){
  s.send("pause");
}

function resumeTimer(){
  s.send("resume");
}

function resetTimer(){
  s.send("reset");
}

function learnIR(){
  s.send("start_learning");
}

let lastAdd = 0;

function addTime(){

  let now = Date.now();

  // Anti spam
  if(now - lastAdd < 1200){
    return;
  }

  lastAdd = now;

  let extra = getAddSeconds();

  if(extra <= 0){
    return;
  }

  s.send("addtime:"+extra);
}

s.onmessage = (e)=>{

  // =========================
  // STATE MESSAGE
  // =========================
  if(e.data.startsWith("state:")){

    let parts = e.data.split(":");

    let mode = parts[1];
    let sec  = parseInt(parts[2]);

    let hh = Math.floor(sec/3600).toString().padStart(2,'0');
    let mm = Math.floor((sec%3600)/60).toString().padStart(2,'0');
    let ss = (sec%60).toString().padStart(2,'0');

    document.getElementById("disp").innerText =
      `${hh}:${mm}:${ss}`;
  }
}

</script>
</body>
</html>
)rawliteral";

// ==========================================
// TFT FUNCTIONS
// ==========================================
void drawLogoDusitek() {
  tft.setSwapBytes(false);
  tft.pushImage(8, 145, LOGO_DUSITEK_WIDTH, LOGO_DUSITEK_HEIGHT, logoDusitek);
  tft.setSwapBytes(true);
}

void drawBackground() {
  tft.pushImage(0, 30, 320, 180, gambarsantuy);
  drawLogoDusitek();
}

void updateHeader(const char *msg, uint16_t color) {

  tft.fillRect(0, 0, 320, 30, TFT_BLACK);

  tft.setTextColor(color, TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);

  tft.drawString(msg, 160, 15);
}

void drawStatusBar() {

  tft.fillRect(0, 210, 320, 30, TFT_NAVY);

  tft.setTextColor(TFT_WHITE, TFT_NAVY);
  tft.setTextSize(1);
  tft.setTextDatum(MC_DATUM);

  String status;

  if(isAPMode){
    status = "SETUP MODE | 192.168.4.1";
  }
  else{
    status = WiFi.localIP().toString();
  }

  tft.drawString(status, 160, 225);
}

void drawStandbyScreen() {
  // Gambar ulang background + logo agar bekas angka timer hilang
  drawBackground();

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);

  tft.drawString("SMART SWITCH IOT", 210, 95);

  tft.setTextSize(1);
  tft.drawString("Ready to Use", 210, 120);

  drawStatusBar();
}

void showStandbyMode() {
  lastDisplayedTime = -1;
  updateHeader("SYSTEM STANDBY", TFT_YELLOW);
  drawStandbyScreen();
}

void updateMainDisplay(long sec, uint16_t color) {

  if(sec == lastDisplayedTime){
  return;
}

lastDisplayedTime = sec;

  char buf[20];

  sprintf(buf,
          "%02d:%02d:%02d",
          sec/3600,
          (sec%3600)/60,
          sec%60);

  tft.fillRect(20, 70, 280, 80, TFT_BLACK);

  tft.setTextColor(color, TFT_BLACK);
  tft.setTextSize(5);
  tft.setTextDatum(MC_DATUM);

  tft.drawString(buf, 160, 120);
}

// ==========================================
// IR
// ==========================================
void fireSavedIR(){

  if(savedCode != 0 && savedProtocol != UNKNOWN){

    irsend.send(savedProtocol,
                savedCode,
                savedBits);
  }
}

void triggerIRLearning(){

  isLearningMode = true;

  learningStartMillis = millis();

  updateHeader("LEARNING IR", TFT_MAGENTA);

  tft.fillRect(0, 30, 320, 180, TFT_BLACK);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);

  tft.drawString("PRESS REMOTE", 160, 100);

  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);

  irrecv.resume();
}

// ==========================================
// POWER CONTROL
// ==========================================
void relayOn(){
  digitalWrite(RELAY_PIN, RELAY_ON);
}

void relayOff(){
  digitalWrite(RELAY_PIN, RELAY_OFF);
}

void syncRelayState(){

  if(isCountingDown || isCountingUp){
    relayOn();
  }
  else{
    relayOff();
  }
}

void turnDeviceOn(){

  relayOn();

  fireSavedIR();

  lastDisplayedTime = -1;

  drawBackground();

  drawStatusBar();

  updateHeader("SELAMAT BERMAIN", TFT_GREEN);
}

void turnDeviceOff(){

  fireSavedIR();

  pendingRelayShutdown = true;

  relayShutdownMillis = millis();

  digitalWrite(BUZZER_PIN, LOW);

  isBuzzing = false;

  updateHeader("SHUTDOWN...", TFT_ORANGE);
}
// ==========================================
// SAVE TIMER STATE
// ==========================================
void saveTimerState(){

  preferences.putBool("cd", isCountingDown);
  preferences.putBool("cu", isCountingUp);

  preferences.putLong("timer", timeRemaining);
  preferences.putLong("elapsed", timeElapsed);
}

void sendCurrentState(){

  char msg[64];

  // =========================
  // COUNTDOWN
  // =========================
  if(isCountingDown){

    sprintf(msg,
            "state:countdown:%ld",
            timeRemaining);
  }

  // =========================
  // COUNTUP
  // =========================
  else if(isCountingUp){

    sprintf(msg,
            "state:countup:%ld",
            timeElapsed);
  }

  // =========================
  // IDLE
  // =========================
  else{

    if(lastModeCountdown){

      sprintf(msg,
              "state:paused_countdown:%ld",
              timeRemaining);
    }

    else{

      sprintf(msg,
              "state:paused_countup:%ld",
              timeElapsed);
    }
  }

  if(ws.count() > 0){
    ws.textAll(msg);
  }
}

// ==========================================
// WEBSOCKET EVENT
// ==========================================
void onEvent(AsyncWebSocket *server,
             AsyncWebSocketClient *client,
             AwsEventType type,
             void *arg,
             uint8_t *data,
             size_t len) {

  if(type == WS_EVT_CONNECT){

    sendCurrentState();
  }

  if(type == WS_EVT_DATA){

    String message = "";

    if(pendingRelayShutdown){
      return;
    }

    message.reserve(len);

    for(size_t i=0; i<len; i++){
      message += (char)data[i];
    }

    // ======================================
    // LEARN IR
    // ======================================
    if(message == "start_learning"){

      triggerIRLearning();
    }

    // ======================================
    // COUNTDOWN
    // ======================================
    else if(message.startsWith("countdown:")){

      timeRemaining = message.substring(10).toInt();

      pendingRelayShutdown = false;
      pendingAddTime = 0;

      timeElapsed = 0;

      isCountingDown = true;
      isCountingUp = false;

      lastModeCountdown = true;

      turnDeviceOn();

      updateMainDisplay(timeRemaining,
                        TFT_GREEN);

      sendCurrentState();

      saveTimerState();
    }

    // ======================================
    // COUNTUP
    // ======================================
    else if(message == "countup"){

      timeElapsed = 0;

      pendingRelayShutdown = false;
      pendingAddTime = 0;

      isCountingUp = true;
      isCountingDown = false;

      lastModeCountdown = false;

      turnDeviceOn();

      updateMainDisplay(timeElapsed,
                        TFT_CYAN);

      sendCurrentState();

      saveTimerState();
    }

    // ======================================
    // RESUME
    // ======================================
    else if(message == "resume"){

    bool canResume = false;

    if(lastModeCountdown && timeRemaining > 0){

      isCountingDown = true;
      isCountingUp = false;

      updateHeader("SYSTEM ACTIVE", TFT_GREEN);

      canResume = true;
    }

    else if(!lastModeCountdown && timeElapsed > 0){

      isCountingUp = true;
      isCountingDown = false;

      updateHeader("SYSTEM ACTIVE", TFT_CYAN);

      canResume = true;
    }

    if(canResume){
      turnDeviceOn();
    }
    else{
      showStandbyMode();
    }

    sendCurrentState();

    saveTimerState();
  }

    // ======================================
    // RESET
    // ======================================
    else if(message == "reset"){

      isCountingDown = false;
      isCountingUp = false;

      timeRemaining = 0;
      timeElapsed = 0;

      pendingAddTime = 0;

      fireSavedIR();

      relayOff();

      pendingRelayShutdown = false;

      digitalWrite(BUZZER_PIN, LOW);
      isBuzzing = false;

      showStandbyMode();

      sendCurrentState(); 

      saveTimerState();
    }

    // ======================================
    // ADD TIME
    // ======================================
    else if(message.startsWith("addtime:")){

      static unsigned long lastAddMillis = 0;

      if(millis() - lastAddMillis < 300){
        return;
      }

      lastAddMillis = millis();

      long extra =
        message.substring(8).toInt();

      if(extra <= 0 || extra > 86400){
        return;
      }

      pendingAddTime += extra;
    }
    // ======================================
    // PAUSE
    // ======================================
    else if(message == "pause"){

        isCountingDown = false;
        isCountingUp = false;

        relayOff();

        updateHeader("PAUSED", TFT_ORANGE);

        sendCurrentState();

        saveTimerState();
    }
  }
}

String getWifiStatusPage(){

  String savedSsid = preferences.getString("ssid", "");
  String ipAddress;

  if(isAPMode){
    ipAddress = WiFi.softAPIP().toString();
  }
  else{
    ipAddress = WiFi.localIP().toString();
  }

  String connectionStatus;

  if(WiFi.status() == WL_CONNECTED){
    connectionStatus = "Connected";
  }
  else{
    connectionStatus = "Disconnected / AP Mode";
  }

  String modeText;

  if(isAPMode){
    modeText = "Setup Mode";
  }
  else{
    modeText = "Normal Mode";
  }

  String html = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>WiFi Status</title>
    <style>
      body{
        background:#121212;
        color:white;
        font-family:Arial;
        padding:20px;
      }
      .card{
        background:#1e1e1e;
        max-width:420px;
        margin:auto;
        padding:20px;
        border-radius:15px;
      }
      h2{
        text-align:center;
      }
      .row{
        margin:12px 0;
        padding:10px;
        background:#2a2a2a;
        border-radius:8px;
      }
      a, button{
        display:block;
        text-align:center;
        text-decoration:none;
        margin-top:15px;
        padding:12px;
        border-radius:8px;
        border:none;
        font-weight:bold;
      }
      .reset{
        background:#e23e57;
        color:white;
      }
      .back{
        background:#00adb5;
        color:black;
      }
    </style>
  </head>
  <body>
    <div class="card">
      <h2>WiFi Status</h2>
  )rawliteral";

  html += "<div class='row'><b>SSID Tersimpan:</b><br>" + savedSsid + "</div>";
  html += "<div class='row'><b>Status:</b><br>" + connectionStatus + "</div>";
  html += "<div class='row'><b>IP Address:</b><br>" + ipAddress + "</div>";
  html += "<div class='row'><b>Mode:</b><br>" + modeText + "</div>";

  html += R"rawliteral(
      <a class="reset" href="/resetwifi">Reset WiFi</a>
      <a class="back" href="/">Kembali</a>
    </div>
  </body>
  </html>
  )rawliteral";

  return html;
}

void stopSystemForWifiSetup(){

  isCountingDown = false;
  isCountingUp = false;

  timeRemaining = 0;
  timeElapsed = 0;

  pendingAddTime = 0;
  pendingRelayShutdown = false;

  relayOff();

  digitalWrite(BUZZER_PIN, LOW);
  isBuzzing = false;
  buzzerState = false;

  preferences.putBool("cd", false);
  preferences.putBool("cu", false);
  preferences.putLong("timer", 0);
  preferences.putLong("elapsed", 0);

  updateHeader("WIFI SETUP", TFT_RED);
}
// ==========================================
// SETUP
// ==========================================
void setup() {

 Serial.begin(115200);

  digitalWrite(RELAY_PIN, RELAY_OFF);
  pinMode(RELAY_PIN, OUTPUT);

  delay(50);

  pinMode(BUZZER_PIN, OUTPUT);

  relayOff();
  digitalWrite(BUZZER_PIN, LOW);

  // TFT
  tft.init();
  tft.setRotation(1);
  tft.setSwapBytes(true);
  tft.fillScreen(TFT_BLACK);

  drawBackground();

  updateHeader("BOOTING...", TFT_WHITE);

  drawStatusBar();

  // Preferences
  preferences.begin("device_data", false);

  // Restore Timer
  isCountingDown = preferences.getBool("cd", false);
  isCountingUp   = preferences.getBool("cu", false);

  timeRemaining  = preferences.getLong("timer", 0);
  timeElapsed    = preferences.getLong("elapsed", 0);

  syncRelayState();

  // Restore IR
  savedCode = preferences.getUInt("ir_code", 0);

  savedProtocol =
    (decode_type_t)
    preferences.getInt("ir_proto", (int)UNKNOWN);

  savedBits =
    preferences.getUInt("ir_bits", 0);

  // IR START
  irsend.begin();
  irrecv.enableIRIn();

  // ======================================
  // WIFI
  // ======================================
  String ssid = preferences.getString("ssid", "");
  String pass = preferences.getString("pass", "");

  ssid.trim();  
  pass.trim();

  if(ssid == ""){

    isAPMode = true;
  }
  else{

    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(false);
    WiFi.setHostname("IoT-Counter");

    Serial.print("Connecting to SSID: ");
    Serial.println(ssid);

    WiFi.begin(ssid.c_str(), pass.c_str());

    updateHeader("CONNECTING WIFI", TFT_CYAN);

    int attempts = 0;

    while(WiFi.status() != WL_CONNECTED && attempts < 30){
      delay(500);
      Serial.print(".");
      yield();
      attempts++;
    }

    Serial.println();
    Serial.print("WiFi status: ");
    Serial.println(WiFi.status());

    if(WiFi.status() != WL_CONNECTED){
      isAPMode = true;
    }
  }

  // ======================================
  // AP MODE
  // ======================================
  if(isAPMode){

    WiFi.disconnect(true);

    delay(100);

    WiFi.mode(WIFI_AP);

    WiFi.softAP("IoT_Counter_Setup");

    updateHeader("SETUP MODE", TFT_MAGENTA);

    tft.fillRect(0, 30, 320, 180, TFT_BLACK);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextDatum(MC_DATUM);

    tft.drawString("CONNECT WIFI:", 160, 90);
    tft.drawString("IoT_Counter_Setup", 160, 130);

    drawStatusBar();

    server.on("/", HTTP_GET,
    [](AsyncWebServerRequest *request){

      String html = R"rawliteral(
      <!DOCTYPE html>
      <html>
      <body style='background:#121212;color:white;font-family:Arial;text-align:center;margin-top:50px;'>

      <h2>WiFi Setup</h2>

      <form action='/save'>
      <input name='ssid' placeholder='SSID'><br><br>
      <input name='pass' placeholder='Password'><br><br>
      <button type='submit'>Save</button>
      </form>

      </body>
      </html>
      )rawliteral";

      request->send(200,
                    "text/html",
                    html);
    });

    server.on("/save", HTTP_GET,
    [](AsyncWebServerRequest *request){

      if(request->hasParam("ssid") &&
         request->hasParam("pass")){

        preferences.putString(
          "ssid",
          request->getParam("ssid")->value()
        );

        preferences.putString(
          "pass",
          request->getParam("pass")->value()
        );

        request->send(200,
                      "text/html",
                      "<h2>Saved. Restarting...</h2>");

        delay(2000);

        ESP.restart();
      }
    });

    server.on("/resetwifi", HTTP_GET,
    [](AsyncWebServerRequest *request){

      stopSystemForWifiSetup();

      preferences.putString("ssid", "");
      preferences.putString("pass", "");

      request->send(200,
                    "text/html",
                    "<h2>WiFi data cleared. Restarting to Setup Mode...</h2>");

      delay(1000);

      ESP.restart();
    });

    

    server.begin();
  }

  // ======================================
  // NORMAL MODE
  // ======================================
  else{

    WiFi.setSleep(false);

    updateHeader("WIFI CONNECTED", TFT_GREEN);

    tft.fillRect(0, 30, 320, 180, TFT_BLACK);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextDatum(MC_DATUM);

    tft.drawString("OPEN BROWSER:", 160, 90);

    String ip = WiFi.localIP().toString();

    tft.drawString(ip, 160, 130);

    delay(3000);

    drawBackground();

    drawStatusBar();

    // Websocket
    ws.onEvent(onEvent);

    server.addHandler(&ws);

    // Main Page
    server.on("/", HTTP_GET,
    [](AsyncWebServerRequest *request){

      request->send_P(200,
                      "text/html",
                      index_html);
    });

    server.on("/wifi", HTTP_GET,
    [](AsyncWebServerRequest *request){

      request->send(200,
                    "text/html",
                    getWifiStatusPage());
    });

    // Reset WiFi
    server.on("/resetwifi", HTTP_GET,
    [](AsyncWebServerRequest *request){

      stopSystemForWifiSetup();

      preferences.putString("ssid", "");
      preferences.putString("pass", "");

      request->send(200,
                    "text/html",
                    "<h2>WiFi data cleared. Restarting to Setup Mode...</h2>");

      delay(1000);

      ESP.restart();
    });

   server.begin();

    // ======================================
    // OTA
    // ======================================
    ArduinoOTA.setHostname("IoT-Counter");

    ArduinoOTA.onStart([]() {

      ws.closeAll();

      updateHeader("OTA UPDATE",
                  TFT_ORANGE);

      tft.fillRect(0, 30, 320, 180, TFT_BLACK);

      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.setTextSize(2);
      tft.setTextDatum(MC_DATUM);

      tft.drawString("UPDATING...", 160, 120);
    });

    ArduinoOTA.onEnd([]() {

      updateHeader("UPDATE DONE",
                  TFT_GREEN);
    });

    ArduinoOTA.onError([](ota_error_t error) {

      updateHeader("OTA FAILED",
                  TFT_RED);
    });

    ArduinoOTA.setRebootOnSuccess(true);

    ArduinoOTA.setTimeout(10000);

    ArduinoOTA.begin();

    // Restore Timer
    if(isCountingDown){

      turnDeviceOn();

      updateMainDisplay(timeRemaining,
                        TFT_GREEN);
    }

    else if(isCountingUp){

      turnDeviceOn();

      updateMainDisplay(timeElapsed,
                        TFT_CYAN);
    }

    
      else{
        showStandbyMode();
      
    }
  }
}

// ==========================================
// LOOP
// ==========================================
void loop() {

  ArduinoOTA.handle();

  if(!isAPMode && millis() - wifiCheckMillis >= 5000){

  wifiCheckMillis = millis();

  if(WiFi.status() != WL_CONNECTED){

    if(wifiLostMillis == 0){
      wifiLostMillis = millis();
      updateHeader("WIFI LOST", TFT_RED);
      Serial.println("WiFi lost. Trying reconnect...");
    }

    WiFi.disconnect(false);
    delay(100);
    WiFi.reconnect();

    Serial.print("WiFi status: ");
    Serial.println(WiFi.status());
  }
  else{

    if(wifiLostMillis != 0){
      Serial.println("WiFi reconnected.");
      updateHeader("WIFI CONNECTED", TFT_GREEN);
      delay(500);

      if(!isCountingDown && !isCountingUp){
        showStandbyMode();
      }
    }

    wifiLostMillis = 0;
    wifiLostProcess = false;
  }
}

  static unsigned long wsCleanupMillis = 0;

  if(millis() - wsCleanupMillis >= 5000){

    wsCleanupMillis = millis();

    ws.cleanupClients();
  }
  // ======================================
  // IR LEARNING
  // ======================================
  if(isLearningMode){

    // AUTO EXIT LEARNING MODE
  if(millis() - learningStartMillis > 10000){

    isLearningMode = false;

    showStandbyMode();
  }

    if(irrecv.decode(&results)){

      savedCode = results.value;
      savedProtocol = results.decode_type;
      savedBits = results.bits;

      preferences.putUInt("ir_code",
                          savedCode);

      preferences.putInt("ir_proto",
                         (int)savedProtocol);

      preferences.putUInt("ir_bits",
                          savedBits);

      updateHeader("IR SAVED",
                   TFT_GREEN);

      tft.fillRect(0, 30, 320, 180, TFT_BLACK);

      tft.setTextColor(TFT_GREEN,
                       TFT_BLACK);

      tft.setTextSize(2);

      tft.setCursor(10, 80);
      tft.print("PROTO: ");
      tft.println(typeToString(savedProtocol));

      tft.setCursor(10, 120);
      tft.print("HEX: 0x");
      tft.println(savedCode, HEX);

      digitalWrite(BUZZER_PIN, HIGH);
      delay(200);
      digitalWrite(BUZZER_PIN, LOW);

      delay(100);

      digitalWrite(BUZZER_PIN, HIGH);
      delay(200);
      digitalWrite(BUZZER_PIN, LOW);

      delay(2000);

      isLearningMode = false;

      showStandbyMode();

      irrecv.resume();
    }

    return;
  }

  // ======================================
// TIMER
// ======================================
unsigned long currentMillis = millis();

if(pendingRelayShutdown){

  if(currentMillis - relayShutdownMillis >= 10000){

    relayOff();

    pendingRelayShutdown = false;

    isCountingDown = false;
    isCountingUp = false;

    showStandbyMode();

    sendCurrentState();

    saveTimerState();
  }
}

if(currentMillis - statusMillis >= 5000){

  statusMillis = currentMillis;

  drawStatusBar();
}

if(currentMillis - previousMillis >= 1000){

  yield();

  previousMillis = currentMillis;

  // ==================================
  // COUNTDOWN
  // ==================================
  if(isCountingDown){

    // ADD TIME
    if(pendingAddTime > 0){

      timeRemaining += pendingAddTime;

      if(timeRemaining > 999999){
        timeRemaining = 999999;
      }

      pendingAddTime = 0;
    }

    // NORMAL COUNTDOWN
    if(timeRemaining > 0){

      timeRemaining--;

      sendCurrentState();

      updateMainDisplay(timeRemaining,
                        TFT_GREEN);
    }

    // FINISHED
    else{

      isCountingDown = false;

      turnDeviceOff();

      updateHeader("TIME EXPIRED",
                   TFT_RED);

      isBuzzing = true;

      beepCount = 0;

      buzzerState = true;

      digitalWrite(BUZZER_PIN, HIGH);

      buzzerMillis = millis();

      saveTimerState();
    }
  }

  // ==================================
  // COUNTUP
  // ==================================
  if(isCountingUp){

    // ADD TIME
    if(pendingAddTime > 0){

      timeElapsed += pendingAddTime;

      if(timeElapsed > 999999){
        timeElapsed = 999999;
      }

      pendingAddTime = 0;
    }

    // NORMAL COUNTUP
    timeElapsed++;

    sendCurrentState();

    updateMainDisplay(timeElapsed,
                      TFT_CYAN);
  }
}
  // ======================================
  // AUTO SAVE
  // ======================================
  if(currentMillis - saveMillis >= 10000){

    saveMillis = currentMillis;

    saveTimerState();
  }

  // ======================================
  // BUZZER
  // ======================================
  if(isBuzzing){

    unsigned long currentBuzz = millis();

    if(buzzerState &&
       currentBuzz - buzzerMillis >= 1000){

      digitalWrite(BUZZER_PIN, LOW);

      buzzerState = false;

      buzzerMillis = currentBuzz;
    }

    else if(!buzzerState &&
            currentBuzz - buzzerMillis >= 500){

      beepCount++;

      if(beepCount >= 3){

        isBuzzing = false;
      }
      else{

        digitalWrite(BUZZER_PIN, HIGH);

        buzzerState = true;

        buzzerMillis = currentBuzz;
      }
    }
  }
 delay(1);
}