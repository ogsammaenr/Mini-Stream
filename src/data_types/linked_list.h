#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "../ministream.h"

// Doğrusal arama O(n) 
Sarki* sarki_ara_liste(Sarki* bas, int id);

// Benchmark için n elemanlı liste üretici 
Sarki* veri_uret_liste(int n);

// Benchmark bittikten sonra listeyi temizleyici 
void liste_temizle_hepsi(Sarki* bas);

#endif