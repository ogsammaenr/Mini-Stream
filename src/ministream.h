#ifndef MINISTREAM_H
#define MINISTREAM_H

/* 1. Şarkı Yapısı */
typedef struct Sarki {
    int id;
    char baslik[100];      /* şarkı adı */
    char sanatci[100];     /* sanatçı adı */
    char album[100];       /* albüm adı */
    int sure_sn;           /* süre (saniye) */
    int yil;               /* çıkış yılı */
    int ref_sayisi;        /* kaç çalma listesinde kullanılıyor */
    struct Sarki* sonraki; /* linked list için next pointer */
} Sarki;

/* 2. Çalma Listesi Yapısı */
typedef struct {
    int id;
    char isim[50];
    Sarki** sarkilar;      /* heap'teki pointer dizisi */
    int sarki_sayisi;      /* şu an listede kaç şarkı var */
    int kapasite;          /* dizi kapasitesi (realloc ile büyüyecek) */
} CalmaListesi;

/* 3. Kullanıcı Yapısı */
typedef struct {
    int id;
    char isim[50];
    CalmaListesi** listeler; /* kullanıcının çalma listeleri (pointer dizisi) */
    int liste_sayisi;
} Kullanici;



Sarki* sarki_olustur(int id, const char* baslik, const char* sanatci, const char* album, int sure);
int sarki_sil(Sarki* sarki);
CalmaListesi* liste_olustur(int id, const char* isim);
int liste_sarki_ekle(CalmaListesi* liste, Sarki* sarki);
void liste_sarki_cikar(CalmaListesi* liste, int idx);
void liste_temizle(CalmaListesi* liste);
void kopya_modeli_test(int n_sarki, int n_liste, int sarki_per_liste);
void pointer_modeli_test(int n_sarki, int n_liste, int sarki_per_liste);

#endif