/*
 * Industrial Monitoring System
 * Sensor Suhu & Kelembaban (DHT22) + LCD + Kirim Data ke InfluxDB
 * MCU: ESP8266
 *
 * CATATAN:
 * Semua data rahasia (WiFi, host/token InfluxDB) TIDAK ditulis
 * langsung di file ini, tapi diambil dari "secrets.h".
 * Sebelum compile, pastikan kamu sudah:
 *   1. Copy "secrets.h.example" -> "secrets.h"
 *   2. Isi "secrets.h" dengan credential asli kamu
 * File "secrets.h" sudah otomatis diblokir dari Git lewat .gitignore.
 */

// ================= SECRETS =================
#include "secrets.h"

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN D4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

// Alamat LCD (ubah ke 0x3F jika tidak tampil)
LiquidCrystal_I2C lcd(0x27, 16, 2);

const char* ssid     = SECRET_SSID;
const char* password = SECRET_PASSWORD;

const char* influxHost = SECRET_INFLUX_HOST;
const int influxPort   = SECRET_INFLUX_PORT;

const char* org    = SECRET_INFLUX_ORG;
const char* bucket = SECRET_INFLUX_BUCKET;
const char* token  = SECRET_INFLUX_TOKEN;

bool influxStatus = false;

void setup() {

  Serial.begin(115200);

  dht.begin();

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Industrial");
  lcd.setCursor(0, 1);
  lcd.print("Monitoring");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting...");
  lcd.setCursor(0, 1);
  lcd.print("WiFi");

  WiFi.begin(ssid, password);

  Serial.print("Menghubungkan WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Terhubung");
  Serial.print("IP ESP8266: ");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  delay(2000);
}

void loop() {

  float suhu = dht.readTemperature();
  float kelembaban = dht.readHumidity();

  if (isnan(suhu) || isnan(kelembaban)) {

    Serial.println("Gagal membaca DHT22");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sensor Error");
    lcd.setCursor(0, 1);
    lcd.print("Check DHT22");

    delay(5000);
    return;
  }

  Serial.print("Suhu: ");
  Serial.print(suhu);
  Serial.print(" C  ");

  Serial.print("Kelembaban: ");
  Serial.print(kelembaban);
  Serial.println(" %");

  influxStatus = false;

  if (WiFi.status() == WL_CONNECTED) {

    WiFiClient client;
    HTTPClient http;

    String url =
      "http://" + String(influxHost) + ":" + String(influxPort) + "/api/v2/write?org=" + String(org) + "&bucket=" + String(bucket) + "&precision=s";

    String data =
      "dht22 temperature=" + String(suhu) + ",humidity=" + String(kelembaban);

    http.begin(client, url);

    http.addHeader("Authorization", "Token " + String(token));
    http.addHeader("Content-Type", "text/plain");

    int httpCode = http.POST(data);

    Serial.print("HTTP Response: ");
    Serial.println(httpCode);

    if (httpCode == 204)
      influxStatus = true;

    http.end();
  }

  //===========================
  // Tampilan 1
  //===========================

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.print(suhu, 1);
  lcd.write(byte(223));
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Hum :");
  lcd.print(kelembaban, 1);
  lcd.print("%");

  delay(2000);

  //===========================
  // Tampilan 2
  //===========================

  lcd.clear();

  lcd.setCursor(0, 0);

  if (WiFi.status() == WL_CONNECTED)
    lcd.print("WiFi: Connected");
  else
    lcd.print("WiFi: Offline");

  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());

  delay(2000);

  //===========================
  // Tampilan 3
  //===========================

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("InfluxDB");

  lcd.setCursor(0, 1);

  if (influxStatus)
    lcd.print("Connected");
  else
    lcd.print("Failed");

  delay(2000);
}
