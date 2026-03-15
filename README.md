# 🎧 MiniStream - High-Performance In-Memory Database Engine

![C](https://img.shields.io/badge/Language-C-blue.svg)
![Python](https://img.shields.io/badge/Backend-Python-yellow.svg)
![Valgrind](https://img.shields.io/badge/Memory-0%20Leaks%20(Valgrind%20Verified)-success.svg)
![OpenMP](https://img.shields.io/badge/Multithreading-OpenMP-orange.svg)

MiniStream, devasa veri setlerini (milyonlarca şarkı ve çalma listesi) milisaniyeler içinde işlemek, aramak ve yönetmek için **C dilinde sıfırdan geliştirilmiş**, yüksek performanslı bir bellek-içi (in-memory) veri motorudur. 

Proje, düşük seviyeli (low-level) bellek yönetimi algoritmalarını modern bir **Python Web Dashboard** ile birleştirerek uçtan uca (Full-Stack) bir sistem mühendisliği çözümü sunar.

👉 **[Sistem Mimari ve Tasarım Raporunu (RAPOR.md) Okumak İçin Tıklayın](rapor/RAPOR.md)**

---

## 🚀 Temel Mimari ve Özellikler

- **Pointer Modeli (Zero-Copy Architecture):** Geleneksel kopyalama mantığı yerine referans paylaşımı kullanılarak RAM tüketimi **18 kat** azaltılmış ve sistem hızı **3 kat** artırılmıştır.
- **Akıllı Çöp Toplayıcı (Reference Counting):** Çalma listeleri ve şarkılar arasında otonom bir referans sayım mekanizması kurulmuştur. Bu sayede *Use-After-Free* ve *Dangling Pointer* gibi ölümcül bellek zafiyetleri %100 engellenmiştir.
- **Gelişmiş Arama Algoritmaları:** Milyonlarca veri üzerinde arama yapmak için üç farklı veri yapısı desteklenir:
  - `Linked List` - Zaman Karmaşıklığı: O(n)
  - `Chaining Hash Map` - Zaman Karmaşıklığı: O(1)
  - `Double Hash Map` (L1 Cache Uyumlu) - Zaman Karmaşıklığı: O(1) Mutlak Performans
- **Multithreading (Ağır Yük Testi):** OpenMP kullanılarak çoklu çekirdek mimarisi aktif edilmiş, 1000'lerce eşzamanlı arama sorgusu paralel olarak işlenmiştir.
- **Full-Stack Dashboard:** C motoru dinamik kütüphane (`.so`) olarak derlenmiş, Python HTTP sunucusu ile bir REST API'ye dönüştürülmüş ve tarayıcı tabanlı canlı bir analitik ekranına (Chart.js) bağlanmıştır.

---

## 📂 Klasör Yapısı

```text
ministream/
├── src/                # C Çekirdek Motoru (Hash Map, Linked List, Memory Tracker)
├── test/               # Birim (Unit), Bellek (Memory) ve Multithread Testleri
├── backend/            # Python REST API Sunucusu (server.py)
├── dashboard/          # HTML/JS Canlı Analitik Arayüzü (index.html)
├── rapor/              # Tasarım Raporu ve Valgrind 0 Hata Kanıtları
├── Makefile            # Otomatik Derleme ve Test Komutları
└── README.md           # Proje Dokümantasyonu
```
## 🛠️ Nasıl Kurulur ve Çalıştırılır?

### Gereksinimler
* gcc (C Derleyici)

* make (Derleme Otomasyonu)

* python3 (Web Dashboard için)

* valgrind (Bellek Sızıntısı Analizi için - Linux/WSL)

### Ön hazırlık (Veri Oluşturma)
Testleri yapabilmek icin gereken verileri oluşturmak icin:

```bash
make generator
./generator [Üretmek istediğiniz veri sayısı]
```

### 1. Web Dashboard'u Başlatmak (Önerilen)
Projenin gücünü görsel arayüzde test etmek için C kodunu Python kütüphanesine çevirip sunucuyu başlatın:

```bash
make ministream.so
python3 backend/server.py
```
👉 Tarayıcınızda açın: http://localhost:3030

### 2. Terminal Testlerini Çalıştırmak
Sistemin algoritmik gücünü ve sağlamlığını terminal üzerinden test etmek için:

```bash
# Temel fonksiyonları test et:
make test_temel
./test_temel

# Ağır yük (Multithread) simülasyonunu başlat:
make test_multithread
./test_multithread
```

### 3. Bellek Sızıntısı (Valgrind) Analizleri
Projenin sıfır bellek sızıntısıyla (0 errors) çalıştığını kanıtlayan testler:

```bash
make test_bellek
valgrind --leak-check=full ./test_bellek
```
