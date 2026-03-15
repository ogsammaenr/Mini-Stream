# MiniStream Tasarım Raporu

**Öğrenci:** Osman Aytaş - 2415501033
**Tarih:** [Tarih]

---

## Soru 1 : Neden Pointer Modeli 

### Hipotez ve Tasarım Kararı
Projeye başlarken çözmemiz gereken en büyük mimari problem, çalma listelerindeki şarkı verilerinin bellekte nasıl tutulacağıydı.  Rakip firmaların kullandığı geleneksel "Kopya Modeli", her çalma listesi için şarkının tüm verilerini (ad, sanatçı, süre vb.) `malloc` ile bellekte yeniden oluşturup kopyalamaktaydı. 

Bu mantık mühendislik açısından son derece kötüdür.  Hedeflenen 500.000 kullanıcı ve her kullanıcının 20 çalma listesi olduğu bir senaryoda, aynı şarkı milyonlarca kez RAM'e yazılacaktır.  Bu da gereksiz `malloc` çağrılarıyla işlemciyi yoracak ve gigabaytlarca bellek israfına yol açacaktır. 

Bizim hipotezimiz ve çözümümüz **Pointer Modeli**dir. Bu modelde, 1.2 Milyon şarkı veritabanına sadece bir kez yüklenir.  Çalma listeleri ise şarkının verisini kopyalamak yerine, sadece o şarkının RAM'deki 8 byte'lık "Adresini (Pointer)" tutar. Beklentimiz, bu modelin bellek tüketimini ve çalışma süresini dramatik ölçüde düşürmesidir.

### Ölçüm Sonuçları
*(Test Senaryosu: 1.200.000 Şarkı, 500.000 Çalma Listesi, Liste Başına 50 Şarkı)*

| Metrik | Kopya Modeli (Rakip Mantığı) | Pointer Modeli (Bizim Tasarım) | İyileştirme |
|---|---|---|---|
| **Toplam malloc sayısı** | 27.200.001 İşlem | 2.200.001 İşlem | **12.4x Daha Az** |
| **Bellek Kullanımı (RAM)** | 8277.13 MB (~8.2 GB) | 457.00 MB | **18.1x Daha Az** |
| **Çalışma Süresi** | 4993.55 ms | 1736.33 ms | **2.9x Daha Hızlı** |

### Sonuçların Değerlendirmesi
Sonuçlar hipotezimizi %100 doğrulamıştır. Rakibin mantığıyla (Kopya Modeli) sistem 8.2 GB RAM tüketerek çökme noktasına gelmiş ve 27 milyon kez işletim sisteminden bellek tahsisi (`malloc`) istemiştir. Pointer modeli ise referans paylaşımı yaparak RAM tüketimini 457 MB seviyesine indirmiş, `malloc` yükünü ortadan kaldırdığı için de işlemleri yaklaşık 3 kat daha hızlı bitirmiştir.

---

## Soru 2 : Linked List mi?, Hash Map mi?

Projenin ikinci aşamasında, veritabanındaki bir şarkıyı bulmak için üç farklı arama algoritması tasarlanmış ve kıyaslanmıştır:

* **1. Linked List O(n):** Verilerin birbirine pointer ile bağlandığı doğrusal yapıdır. Veri büyüdükçe arama süresinin lineer olarak (çok fazla) artmasını bekliyoruz.

* **2. Chaining Hash Map O(1):** Proje isterlerinde yer alan bu yapı, 1024 uzunluğunda bir dizi (kova) kullanır. Çakışmalar (Collision) Linked List ile çözülür. Beklentimiz; Linked List'e göre çok hızlı olmasıdır. Ancak veri 100.000'e çıktığında, 1024'lük diziye sığmayan veriler kovalar içinde uzun zincirler (kova başına ~97 şarkı) oluşturacağı için performansının teorik O(1)'den saparak bir miktar yavaşlamasıdır.

* **3. Double Hash Map O(1):** Projeye ekstra olarak entegre edilen, yüksek performanslı "Açık Adresleme (Open Addressing)" yapısıdır. Dizinin uzunluğu 1024 değil, devasa bir asal sayı olan **2.000.003** olarak belirlenmiştir. Çakışmalar ikinci bir Hash fonksiyonu ile çözülür. Veriler bellekte ardışık (flat array) tutulduğu için işlemcinin L1 Önbelleğini (Cache) kusursuz kullanır. Beklentimiz; büyük verilerde sistemin mutlak galibi olmasıdır.

### Single Core (Tek Çekirdek) Saf Algoritma Testi
*(Ölçümler, işlemci donanım hileleri devreden çıkarılarak 1000 rastgele sorgu üzerinden saf algoritmik hızı görmek için yapılmıştır.)*

| Şarkı Sayısı (N) | Linked List O(n) | Chaining Map O(1) (1024 Kova) | Double Hash O(1) (2M Kova) |
|---|---|---|---|
| **100** | 0.14 ms | 0.01 ms | 0.01 ms |
| **10.000** | 14.75 ms | 0.10 ms | 0.04 ms |
| **100.000** | 462.56 ms | 3.55 ms | 0.07 ms |

**Yorum:** Beklentilerimiz tam olarak gerçekleşti. 100.000 veride Linked List (462 ms) çökerken, Chaining Hash Map (3.55 ms) zincir uzamaları sebebiyle hafif bir darboğaza girmiştir. Double Hash ise L1 Cache uyumu ve devasa tablosu sayesinde 0.07 ms gibi astronomik bir hızda kalarak mimari üstünlüğünü kanıtlamıştır.

---

### Stres Testi ve Multithreading (Çoklu Çekirdek)

Arama algoritmalarının, özellikle binlerce eşzamanlı kullanıcının istek yaptığı gerçek bir sunucu ortamında nasıl davranacağını simüle etmek için sisteme bir "Stres Testi" eklenmiştir. Bu testte arama işlemlerini hızlandırmak amacıyla **Multithread (Çoklu İş Parçacığı)** mimarisi kullanılmıştır.

**Kullanılan Kütüphane ve Yöntem:** Sisteme C dilinin standart paralel programlama kütüphanesi olan `<omp.h>` (OpenMP) entegre edilmiştir. Arama işleminin yapıldığı `for` döngülerinin hemen üzerine `#pragma omp parallel for` derleyici direktifi eklenmiştir. Arama (okuma) işlemleri birbirinden bağımsız olduğu için, bu direktif 1000 arama sorgusunu işlemcinin tüm çekirdeklerine eşit olarak dağıtıp aynı anda (paralel) çalıştırarak büyük bir hız artışı sağlamıştır.

### Multithread (OpenMP) Arama Testi Sonuçları
*(Ölçümler 1000 sorgu üzerinden tüm CPU çekirdekleri aktif edilerek yapılmıştır.)*

| Şarkı Sayısı (N) | Linked List O(n) | Chaining Map O(1) | Double Hash O(1) |
|---|---|---|---|
| **100** | 0.50 ms | 0.05 ms | 0.14 ms |
| **10.000** | 1.83 ms | 0.99 ms | 0.24 ms |
| **100.000** | 96.62 ms | 1.04 ms | 0.13 ms |
| **1.200.000** | 2109.81 ms | 11.88 ms | 0.25 ms |

**Yorum:** Multithread mimarisi, büyük verilerde işlemci gücünü net bir şekilde hissettirmiştir. Tek çekirdekte 462 ms süren 100.000'lik Linked List araması, çekirdeklerin gücü birleştirildiğinde 96 ms'ye kadar düşmüştür. 

*Not: 100 şarkılık çok küçük veri setinde, Multithread sürelerinin (0.14 ms) Tek Çekirdek sürelerinden (0.01 ms) daha yavaş çıkmasının sebebi "Thread Oluşturma Maliyeti"dir (Overhead). İşletim sisteminin çekirdekleri uyandırıp iş dağıtması, işin kendisinden daha uzun sürmüştür. Paralel programlamanın gerçek gücü, iş yükü (N) on binleri geçtiğinde ortaya çıkmaktadır.*

## Soru 3 : `ref_sayisi` olmasaydı ne olurdu?

### 1-3. İsterler: Gerçekleştirilen Adımlar
1. `sarki_sil()` fonksiyonundaki `ref_sayisi > 0` güvenlik kontrolü (if bloğu) koddan tamamen kaldırılarak koruma devreden çıkarıldı.
2. `Sarki` objesi (Adı: "Tehlikeli Madde") oluşturuldu ve hem `Liste 1`'e hem de `Liste 2`'ye eklendi. Ardından bu şarkı, `Liste 1` üzerinden `sarki_sil()` fonksiyonu çağrılarak bellekten (`free` ile) tamamen silindi.
3. Şarkı RAM'den silinmiş olmasına rağmen, `Liste 2` üzerinden bu şarkının adına erişilmeye (`printf` ile okumaya) çalışıldı.

### 4. İster: Valgrind Çıktısı
Yapılan bu sabotaj testi sonucunda Valgrind, programda tam 26 adet bellek ihlali tespit etmiştir. Çıktının özet kısmı aşağıdadır:

---
```
==31487== LEAK SUMMARY:
==31487==    definitely lost: 144 bytes in 2 blocks
==31487==    indirectly lost: 160 bytes in 2 blocks
==31487==      possibly lost: 0 bytes in 0 blocks
==31487==    still reachable: 104 bytes in 2 blocks
==31487==         suppressed: 0 bytes in 0 blocks
==31487== Reachable blocks (those to which a pointer was found) are not shown.
==31487== To see them, rerun with: --leak-check=full --show-leak-kinds=all
==31487== 
==31487== For lists of detected and suppressed errors, rerun with: -s
==31487== ERROR SUMMARY: 26 errors from 7 contexts (suppressed: 0 from 0)
```
---
### 5. İster: Bu hata neden tehlikelidir? Neden bazen "çalışıyor" görünür?
**Neden Tehlikelidir?**

* *Bu duruma yazılım literatüründe **"Use-After-Free" (Serbest Bırakıldıktan Sonra Kullanım)** ve **"Dangling Pointer" (Sallanan İşaretçi)** adı verilir. Şarkının belleği `Liste 1` tarafından işletim sistemine iade edilmiştir, ancak `Liste 2` hala o eski ve artık geçersiz olan bellek adresini işaret etmektedir. Eğer sistem yoğun yük altındayken işletim sistemi o bellek adresini başka bir veriye (örneğin başka bir kullanıcının şifresine veya sistem değişkenine) tahsis ederse, program yanlış veriyi okur, beklenmedik tepkiler verir veya **Segmentation Fault** yiyerek aniden çöker. Siber güvenlikte hackerlar sistemi ele geçirmek için tam olarak bu açıkları kullanırlar.*

**Neden Bazen "Çalışıyor" Görünür?**

* *Bir veriyi `free()` fonksiyonu ile sildiğimizde, işletim sistemi o RAM bloğunun içindeki verileri anında silmez (üzerine sıfır yazmaz); sadece o bloğun "yeniden kullanılabilir" olduğunu işaretler. Eğer biz silme işleminden hemen sonra o adresi tekrar okumaya çalışırsak ve o saniyeler içinde o adrese henüz yeni bir veri yazılmamışsa, program eski veriyi bir **"Hayalet / Zombi Veri"** olarak sorunsuz okur ve ekrana basar. Bu durum yazılımcıyı "Kodum düzgün çalışıyor" şeklinde kandırır, ancak sistem canlıya (Production) alınıp yük bindiğinde rastgele anlarda çökmelere sebep olur.*

İşte `ref_sayisi` (Referans Sayımı), bir belleği hala kullanan listeler varken o belleğin silinmesini engelleyen ve bu ölümcül hataları %100 önleyen mimari güvenlik duvarıdır.

## Soru 4 — Bu sistemi 10x büyütsek ne değişirdi?

### İstenen: Hesabı yap
"500.000 -> 5.000.000 kullanıcı demek:

* **Pointer dizileri:** **~40 GB**

  *(Hesap: 5.000.000 kullanıcı x 20 liste = 100.000.000 çalma listesi. Her listede ortalama 50 şarkı olduğunu varsayarsak: 100.000.000 x 50 şarkı x 8 byte (pointer boyutu) = 40.000.000.000 byte)*
* **Hash map overhead:** **~0.2 GB (200 MB)**

  *(Hesap: Şarkı havuzunun da 1.2 milyondan 12 milyona çıktığını varsayarsak, Double Hash tablosunun çökmemesi için kapasitenin (asal sayı olarak) yaklaşık 25 milyona çıkarılması gerekir. 25.000.000 x 8 byte (pointer) = 200.000.000 byte boşluk/dizi yükü)*

**İlk darboğaz RAM Kapasitesi (Memory Bound) yapısı olurdu çünkü...**
Sadece şarkı adreslerini tutan pointer dizileri bile 40 GB yer kaplamaktadır. Standart bir sunucunun RAM sınırları aşılacağı için işletim sistemi "Swap" (Diski RAM gibi kullanma) işlemine başlayacaktır. Swap başladığı an disk okuma hızları devreye gireceği için O(1) veya O(n) algoritmalarının hiçbir anlamı kalmaz, sistem kilitlenir (Thrashing).

**Şu mimari kararı değiştirirdim:** Çalma listelerindeki pointer dizilerini ve kullanıcı verilerini sürekli uygulamanın ana belleğinde (C Heap) tutma kararından vazgeçerdim. Bunun yerine yatayda ölçeklenebilen disk tabanlı bir veritabanı (örneğin PostgreSQL) kullanırdım. RAM'de sadece o an aktif olan/müzik dinleyen kullanıcıların listelerini tutan bir "Lazy Loading (Tembel Yükleme) ve Cache" mimarisi kurgulardım.

---

### Ek Soru: Kullanıcı Struct'ını Sisteme Eklemek
Kullanici struct'ını sisteme eklediğinizi düşünün. Her kullanıcının 20 CalmaListesi'si var.

`5.000.000 kullanıcı x 20 liste x sizeof(CalmaListesi)` = **6.400.000.000 byte (~6.4 GB)**
*(Hesap: Standart bir CalmaListesi struct'ı; id(4), isim[50], sarkilar**(8), eleman_sayisi(4) vb. ile padding dahil ortalama 64 byte yer kaplar. 100.000.000 liste x 64 = 6.4 GB)*

**Bu ek bellek yükü sistemi nasıl etkilerdi?**
Sisteme sadece "boş liste yapılarını" tanımlamak için bile fazladan 6.4 GB yük binmiş olurdu. Pointer dizileriyle (40 GB) birleştiğinde toplam RAM kullanımı 46.4 GB'a fırlar. Bu durum şiddetli bir **"Memory Fragmentation" (Bellek Parçalanması)** yaratır. Milyonlarca küçük 64 byte'lık yapının RAM'e rastgele dağılması, işlemcinin önbellek (L1/L2 Cache) ıskalama (Cache Miss) oranlarını zirveye çıkarır ve arama/erişim performansını dramatik ölçüde düşürürdü.