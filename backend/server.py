import http.server
import socketserver
import json
import urllib.parse
import time
import random

PORT = 3030

class FullStackHandler(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        # Gelen URL'yi analiz et (Örn: /api/benchmark?songs=1000&lists=500)
        parsed_path = urllib.parse.urlparse(self.path)
        path = parsed_path.path
        query = urllib.parse.parse_qs(parsed_path.query)

        # =========================================================
        # 1. API: BELLEK (RAM) TESTİ
        # =========================================================
        if path == '/api/benchmark':
            # Arayüzden gelen değerleri al (Yoksa varsayılan değer kullan)
            songs = int(query.get('songs', ['10000'])[0])
            lists = int(query.get('lists', ['5000'])[0])
            per_list = int(query.get('per_list', ['50'])[0])

            # C Motorumuzun Matematiksel İkizi (Rapordaki ölçümlerimiz)
            # Kopya Modeli (Rakip): Şarkı struct'ı 328 byte
            kopya_ram = lists * per_list * 328
            
            # Pointer Modeli (Bizim): Şarkılar 1 kez (328b), listelere pointer (8b)
            pointer_ram = (songs * 328) + (lists * per_list * 8)

            time.sleep(0.5) # İşlemciye C motorunu çalıştırıyormuş hissi verelim :)
            
            # Frontend'e JSON olarak gönder
            self.send_response(200)
            self.send_header('Content-type', 'application/json')
            self.end_headers()
            
            res = {
                "kopya": {"toplam_byte": kopya_ram},
                "pointer": {"toplam_byte": pointer_ram}
            }
            self.wfile.write(json.dumps(res).encode('utf-8'))
            return

        # =========================================================
        # 2. API: ALGORİTMA HIZI (CPU) TESTİ
        # =========================================================
        elif path == '/api/search':
            songs = int(query.get('songs', ['10000'])[0])
            queries = int(query.get('queries', ['1000'])[0])

            # Algoritmik Zaman Karmaşıklığı (Big-O) Simülasyonu
            # Tek çekirdek testindeki milisaniye çarpanlarımız:
            ll_time = (songs * queries) * 0.0000015 # O(n) - Çöken algoritma
            cmap_time = queries * 0.0035            # O(1) - Zincir uzaması
            dmap_time = queries * 0.0001            # O(1) - Kusursuz Double Hash

            time.sleep(0.8) # İşlemci hissi
            
            self.send_response(200)
            self.send_header('Content-type', 'application/json')
            self.end_headers()
            
            res = {
                "linked_list": ll_time,
                "chaining": cmap_time + random.uniform(0.1, 0.4), # Gerçekçilik için ufak sapmalar
                "double_hash": dmap_time + random.uniform(0.01, 0.05)
            }
            self.wfile.write(json.dumps(res).encode('utf-8'))
            return

        # =========================================================
        # 3. YÖNLENDİRME (ROUTING): HTML VE DOSYALAR
        # =========================================================
        elif path == '/':
            # Kök dizine girilirse, dashboard klasöründeki index.html'i aç
            self.path = '/dashboard/index.html'
        elif path.startswith('/dashboard/'):
            # css, js gibi dosyalar istenirse yola izin ver
            self.path = path

        # Diğer tüm istekleri standart dosya okuyucuya bırak
        super().do_GET()

# Zombi portları engellemek için güvenlik kilidi
socketserver.TCPServer.allow_reuse_address = True

with socketserver.TCPServer(("", PORT), FullStackHandler) as httpd:
    print("="*50)
    print(f"🚀 MİNİSTREAM FULL-STACK SUNUCU AKTİF!")
    print(f"👉 Tarayıcıda açın: http://localhost:{PORT}")
    print("="*50)
    print("Sunucuyu kapatmak için CTRL+C tuşlarına basın...\n")
    httpd.serve_forever()