/* test/test_temel.c */
#include <stdio.h>
#include <string.h>
#include "../src/ministream.h"
#include "../src/bellek_izci.h"

/* Geçti / Kaldı yazdıran yardımcı makro [cite: 597-603] */
#define TEST(isim, kosul) \
do { \
    if (kosul) { printf(" ✔ %s\n", isim); } \
    else       { printf(" ✖ %s - BASARISIZ\n", isim); } \
} while(0)

/* Test 1: Şarkı oluşturma [cite: 604-625] */
void test_sarki_olustur() {
    printf("\n[TEST] sarki_olustur\n");
    izci_sifirla();
    
    Sarki* s = sarki_olustur(1, "Bohemian Rhapsody", "Queen", "A Night at the Opera", 354);
    
    TEST("sarki NULL degil", s != NULL);
    TEST("id dogru", s->id == 1);
    TEST("baslik dogru", strcmp(s->baslik, "Bohemian Rhapsody") == 0);
    TEST("ref_sayisi baslangicta 0", s->ref_sayisi == 0);
    TEST("1 malloc yapildi", izci_malloc_sayisi() == 1);
    
    izlened_free(s, sizeof(Sarki));
    TEST("SIZINTI yok", aktif_bellek() == 0);
}

/* Test 2: Listeye şarkı ekleme [cite: 626-648] */
void test_liste_sarki_ekle() {
    printf("\n[TEST] liste_sarki_ekle\n");
    izci_sifirla();
    
    CalmaListesi* liste = liste_olustur(1, "Favoriler");
    Sarki* s1 = sarki_olustur(1, "Track A", "Sanatci", "Album", 200);
    Sarki* s2 = sarki_olustur(2, "Track B", "Sanatci", "Album", 210);
    
    liste_sarki_ekle(liste, s1);
    liste_sarki_ekle(liste, s2);
    
    TEST("2 sarki eklendi", liste->sarki_sayisi == 2);
    TEST("s1 ref_sayisi artti", s1->ref_sayisi == 1);
    TEST("s2 ref_sayisi artti", s2->ref_sayisi == 1);
    
    /* Adresler aynı mı? Kopya yok, pointer kopyalandı mı? */
    TEST("pointer kopyalandi", liste->sarkilar[0] == s1);
    
    liste_temizle(liste);
    TEST("SIZINTI yok", aktif_bellek() == 0);
}

/* Test 3: ref_sayisi yönetimi [cite: 649-673] */
void test_ref_sayisi() {
    printf("\n[TEST] ref_sayisi\n");
    izci_sifirla();
    
    Sarki* s = sarki_olustur(1, "Shared Track", "Sanatci", "Album", 180);
    CalmaListesi* l1 = liste_olustur(1, "Liste 1");
    CalmaListesi* l2 = liste_olustur(2, "Liste 2");
    
    liste_sarki_ekle(l1, s);
    liste_sarki_ekle(l2, s);
    TEST("2 listede ref_sayisi == 2", s->ref_sayisi == 2);
    
    liste_sarki_cikar(l1, 0);
    TEST("1 cikarinca ref_sayisi == 1", s->ref_sayisi == 1);
    
    /* ref_sayisi > 0 iken silme reddedilmeli */
    int sonuc = sarki_sil(s);
    TEST("ref_sayisi>0 iken silinemez", sonuc == -1);
    
    liste_temizle(l2);
    // Şarkı liste_temizle içinde otomatik silindiği için
    // tekrar s->ref_sayisi okumak veya sarki_sil(s) çağırmak yasaktır!
    
    // Güvenlik için Dangling Pointer'ı NULL yapalım [cite: 1125-1128]
    s = NULL; 
    liste_temizle(l1);
    
    TEST("SIZINTI yok", aktif_bellek() == 0);
}

int main() {
    printf("===== MiniStream Birim Testleri =====\n");
    test_sarki_olustur();
    test_liste_sarki_ekle();
    test_ref_sayisi();
    printf("\n=====================================\n");
    return 0;
}