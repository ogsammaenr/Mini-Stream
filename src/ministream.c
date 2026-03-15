#include <stdio.h>
#include <string.h>
#include <time.h>
#include <omp.h>
#include "ministream.h"
#include "bellek_izci.h"
#include "data_types/hash_map.h"
#include "data_types/double_hash.h"

extern Sarki* csv_yukle(const char* dosya_yolu, int limit, int* toplam);

double ms_olc(struct timespec t1, struct timespec t2) {
    return (t2.tv_sec - t1.tv_sec) * 1000.0 + (t2.tv_nsec - t1.tv_nsec) / 1e6;
}

// 1. Şarkı Oluşturma [cite: 180]
Sarki* sarki_olustur(int id, const char* baslik, const char* sanatci, const char* album, int sure) {
    Sarki* s = (Sarki*)izlenen_malloc(sizeof(Sarki));
    if (!s) return NULL;
    
    s->id = id;
    strncpy(s->baslik, baslik, 99); s->baslik[99] = '\0';
    strncpy(s->sanatci, sanatci, 99); s->sanatci[99] = '\0';
    strncpy(s->album, album, 99); s->album[99] = '\0';
    s->sure_sn = sure;
    s->yil = 2000; // Varsayılan veya parametre eklenebilir
    s->ref_sayisi = 0; // Başlangıçta hiçbir listede yok [cite: 180]
    s->sonraki = NULL;
    
    return s;
}

// 2. Şarkı Silme Güvenlik Kontrollü
int sarki_sil(Sarki* sarki) {
    // if (!sarki) return -1;
    // // Eğer şarkı hala bir yerlerde çalma listesindeyse SİLMEYİ REDDET!
    // if (sarki->ref_sayisi > 0) {
    //     printf("UYARI: '%s' hala %d listede kullaniliyor!\n", sarki->baslik, sarki->ref_sayisi);
    //     return -1;
    // }
    izlened_free(sarki, sizeof(Sarki));
    return 0;
}

// 3. Çalma Listesi Oluşturma (Dinamik) [cite: 180]
CalmaListesi* liste_olustur(int id, const char* isim) {
    CalmaListesi* liste = (CalmaListesi*)izlenen_malloc(sizeof(CalmaListesi));
    if (!liste) return NULL;
    
    liste->id = id;
    strncpy(liste->isim, isim, 49); liste->isim[49] = '\0';
    liste->sarki_sayisi = 0;
    liste->kapasite = 10; // Başlangıç kapasitesi [cite: 180]
    
    // Şarkı adreslerini tutacak olan DİZİYİ (Sarki**) heap'te oluşturuyoruz
    liste->sarkilar = (Sarki**)izlenen_malloc(liste->kapasite * sizeof(Sarki*));
    
    return liste;
}

// 4. Listeye Şarkı Ekleme (Realloc Mantığı) [cite: 180, 906-917]
int liste_sarki_ekle(CalmaListesi* liste, Sarki* sarki) {
    if (!liste || !sarki) return -1;
    
    // Kapasite dolduysa diziyi 2 katına çıkar [cite: 907-909]
    if (liste->sarki_sayisi >= liste->kapasite) {
        liste->kapasite *= 2;
        // NOT: realloc'u izlenen_malloc ile sarmalamadığımız için standart realloc kullanıyoruz [cite: 531]
        liste->sarkilar = realloc(liste->sarkilar, liste->kapasite * sizeof(Sarki*));
        if (!liste->sarkilar) return -1;
    }
    
    liste->sarkilar[liste->sarki_sayisi++] = sarki; // Şarkının ADRESİNİ ekle [cite: 915]
    sarki->ref_sayisi++; // Şarkının kullanım sayısını artır 
    
    return 0;
}

// 5. Listeden Şarkı Çıkarma (Swap Mantığı) [cite: 181, 1136-1138]
void liste_sarki_cikar(CalmaListesi* liste, int idx) {
    if (!liste || idx < 0 || idx >= liste->sarki_sayisi) return;
    
    // 1. Şarkının referansını azalt [cite: 1137]
    liste->sarkilar[idx]->ref_sayisi--;
    
    // 2. Dizide boşluk kalmaması için en sondaki elemanı silinen yere taşı (Swap) [cite: 1138]
    liste->sarki_sayisi--;
    liste->sarkilar[idx] = liste->sarkilar[liste->sarki_sayisi]; 
}

// 6. Listeyi Tamamen Temizleme [cite: 181]
void liste_temizle(CalmaListesi* liste) {
    if (!liste) return;
    
    // İçindeki tüm şarkıların referansını düşür
    for (int i = 0; i < liste->sarki_sayisi; i++) {
        Sarki* s = liste->sarkilar[i];
        s->ref_sayisi--;
        // Eğer bu şarkıyı kullanan başka GÜNCEL liste kalmadıysa, RAM'den sil!
        if (s->ref_sayisi == 0) {
            sarki_sil(s);
        }
    }
    
    // DİKKAT DÜZELTME: İlk kapasite 10 olduğu için izleyiciden sadece 10'luk kısmı düşüyoruz.
    // realloc ile büyüyen kısımlar izleyiciye dahil edilmediği için onları hesaptan düşemeyiz!
    izlened_free(liste->sarkilar, 10 * sizeof(Sarki*));
    izlened_free(liste, sizeof(CalmaListesi));
}
// 1. Rakiplerin Yaptığı (Kopya Modeli)
void kopya_modeli_test(int n_sarki, int n_liste, int sarki_per_liste) {
    // 1. Orijinal şarkı havuzu oluştur
    Sarki** havuz = (Sarki**)izlenen_malloc(n_sarki * sizeof(Sarki*));
    for (int i = 0; i < n_sarki; i++) {
        havuz[i] = sarki_olustur(i, "Test Sarki", "Sanatci", "Album", 200);
    }

    // 2. Çalma listeleri oluştur ve KOPYALAYARAK ekle
    for (int i = 0; i < n_liste; i++) {
        CalmaListesi* liste = liste_olustur(i, "Kopya Liste");
        for (int j = 0; j < sarki_per_liste; j++) {
            // Rakiplerin yaptığı ölümcül hata: Her eklemede yeni malloc ve memcpy!
            Sarki* kopya = (Sarki*)izlenen_malloc(sizeof(Sarki));
            memcpy(kopya, havuz[rand() % n_sarki], sizeof(Sarki));
            kopya->ref_sayisi = 0; // Bu kopya sadece bu listeye ait
            kopya->sonraki = NULL;
            
            liste_sarki_ekle(liste, kopya);
        }
        // Listeyi sil (İçindeki şarkı kopyaları da ref_sayisi 0 olduğu için silinir)
        liste_temizle(liste);
    }

    // 3. Orijinal havuzu temizle
    for (int i = 0; i < n_sarki; i++) sarki_sil(havuz[i]);
    izlened_free(havuz, n_sarki * sizeof(Sarki*));
}

// 2. Bizim Tasarımımız (Pointer Modeli)
void pointer_modeli_test(int n_sarki, int n_liste, int sarki_per_liste) {
    Sarki** havuz = (Sarki**)izlenen_malloc(n_sarki * sizeof(Sarki*));
    for (int i = 0; i < n_sarki; i++) {
        havuz[i] = sarki_olustur(i, "Test Sarki", "Sanatci", "Album", 200);
        // DİKKAT DÜZELTME: Havuzun kendisi de bu şarkıların sahibidir!
        // Bu sayede listeler silinse bile ref_sayisi asla 0'a düşmez ve şarkılar havuzda kalır.
        havuz[i]->ref_sayisi = 1; 
    }

    for (int i = 0; i < n_liste; i++) {
        CalmaListesi* liste = liste_olustur(i, "Pointer Liste");
        for (int j = 0; j < sarki_per_liste; j++) {
            liste_sarki_ekle(liste, havuz[rand() % n_sarki]);
        }
        liste_temizle(liste);
    }

    // Orijinal havuzu temizle
    for (int i = 0; i < n_sarki; i++) {
        // DİKKAT DÜZELTME: Havuz referansını kaldırıyoruz ki güvenle silinebilsin
        havuz[i]->ref_sayisi = 0; 
        sarki_sil(havuz[i]);
    }
    izlened_free(havuz, n_sarki * sizeof(Sarki*));
}

const char* deney_json(int n_sarki, int n_liste, int sarki_per_liste) {
    static char json_buffer[2048]; 
    
    struct timespec t1, t2;
    double sure_kopya, sure_pointer;
    
    // 1. Kopya Testi
    izci_sifirla();
    clock_gettime(CLOCK_MONOTONIC, &t1);
    kopya_modeli_test(n_sarki, n_liste, sarki_per_liste);
    clock_gettime(CLOCK_MONOTONIC, &t2);
    sure_kopya = ms_olc(t1, t2);
    size_t mem_kopya = izci_toplam_ayrildi();
    int malloc_kopya = izci_malloc_sayisi();

    // 2. Pointer Testi
    izci_sifirla();
    clock_gettime(CLOCK_MONOTONIC, &t1);
    pointer_modeli_test(n_sarki, n_liste, sarki_per_liste);
    clock_gettime(CLOCK_MONOTONIC, &t2);
    sure_pointer = ms_olc(t1, t2);
    size_t mem_pointer = izci_toplam_ayrildi();
    int malloc_pointer = izci_malloc_sayisi();

    // JSON Formatında Metin Oluşturma
    sprintf(json_buffer, 
        "{\n"
        "  \"kopya\": { \"malloc_sayisi\": %d, \"toplam_byte\": %zu },\n"
        "  \"pointer\": { \"malloc_sayisi\": %d, \"toplam_byte\": %zu },\n"
        "  \"bellek_farki\": %.1f,\n"
        "  \"hiz_farki\": %.1f\n"
        "}\n", 
        malloc_kopya, mem_kopya, 
        malloc_pointer, mem_pointer, 
        mem_pointer > 0 ? (double)mem_kopya / mem_pointer : 0.0, 
        sure_pointer > 0 ? sure_kopya / sure_pointer : 0.0
    );

    return json_buffer;
}

const char* arama_json(int n_sarki, int n_sorgu) {
    static char json_buffer[1024];
    int toplam_sarki = 0;
    // DİKKAT: Kaggle veri setinin yolu sende sarkilar.csv ise burayı ona göre güncelle
    const char* path = "data/generated_tracks.csv"; 

    int* sorgular = (int*)izlenen_malloc(n_sorgu * sizeof(int));
    for(int i = 0; i < n_sorgu; i++) sorgular[i] = rand() % n_sarki; 
    
    struct timespec t1, t2;
    double sure_liste = 0, sure_cmap = 0, sure_dmap = 0;

    // 1. LINKED LIST TESTİ
    Sarki* liste_basi = csv_yukle(path, n_sarki, &toplam_sarki);
    if (liste_basi) {
        clock_gettime(CLOCK_MONOTONIC, &t1);

        #pragma omp parallel for
        for(int i = 0; i < n_sorgu; i++) {
            Sarki* tmp = liste_basi;
            while(tmp) {
                if(tmp->id == sorgular[i]) break;
                tmp = tmp->sonraki;
            }
        }
        clock_gettime(CLOCK_MONOTONIC, &t2);
        sure_liste = ms_olc(t1, t2);
        
        Sarki* curr_sil = liste_basi;
        while(curr_sil) {
            Sarki* sil = curr_sil;
            curr_sil = curr_sil->sonraki;
            izlened_free(sil, sizeof(Sarki));
        }
    }

    // 2. CHAINING MAP TESTİ
    Sarki* map_liste = csv_yukle(path, n_sarki, &toplam_sarki);
    HashMap* cmap = hashmap_olustur();
    Sarki* c_curr = map_liste;
    while(c_curr) {
        Sarki* yedek = c_curr->sonraki;
        c_curr->sonraki = NULL;
        hashmap_ekle(cmap, c_curr);
        c_curr = yedek;
    }
    clock_gettime(CLOCK_MONOTONIC, &t1);

    #pragma omp parallel for
    for(int i = 0; i < n_sorgu; i++) sarki_ara_map(cmap, sorgular[i]);

    clock_gettime(CLOCK_MONOTONIC, &t2);
    sure_cmap = ms_olc(t1, t2);
    hashmap_temizle(cmap);

    // 3. DOUBLE HASH TESTİ
    Sarki* dmap_liste = csv_yukle(path, n_sarki, &toplam_sarki);
    DoubleHashMap* dmap = dhash_olustur();
    Sarki* d_curr = dmap_liste;
    while(d_curr) {
        Sarki* yedek = d_curr->sonraki;
        d_curr->sonraki = NULL;
        dhash_ekle(dmap, d_curr);
        d_curr = yedek;
    }
    clock_gettime(CLOCK_MONOTONIC, &t1);

    #pragma omp parallel for
    for(int i = 0; i < n_sorgu; i++) dhash_ara(dmap, sorgular[i]);
    
    clock_gettime(CLOCK_MONOTONIC, &t2);
    sure_dmap = ms_olc(t1, t2);
    dhash_temizle(dmap);

    izlened_free(sorgular, n_sorgu * sizeof(int));

    // JSON Çıktısı
    sprintf(json_buffer, 
        "{\n"
        "  \"linked_list\": %.2f,\n"
        "  \"chaining\": %.2f,\n"
        "  \"double_hash\": %.2f\n"
        "}\n", 
        sure_liste, sure_cmap, sure_dmap
    );

    return json_buffer;
}