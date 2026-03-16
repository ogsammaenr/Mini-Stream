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

// Belirli bir aralıkta rastgele ondalıklı sayı üretici
double rastgele_ondalik(double min, double max) {
    return min + ((double)rand() / RAND_MAX) * (max - min);
}

// ==========================================
// KELİME HAVUZLARI (DİNAMİK İSİM ÜRETİMİ İÇİN)
// ==========================================
const char* adjectives[] = {
    "Midnight", "Neon", "Dark", "Silent", "Electric", "Lost", "Golden", "Crimson", "Fading", "Endless",
    "Cosmic", "Lunar", "Solar", "Hidden", "Broken", "Falling", "Rising", "Frozen", "Burning", "Dancing",
    "Velvet", "Crystal", "Silver", "Shadowy", "Radiant", "Mystic", "Distant", "Echoing", "Blinding", "Shattered",
    "Whispering", "Thundering", "Bleeding", "Floating", "Drifting", "Rusty", "Astral", "Quantum", "Virtual", "Lucid",
    "Sacred", "Cursed", "Twisted", "Parallel", "Infinite", "Wandering", "Ethereal", "Savage", "Gentle", "Bitter",
    "Toxic", "Liquid", "Dense", "Vapor", "Cyber", "Sonic", "Phantom", "Heavy", "Light", "Blind",
    "Deaf", "Muted", "Screaming", "Weeping", "Laughing", "Dying", "Living", "Immortal", "Mortal", "Divine",
    "Unholy", "Blessed", "Doomed", "Forgotten", "Remembered", "Secret", "Public", "Royal", "Peasant", "Urban",
    "Rural", "Wild", "Tame", "Angry", "Calm", "Stormy", "Peaceful", "Cold", "Hot", "Warm",
    "Freezing", "Boiling", "Hollow", "Fragile", "Tough", "Rough", "Smooth", "Sharp", "Dull", "Hazy",
    "Static", "Kinetic", "Obsidian", "Neon-Soaked", "Gilded", "Synthetic", "Aetheric", "Submerged", "Fractured", "Volatile",
    "Primal", "Mechanical", "Ethereal", "Chromatic", "Vivid", "Pale", "Infinite", "Terminal", "Sullen", "Resonant",
    "Glimmering", "Feral", "Industrial", "Velvet", "Metallic", "Organic", "Digital", "Analog", "Spectral", "Monochrome",
    "Gothic", "Retro", "Obscure", "Fleeting", "Ancient", "Modern", "Brutal", "Elegant", "Solitary", "Crowded",
    "Faded", "Glowing", "Invisible", "Vibrant", "Restless", "Manic", "Serene", "Atomic", "Molecular", "Cosmic"
};

const char* nouns[] = {
    "Dreams", "City", "Shadows", "Waves", "Echoes", "Light", "Memories", "Horizon", "Fire", "Storm",
    "Vibes", "Nights", "Days", "Skies", "Oceans", "Hearts", "Stars", "Winds", "Enigma", "Rain",
    "Streets", "Melodies", "Tears", "Sunsets", "Sunrises", "Galaxy", "Planet", "Forest", "Desert", "River",
    "Mountain", "Silence", "Chaos", "Harmony", "Symphony", "Rhythm", "Beat", "Pulse", "Reality", "Fantasy",
    "Nightmare", "Ghost", "Spirit", "Soul", "Machine", "Matrix", "Future", "Past", "Illusion", "Paradox",
    "Kingdom", "Empire", "Village", "Town", "Road", "Highway", "Alley", "Sea", "Lake", "Pond",
    "Puddle", "Drop", "Smile", "Frown", "Voice", "Whisper", "Scream", "Shout", "Song", "Tune",
    "Chord", "Note", "Bass", "Treble", "Synth", "Guitar", "Drum", "Piano", "Violin", "Cello",
    "Flute", "Horn", "Trumpet", "Brass", "Wood", "Metal", "Plastic", "Glass", "Mirror", "Window",
    "Door", "Wall", "Floor", "Ceiling", "Roof", "Space", "Void", "Abyss", "Chasm", "Canyon",
    "Circuit", "Signal", "Fragment", "Engine", "Tower", "Temple", "Cortex", "Anomaly", "Threshold", "Avenue",
    "Harbor", "Sanctuary", "Monolith", "Archive", "Protocol", "Network", "Gravity", "Orbit", "Satellite", "Comet",
    "Blueprint", "Artifact", "Serum", "Vortex", "Canyon", "Glacier", "Summit", "Valley", "Labyrinth", "Corridor",
    "Canvas", "Statue", "Relic", "Engine", "Turbine", "Prophet", "Nomad", "Outcast", "Citizen", "Pilot",
    "Radio", "Radar", "Compass", "Beacon", "Anchor", "Chain", "Bridge", "Talon", "Feather", "Bone"
};

const char* artist_pool[] = {
    "The Weekends", "Neon Eclipse", "Luna Sparks", "DJ Apollo", "Crimson Flow", "Echo Valley", "Solaris", "Nova Rhythm", "The Midnight", "Silver Keys",
    "Arctic Monkeys", "Daft Punk", "Synthwave Boys", "Lofi Girl", "Rockstar 99", "Velvet Underground", "Cosmic Drifters", "The Quiet Room", "Electric Giants", "Midnight Riders",
    "Ghost Frequency", "Parallel Minds", "The Analog Kids", "Digital Ghosts", "Solar Flare", "Lunar Tides", "Crystal Castles", "The Shattered", "Floating Points", "Rusty Machines",
    "Astral Projection", "Quantum Leap", "Virtual Riot", "Lucid Dreamers", "Sacred Geometry", "The Cursed", "Twisted Sister", "Infinite Loop", "Wandering Souls", "Ethereal Voices",
    "Savage Garden", "Gentle Giant", "Bitter Sweet", "The Matrix", "Future Sound", "Past Lives", "Cybernetic", "The Holograms", "Neon Genesis", "Retro Wave",
    "DJ Horizon", "The Soundscapes", "Vaporwave Surfers", "Synth City", "Neon Knights", "Dark Matter", "Supernova", "Black Hole", "Event Horizon", "Time Travelers",
    "Space Cowboys", "Galactic Groove", "The Asteroids", "Meteor Shower", "Comet Tail", "Starlight", "Moonbeam", "Sunray", "Eclipse", "Phase",
    "Frequency", "Amplitude", "Wavelength", "Resonance", "Harmonic", "Melodic", "Rhythmic", "Acoustic", "Magnetic", "Kinetic",
    "Static", "Dynamic", "Fluid", "Gas", "Plasma", "Ether", "Zero Gravity", "Deep Core", "Mantle", "Tectonic",
    "The Outliers", "Silicon Valley Boys", "Carbon Copy", "Atomic Bloom", "Neon Samurai", "Iron Lotus", "The Glitch Mob", "Binary Solo", "Omega Point", "Neural Link",
    "Frequency Shifters", "Terminal Velocity", "The Algorithm", "Paper Planes", "Static Age", "Marble Statues", "Golden Era", "Desert Rose", "Midnight Express", "Night Crawler",
    "The Architect", "Siren Call", "Electric Dreams", "Radio Silence", "Voodoo Logic", "Primal Screamers", "The Seers", "Hidden Figures", "Analog Dreams", "Digital Nomads",
    "The Last Byte", "Vector Fields", "Monochrome Skies", "Velvet Thunder", "The Alchemists", "Liquid Gold", "Solar Winds", "Plasma Burn", "The Void Walkers", "Iron & Wine",
    "Deep Space Nine", "The Pulsars", "Nebula Seven", "Star Map", "The Cartographers", "Echo Location", "Sound Theory", "The Method", "Abstract Art", "Urban Legends"
};

// Dizilerin eleman sayılarını hesapla
#define ADJ_COUNT (sizeof(adjectives)/sizeof(adjectives[0]))
#define NOUN_COUNT (sizeof(nouns)/sizeof(nouns[0]))
#define ARTIST_COUNT (sizeof(artist_pool)/sizeof(artist_pool[0]))


int main(int argc, char *argv[]) {
    // Rastgelelik çekirdeğini başlat
    srand(time(NULL));

    // Varsayılan boyut 1 Milyon satır
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
    printf("   MINISTREAM GERCEKCI VERI JENERATORU BASLATILDI\n");
    printf("==================================================\n");
    printf("Uretilecek Satir Sayisi : %d\n", SATIR_SAYISI);
    printf("Hedef Dosya             : %s\n", dosya_adi);
    printf("Lutfen bekleyin...\n");

    clock_t baslangic = clock();

    // CSV Başlığı (Header)
    fprintf(file, "id,name,album,album_id,artists,artist_ids,track_number,disc_number,explicit,danceability,energy,key,loudness,mode,speechiness,acousticness,instrumentalness,liveness,valence,tempo,duration_ms,time_signature,year,release_date\n");

    char id[23], album_id[23], artist_id[23];
    char track_name[64];
    char album_name[64];
    
    // Satırları Üret ve Diske Yaz
    for (int i = 1; i <= SATIR_SAYISI; i++) {
        rastgele_id_uret(id);
        rastgele_id_uret(album_id);
        rastgele_id_uret(artist_id);

        // --- YENİ: Gerçekçi İsim Üretimi ---
        // Sıfat ve isimi birleştirerek havalı bir şarkı ismi oluştur ("Neon Echoes" vb.)
        sprintf(track_name, "%s %s", adjectives[rand() % ADJ_COUNT], nouns[rand() % NOUN_COUNT]);
        // Albüm ismini de aynı mantıkla üret
        sprintf(album_name, "%s %s", adjectives[rand() % ADJ_COUNT], nouns[rand() % NOUN_COUNT]);
        // Havuzdan rastgele bir sanatçı seç
        const char* artist = artist_pool[rand() % ARTIST_COUNT];

        int year = 1970 + (rand() % 54); // 1970 - 2023 arası
        int month = 1 + (rand() % 12);
        int day = 1 + (rand() % 28);
        
        int duration = 150000 + (rand() % 150000); // 150sn ile 300sn arası
        int track_num = 1 + (rand() % 15);
        int explicit_flag = rand() % 2; // 0 veya 1

        // Veriyi CSV formatında bas (Artık Track_X yerine string basıyoruz)
        fprintf(file, 
            "%s,%s,%s,%s,['%s'],['%s'],%d,1,%s,%.3f,%.3f,%d,%.3f,%d,%.4f,%.4f,%.6f,%.3f,%.3f,%.3f,%d,4.0,%d,%d-%02d-%02d\n",
            id, track_name, album_name, album_id, artist, artist_id, track_num, 
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

        if (i % 500000 == 0) {
            printf("... %d satir uretildi.\n", i);
        }
    }

    fclose(file);
    clock_t bitis = clock();
    double sure = (double)(bitis - baslangic) / CLOCKS_PER_SEC;

    printf("\n>>> BASARILI! %d gercekci satir veri %.2f saniyede uretildi.\n", SATIR_SAYISI, sure);
    printf(">>> Dosya boyutu yaklasik: %.1f MB\n", (SATIR_SAYISI * 250.0) / (1024.0 * 1024.0));
    
    return 0;
}