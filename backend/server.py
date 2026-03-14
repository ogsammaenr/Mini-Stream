import ctypes
import os
from http.server import BaseHTTPRequestHandler, HTTPServer
from urllib.parse import urlparse, parse_qs

lib_path = os.path.abspath('./ministream.so')
try:
    lib = ctypes.CDLL(lib_path)
    
    # 1. Bellek Fonksiyonu
    lib.deney_json.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int]
    lib.deney_json.restype = ctypes.c_char_p
    
    # 2. YENİ: Arama Hızı Fonksiyonu
    lib.arama_json.argtypes = [ctypes.c_int, ctypes.c_int]
    lib.arama_json.restype = ctypes.c_char_p
except OSError:
    print(f"HATA: {lib_path} bulunamadi!")
    exit(1)

class Handler(BaseHTTPRequestHandler):
    def do_GET(self):
        parsed_path = urlparse(self.path)
        query_params = parse_qs(parsed_path.query)
        
        if parsed_path.path == '/benchmark':
            try:
                n_sarki = int(query_params.get('songs', ['10000'])[0])
                n_liste = int(query_params.get('lists', ['5000'])[0])
                per_list = int(query_params.get('per_list', ['50'])[0])
            except ValueError:
                n_sarki, n_liste, per_list = 10000, 5000, 50

            sonuc = lib.deney_json(n_sarki, n_liste, per_list)
            self.send_json(sonuc)
            
        # YENİ: Arama Hızı Endpoint'i
        elif parsed_path.path == '/search':
            try:
                n_sarki = int(query_params.get('songs', ['10000'])[0])
                n_sorgu = int(query_params.get('queries', ['1000'])[0])
            except ValueError:
                n_sarki, n_sorgu = 10000, 1000
                
            sonuc = lib.arama_json(n_sarki, n_sorgu)
            self.send_json(sonuc)
            
        else:
            self.send_response(404)
            self.end_headers()

    def send_json(self, sonuc):
        try:
            self.send_response(200)
            self.send_header('Content-Type', 'application/json')
            self.send_header('Access-Control-Allow-Origin', '*') 
            self.end_headers()
            self.wfile.write(sonuc)
        except BrokenPipeError:
            pass

    def log_message(self, format, *args):
        pass

print("🚀 MiniStream Backend Aktif! Port: 8765")
HTTPServer(('localhost', 8765), Handler).serve_forever()