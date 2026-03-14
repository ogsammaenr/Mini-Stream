#include <stdio.h>
#include <stdlib.h>
#include "ministream.h"
#include "bellek_izci.h"

// 500.000 kullanıcıyı simüle edelim
Kullanici* kullanicilari_olustur(int adet) {
    printf("[!] %d kullanici icin bellek ayriliyor...\n", adet);
    Kullanici* kullanicilar = (Kullanici*)izlenen_malloc(sizeof(Kullanici) * adet);
    
    for (int i = 0; i < adet; i++) {
        kullanicilar[i].id = i; // PDF'e göre sadece 'id' kullanıyoruz
        sprintf(kullanicilar[i].isim, "User_%d", i);
        kullanicilar[i].liste_sayisi = 20;
        
        // 1. DİKKAT: Artık listeler çift yıldızlı (**) bir pointer. Önce bu dizi için RAM'de yer açmalıyız!
        kullanicilar[i].listeler = (CalmaListesi**)izlenen_malloc(20 * sizeof(CalmaListesi*));
        
        for (int j = 0; j < 20; j++) {
            char liste_adi[50];
            sprintf(liste_adi, "Liste_%d", j);
            // 2. Kendi yazdığımız resmi 'liste_olustur' fonksiyonunu çağırıp adresi diziye koyuyoruz
            kullanicilar[i].listeler[j] = liste_olustur(j, liste_adi);
        }
    }
    return kullanicilar;
}

// Bir listeye sarki ekle
void listeye_sarki_ekle(Kullanici* u, int liste_no, Sarki* s) {
    if (!u || !s) return;
    // 3. Artık u->listeler[liste_no] doğrudan bir (CalmaListesi*) pointer'ı. 
    // Nokta ile uğraşmak yerine doğrudan resmi fonksiyonumuza gönderiyoruz!
    liste_sarki_ekle(u->listeler[liste_no], s);
}