#ifndef DOUBLE_HASH_H
#define DOUBLE_HASH_H

#include "../ministream.h"

// 2 Milyona en yakın asal sayı
#define DHASH_TABLO_BOYUTU 2000003 

typedef struct {
    // Sadece devasa bir pointer dizisi, 'sonraki' (linked list) mantığı yok!
    Sarki* kovalar[DHASH_TABLO_BOYUTU];
} DoubleHashMap;

DoubleHashMap* dhash_olustur(void);
void dhash_ekle(DoubleHashMap* map, Sarki* sarki);
Sarki* dhash_ara(DoubleHashMap* map, int id);
DoubleHashMap* veri_uret_dhash(int n);
void dhash_temizle(DoubleHashMap* map);

#endif