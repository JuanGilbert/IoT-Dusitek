\# Testing Smart Switch IoT



Dokumen ini berisi skenario pengujian sistem \*\*Smart Switch IoT berbasis ESP32 dan IR Control System\*\*. Pengujian dilakukan untuk memastikan setiap fitur utama dapat berjalan sesuai fungsi yang dirancang.



\---



\## 1. Tujuan Pengujian



Pengujian dilakukan untuk memastikan bahwa sistem dapat:



\- ESP32 terhubung ke WiFi atau masuk setup mode.

\- Web UI dapat diakses melalui browser.

\- Timer countdown dan countup dapat berjalan.

\- Relay dapat mengontrol stop kontak AC.

\- TFT dapat menampilkan status sistem dan timer.

\- Buzzer dapat memberikan indikator suara.

\- IR learning dapat masuk ke mode pembelajaran remote.

\- IR transmitter dapat memancarkan sinyal IR.

\- Sistem tetap stabil saat ESP32, TFT, relay, dan komponen lain berjalan bersamaan.



\---



\## 2. Tabel Hasil Pengujian



| No | Aspek Pengujian | Test Case / Skenario Pengujian | Expected Output | Actual Output | Status |

|---|---|---|---|---|---|

| 1 | Koneksi ESP32 ke WiFi | Menyalakan ESP32 dan menghubungkannya ke jaringan WiFi tersimpan. | ESP32 dapat terhubung ke jaringan atau masuk setup mode jika WiFi belum tersedia. | ESP32 berhasil terhubung ke WiFi atau masuk setup mode. | Berhasil |

| 2 | TFT Display | Menyalakan sistem dan mengamati tampilan pada TFT. | TFT menampilkan status sistem, alamat IP, dan timer. | TFT berhasil menampilkan status sistem, alamat IP, dan timer. | Berhasil |

| 3 | Web UI | Mengakses alamat IP ESP32 melalui browser. | Halaman kontrol dapat diakses melalui browser. | Halaman Web UI berhasil dibuka dan digunakan. | Berhasil |

| 4 | Countdown/Countup | Memasukkan waktu melalui Web UI dan menjalankan mode countdown/countup. | Timer berjalan sesuai input pengguna. | Timer berhasil berjalan sesuai input pengguna. | Berhasil |

| 5 | Relay dan Stop Kontak AC | Menjalankan timer dan mengecek output stop kontak AC. | Stop kontak aktif saat timer berjalan dan mati saat standby. | Stop kontak berhasil aktif saat timer berjalan dan mati saat standby. | Berhasil |

| 6 | Buzzer | Menekan tombol Learn Remote dan mengamati respon buzzer. | Buzzer berbunyi saat mode Learn Remote atau kondisi tertentu. | Buzzer berhasil berbunyi saat mode Learn Remote dijalankan. | Berhasil |

| 7 | IR Learning | Menekan tombol Learn Remote melalui Web UI. | Sistem masuk mode learning dan TFT menampilkan status learning. | Sistem berhasil masuk mode learning dan TFT menampilkan tampilan Learn Remote. | Berhasil pada mode awal |

| 8 | IR Transmitter | Mengirimkan sinyal IR yang telah dipelajari ke perangkat TV. | Sinyal IR dapat dipancarkan untuk mengontrol atau mematikan TV. | Sinyal IR pernah berhasil diuji oleh tim dan dapat mematikan TV. | Berhasil |

| 9 | OTA Update | Melakukan pembaruan firmware ESP32 melalui jaringan WiFi. | Firmware dapat diperbarui melalui jaringan. | OTA berhasil berjalan setelah dilakukan penyesuaian pada firmware dan koneksi. | Berhasil |

| 10 | Stabilitas Daya | Menjalankan ESP32, TFT, dan relay secara bersamaan. | ESP32 dan TFT tidak mengalami bootloop saat relay berjalan. | Sistem berhasil stabil setelah penggantian relay dan perbaikan wiring. | Berhasil |



\---



\## 3. Prosedur Pengujian



\### 3.1 Tes Koneksi WiFi



Langkah pengujian:



1\. Nyalakan ESP32.

2\. Amati tampilan pada TFT.

3\. Jika WiFi sudah tersimpan, ESP32 akan mencoba terhubung ke jaringan.

4\. Jika WiFi belum tersimpan atau gagal terhubung, ESP32 masuk ke setup mode.



Expected output:



```text

ESP32 berhasil terkoneksi ke WiFi dan menampilkan IP address,

atau masuk setup mode dengan alamat 192.168.4.1.



\### 3.2 Tes Web UI



Langkah pengujian:



1. Pastikan ESP32 sudah terhubung ke WiFi.
2. Lihat IP address pada TFT.
3. Buka browser pada HP/laptop.
4. Masukkan IP address ESP32.
5. Pastikan halaman Web UI terbuka.



Expected output:

Halaman IoT Counter tampil dan tombol kontrol dapat digunakan.



\### 3.3 Tes Countdown



Langkah pengujian:



1. Buka Web UI.
2. Masukkan waktu pada input jam, menit, atau detik.
3. Klik tombol Countdown.
4. Amati tampilan TFT, relay, dan stop kontak.



Expected output:



* Timer berjalan mundur.
* Relay aktif.
* Stop kontak mendapatkan listrik.
* TFT menampilkan waktu countdown.



\### 3.4 Tes Countup



Langkah pengujian:



1. Buka Web UI.
2. Klik tombol Countup.
3. Amati tampilan TFT, relay, dan stop kontak.



Expected output:



* Timer berjalan naik.
* Relay aktif.
* Stop kontak mendapatkan listrik.
* TFT menampilkan waktu countup.



\### 3.5 Tes Pause



Langkah pengujian:



1. Jalankan countdown atau countup.
2. Klik tombol Pause.
3. Amati tampilan TFT dan timer.



Expected output:



* Timer berhenti sementara.
* Sisa waktu tetap terlihat pada TFT.
* Header berubah menjadi PAUSED.



Catatan:



* Pada sistem ini, pause digunakan untuk menghentikan timer sementara.
* Perilaku relay dapat disesuaikan sesuai kebutuhan penggunaan.



\### 3.6 Tes Resume



Langkah pengujian:



1. Jalankan countdown.
2. Klik Pause.
3. Klik Resume.
4. Amati apakah timer berjalan kembali.



Expected output:

* Timer berjalan kembali jika masih ada waktu.
* Relay aktif kembali saat timer dilanjutkan.



\### 3.7 Tes Reset



Langkah pengujian:



1. Jalankan countdown.
2. Klik Reset.
3. Amati tampilan TFT, relay, dan stop kontak.



Expected output:



* Timer berhenti.
* Relay mati.
* Stop kontak tidak mendapatkan listrik.
* TFT kembali ke mode standby.



\### 3.8 Tes Relay dan Stop Kontak AC



Langkah pengujian:



1. Atur multimeter ke mode pengukuran AC.
2. Ukur tegangan L-N stop kontak saat standby.
3. Jalankan timer dari Web UI.
4. Ukur kembali tegangan L-N stop kontak saat timer berjalan.



Expected output:



1. Saat standby: 0V.
2. Saat timer berjalan: sekitar 220V AC.



Catatan:



* Pengujian AC harus dilakukan dengan hati-hati.
* Jangan menyentuh terminal AC saat alat terhubung listrik.



\### 3.9 Tes Buzzer



Langkah pengujian:



1. Buka Web UI.
2. Tekan tombol Learn Remote.
3. Dengarkan respon buzzer.



Expected output:

Buzzer berbunyi singkat ketika mode Learn Remote aktif.



\### 3.10 Tes IR Learning



Langkah pengujian:



1. Tekan tombol Learn Remote.
2. Arahkan remote ke IR receiver.
3. Tekan tombol pada remote.
4. Amati tampilan TFT.



Expected output:

TFT menampilkan status IR tersimpan, protokol, dan kode HEX.



Catatan:

Jika tidak ada remote atau perangkat target, pengujian dapat dilakukan sampai tahap sistem masuk mode learning.



\### 3.11 Tes IR Transmitter



Langkah pengujian:



1. Pastikan kode IR sudah tersimpan.
2. Arahkan IR transmitter ke perangkat target seperti TV.
3. Jalankan perintah yang memicu pengiriman IR.
4. Amati respon perangkat target.



Expected output:

Perangkat target merespon sinyal IR, misalnya TV mati/menyala sesuai sinyal yang dikirim.



Catatan:

Jika belum ada perangkat target, tes sementara dapat dilakukan menggunakan kamera HP untuk melihat kedipan LED IR.



\### 3.12 Tes OTA Update



Langkah pengujian:



1. Pastikan ESP32 dan laptop berada dalam jaringan WiFi yang sama.
2. Pastikan ArduinoOTA sudah aktif pada firmware.
3. Pilih port OTA pada Arduino IDE.
4. Upload firmware melalui jaringan.



Expected output:

* Firmware berhasil diperbarui melalui WiFi.
* ESP32 melakukan restart setelah update selesai.



\### 3.13 Tes Stabilitas Sistem



Langkah pengujian:



1. Nyalakan ESP32 bersama TFT.
2. Jalankan Web UI.
3. Aktifkan timer.
4. Amati apakah sistem berjalan normal saat relay aktif.
5. Uji beberapa kali dengan countdown, reset, pause, dan resume.



Expected output:



* ESP32 tidak bootloop.
* TFT tidak freeze.
* Relay bekerja sesuai perintah.
* Web UI tetap dapat digunakan.



\## 4. Catatan Pengujian

Beberapa catatan dari proses pengujian:

* Pengujian dilakukan secara bertahap dari komponen kecil hingga sistem penuh.
* Pengujian AC dilakukan setelah jalur DC dan firmware berjalan.
* Beban awal yang digunakan sebaiknya beban kecil, seperti charger HP atau lampu kecil.
* Jika relay tidak bekerja, cek logika RELAY\_ON dan RELAY\_OFF.
* Jika stop kontak tidak aktif saat timer berjalan, cek jalur COM dan NO relay.
* Jika ESP32 bootloop, cek power supply, relay, dan distribusi GND.
* Jika TFT putih, cek konfigurasi TFT\_eSPI, pin SPI, dan suplai daya.
* Jika warna gambar aneh, cek byte order RGB565 atau setSwapBytes().
* Jika IR tidak terbaca, cek pin IR receiver, arah remote, dan tegangan modul.



\## 5. Kesimpulan Pengujian

Berdasarkan pengujian yang dilakukan, sistem Smart Switch IoT berbasis ESP32 telah berjalan pada tahap prototipe fungsional. Sistem mampu menjalankan timer melalui Web UI, menampilkan informasi pada TFT, mengontrol stop kontak AC melalui relay, memberikan indikator melalui buzzer, serta menjalankan mode awal IR learning.



Pengujian lanjutan tetap diperlukan untuk memastikan sistem siap digunakan dalam jangka panjang, khususnya pada aspek keamanan casing, durasi penggunaan, kestabilan daya, dan kompatibilitas IR terhadap berbagai perangkat.

