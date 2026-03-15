#ifndef BELLEK_IZCI_H
#define BELLEK_IZCI_H

#include <stdlib.h>

typedef struct {
    size_t toplam_ayrildi; // Toplam malloc edilen miktar 
    size_t toplam_serbest;  // Toplam free edilen miktar 
    int malloc_sayisi;      // Kaç kez yer ayrıldı 
    int free_sayisi;        // Kaç kez serbest bırakıldı 
} BellekIzci;

// Proje genelinde malloc/free yerine bunları kullanacağız 
void* izlenen_malloc(size_t boyut);
void izlened_free(void* ptr, size_t boyut);

// Raporlama ve kontrol fonksiyonları 
void izci_sifirla(void);
void bellek_raporu_yazdir(void);
size_t aktif_bellek(void);

// Getterlar (statik değişkene dışarıdan erişim için)
int izci_malloc_sayisi(void);
int izci_free_sayisi(void);
size_t izci_toplam_ayrildi(void);
size_t izci_toplam_serbest(void);

#endif