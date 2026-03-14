#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "../ministream.h"

#define TABLO_BOYUTU 1024

typedef struct {
    Sarki* kovalar[TABLO_BOYUTU]; // Her kova bir Linked List başıdır
} HashMap;

// Fonksiyon prototipleri
HashMap* hashmap_olustur(void);
int hash_fonk(int id);
void hashmap_ekle(HashMap* map, Sarki* sarki);
Sarki* sarki_ara_map(HashMap* map, int id);
void hashmap_temizle(HashMap* map);
HashMap* veri_uret_map(int n);
void hashmap_temizle(HashMap* map);

#endif