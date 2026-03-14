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

int main() {
    printf("\n==========================================\n");
    printf("   MINISTREAM BIRIM TEST (UNIT TEST) MODU\n");
    printf("==========================================\n");
    
    test_bellek_izci();
    test_chaining_hashmap();
    test_double_hashmap();
    
    printf("==========================================\n");
    printf("🚀 TUM TESTLER %%100 BASARIYLA GECTI!\n");
    printf("==========================================\n\n");
    
    return 0;
}