#include "double_hash.h"
#include "../bellek_izci.h"
#include <stdlib.h>
#include <stdio.h>

// 1. Birinci Hash (İlk denenecek kapı)
static int hash1(int id) {
    return id % DHASH_TABLO_BOYUTU;
}

// 2. İkinci Hash (Kapı doluysa kaç adım zıplanacak?)
static int hash2(int id) {
    int prime = 1999993; 
    return prime - (id % prime); 
}

DoubleHashMap* dhash_olustur(void) {
    DoubleHashMap* map = (DoubleHashMap*)izlenen_malloc(sizeof(DoubleHashMap));
    if (map) {
        for (int i = 0; i < DHASH_TABLO_BOYUTU; i++) {
            map->kovalar[i] = NULL;
        }
    }
    return map;
}

void dhash_ekle(DoubleHashMap* map, Sarki* sarki) {
    int idx = hash1(sarki->id);
    int adim = hash2(sarki->id);

    // Boş yer bulana kadar zıpla
    while (map->kovalar[idx] != NULL) {
        idx = (idx + adim) % DHASH_TABLO_BOYUTU;
    }
    map->kovalar[idx] = sarki;
}

Sarki* dhash_ara(DoubleHashMap* map, int id) {
    int idx = hash1(id);
    int adim = hash2(id);

    while (map->kovalar[idx] != NULL) {
        if (map->kovalar[idx]->id == id) {
            return map->kovalar[idx]; // Bulundu!
        }
        idx = (idx + adim) % DHASH_TABLO_BOYUTU;
    }
    return NULL; 
}

DoubleHashMap* veri_uret_dhash(int n) {
    DoubleHashMap* map = dhash_olustur();
    for (int i = 0; i < n; i++) {
        char baslik[32];
        sprintf(baslik, "Test Sarki %d", i);
        Sarki* s = sarki_olustur(i, baslik, "Sanatci", "Album", 200);
        if (s) dhash_ekle(map, s);
    }
    return map;
}

void dhash_temizle(DoubleHashMap* map) {
    if (!map) return;
    for (int i = 0; i < DHASH_TABLO_BOYUTU; i++) {
        if (map->kovalar[i] != NULL) {
            izlened_free(map->kovalar[i], sizeof(Sarki));
        }
    }
    izlened_free(map, sizeof(DoubleHashMap));
}