# InduSense-IoT
InduSense is an Industrial IoT monitoring system designed to monitor temperature and humidity in real time using ESP8266, DHT22, MQTT, InfluxDB, and Grafana.
# Industrial Monitoring System (ESP8266 + DHT22 + InfluxDB)

Sistem pemantauan suhu dan kelembaban berbasis **ESP8266**, menggunakan sensor **DHT22**, ditampilkan lewat **LCD 16×2 I2C**, dan data dikirim otomatis ke **InfluxDB** (time-series database) lewat HTTP API untuk keperluan monitoring & dashboard industri.


---

## ✨ Fitur

- Pembacaan suhu & kelembaban real-time menggunakan sensor **DHT22**
- Pengiriman data otomatis ke **InfluxDB** lewat HTTP API v2 setiap loop
- Tampilan bergantian di LCD 16×2: data sensor, status WiFi + IP, status koneksi InfluxDB
- Deteksi otomatis kalau pembacaan sensor gagal (sensor error handling)
- Kredensial (WiFi, host/token InfluxDB) dipisah ke file `secrets.h` supaya aman saat diupload ke GitHub

---

## 🧰 Komponen yang Dipakai

### Komponen Utama

1. **ESP8266** (NodeMCU/Wemos D1 Mini) — sebagai mikrokontroler utama, sekaligus modul WiFi
2. **Sensor DHT22** — mengukur suhu dan kelembaban udara
3. **LCD 16×2 I2C** — menampilkan data sensor & status koneksi
4. **Kabel jumper** — penghubung antar-komponen
5. **Sumber daya 5V** — untuk ESP8266/sistem

### Komponen/Layanan Software

- **InfluxDB** — database time-series untuk menyimpan data suhu & kelembaban
- **Wi-Fi** — komunikasi ESP8266 dengan internet/jaringan lokal

---

## 🔌 Wiring ESP8266

| Komponen     | Pin Komponen | ESP8266            |
|---------------|--------------|-----------------------|
| DHT22          | VCC          | 3.3V/5V (sesuai modul)  |
|                | GND          | GND                     |
|                | DATA         | D4                      |
| LCD I2C 16×2    | VCC          | 5V/VIN                  |
|                | GND          | GND                     |
|                | SDA          | D2 (default I2C ESP8266) |
|                | SCL          | D1 (default I2C ESP8266) |

> Lihat gambar wiring lengkap di folder `images/` untuk detail penyambungan kabel.

### Penjelasan Wiring

Sensor DHT22 dihubungkan ke pin **D4** pada ESP8266 sebagai jalur data, sedangkan VCC dan GND dihubungkan ke sumber daya sesuai spesifikasi modul sensor yang digunakan. LCD 16×2 dengan modul I2C dihubungkan melalui jalur SDA dan SCL bawaan ESP8266 (D2 dan D1), sedangkan VCC dan GND masing-masing dihubungkan ke 5V dan GND ESP8266.

---

## 📊 Format Data ke InfluxDB

Data dikirim dalam format **Line Protocol** InfluxDB v2, lewat HTTP POST ke endpoint `/api/v2/write`:

```
dht22 temperature=<nilai_suhu>,humidity=<nilai_kelembaban>
```

Header yang dikirim:
- `Authorization: Token <token_influx>`
- `Content-Type: text/plain`

Response `204` dari server menandakan data berhasil tersimpan.

---

## 📚 Library yang Dibutuhkan

Install lewat Arduino IDE Library Manager:

- `ESP8266WiFi` (bawaan board ESP8266)
- `ESP8266HTTPClient` (bawaan board ESP8266)
- `DHT sensor library` (by Adafruit)
- `LiquidCrystal_I2C`

Pastikan board **ESP8266** sudah terpasang di Arduino IDE (via Boards Manager).

---

## ⚙️ Instalasi & Konfigurasi

1. Clone atau download repo ini
2. Duplikat file `secrets.h.example` menjadi `secrets.h`
3. Isi `secrets.h` dengan data asli kamu:
   ```cpp
   #define SECRET_SSID          "wifi_kamu"
   #define SECRET_PASSWORD      "password_wifi_kamu"
   #define SECRET_INFLUX_HOST   "host_influxdb_kamu"
   #define SECRET_INFLUX_PORT   8086
   #define SECRET_INFLUX_ORG    "org_influxdb_kamu"
   #define SECRET_INFLUX_BUCKET "bucket_influxdb_kamu"
   #define SECRET_INFLUX_TOKEN  "token_influxdb_kamu"
   ```
4. Buka `industrial-monitor.ino` di Arduino IDE
5. Pilih board **ESP8266 (NodeMCU 1.0 / sesuai modul kamu)**, pilih port yang sesuai
6. Upload ke board

> ⚠️ File `secrets.h` sudah otomatis diabaikan git lewat `.gitignore`, jangan pernah upload file itu ke GitHub karena berisi data pribadi/kredensial — terutama **token InfluxDB**, karena token yang bocor bisa dipakai orang lain untuk menulis atau membaca data ke database kamu.

---

## 🗂️ Struktur File

```
industrial-monitor/
├── industrial-monitor.ino  # Kode utama
├── secrets.h.example         # Template kredensial
├── .gitignore
├── README.md
└── images/
    ├── foto-alat.jpg
    └── wiring-diagram.jpg
```

---

## 🧠 Cara Kerja Singkat

1. ESP8266 terhubung ke WiFi menggunakan kredensial dari `secrets.h`
2. Setiap loop, sensor DHT22 membaca suhu & kelembaban
3. Jika pembacaan gagal (NaN), LCD menampilkan pesan error dan sistem menunggu sebelum mencoba lagi
4. Jika pembacaan berhasil dan WiFi terhubung, data dikirim ke InfluxDB lewat HTTP POST
5. LCD menampilkan 3 tampilan bergantian: data sensor, status WiFi + IP address, dan status koneksi InfluxDB

---

## 📄 Lisensi

Bebas digunakan dan dimodifikasi untuk keperluan edukasi maupun pengembangan lebih lanjut.
