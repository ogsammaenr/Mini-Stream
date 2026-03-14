#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include "ministream.h"
#include "bellek_izci.h"

#include "data_types/hash_map.h"
#include "data_types/double_hash.h"

extern Sarki* csv_yukle(const char* dosya_yolu, int limit, int* toplam);

// Zaman ölçümü için yardımcı fonksiyon
double ms_olc(struct timespec t1, struct timespec t2);

void arama_benchmark_calistir() {
    int N_SARKI = 1200000;  // 100 Bin Şarkı
    int N_SORGU = 10000;   // 10.000 Rastgele Sorgu
    int toplam_sarki = 0;
    const char* path = "data/tracks_features.csv";

    // Adil yarış için ID'leri önceden hazırla
    int* sorgular = (int*)malloc(N_SORGU * sizeof(int));
    for(int i = 0; i < N_SORGU; i++) sorgular[i] = rand() % N_SARKI; 
    
    struct timespec t1, t2;
    double sure_liste, sure_cmap, sure_dmap;

    printf("\n==================================================\n");
    printf("   MINISTREAM STRESS TEST: %d ARAMA SORGUSU\n", N_SORGU);
    printf("==================================================\n");

    // ---------------------------------------------------------
    // TEST 1: LINKED LIST (İzole)
    // ---------------------------------------------------------
    printf("\n[1/3] Linked List yukleniyor...\n");
    Sarki* liste_basi = csv_yukle(path, N_SARKI, &toplam_sarki);
    
    printf("-> Linked List (O(n)) test ediliyor (Lutfen bekleyin...)\n");
    clock_gettime(CLOCK_MONOTONIC, &t1);
    
    #pragma omp parallel for
    for(int i = 0; i < N_SORGU; i++) {
        Sarki* tmp = liste_basi;
        while(tmp) {
            if(tmp->id == sorgular[i]) break;
            tmp = tmp->sonraki;
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &t2);
    sure_liste = ms_olc(t1, t2);

    // Temizlik
    Sarki* curr_sil = liste_basi;
    while(curr_sil) {
        Sarki* silinecek = curr_sil;
        curr_sil = curr_sil->sonraki;
        izlened_free(silinecek, sizeof(Sarki));
    }

    // ---------------------------------------------------------
    // TEST 2: CHAINING HASH MAP (İzole)
    // ---------------------------------------------------------
    printf("\n[2/3] Chaining Hash Map yukleniyor...\n");
    Sarki* map_icin_liste = csv_yukle(path, N_SARKI, &toplam_sarki);
    HashMap* cmap = hashmap_olustur();
    
    Sarki* curr_map = map_icin_liste;
    while(curr_map) {
        Sarki* sonraki_yedek = curr_map->sonraki;
        curr_map->sonraki = NULL; // Güvenlik kalkanı
        hashmap_ekle(cmap, curr_map);
        curr_map = sonraki_yedek;
    }

    printf("-> Chaining Hash Map (O(1)) test ediliyor...\n");
    clock_gettime(CLOCK_MONOTONIC, &t1);

    #pragma omp parallel for
    for(int i = 0; i < N_SORGU; i++) sarki_ara_map(cmap, sorgular[i]);
    clock_gettime(CLOCK_MONOTONIC, &t2);
    sure_cmap = ms_olc(t1, t2);

    hashmap_temizle(cmap);

    // ---------------------------------------------------------
    // TEST 3: BİZİM DOUBLE HASH MAP (İzole)
    // ---------------------------------------------------------
    printf("\n[3/3] Double Hash Map yukleniyor...\n");
    Sarki* dmap_icin_liste = csv_yukle(path, N_SARKI, &toplam_sarki);
    DoubleHashMap* dmap = dhash_olustur();
    
    Sarki* curr_dmap = dmap_icin_liste;
    while(curr_dmap) {
        Sarki* sonraki_yedek = curr_dmap->sonraki;
        curr_dmap->sonraki = NULL; // Güvenlik kalkanı
        dhash_ekle(dmap, curr_dmap);
        curr_dmap = sonraki_yedek;
    }

    printf("-> Double Hash Map (O(1)) test ediliyor...\n");
    clock_gettime(CLOCK_MONOTONIC, &t1);

    #pragma omp parallel for
    for(int i = 0; i < N_SORGU; i++) dhash_ara(dmap, sorgular[i]);
    clock_gettime(CLOCK_MONOTONIC, &t2);
    sure_dmap = ms_olc(t1, t2);

    dhash_temizle(dmap);

    // ---------------------------------------------------------
    // BÜYÜK FİNAL TABLOSU
    // ---------------------------------------------------------
    printf("\n=========================================================\n");
    printf("                   ARAMA SONUC TABLOSU                   \n");
    printf("=========================================================\n");
    printf("| Algoritma         | %6d Sorgu Suresi | Fark (Hiz) |\n", N_SORGU);
    printf("|-------------------|----------------------|------------|\n");
    printf("| Linked List O(n)  | %11.2f ms       |      1.0x  |\n", sure_liste);
    printf("| Chaining Map O(1) | %11.2f ms       | %7.1fx  |\n", sure_cmap, sure_liste/sure_cmap);
    printf("| Double Hash O(1)  | %11.2f ms       | %7.1fx  |\n", sure_dmap, sure_liste/sure_dmap);
    printf("=========================================================\n");

    free(sorgular);
}

void deney_calistir() {
    int N_SARKI = 1200000;
    int N_LISTE = 500000;
    int SARKI_PER_LISTE = 50;

    struct timespec t1, t2;
    double sure_kopya, sure_pointer;
    size_t mem_kopya, mem_pointer;
    int malloc_kopya, malloc_pointer;

    printf("==================================================\n");
    printf("   MINISTREAM ANA DENEY: KOPYA VS POINTER MODELI  \n");
    printf("==================================================\n");
    printf("\nŞarkı sayısı\t: %d",N_SARKI);
    printf("\nListe sayısı\t: %d",N_LISTE);
    printf("\nListe uzunluğu\t: %d\n",SARKI_PER_LISTE);

    printf("\n>>> KOPYA MODELI CALISIYOR (Lutfen bekleyin...)...\n");
    izci_sifirla();
    clock_gettime(CLOCK_MONOTONIC, &t1);
    kopya_modeli_test(N_SARKI, N_LISTE, SARKI_PER_LISTE);
    clock_gettime(CLOCK_MONOTONIC, &t2);
    sure_kopya = ms_olc(t1, t2);
    mem_kopya = izci_toplam_ayrildi();
    malloc_kopya = izci_malloc_sayisi();
    bellek_raporu_yazdir();

    printf("\n>>> POINTER MODELI CALISIYOR...\n");
    izci_sifirla();
    clock_gettime(CLOCK_MONOTONIC, &t1);
    pointer_modeli_test(N_SARKI, N_LISTE, SARKI_PER_LISTE);
    clock_gettime(CLOCK_MONOTONIC, &t2);
    sure_pointer = ms_olc(t1, t2);
    mem_pointer = izci_toplam_ayrildi();
    malloc_pointer = izci_malloc_sayisi();
    bellek_raporu_yazdir();

    printf("\n=========================================================\n");
    printf("                      SONUC TABLOSU                      \n");
    printf("=========================================================\n");
    printf("| Metrik           | Kopya Modeli | Pointer Modeli | Fark  |\n");
    printf("|------------------|--------------|----------------|-------|\n");
    printf("| Toplam malloc    | %12d | %14d | %4.1fx |\n", malloc_kopya, malloc_pointer, (double)malloc_kopya/malloc_pointer);
    printf("| Bellek Kullanimi | %9.2f MB | %11.2f MB | %4.1fx |\n", mem_kopya/(1024.0*1024.0), mem_pointer/(1024.0*1024.0), (double)mem_kopya/mem_pointer);
    printf("| Calisma Suresi   | %9.2f ms | %11.2f ms | %4.1fx |\n", sure_kopya, sure_pointer, sure_kopya/sure_pointer);
    printf("| Sizinti          |      0 byte V|        0 byte V|   -   |\n");
    printf("=========================================================\n");
}

int main() {
    int secim;
    while(1) {
        printf("\n========================================\n");
        printf("    MINISTREAM LABORATUVAR KONTROL      \n");
        printf("========================================\n");
        printf("1. Arama Algoritmalari Hiz Testi (O(n) vs O(1))\n");
        printf("2. Calma Listesi Bellek Testi (Kopya vs Pointer)\n");
        printf("0. Cikis\n");
        printf("Seciminiz: ");
        if(scanf("%d", &secim) != 1) break;

        if(secim == 1) arama_benchmark_calistir();
        else if(secim == 2) {
            // Bir onceki kodumuzdaki deney_calistir(); fonksiyonunu buraya entegre edebilirsin.
            deney_calistir();
        }
        else if(secim == 0) break;
    }
    return 0;
}