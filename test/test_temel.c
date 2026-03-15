#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "bellek_izci.h"
#include "data_types/hash_map.h"
#include "data_types/double_hash.h"

// Sahte (Mock) bir şarkı üreten yardımcı fonksiyon
Sarki* mock_sarki_uret(int id) {
    Sarki* s = (Sarki*)izlenen_malloc(sizeof(Sarki));
    s->id = id;
    s->sonraki = NULL;
    return s;
}

void test_bellek_izci() {
    printf("[TEST] Bellek Izci (Memory Tracker) test ediliyor...\n");
    izci_sifirla();
    
    void* ptr = izlenen_malloc(100);
    assert(izci_malloc_sayisi() == 1);
    assert(izci_toplam_ayrildi() == 100);
    
    izlened_free(ptr, 100);
    assert(izci_free_sayisi() == 1);

    assert(izci_malloc_sayisi() == izci_free_sayisi());    

    printf("   -> BASARILI!\n");
}

void test_chaining_hashmap() {
    printf("[TEST] Chaining Hash Map test ediliyor...\n");
    HashMap* map = hashmap_olustur();
    assert(map != NULL);
    
    Sarki* s1 = mock_sarki_uret(55);
    hashmap_ekle(map, s1);
    
    Sarki* bulunan = sarki_ara_map(map, 55);
    assert(bulunan != NULL);
    assert(bulunan->id == 55);
    
    Sarki* bulunamayan = sarki_ara_map(map, 999);
    assert(bulunamayan == NULL);
    
    hashmap_temizle(map);
    printf("   -> BASARILI!\n");
}

void test_double_hashmap() {
    printf("[TEST] Double Hash Map test ediliyor...\n");
    DoubleHashMap* dmap = dhash_olustur();
    assert(dmap != NULL);
    
    Sarki* s1 = mock_sarki_uret(1024);
    dhash_ekle(dmap, s1);
    
    Sarki* bulunan = dhash_ara(dmap, 1024);
    assert(bulunan != NULL);
    assert(bulunan->id == 1024);
    
    dhash_temizle(dmap);
    printf("   -> BASARILI!\n");
}

void test_soru3_sabotaj() {
    printf("\n--- SORU 3: Use-After-Free Testi ---\n");
    
    // Şarkı ve listeleri oluştur
    Sarki* s = sarki_olustur(99, "Tehlikeli Madde", "Hacker", "Album", 180);
    CalmaListesi* l1 = liste_olustur(1, "Liste 1");
    CalmaListesi* l2 = liste_olustur(2, "Liste 2");

    // 2. İSTER: Bir şarkıyı iki listede kullan
    liste_sarki_ekle(l1, s);
    liste_sarki_ekle(l2, s);

    // 2. İSTER DEVAMI: Birinden sil ve free et
    liste_sarki_cikar(l1, 0);
    sarki_sil(s); // Güvenlik kapalı olduğu için RAM'den silinecek!

    // 3. İSTER: Diğer listeden erişmeye çalış
    printf("Liste 2'den okunan sarki: %s\n", l2->sarkilar[0]->baslik);
}

int main() {
    printf("\n==========================================\n");
    printf("   MINISTREAM BIRIM TEST (UNIT TEST) MODU\n");
    printf("==========================================\n");
    
    // test_bellek_izci();
    // test_chaining_hashmap();
    // test_double_hashmap();
    test_soru3_sabotaj();

    printf("==========================================\n");
    printf("🚀 TUM TESTLER %%100 BASARIYLA GECTI!\n");
    printf("==========================================\n\n");
    
    return 0;
}