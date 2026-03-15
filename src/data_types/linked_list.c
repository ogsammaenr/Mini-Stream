#include "linked_list.h"
#include "../bellek_izci.h"
#include <stdlib.h>
#include <stdio.h>

// 1. O(n) Karmaşıklığında Doğrusal Arama 
Sarki* sarki_ara_liste(Sarki* bas, int id) {
    Sarki* curr = bas;
    while (curr != NULL) {
        if (curr->id == id) return curr;
        curr = curr->sonraki; // Bulana kadar bir sonrakine geç
    }
    return NULL;
}

// 2. Benchmark için Sahte Veri Üretici 
Sarki* veri_uret_liste(int n) {
    Sarki* bas = NULL;
    for (int i = 0; i < n; i++) {
        // ministream.c'deki sarki_olustur fonksiyonumuzu kullanıyoruz
        char baslik[32];
        sprintf(baslik, "Test Sarki %d", i);
        Sarki* s = sarki_olustur(i, baslik, "Sanatci", "Album", 200);
        
        if (s) {
            s->sonraki = bas; // Başa ekle (LIFO)
            bas = s;
        }
    }
    return bas;
}

// 3. Benchmark Sonrası Temizlik 
void liste_temizle_hepsi(Sarki* bas) {
    Sarki* curr = bas;
    while (curr != NULL) {
        Sarki* silinecek = curr;
        curr = curr->sonraki;
        // Bu şarkılar hiçbir çalma listesine girmediği için doğrudan free edebiliriz
        izlened_free(silinecek, sizeof(Sarki)); 
    }
}