#include <WiFi.h>
#include <ESP_Mail_Client.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include "time.h"

// -------- LCD --------
LiquidCrystal_PCF8574 lcd(0x27);

// -------- WiFi --------
#define WIFI_SSID "OPPO A78 5G"
#define WIFI_PASSWORD "12345678"

// -------- Email --------
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 587
#define AUTHOR_EMAIL "umamuralikrishna0@gmail.com"
#define AUTHOR_PASSWORD "imjdooizcpqnsfqn"
#define RECIPIENT_EMAIL "nn024856@gmail.com"

// -------- Pins --------
#define PH_PIN 34
#define TDS_PIN 35
#define IR_PIN 32
#define BUZZER 25
#define FAN 26

// -------- Thresholds --------
float PH_LOW = 6.5;
float PH_HIGH = 8.5;
float TDS_HIGH = 500;

// -------- Variables --------
float phValue = 0;
float tdsValue = 0;

SMTPSession smtp;

// -------- Timers --------
unsigned long lastLCDTime = 0;
unsigned long lastSensorTime = 0;
unsigned long lastAlertTime = 0;

const unsigned long lcdInterval = 1500;
const unsigned long sensorInterval = 1000;
const unsigned long alertInterval = 60000;

// -------- Sensor Functions --------
float readPH() {
  return (analogRead(PH_PIN) / 4095.0) * 14.0;
}

float readTDS() {
  return (analogRead(TDS_PIN) / 4095.0) * 1000;
}

// -------- Email Function --------
void sendEmail(String message) {

  SMTP_Message msg;

  msg.sender.name = "ESP32 Alert";
  msg.sender.email = AUTHOR_EMAIL;
  msg.subject = "Aquaculture Alert!";
  msg.addRecipient("User", RECIPIENT_EMAIL);

  msg.text.content = message;
  msg.text.charSet = "us-ascii";

  ESP_Mail_Session session;

  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;

  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";

  // ✅ IMPORTANT (TIME CONFIG)
  session.time.ntp_server = "pool.ntp.org,time.nist.gov";
  session.time.gmt_offset = 0;
  session.time.day_light_offset = 0;

  // ✅ TLS
  session.secure.startTLS = true;

  Serial.println("📧 Sending Email...");

  if (!smtp.connect(&session)) {
    Serial.println("❌ SMTP Connect Failed:");
    Serial.println(smtp.errorReason());
    return;
  }

  if (!MailClient.sendMail(&smtp, &msg)) {
    Serial.println("❌ Send Failed:");
    Serial.println(smtp.errorReason());
  } else {
    Serial.println("✅ Email Sent Successfully!");
  }

  smtp.closeSession();
}

// -------- TASK FUNCTION --------
void emailTask(void *parameter) {

  String msg = *((String*)parameter);

  sendEmail(msg);

  delete (String*)parameter;
  vTaskDelete(NULL);
}

// -------- Setup --------
void setup() {

  Serial.begin(115200);

  pinMode(FAN, OUTPUT);
  pinMode(IR_PIN, INPUT_PULLUP);

  digitalWrite(FAN, LOW);

  // BUZZER PWM
  ledcSetup(0, 2000, 8);
  ledcAttachPin(BUZZER, 0);
  ledcWrite(0, 0);

  lcd.begin(16, 2);
  lcd.setBacklight(255);

  lcd.print("Connecting WiFi");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  lcd.clear();
  lcd.print("WiFi Connected");
  delay(1000);

  // ✅ DEBUG ENABLE
  smtp.debug(1);

  // ✅ TIME SYNC (VERY IMPORTANT)
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  Serial.println("Syncing time...");
  while (time(nullptr) < 100000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nTime Synced!");
}

// -------- Loop --------
void loop() {

  // WiFi reconnect
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.reconnect();
  }

  // -------- SENSOR READ --------
  if (millis() - lastSensorTime > sensorInterval) {
    lastSensorTime = millis();

    phValue = readPH();
    tdsValue = readTDS();

    Serial.println("----- SENSOR DATA -----");
    Serial.print("pH: "); Serial.println(phValue);
    Serial.print("TDS: "); Serial.println(tdsValue);
  }

  int irState = digitalRead(IR_PIN);

  bool alert = false;
  String message = "Aquaculture Alert!\n";

  // -------- CONDITIONS --------
  if (phValue < PH_LOW || phValue > PH_HIGH) {
    message += "pH out of range\n";
    alert = true;
  }

  if (tdsValue > TDS_HIGH) {
    message += "High TDS\n";
    alert = true;
  }

  // -------- OXYGEN --------
  if (irState == HIGH) {
    message += "Low Oxygen\n";

    digitalWrite(FAN, HIGH);
    ledcWrite(0, 255);

    alert = true;
  } else {
    digitalWrite(FAN, LOW);
    ledcWrite(0, 0);
  }

  // -------- LCD --------
  static bool screenToggle = false;

  if (millis() - lastLCDTime > lcdInterval) {
    lastLCDTime = millis();
    screenToggle = !screenToggle;

    lcd.clear();

    if (screenToggle) {
      lcd.setCursor(0, 0);
      lcd.print("pH:");
      lcd.print(phValue, 2);

      lcd.setCursor(0, 1);
      lcd.print("TDS:");
      lcd.print(tdsValue, 1);
    } else {
      lcd.setCursor(0, 0);
      lcd.print("Oxygen:");
      lcd.print(irState == LOW ? "OK" : "LOW");
    }
  }

  // -------- EMAIL ALERT --------
  if (alert && millis() - lastAlertTime > alertInterval) {

    Serial.println("⚠ ALERT TRIGGERED");

    String *msgPtr = new String(message);

    xTaskCreatePinnedToCore(
      emailTask,
      "Email Task",
      10000,
      msgPtr,
      1,
      NULL,
      1
    );

    lastAlertTime = millis();
  }
}
