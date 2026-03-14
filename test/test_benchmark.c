#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "data_types/hash_map.h"
#include "data_types/double_hash.h"
#include "bellek_izci.h"  // YENİ: Bellek dedektifimizi çağırıyoruz!

// Testler için özel milisaniye ölçücü
double test_sure_olc(struct timespec t1, struct timespec t2) {
    return (t2.tv_sec - t1.tv_sec) * 1000.0 + (t2.tv_nsec - t1.tv_nsec) / 1e6;
}

// RAM'de anında on binlerce sahte (mock) şarkı üretir (Artık İZLENEN bellek ile)
Sarki* mock_liste_olustur(int adet) {
    Sarki* bas = NULL;
    Sarki* son = NULL;
    for(int i = 0; i < adet; i++) {
        Sarki* s = (Sarki*)izlenen_malloc(sizeof(Sarki)); // YENİ: malloc yerine izlenen_malloc
        s->id = i;
        s->sonraki = NULL;
        if(!bas) { bas = s; son = s; }
        else { son->sonraki = s; son = s; }
    }
    return bas;
}

// Orijinal listeyi temizleme fonksiyonu
void mock_liste_temizle(Sarki* liste) {
    Sarki* gecici = liste;
    while(gecici) {
        Sarki* sil = gecici;
        gecici = gecici->sonraki;
        izlened_free(sil, sizeof(Sarki));
    }
}

int main() {
    printf("\n=======================================================\n");
    printf("   🚀 OTOMATIK PERFORMANS VE BELLEK KANITLAMA TESTI\n");
    printf("=======================================================\n");

    izci_sifirla(); // Sayacı sıfırla, dinlemeye başla!

    int N_VERI = 200000;  
    int N_SORGU = 50000;  

    printf("-> %d Mock Sarki RAM'de uretiliyor...\n", N_VERI);
    Sarki* liste = mock_liste_olustur(N_VERI);

    struct timespec t1, t2;
    double sure_ll, sure_cmap, sure_dmap;

    // ---------------------------------------------------------
    // 1. LINKED LIST TESTI
    // ---------------------------------------------------------
    clock_gettime(CLOCK_MONOTONIC, &t1);
    for(int i = 0; i < N_SORGU; i++) {
        int aranan = rand() % N_VERI;
        Sarki* tmp = liste;
        while(tmp) { 
            if(tmp->id == aranan) break; 
            tmp = tmp->sonraki; 
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &t2);
    sure_ll = test_sure_olc(t1, t2);

    // ---------------------------------------------------------
    // 2. CHAINING MAP TESTI
    // ---------------------------------------------------------
    HashMap* cmap = hashmap_olustur();
    Sarki* tmp_c = liste;
    while(tmp_c) {
        Sarki* kopya = (Sarki*)izlenen_malloc(sizeof(Sarki)); // YENİ
        kopya->id = tmp_c->id;
        kopya->sonraki = NULL;
        hashmap_ekle(cmap, kopya);
        tmp_c = tmp_c->sonraki;
    }
    
    clock_gettime(CLOCK_MONOTONIC, &t1);
    for(int i = 0; i < N_SORGU; i++) sarki_ara_map(cmap, rand() % N_VERI);
    clock_gettime(CLOCK_MONOTONIC, &t2);
    sure_cmap = test_sure_olc(t1, t2);

    // ---------------------------------------------------------
    // 3. DOUBLE HASH TESTI
    // ---------------------------------------------------------
    DoubleHashMap* dmap = dhash_olustur();
    Sarki* tmp_d = liste;
    while(tmp_d) {
        Sarki* kopya = (Sarki*)izlenen_malloc(sizeof(Sarki)); // YENİ
        kopya->id = tmp_d->id; 
        kopya->sonraki = NULL;
        dhash_ekle(dmap, kopya);
        tmp_d = tmp_d->sonraki;
    }
    
    clock_gettime(CLOCK_MONOTONIC, &t1);
    for(int i = 0; i < N_SORGU; i++) dhash_ara(dmap, rand() % N_VERI);
    clock_gettime(CLOCK_MONOTONIC, &t2);
    sure_dmap = test_sure_olc(t1, t2);

    // ---------------------------------------------------------
    // BÜYÜK YÜZLEŞME (HIZ KANITLARI)
    // ---------------------------------------------------------
    printf("\n[SURELER]\n");
    printf("- Linked List O(n)  : %.2f ms\n", sure_ll);
    printf("- Chaining Map O(1) : %.2f ms\n", sure_cmap);
    printf("- Double Hash O(1)  : %.2f ms\n", sure_dmap);

    printf("\n[OTOMATIK KANITLAR]\n");
    assert(sure_cmap < sure_ll); 
    printf("   ✔ Kanit 1: Chaining Map, Linked List'ten hizli.\n");
    assert(sure_dmap < sure_ll); 
    printf("   ✔ Kanit 2: Double Hash, Linked List'ten hizli.\n");

    // ---------------------------------------------------------
    // BELLEK TEMİZLİĞİ VE SIZINTI KANITI (MEMORY LEAK TEST)
    // ---------------------------------------------------------
    printf("\n[BELLEK SIZINTISI TESTI (MEMORY LEAK)]\n");
    
    // İşimiz biten her şeyi RAM'den siliyoruz
    hashmap_temizle(cmap);
    dhash_temizle(dmap);
    mock_liste_temizle(liste);

    long ayrilan = izci_malloc_sayisi();
    long silinen = izci_free_sayisi();
    
    printf("- Toplam Ayrilan Bellek Blogu : %ld\n", ayrilan);
    printf("- Toplam Silinen Bellek Blogu : %ld\n", silinen);

    // KANIT 3: Ayrılan bellek blok sayısı, silinen blok sayısına EŞİT olmak ZORUNDA!
    assert(ayrilan == silinen);
    printf("   ✔ Kanit 3: SIFIR BELLEK SIZINTISI (0 Bytes Leaked) kanitlandi!\n");

    printf("\n=======================================================\n");
    printf("✅ PERFORMANS VE BELLEK TESTLERI KUSURSUZ GECTI!\n");
    printf("=======================================================\n\n");

    return 0;
}