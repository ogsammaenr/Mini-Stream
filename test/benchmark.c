/* test/benchmark.c */
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "../src/ministream.h"
#include "../src/bellek_izci.h"
#include "../src/data_types/linked_list.h"
#include "../src/data_types/hash_map.h"
#include "../src/data_types/double_hash.h" 

// Milisaniye cinsinden süre hesapla
double ms_olc(struct timespec t1, struct timespec t2) {
    return (t2.tv_sec - t1.tv_sec) * 1000.0 + (t2.tv_nsec - t1.tv_nsec) / 1e6;
}

void benchmark_arama(int n_sarki) {
    struct timespec t1, t2;
    srand(42); // Adil bir yarış için aynı rastgele sayılar

    /* --- 1. Linked list Testi --- */
    izci_sifirla();
    Sarki* bas = veri_uret_liste(n_sarki);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    for (int i = 0; i < 1000; i++) {
        sarki_ara_liste(bas, rand() % n_sarki);
    }
    clock_gettime(CLOCK_MONOTONIC, &t2);
    double liste_ms = ms_olc(t1, t2);
    liste_temizle_hepsi(bas);

    /* --- 2. Chaining Hash Map Testi --- */
    izci_sifirla();
    HashMap* map = veri_uret_map(n_sarki);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    for (int i = 0; i < 1000; i++) {
        sarki_ara_map(map, rand() % n_sarki);
    }
    clock_gettime(CLOCK_MONOTONIC, &t2);
    double map_ms = ms_olc(t1, t2);
    hashmap_temizle(map);

    /* --- 3. Double Hashing Testi --- */
    izci_sifirla();
    DoubleHashMap* dmap = veri_uret_dhash(n_sarki);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    for (int i = 0; i < 1000; i++) {
        dhash_ara(dmap, rand() % n_sarki);
    }
    clock_gettime(CLOCK_MONOTONIC, &t2);
    double dhash_ms = ms_olc(t1, t2);
    dhash_temizle(dmap);

    // Sonuçları yazdır
    printf("| %8d | %10.3f ms | %10.3f ms | %10.3f ms |\n",
           n_sarki, liste_ms, map_ms, dhash_ms);
}

int main() {
    printf("\n=== 3'LU ARAMA BENCHMARK (1000 sorgu) ===\n");
    printf("| N sarki  | LinkedList | Chaining   | DoubleHash |\n");
    printf("|----------|------------|------------|------------|\n");
    
    int boyutlar[] = {100, 1000, 10000, 100000};
    for (int i = 0; i < 4; i++) {
        benchmark_arama(boyutlar[i]);
    }
    printf("=========================================================\n\n");
    
    return 0;
}