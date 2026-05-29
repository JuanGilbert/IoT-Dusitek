# Smart Switch IoT Berbasis ESP32 dan IR Control System

Project ini merupakan sistem Smart Switch IoT berbasis ESP32 yang dikembangkan untuk mengontrol penggunaan perangkat pada skenario rental PS. Sistem ini menggunakan timer berbasis Web UI untuk mengaktifkan atau menonaktifkan stop kontak AC melalui relay. Selain itu, sistem juga dilengkapi TFT display sebagai tampilan status, buzzer sebagai indikator, serta IR receiver dan IR transmitter untuk fitur pembelajaran dan pemancaran sinyal remote.

Project ini berada pada tahap prototipe fungsional. Fungsi utama sistem telah berjalan, seperti kontrol timer, relay, Web UI, TFT display, buzzer, stop kontak AC, dan fitur awal IR. Pengembangan lanjutan dapat difokuskan pada perapian casing, pengujian jangka panjang, serta integrasi dengan sistem billing atau manajemen rental.

## Fitur Utama

- Kontrol stop kontak AC menggunakan relay.
- Timer berbasis Web UI melalui browser.
- Mode countdown dan countup.
- Fitur pause, resume, reset, dan add time.
- TFT display untuk menampilkan status sistem, IP address, timer, dan mode tertentu.
- Buzzer sebagai indikator suara.
- IR learning untuk membaca sinyal remote.
- IR transmitter untuk mengirim ulang sinyal remote.
- WiFi setup mode jika ESP32 belum memiliki koneksi WiFi tersimpan.
- OTA update untuk pembaruan firmware melalui jaringan.
- Penyimpanan konfigurasi menggunakan Preferences/NVS ESP32.

## Komponen Hardware

| Komponen | Fungsi |
|---|---|
| ESP32 DevKit | Mikrokontroler utama |
| TFT SPI Display | Menampilkan status sistem dan timer |
| Relay Module | Mengontrol jalur listrik stop kontak |
| HLK-PM01 / AC-DC 5V | Mengubah AC 220V menjadi DC 5V |
| IR Receiver | Menerima sinyal remote |
| IR Transmitter | Mengirim sinyal IR ke perangkat TV |
| Buzzer | Indikator suara |
| Terminal Block | Distribusi dan sambungan kabel |
| Stop Kontak AC | Output daya untuk perangkat |
| RC Snubber | Mengurangi lonjakan/noise pada relay |
| Kabel NYAF | Jalur kabel untuk AC |
| Kabel Jumper | Jalur sinyal dan DC |
| Casing / Box | Pelindung rangkaian dan instalasi |

## Pin ESP32

| Komponen | Pin ESP32 |
|---|---|
| Relay | GPIO 26 |
| IR Transmitter | GPIO 25 |
| IR Receiver | GPIO 34 |
| Buzzer | GPIO 17 |
| TFT SPI | Mengikuti konfigurasi `TFT_eSPI/User_Setup.h` |

Catatan: Pin TFT dapat berbeda tergantung modul TFT yang digunakan. Pastikan konfigurasi pada library `TFT_eSPI` sudah sesuai dengan wiring.

## Library yang Digunakan

Project ini menggunakan beberapa library Arduino:

- `WiFi.h`
- `AsyncTCP.h`
- `ESPAsyncWebServer.h`
- `IRremoteESP8266.h`
- `Preferences.h`
- `TFT_eSPI.h`
- `ArduinoOTA.h`

Library eksternal yang perlu di-install:

- AsyncTCP
- ESPAsyncWebServer
- IRremoteESP8266
- TFT_eSPI

## Struktur File

Contoh struktur project:

```text
smart-switch-iot-esp32/
├── iotweb.ino
├── images.h
├── README.md
└── docs/


## Cara Upload Firmware
1. Buka project di Arduino IDE.
2. Pilih board ESP32 Dev Module.
3. Pilih port COM ESP32.
4. Atur upload speed ke 115200.
5. Klik Upload.
6. Jika gagal saat connecting, tekan tombol BOOT.

## Cara Penggunaan
1. Nyalakan alat.
2. Hubungkan ESP32 ke WiFi atau masuk setup mode.
3. Buka IP address ESP32 melalui browser.
4. Masukkan waktu pada Web UI.
5. Jalankan countdown atau countup.
6. Stop kontak aktif saat timer berjalan dan mati saat timer berhenti.

## Catatan Keamanan

Project ini melibatkan listrik AC 220V. Pastikan seluruh proses wiring dilakukan dalam kondisi listrik mati. Gunakan terminal block, isolasi, casing, dan pengaman yang sesuai. Jangan menyentuh bagian terminal AC ketika alat sedang terhubung ke listrik. Pengujian AC sebaiknya dilakukan secara bertahap dan diawasi oleh orang yang memahami kelistrikan.

## Status Project

Project berada pada tahap prototipe fungsional. Sistem utama sudah berjalan, meliputi ESP32, TFT, relay, Web UI timer, buzzer, IR learning, IR transmitter, dan stop kontak AC. Pengembangan lanjutan dapat difokuskan pada penyempurnaan casing, perapian wiring, pengujian jangka panjang, serta integrasi dengan sistem billing atau manajemen rental.