#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Spotify tarzı rastgele 22 haneli ID üretici
void rastgele_id_uret(char *str) {
    const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < 22; i++) {
        int key = rand() % (int)(sizeof(charset) - 1);
        str[i] = charset[key];
    }
    str[22] = '\0';
}

// Belirli bir aralıkta rastgele ondalıklı sayı üretici (Örn: danceability için 0.0 ile 1.0 arası)
double rastgele_ondalik(double min, double max) {
    return min + ((double)rand() / RAND_MAX) * (max - min);
}

int main(int argc, char *argv[]) {
    // Varsayılan boyut 1 Milyon satır, ancak dışarıdan parametre girilirse onu al
    int SATIR_SAYISI = 1000000;
    if (argc > 1) {
        SATIR_SAYISI = atoi(argv[1]);
    }

    const char* dosya_adi = "data/generated_tracks.csv";
    FILE *file = fopen(dosya_adi, "w");
    if (!file) {
        printf("HATA: %s dosyasi olusturulamadi. 'data' klasorunun var oldugundan emin olun.\n", dosya_adi);
        return 1;
    }

    printf("==================================================\n");
    printf("   MINISTREAM VERI JENERATORU BASLATILDI\n");
    printf("==================================================\n");
    printf("Uretilecek Satir Sayisi : %d\n", SATIR_SAYISI);
    printf("Hedef Dosya             : %s\n", dosya_adi);
    printf("Lutfen bekleyin...\n");

    // Zaman ölçümüne başla
    clock_t baslangic = clock();

    // CSV Başlığı (Header)
    fprintf(file, "id,name,album,album_id,artists,artist_ids,track_number,disc_number,explicit,danceability,energy,key,loudness,mode,speechiness,acousticness,instrumentalness,liveness,valence,tempo,duration_ms,time_signature,year,release_date\n");

    char id[23], album_id[23], artist_id[23];
    
    // Satırları Üret ve Diske Yaz
    for (int i = 1; i <= SATIR_SAYISI; i++) {
        rastgele_id_uret(id);
        rastgele_id_uret(album_id);
        rastgele_id_uret(artist_id);

        int year = 1970 + (rand() % 54); // 1970 - 2023 arası
        int month = 1 + (rand() % 12);
        int day = 1 + (rand() % 28);
        
        int duration = 150000 + (rand() % 150000); // 150sn ile 300sn arası
        int track_num = 1 + (rand() % 15);
        int explicit_flag = rand() % 2; // 0 veya 1

        // Veriyi CSV formatında bas
        fprintf(file, 
            "%s,Track_%d,Album_%d,%s,['Artist_%d'],['%s'],%d,1,%s,%.3f,%.3f,%d,%.3f,%d,%.4f,%.4f,%.6f,%.3f,%.3f,%.3f,%d,4.0,%d,%d-%02d-%02d\n",
            id, i, i, album_id, i, artist_id, track_num, 
            explicit_flag ? "True" : "False",
            rastgele_ondalik(0.1, 0.9), // danceability
            rastgele_ondalik(0.4, 0.99), // energy
            rand() % 12, // key
            rastgele_ondalik(-15.0, -2.0), // loudness
            rand() % 2, // mode
            rastgele_ondalik(0.01, 0.5), // speechiness
            rastgele_ondalik(0.001, 0.8), // acousticness
            rastgele_ondalik(0.0, 0.1), // instrumentalness
            rastgele_ondalik(0.05, 0.5), // liveness
            rastgele_ondalik(0.2, 0.9), // valence
            rastgele_ondalik(80.0, 180.0), // tempo
            duration, year, year, month, day
        );

        // Her 500.000 satırda bir bilgi ver
        if (i % 500000 == 0) {
            printf("... %d satir uretildi.\n", i);
        }
    }

    fclose(file);
    clock_t bitis = clock();
    double sure = (double)(bitis - baslangic) / CLOCKS_PER_SEC;

    printf("\n>>> BASARILI! %d satir veri %.2f saniyede uretildi.\n", SATIR_SAYISI, sure);
    printf(">>> Dosya boyutu yaklasik: %.1f MB\n", (SATIR_SAYISI * 250.0) / (1024.0 * 1024.0));
    
    return 0;
}