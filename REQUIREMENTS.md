# Requirements Smart Switch IoT

Dokumen ini berisi kebutuhan hardware, software, library, konfigurasi, dan kebutuhan pengujian untuk menjalankan project **Smart Switch IoT berbasis ESP32 dan IR Control System**.

Project ini menggunakan ESP32 sebagai mikrokontroler utama untuk mengontrol stop kontak AC berdasarkan timer dari Web UI, menampilkan status melalui TFT display, memberikan indikator suara melalui buzzer, serta mendukung fitur IR learning dan IR transmitter.

---

## 1. Hardware Requirements

| Komponen | Spesifikasi / Keterangan | Fungsi |
|---|---|---|
| ESP32 DevKit | ESP32 dengan WiFi | Mikrokontroler utama |
| TFT SPI Display | TFT berbasis SPI, contoh ILI9341 | Menampilkan status sistem, IP address, dan timer |
| Relay Module | Relay 1 channel atau 2 channel 5V | Mengontrol jalur listrik stop kontak |
| HLK-PM01 / AC-DC 5V | Input AC 220V, output DC 5V | Power supply untuk ESP32 dan komponen lain |
| IR Receiver | Modul penerima IR | Menerima sinyal dari remote |
| IR Transmitter | Modul pemancar IR | Mengirim sinyal IR ke TV/perangkat target |
| Buzzer | Active buzzer/module buzzer | Indikator suara |
| Terminal Block | Terminal sambungan kabel | Membagi dan merapikan jalur kabel |
| Stop Kontak AC | Output AC untuk perangkat | Tempat perangkat TV/monitor dicolokkan |
| RC Snubber | Modul/perangkat snubber | Mengurangi noise/lonjakan pada relay |
| Kabel NYAF | Kabel AC sesuai kebutuhan | Jalur fasa dan netral AC |
| Kabel Jumper | Male/female jumper | Jalur sinyal DC |
| Casing / Box | Akrilik/3D print/box elektrik | Pelindung rangkaian |

---

## 2. Software Requirements

| Software | Fungsi |
|---|---|
| Arduino IDE | Menulis, compile, dan upload firmware |
| ESP32 Board Package | Menambahkan dukungan board ESP32 di Arduino IDE |
| Browser | Mengakses Web UI ESP32 |
| Driver USB ESP32 | Agar ESP32 terbaca sebagai port COM |
| Git | Version control sebelum upload ke GitHub |

---

## 3. Arduino Board Requirements

Board yang digunakan pada Arduino IDE:

```text
ESP32 Dev Module
Setting yang disarankan
Board        : ESP32 Dev Module
Upload Speed : 115200
Port         : Sesuai COM ESP32 yang terbaca

## 4. Library Requirements

Library yang digunakan pada firmware:
WiFi.h
AsyncTCP.h
ESPAsyncWebServer.h
IRremoteESP8266.h
IRsend.h
IRrecv.h
IRutils.h
Preferences.h
TFT_eSPI.h
ArduinoOTA.h

Library bawaan ESP32:
WiFi
Preferences
ArduinoOTA

Library eksternal yang perlu di-install:
AsyncTCP
ESPAsyncWebServer
IRremoteESP8266
TFT_eSPI

## 5. Pin Requirements
| Fungsi         |                   Pin ESP32 | Keterangan         |
| -------------- | --------------------------: | ------------------ |
| Relay          |                     GPIO 26 | Mengontrol relay   |
| IR Transmitter |                     GPIO 25 | Mengirim sinyal IR |
| IR Receiver    |                     GPIO 34 | Menerima sinyal IR |
| Buzzer         |                     GPIO 17 | Indikator suara    |
| TFT SPI        | Sesuai konfigurasi TFT_eSPI | Display sistem     |
Catatan:
GPIO 34 hanya input, sehingga cocok untuk IR receiver.
Pin TFT harus disesuaikan dengan konfigurasi pada TFT_eSPI/User_Setup.h.

## 6. TFT_eSPI Requirements
Project menggunakan library TFT_eSPI, sehingga konfigurasi display harus disesuaikan sebelum upload firmware.
Contoh konfigurasi umum:

#define ILI9341_DRIVER
#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   5
#define TFT_DC   2
#define TFT_RST  4
Catatan:
Driver TFT harus sesuai dengan modul yang digunakan.
Jika layar putih, cek kembali driver TFT, pin SPI, power, dan konfigurasi User_Setup.h.
Jika gambar tampil dengan warna aneh, cek pengaturan setSwapBytes() atau byte order gambar RGB565.
Untuk pin LED pada TFT menggunakan pin 3.3V dari ESP

## 7. Image Requirements
Project menggunakan file images.h untuk menyimpan gambar background dan logo dalam bentuk array RGB565.
Format gambar yang digunakan:
Format warna : RGB565 / 16-bit
Tipe array   : uint16_t
File         : images.h
1. Contoh pemanggilan gambar: tft.pushImage(0, 30, 320, 180, gambarsantuy);
2. Untuk logo: tft.pushImage(8, 145, LOGO_DUSITEK_WIDTH, LOGO_DUSITEK_HEIGHT, logoDusitek);
Catatan:
Ukuran gambar jangan terlalu besar agar firmware tetap muat.
Background TFT yang digunakan berukuran 320 x 180 px.
Logo disarankan berukuran kecil, misalnya 80 x 60 px.
Jika warna gambar aneh, sesuaikan byte order atau tft.setSwapBytes().

## 8. Power Requirements
Sistem membutuhkan sumber daya DC 5V untuk ESP32, TFT, relay, buzzer, dan modul IR.
Rekomendasi power:
5V minimal 1A
Lebih aman 5V 2A jika banyak komponen aktif bersamaan
Catatan:
HLK-PM01 dapat digunakan untuk mengubah AC 220V menjadi DC 5V.
Jangan mengandalkan breadboard untuk distribusi daya final.
Jalur 5V dan GND sebaiknya dibuat langsung dan kuat.
Semua GND pada bagian DC harus tersambung bersama.
Jika ESP32 bootloop saat relay aktif, cek kapasitas power supply, wiring, dan kualitas sambungan.

## 9. Network Requirements
Sistem membutuhkan koneksi WiFi untuk:
Mengakses Web UI.
Mengirim perintah timer dari browser ke ESP32.
Menampilkan IP address pada TFT.
Melakukan OTA update firmware.
Jika WiFi belum tersimpan, sistem akan masuk ke setup mode.
Setup mode:
SSID ESP32 : IoT_Counter_Setup
Alamat IP  : 192.168.4.1

## 10. Functional Requirements
| No | Kebutuhan Fungsional               | Keterangan                                                |
| -- | ---------------------------------- | --------------------------------------------------------- |
| 1  | Sistem dapat terhubung ke WiFi     | ESP32 terhubung ke jaringan atau masuk setup mode         |
| 2  | Sistem menyediakan Web UI          | Pengguna dapat mengakses kontrol melalui browser          |
| 3  | Sistem dapat menjalankan countdown | Timer berjalan mundur sesuai input                        |
| 4  | Sistem dapat menjalankan countup   | Timer berjalan naik dari nol                              |
| 5  | Sistem dapat melakukan pause       | Timer berhenti sementara                                  |
| 6  | Sistem dapat melakukan resume      | Timer berjalan kembali jika masih ada waktu               |
| 7  | Sistem dapat melakukan reset       | Timer berhenti dan sistem kembali standby                 |
| 8  | Relay aktif saat timer berjalan    | Stop kontak mendapatkan listrik                           |
| 9  | Relay mati saat standby/reset      | Stop kontak tidak mendapatkan listrik                     |
| 10 | TFT menampilkan status             | Menampilkan IP, timer, standby, learning, dan status lain |
| 11 | Buzzer berbunyi sebagai indikator  | Terutama saat Learn Remote atau kondisi tertentu          |
| 12 | Sistem dapat masuk IR learning     | ESP32 siap menerima sinyal remote                         |
| 13 | Sistem dapat mengirim sinyal IR    | IR transmitter mengirim sinyal yang tersimpan             |
| 14 | Sistem mendukung OTA update        | Firmware dapat diperbarui melalui WiFi                    |

## 11. Safety Requirements
Karena project menggunakan listrik AC 220V, maka beberapa hal berikut wajib diperhatikan:
Jalur AC dan DC harus dipisahkan.
Terminal AC harus tertutup.
Kabel AC harus terpasang kuat dan tidak longgar.
Stop kontak harus terpasang kuat pada casing.
Jangan melakukan wiring saat listrik tersambung.
Gunakan casing untuk mencegah kontak langsung dengan jalur AC.
Gunakan beban kecil terlebih dahulu saat pengujian awal.
Jangan menyambungkan ground ke netral jika tidak ada jalur ground asli.
Pastikan tidak ada kabel fasa dan netral yang short.
Gunakan multimeter pada mode yang sesuai saat melakukan pengukuran AC.

## 12. Testing Requirements
Pengujian minimal yang perlu dilakukan:
| Pengujian          | Tujuan                                                    |
| ------------------ | --------------------------------------------------------- |
| Tes WiFi           | Memastikan ESP32 dapat terhubung ke jaringan              |
| Tes TFT            | Memastikan display menampilkan status                     |
| Tes Web UI         | Memastikan halaman kontrol dapat diakses                  |
| Tes Timer          | Memastikan countdown/countup berjalan                     |
| Tes Relay          | Memastikan relay ON/OFF sesuai timer                      |
| Tes Stop Kontak    | Memastikan output AC aktif saat timer berjalan            |
| Tes Buzzer         | Memastikan indikator suara bekerja                        |
| Tes IR Learning    | Memastikan mode learning aktif                            |
| Tes IR Transmitter | Memastikan sinyal IR dapat dikirim                        |
| Tes OTA            | Memastikan firmware dapat diperbarui lewat jaringan       |
| Tes Stabilitas     | Memastikan ESP32 tidak bootloop saat semua komponen aktif |

## 13. Project Status
Project telah berhasil berjalan pada tahap prototipe fungsional. Fungsi utama seperti ESP32, TFT display, Web UI, timer, relay, stop kontak AC, buzzer, IR learning, dan IR transmitter telah diuji secara bertahap.
Pengembangan berikutnya dapat mencakup:

Perapian wiring.
Desain casing yang lebih aman.
Pengujian jangka panjang.
Pengujian IR pada berbagai perangkat.
Integrasi dengan sistem billing atau manajemen rental.
