#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <ESPmDNS.h>

/* --- إعدادات البرمجة اليدوية --- */
/*
    جزئية الريست 
    1-ونحرق الكود  true ب reset_to_defaults  نخلي 
    2-ونحرق الكود  false ب reset_to_defaults  نخلي 


*/
/*--------  Default User & Password Web ----------*/
bool force_defaults = false; 
String d_usr = "moamen";
String d_pwd = "moamen123";
int d_code[3] = {0, 4, 0};     // default Code Pass 

/* --- Hardware Mapping --- */
const char* ssid = "Moamen_Secure_Lock";
const char* pass = "652005"; 

const int BCD_P[] = {35, 33, 25, 32};
const int PIN_ENT = 16, PIN_RST = 19, PIN_BUZ = 21, PIN_OUT = 2, PIN_CNT = 17, PIN_CLK = 27, PIN_BLK = 18, PIN_PWR = 4;

/* --- System State --- */
WebServer server(80);
Preferences prefs;
String usr, pwd;
bool is_auth = false;
int sys_p[3], in_p[3], idx = 0, f_cnt = 0;
unsigned long bl_tm = 0, pwr_tm = 0;
bool s_st = true, unlocked = false, locked = false;

volatile bool cnt_trig = false;
volatile unsigned long last_int = 0;
String logs[5] = {"System Started", "-", "-", "-", "-"};

/* --- Core Logic --- */
void IRAM_ATTR cnt_isr() {
  unsigned long now = millis();
  if (now - last_int > 200) { cnt_trig = true; }
  last_int = now;
}

void add_log(String e) {
  for (int i = 4; i > 0; i--) logs[i] = logs[i-1];
  logs[0] = e;
}

void beep(int t) { digitalWrite(PIN_BUZ, 1); delay(t); digitalWrite(PIN_BUZ, 0); }

void boot_s() { for(int i=0; i<3; i++) { beep(80); delay(80); } }
void web_ok_s() { beep(100); delay(100); beep(100); }
void upd_s() { for(int i=0; i<5; i++) { beep(40); delay(40); } }

void hw_rst() { digitalWrite(PIN_RST, 1); delay(20); digitalWrite(PIN_RST, 0); }

int read_hw() {
  int v = 0;
  for (int i = 0; i < 4; i++) v |= (digitalRead(BCD_P[i]) << i);
  if (v > 9) { hw_rst(); v = 0; }
  return v;
}

void move_to(int target) {
  int cur = read_hw();
  int stp = target - cur;
  if (stp < 0) stp += 10;
  while(stp--) { digitalWrite(PIN_CLK, 1); delay(5); digitalWrite(PIN_CLK, 0); delay(40); }
}

void anim_success() {
  digitalWrite(PIN_BLK, 1); hw_rst();
  for (int i = 0; i <= 8; i++) { move_to(i); beep(30); delay(50); }
  beep(100); delay(50); beep(200);
}

void anim_fail() {
  digitalWrite(PIN_BLK, 1); hw_rst();
  for (int i = 0; i <= 9; i++) { move_to(i); digitalWrite(PIN_BUZ, 1); delay(20); digitalWrite(PIN_BUZ, 0); delay(30); }
  beep(500); 
}

/* --- Web Interface --- */
String get_page(String msg = "") {
  String css = R"raw(
    <style>
      body{background:#f0f2f5;font-family:sans-serif;text-align:center;margin:0;direction:rtl;}
      .box{max-width:420px;margin:30px auto;padding:15px;}
      .card{background:#fff;padding:25px;border-radius:15px;box-shadow:0 8px 20px rgba(0,0,0,0.05);margin-bottom:20px;}
      .name{font-family:'Times New Roman',serif;font-size:32px;font-weight:bold;color:#1877f2;margin-bottom:20px;}
      h3{font-size:15px;color:#1877f2;text-align:right;border-right:4px solid #1877f2;padding-right:10px;margin:15px 0;}
      input{width:100%;padding:14px;margin:10px 0;border:1px solid #ddd;border-radius:8px;box-sizing:border-box;text-align:center;font-size:16px;}
      .btn{background:#1877f2;color:#fff;border:none;padding:15px;width:100%;border-radius:8px;font-weight:bold;cursor:pointer;}
      .warn{background:#fff3cd;color:#856404;padding:10px;border-radius:8px;font-size:12px;margin-bottom:15px;border:1px solid #ffeeba;font-weight:bold;}
      .ok{color:#28a745;margin-bottom:10px;font-weight:bold;} .err{color:#dc3545;margin-bottom:10px;font-weight:bold;}
      .l-item{text-align:right;font-size:12px;color:#555;padding:5px 0;border-bottom:1px solid #eee;}
    </style>
  )raw";

  String body = "<div class='box'><div class='name'>مؤمن أحمد السيد</div>" + msg;
  if(!is_auth) {
    body += "<div class='card'><h2>دخول النظام</h2><form action='/login' method='POST'><input name='u' placeholder='المستخدم'><input name='p' type='password' placeholder='كلمة المرور'><button class='btn'>دخول</button></form></div>";
  } else {
    body += "<div class='card'><h3>تعديل كلمة مرور النظام</h3><div class='warn'>تحذير: في حالة نسيان كلمة المرور، يرجى الرجوع إلى المبرمج (مؤمن أحمد السيد)</div><form action='/set_kit' method='POST'><input name='c' type='password' maxlength='3' placeholder='كود جديد'><button class='btn'>تحديث</button></form></div>";
    body += "</h3><div class='warn'>يرجي اعادة تشغيل الجهاز ، بعد تعديل كلمة السر  </div>";

    body += "<div class='card'><h3>إعدادات المسؤل </h3><form action='/set_adm' method='POST'><input name='nu' placeholder='مستخدم جديد'><input name='np' type='password' placeholder='كلمة مرور جديد'><button class='btn' style='background:#42b72a'>حفظ</button></form></div>";
    body += "<div class='card'><h3>السجل</h3>";
    for(int i=0;i<5;i++) if(logs[i]!="-") body += "<div class='l-item'>• " + logs[i] + "</div>";
    body += "<br><a href='/logout' style='color:red;text-decoration:none;font-size:12px;'>خروج</a></div>";
  }
  return "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'>" + css + "</head><body>" + body + "</body></html>";
}

void setup() {
  prefs.begin("m_sys", false);
  if (force_defaults) prefs.clear();
  for(int i=0; i<3; i++) sys_p[i] = prefs.getInt(("p"+String(i)).c_str(), d_code[i]);
  usr = prefs.getString("u", d_usr); pwd = prefs.getString("p", d_pwd);

  WiFi.softAP(ssid, pass);
  MDNS.begin("moamen");

  server.on("/", [](){ server.send(200, "text/html", get_page()); });
  server.on("/login", [](){
    if(server.arg("u") == usr && server.arg("p") == pwd) { is_auth = true; web_ok_s(); server.send(200, "text/html", get_page("<div class='ok'>تم الدخول</div>")); }
    else server.send(200, "text/html", get_page("<div class='err'>❌بيانات خاطئة </div>"));
  });
  server.on("/set_kit", [](){
    if(is_auth && server.arg("c").length() == 3) {
      for(int i=0; i<3; i++) { sys_p[i] = server.arg("c").substring(i,i+1).toInt(); prefs.putInt(("p"+String(i)).c_str(), sys_p[i]); }
      upd_s(); add_log("تغيير الباسورد"); server.send(200, "text/html", get_page("<div class='ok'>تم تحديث النظام</div>"));
    }
  });
  server.on("/set_adm", [](){
    if(is_auth) { usr = server.arg("nu"); pwd = server.arg("np"); prefs.putString("u", usr); prefs.putString("p", pwd); upd_s(); add_log("تغيير الأدمن"); server.send(200, "text/html", get_page("<div class='ok'>✅ تم التحديث</div>")); }
  });
  server.on("/logout", [](){ is_auth = false; server.send(200, "text/html", get_page()); });
  server.begin();

  for(int i=0;i<4;i++) pinMode(BCD_P[i], INPUT);
  pinMode(PIN_ENT, INPUT_PULLUP); pinMode(PIN_RST, OUTPUT); pinMode(PIN_BUZ, OUTPUT);
  pinMode(PIN_OUT, OUTPUT); pinMode(PIN_BLK, OUTPUT); digitalWrite(PIN_BLK, 1);
  pinMode(PIN_CNT, INPUT_PULLUP); pinMode(PIN_CLK, OUTPUT); pinMode(PIN_PWR, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(PIN_CNT), cnt_isr, FALLING);
  hw_rst();
  boot_s(); 
}

void loop() {
  server.handleClient();

  if (digitalRead(PIN_PWR) == 0) {
    if (pwr_tm == 0) pwr_tm = millis();
    if (millis() - pwr_tm > 3000) {
      beep(1000); digitalWrite(PIN_OUT, 0); digitalWrite(PIN_BLK, 0);
      WiFi.mode(WIFI_OFF);
      esp_sleep_enable_ext0_wakeup((gpio_num_t)PIN_PWR, 0); 
      esp_deep_sleep_start();
    }
  } else pwr_tm = 0;

  if (locked) { digitalWrite(PIN_BLK, 0); return; }
  if (unlocked) { digitalWrite(PIN_BLK, 0); return; }

  if (cnt_trig) {
    digitalWrite(PIN_CLK, 1); delay(5); digitalWrite(PIN_CLK, 0);
    read_hw(); beep(60);
    cnt_trig = false;
  }

  if (millis() - bl_tm > 400) { s_st = !s_st; digitalWrite(PIN_BLK, !s_st); bl_tm = millis(); }

  if (digitalRead(PIN_ENT) == 0) {
    delay(200); beep(150); digitalWrite(PIN_BLK, 1);
    int v = read_hw();
    if (idx < 3) in_p[idx++] = v;
    hw_rst();
    if (idx == 3) {
      bool ok = true;
      for (int i = 0; i < 3; i++) if (in_p[i] != sys_p[i]) ok = false;
      if (ok) { add_log("دخول ناجح"); anim_success(); unlocked = true; digitalWrite(PIN_OUT, 1); f_cnt = 0; }
      else { f_cnt++; add_log("فشل دخول"); anim_fail(); if (f_cnt >= 3) locked = true; }
      idx = 0; hw_rst();
    }
    while(digitalRead(PIN_ENT) == 0);
  }
}