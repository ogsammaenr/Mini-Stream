#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ministream.h"
#include "bellek_izci.h"
#include <errno.h>

// CSV alanlarını tırnak hassasiyetiyle ayıklar [cite: 293-317]
static void csv_alan_oku(const char* satir, int alan_no, char* hedef, int max_uzunluk) {
    int alan = 0, i = 0, j = 0;
    int tirnak_icinde = 0;

    // Hedef alana kadar ilerle [cite: 298-301]
    while (alan < alan_no && satir[i] != '\0') {
        if (satir[i] == '"') tirnak_icinde = !tirnak_icinde;
        else if (satir[i] == ',' && !tirnak_icinde) alan++;
        i++;
    }

    // Alanı kopyala [cite: 305-316]
    tirnak_icinde = 0;
    while (satir[i] != '\0' && j < max_uzunluk - 1) {
        if (satir[i] == '"') {
            tirnak_icinde = !tirnak_icinde;
        } else {
            if (satir[i] == ',' && !tirnak_icinde) break;
            if (satir[i] == '\n' || satir[i] == '\r') break;
            hedef[j++] = satir[i];
        }
        i++;
    }
    hedef[j] = '\0';
}

Sarki* csv_yukle(const char* dosya_yolu, int limit, int* toplam) {
    FILE* f = fopen(dosya_yolu, "r");
    if (!f) {
        printf("Sistem Hatasi (%s): %s\n", dosya_yolu, strerror(errno));
        return NULL;
    }

    char satir[2048];
    fgets(satir, sizeof(satir), f); // Başlık satırını atla [cite: 322]

    Sarki* bas = NULL;
    *toplam = 0;

    while (fgets(satir, sizeof(satir), f) && *toplam < limit) {
        char s_ad[100], s_alb[100], s_art[100], s_sure[32], s_yil[32];

        // Verileri kolon indekslerine göre çek [cite: 336-356]
        csv_alan_oku(satir, 1, s_ad, 100);    // name
        csv_alan_oku(satir, 2, s_alb, 100);   // album
        csv_alan_oku(satir, 4, s_art, 100);   // artists
        csv_alan_oku(satir, 20, s_sure, 32);  // duration_ms
        csv_alan_oku(satir, 22, s_yil, 32);   // year

        // ministream.c'deki fonksiyonu çağırarak heap'te oluştur [cite: 326]
        Sarki* s = sarki_olustur(*toplam, s_ad, s_art, s_alb, atoi(s_sure) / 1000);
        if (s) {
            s->yil = atoi(s_yil);
            s->sonraki = bas; // Linked list'in başına ekle [cite: 330, 357-358]
            bas = s;
            (*toplam)++;
        }
    }

    fclose(f);
    printf("%d sarki yuklendi.\n", *toplam); 
    return bas;
}