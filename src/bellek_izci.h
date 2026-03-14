#ifndef BELLEK_IZCI_H
#define BELLEK_IZCI_H

#include <stdlib.h>

typedef struct {
    size_t toplam_ayrildi; // Toplam malloc edilen miktar [cite: 455]
    size_t toplam_serbest;  // Toplam free edilen miktar [cite: 457]
    int malloc_sayisi;      // Kaç kez yer ayrıldı [cite: 458]
    int free_sayisi;        // Kaç kez serbest bırakıldı [cite: 459]
} BellekIzci;

// Proje genelinde malloc/free yerine bunları kullanacağız [cite: 462-465, 528]
void* izlenen_malloc(size_t boyut);
void izlened_free(void* ptr, size_t boyut);

// Raporlama ve kontrol fonksiyonları [cite: 466-476]
void izci_sifirla(void);
void bellek_raporu_yazdir(void);
size_t aktif_bellek(void);

// Getterlar (statik değişkene dışarıdan erişim için) [cite: 521-527]
int izci_malloc_sayisi(void);
int izci_free_sayisi(void);
size_t izci_toplam_ayrildi(void);
size_t izci_toplam_serbest(void);

#endif