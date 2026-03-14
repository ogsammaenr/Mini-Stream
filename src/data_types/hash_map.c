#include "hash_map.h"
#include "../bellek_izci.h"
#include <stdlib.h>
#include <stdio.h>

// 1. Boş bir tablo oluştur [cite: 866-869]
HashMap* hashmap_olustur(void) {
    HashMap* map = (HashMap*)izlenen_malloc(sizeof(HashMap));
    for (int i = 0; i < TABLO_BOYUTU; i++) {
        map->kovalar[i] = NULL;
    }
    return map;
}

// 2. Basit Modulo Hash [cite: 856-858]
int hash_fonk(int id) {
    return id % TABLO_BOYUTU;
}

// 3. Chaining ile Ekleme [cite: 871-876]
void hashmap_ekle(HashMap* map, Sarki* sarki) {
    int idx = hash_fonk(sarki->id);
    
    // Yeni şarkıyı ilgili kovanın başına ekle (LIFO mantığı)
    sarki->sonraki = map->kovalar[idx];
    map->kovalar[idx] = sarki;
}

// 4. Arama Operasyonu [cite: 859-863]
Sarki* sarki_ara_map(HashMap* map, int id) {
    int idx = hash_fonk(id);
    Sarki* curr = map->kovalar[idx];

    // Sadece ilgili kovanın içindeki Linked List'i tara
    while (curr != NULL) {
        if (curr->id == id) return curr;
        curr = curr->sonraki;
    }
    return NULL;
}

HashMap* veri_uret_map(int n) {
    HashMap* map = hashmap_olustur();
    for (int i = 0; i < n; i++) {
        char baslik[32];
        sprintf(baslik, "Test Sarki %d", i);
        Sarki* s = sarki_olustur(i, baslik, "Sanatci", "Album", 200);
        if (s) hashmap_ekle(map, s);
    }
    return map;
}

// Hash Map'i ve içindeki tüm şarkıları temizle 
void hashmap_temizle(HashMap* map) {
    if (!map) return;
    for (int i = 0; i < TABLO_BOYUTU; i++) {
        Sarki* curr = map->kovalar[i];
        while (curr) {
            Sarki* sonraki = curr->sonraki;
            izlened_free(curr, sizeof(Sarki));
            curr = sonraki;
        }
    }
    izlened_free(map, sizeof(HashMap));
}