\# Wiring Smart Switch IoT



Dokumen ini menjelaskan wiring dasar project \*\*Smart Switch IoT berbasis ESP32 dan IR Control System\*\*. Wiring dilakukan secara bertahap agar setiap bagian dapat diuji dengan aman, terutama pada jalur listrik AC 220V.



> Peringatan: Project ini menggunakan listrik AC 220V. Pastikan listrik dicabut total sebelum memasang, memindahkan, atau memperbaiki kabel.



\---



\## 1. Pin ESP32



| Komponen | Pin ESP32 | Keterangan |

|---|---:|---|

| Relay IN | GPIO 26 | Kontrol relay |

| IR Transmitter | GPIO 25 | Mengirim sinyal IR |

| IR Receiver | GPIO 34 | Menerima sinyal remote |

| Buzzer | GPIO 17 | Indikator suara |

| TFT SPI | Sesuai konfigurasi TFT\_eSPI | Display sistem |



\---



\## 2. Wiring Power DC 5V



Sumber daya DC berasal dari modul AC-DC seperti HLK-PM01 atau power supply 5V lain.



```text

HLK +5V  → VIN / 5V ESP32

HLK GND  → GND ESP32



HLK +5V  → VCC Relay

HLK GND  → GND Relay



HLK +5V / 3V3 → VCC TFT sesuai modul

HLK GND       → GND TFT



HLK +5V / 3V3 → VCC IR module sesuai modul

HLK GND       → GND IR module



Catatan:

* Semua GND pada bagian DC harus tersambung bersama.
* Jangan menggunakan breadboard untuk instalasi final.
* Untuk upload firmware, ESP32 sebaiknya diberi daya dari USB laptop terlebih dahulu.
* Jika ESP32 atau TFT mengalami bootloop, cek kembali suplai 5V, GND, dan kualitas kabel.



\## 3. Wiring Relay DC



Jika menggunakan relay 2 channel dengan pin VCC, GND, IN1, dan IN2, gunakan satu channel saja.

VCC Relay → 5V

GND Relay → GND

IN1 Relay → GPIO 26 ESP32

IN2 Relay → tidak digunakan

Jika menggunakan IN1, maka jalur AC harus menggunakan terminal relay channel 1:

COM1 dan NO1

Jangan menggunakan IN1 tetapi jalur AC masuk ke COM2/NO2, karena relay bisa terlihat aktif tetapi stop kontak tidak mendapat listrik.



\## 4. Wiring AC 220V ke Relay dan Stop Kontak

Jalur AC yang dikontrol adalah jalur fasa/line.

L / Fasa input AC → Terminal Block L → COM1 Relay

NO1 Relay         → L Stop Kontak

N / Netral input AC → Terminal Block N → N Stop Kontak



Skema sederhana:

AC L ── Terminal L ── COM Relay ── NO Relay ── L Stop Kontak

AC N ── Terminal N ─────────────────────────── N Stop Kontak



Kondisi Kerja:

Timer standby  → Relay OFF → Stop kontak mati

Timer berjalan → Relay ON  → Stop kontak aktif



Catatan:



* Gunakan terminal NO, bukan NC.
* COM relay memang wajar mendapatkan fasa.
* Output stop kontak baru aktif ketika relay menyambungkan COM ke NO.
* Jangan mengubah wiring saat alat masih tersambung ke listrik.



\## 5. Wiring Stop Kontak

Pada stop kontak, biasanya terdapat terminal:

L = Line / Fasa

N = Netral

⏚ = Ground / Arde



Wiring yang digunakan:

NO Relay → L Stop Kontak

N Terminal Block → N Stop Kontak

Ground input → Ground Stop Kontak jika tersedia

Jika sumber listrik tidak memiliki ground asli, terminal ground pada stop kontak dikosongkan.



Catatan:



* Jangan menyambungkan ground ke netral secara sembarangan.
* Pastikan kabel L dan N terjepit kuat pada terminal stop kontak.
* Pastikan tidak ada serabut kabel yang keluar dan menyentuh terminal lain.
* Stop kontak harus terpasang kuat pada casing agar tidak ikut tertarik saat colokan dicabut.



\## 6. Wiring RC Snubber

RC snubber dapat dipasang pada sisi output beban untuk membantu mengurangi noise atau lonjakan pada relay.

RC Snubber 1 → L output setelah NO relay

RC Snubber 2 → N output stop kontak

Skema:

NO Relay ── L Stop Kontak ──┐

&#x20;                           │

&#x20;                        RC Snubber

&#x20;                           │

N Terminal ─ N Stop Kontak ─┘



\## 7. Wiring TFT SPI

Pin TFT mengikuti konfigurasi pada library TFT\_eSPI.

Contoh umum:

TFT VCC  → 3V3 / 5V sesuai modul

TFT GND  → GND

TFT MISO → GPIO 19

TFT MOSI → GPIO 23

TFT SCLK → GPIO 18

TFT CS   → GPIO 5

TFT DC   → GPIO 2

TFT RST  → GPIO 4 / RST / -1 sesuai konfigurasi



Catatan:



* Sesuaikan dengan modul TFT yang digunakan.
* Jika layar putih, cek driver TFT, pin SPI, tegangan VCC, GND, dan konfigurasi User\_Setup.h.
* Jika warna gambar aneh, cek setSwapBytes() atau byte order gambar RGB565.



\## 8. Wiring IR Receiver

IR Receiver OUT / S → GPIO 34 ESP32

IR Receiver VCC     → 3V3 ESP32

IR Receiver GND     → GND ESP32

Catatan:



* Gunakan 3.3V untuk IR receiver agar sinyal aman untuk ESP32.
* Saat mode learning, arahkan remote ke IR receiver.



\## 9. Wiring IR Transmitter



Jika menggunakan modul IR transmitter:



IR Transmitter S / IN → GPIO 25 ESP32

IR Transmitter VCC    → 3V3 / 5V sesuai modul

IR Transmitter GND    → GND ESP32



Jika menggunakan LED IR mentah, sebaiknya gunakan resistor dan transistor driver agar lebih aman dan pancaran lebih kuat.

## 10. Wiring Buzzer



Untuk buzzer module 3 pin:



Buzzer IN / S → GPIO 17 ESP32

Buzzer VCC    → 3V3 / 5V sesuai modul

Buzzer GND    → GND ESP32



Untuk active buzzer 2 pin kecil:



Buzzer + → GPIO 17

Buzzer - → GND



\## 11. Urutan Pengujian Wiring



Urutan pengujian yang disarankan:



1. Tes HLK/power supply tanpa beban.
2. Tes ESP32 saja.
3. Tes ESP32 + TFT.
4. Tes ESP32 + TFT + relay tanpa AC.
5. Tes COM-NO relay menggunakan multimeter mode continuity.
6. Tes jalur AC tanpa beban.
7. Tes stop kontak menggunakan charger HP atau lampu kecil.
8. Tes timer dari Web UI.
9. Tes buzzer.
10. Tes IR learning.
11. Tes IR transmitter ke perangkat target.



\## 12. Checklist Wiring

| Pengecekan                             | Status        |

| -------------------------------------- | ------------- |

| Jalur L masuk ke COM relay             | Belum / Sudah |

| NO relay masuk ke L stop kontak        | Belum / Sudah |

| Netral masuk langsung ke N stop kontak | Belum / Sudah |

| L dan N tidak short                    | Belum / Sudah |

| Relay ON/OFF sesuai timer              | Belum / Sudah |

| Stop kontak mati saat standby          | Belum / Sudah |

| Stop kontak aktif saat timer berjalan  | Belum / Sudah |

| ESP32 dan TFT tidak bootloop           | Belum / Sudah |

| Buzzer berbunyi                        | Belum / Sudah |

| IR learning masuk mode belajar         | Belum / Sudah |



\## 13. Catatan Keamanan

* Jangan melakukan wiring saat listrik tersambung.
* Jangan menyentuh terminal AC saat alat menyala.
* Gunakan casing agar jalur AC tidak terbuka.
* Pisahkan area AC dan DC di dalam box.
* Pastikan terminal L, N, dan relay tidak longgar.
* Gunakan multimeter dengan mode yang benar saat mengukur AC.
* Uji beban kecil terlebih dahulu sebelum menggunakan perangkat utama.

