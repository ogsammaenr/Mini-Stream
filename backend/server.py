import http.server
import socketserver
import json
import urllib.parse
import ctypes
import os
import csv

PORT = 3030

# =========================================================
# 1. C KÜTÜPHANESİNİ YÜKLEME (ctypes KÖPRÜSÜ)
# =========================================================
LIB_PATH = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'ministream.so'))

# Global RAM İşaretçileri (Pointer Tutucular)
C_PLAYLISTS = {}  # { liste_id : c_void_p (RAM Adresi) }
C_SONGS = {}      # { sarki_id : c_void_p (RAM Adresi) }

try:
    ministream_lib = ctypes.CDLL(LIB_PATH)
    
    ministream_lib.deney_json.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int]
    ministream_lib.deney_json.restype = ctypes.c_char_p
    
    ministream_lib.arama_json.argtypes = [ctypes.c_int, ctypes.c_int]
    ministream_lib.arama_json.restype = ctypes.c_char_p

    # CalmaListesi* liste_olustur(int id, const char* isim)
    ministream_lib.liste_olustur.argtypes = [ctypes.c_int, ctypes.c_char_p]
    ministream_lib.liste_olustur.restype = ctypes.c_void_p

    # Sarki* sarki_olustur(int id, const char* baslik, const char* sanatci, const char* album, int sure)
    ministream_lib.sarki_olustur.argtypes = [ctypes.c_int, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_int]
    ministream_lib.sarki_olustur.restype = ctypes.c_void_p

    # int liste_sarki_ekle(CalmaListesi* liste, Sarki* sarki)
    ministream_lib.liste_sarki_ekle.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
    ministream_lib.liste_sarki_ekle.restype = ctypes.c_int

    # void bellek_raporu_yazdir(void)
    ministream_lib.bellek_raporu_yazdir.argtypes = []
    ministream_lib.bellek_raporu_yazdir.restype = None

    # void veritabani_baslat()
    ministream_lib.veritabani_baslat.argtypes = []
    ministream_lib.veritabani_baslat.restype = None

    # const char* c_motoru_kismi_arama(const char* aranacak_kelime)
    ministream_lib.c_motoru_kismi_arama.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int]
    ministream_lib.c_motoru_kismi_arama.restype = ctypes.c_char_p

    # const char* c_motoru_sayfa_getir(int sayfa_no, int sayfa_boyutu)
    ministream_lib.c_motoru_sayfa_getir.argtypes = [ctypes.c_int, ctypes.c_int]
    ministream_lib.c_motoru_sayfa_getir.restype = ctypes.c_char_p

    # void* c_motoru_sarki_pointer_getir(int sarki_id)
    ministream_lib.c_motoru_sarki_pointer_getir.argtypes = [ctypes.c_int]
    ministream_lib.c_motoru_sarki_pointer_getir.restype = ctypes.c_void_p


    print(f"✅ C Kütüphanesi ve Struct köprüleri başarıyla yüklendi!")
except Exception as e:
    print(f"❌ HATA: {e}")
    ministream_lib = None

class FullStackHandler(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        parsed_path = urllib.parse.urlparse(self.path)
        path = parsed_path.path
        query = urllib.parse.parse_qs(parsed_path.query)

        # =========================================================
        # API 1: BELLEK (RAM) TESTİ
        # =========================================================
        if path == '/api/benchmark':
            songs = int(query.get('songs', ['10000'])[0])
            lists = int(query.get('lists', ['5000'])[0])
            per_list = int(query.get('per_list', ['50'])[0])

            self.send_response(200)
            self.send_header('Content-type', 'application/json')
            self.end_headers()

            if ministream_lib:
                result_bytes = ministream_lib.deney_json(songs, lists, per_list)
                self.wfile.write(result_bytes.decode('utf-8').encode('utf-8'))
            else:
                self.wfile.write(json.dumps({"error": "C Kütüphanesi yüklenemedi!"}).encode('utf-8'))
            return

        # =========================================================
        # API 2: ALGORİTMA HIZI (CPU) TESTİ
        # =========================================================
        elif path == '/api/search':
            songs = int(query.get('songs', ['10000'])[0])
            queries = int(query.get('queries', ['1000'])[0])

            self.send_response(200)
            self.send_header('Content-type', 'application/json')
            self.end_headers()

            if ministream_lib:
                result_bytes = ministream_lib.arama_json(songs, queries)
                self.wfile.write(result_bytes.decode('utf-8').encode('utf-8'))
            else:
                self.wfile.write(json.dumps({"error": "C Kütüphanesi yüklenemedi!"}).encode('utf-8'))
            return

        
        # =========================================================
        # API 3: ŞARKILARI GETİR (Artık C RAM'inden Sayfalı Geliyor!)
        # =========================================================
        elif path == '/api/songs':
            page = int(query.get('page', ['1'])[0])
            limit = int(query.get('limit', ['100'])[0])
            
            if ministream_lib:
                # Diske (CSV'ye) hiç gitmeden doğrudan C'nin RAM indexinden sayfayı çek
                json_str_bytes = ministream_lib.c_motoru_sayfa_getir(page, limit)
                result_str = json_str_bytes.decode('utf-8')
                
                self.send_response(200)
                self.send_header('Content-type', 'application/json')
                self.end_headers()
                self.wfile.write(result_str.encode('utf-8'))
            else:
                self.send_response(500)
                self.end_headers()
                self.wfile.write(b'{"error": "C Kutuphanesi Yuklenemedi!"}')
            return

        # =========================================================
        # API 4: C RAM'İNDE ÇALMA LİSTESİ OLUŞTUR
        # =========================================================
        elif path == '/api/playlist/create':
            p_id = int(query.get('id', ['0'])[0])
            name = urllib.parse.unquote(query.get('name', ['Bilinmeyen'])[0])
            
            if ministream_lib:
                c_name = name.encode('utf-8')
                ptr = ministream_lib.liste_olustur(p_id, c_name)
                C_PLAYLISTS[p_id] = ptr 
                
                print(f"\n[C MOTORU] 📀 Yeni CalmaListesi Struct'ı RAM'de ayrıldı!")
                print(f"          -> Liste Adı: {name} | RAM Adresi: {hex(ptr if ptr else 0)}")
                ministream_lib.bellek_raporu_yazdir()

            self.send_response(200)
            self.send_header('Content-type', 'application/json')
            self.end_headers()
            self.wfile.write(json.dumps({"status": "success"}).encode('utf-8'))
            return

        # =========================================================
        # API 5: C RAM'İNDEKİ LİSTEYE GERÇEK POINTER İLE ŞARKI EKLE
        # =========================================================
        elif path == '/api/playlist/add_song':
            pl_id = int(query.get('pl_id', ['0'])[0])
            song_id = int(query.get('song_id', ['0'])[0])
            title = urllib.parse.unquote(query.get('title', [''])[0])

            if ministream_lib and pl_id in C_PLAYLISTS:
                pl_ptr = C_PLAYLISTS[pl_id]
                print(f"\n[C MOTORU] 🎵 '{title}' şarkısı listeye ekleniyor...")
                
                # --- ESKİ MALLOC YAPAN KODU SİLDİK ---
                # ARTIK VERİYİ YENİDEN YARATMIYORUZ, IN-MEMORY VERİTABANINDAN (RAM'DEN) ADRESİNİ ÇEKİYORUZ!
                s_ptr = ministream_lib.c_motoru_sarki_pointer_getir(song_id)
                
                if s_ptr:
                    # RAM'deki o sabit adresi,çalma listesinin sarki işaretçileri arasına ekle (Zero-Copy)
                    ministream_lib.liste_sarki_ekle(pl_ptr, s_ptr)
                    print(f"          -> Şarkı In-Memory DB'den (Adres: {hex(s_ptr)}) alındı.")
                    print(f"          -> Şarkı {hex(pl_ptr)} adresindeki listeye bağlandı.")
                else:
                    print("          -> ❌ HATA: Şarkı RAM'de bulunamadı!")
                
                ministream_lib.bellek_raporu_yazdir()

            self.send_response(200)
            self.send_header('Content-type', 'application/json')
            self.end_headers()
            self.wfile.write(json.dumps({"status": "success"}).encode('utf-8'))
            return


        elif path == '/api/c_search':
            q = urllib.parse.unquote(query.get('q', [''])[0])
            page = int(query.get('page', ['1'])[0])
            limit = int(query.get('limit', ['100'])[0])
            
            if ministream_lib and q:
                c_query = q.encode('utf-8')
                
                # Parametreleri C motoruna paslıyoruz
                json_str_bytes = ministream_lib.c_motoru_kismi_arama(c_query, page, limit)
                result_str = json_str_bytes.decode('utf-8')
                
                self.send_response(200)
                self.send_header('Content-type', 'application/json')
                self.end_headers()
                self.wfile.write(result_str.encode('utf-8'))
                return

        # =========================================================
        # 6. YÖNLENDİRME (ROUTING): HTML VE DOSYALAR
        # =========================================================
        elif path == '/':
            self.path = '/dashboard/index.html'
        elif path.startswith('/dashboard/'):
            self.path = path

        super().do_GET()


socketserver.TCPServer.allow_reuse_address = True
os.chdir(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

with socketserver.TCPServer(("", PORT), FullStackHandler) as httpd:
    print("="*50)
    print(f"🚀 MİNİSTREAM FULL-STACK SUNUCU AKTİF!")
    print(f"⚡ GERÇEK C MOTORU DEVREDE (ctypes köprüsü kuruldu)")
    print(f"👉 Tarayıcıda açın: http://localhost:{PORT}")
    print("="*50)

    if ministream_lib:
        ministream_lib.veritabani_baslat()

    print("Sunucuyu kapatmak için CTRL+C tuşlarına basın...\n")
    httpd.serve_forever()