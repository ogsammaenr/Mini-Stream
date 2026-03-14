# ==========================================
# MINISTREAM - OTOMATİK DERLEME SİSTEMİ
# ==========================================

# --- Renk Kodları ---
CYAN    = \033[1;36m
GREEN   = \033[1;32m
YELLOW  = \033[1;33m
RED     = \033[1;31m
RESET   = \033[0m

# Derleyici ve Bayraklar (Flags)
CC = gcc
CFLAGS = -g -Wall -Wextra -fopenmp
SO_FLAGS = -shared -fPIC

# Dosyalar
CORE_SRCS = src/ministream.c \
            src/bellek_izci.c \
            src/data_types/hash_map.c \
            src/data_types/linked_list.c \
            src/data_types/double_hash.c \
            src/csv_oku.c \
            src/playlist_olusturucu.c

APP_SRCS = src/main.c $(CORE_SRCS)

# Varsayılan Hedef
all: baslangic ministream ministream.so generator test_temel test_benchmark bitis

baslangic:
	@echo -e "\n$(YELLOW)==================================================$(RESET)"
	@echo -e "$(CYAN)🎧 MINISTREAM LABORATUVARI INSA EDILIYOR...$(RESET)"
	@echo -e "$(YELLOW)==================================================$(RESET)\n"

# Hedef 1: Standart C Programı
ministream: $(APP_SRCS)
	@echo -e "$(CYAN)⚙️  [1/3] Çekirdek Motor derleniyor (ministream)...$(RESET)"
	@$(CC) $(CFLAGS) $(APP_SRCS) -o ministream
	@echo -e "$(GREEN)   ✔ Başarılı!$(RESET)"

# Hedef 2: Python Backend için Shared Library
ministream.so: $(CORE_SRCS)
	@echo -e "$(CYAN)🐍 [2/3] Python API Kütüphanesi derleniyor (ministream.so)...$(RESET)"
	@$(CC) $(CFLAGS) $(SO_FLAGS) $(CORE_SRCS) -o ministream.so
	@echo -e "$(GREEN)   ✔ Başarılı!$(RESET)"

# Hedef 3: Veri Seti Jeneratörü
generator: data/dataset_generator.c
	@echo -e "$(CYAN)📊 [3/3] Veri Seti Jeneratörü derleniyor (-O3 ile)...$(RESET)"
	@$(CC) -O3 data/dataset_generator.c -o generator
	@echo -e "$(GREEN)   ✔ Başarılı!$(RESET)"

test_temel: $(CORE_SRCS) test/test_temel.c
	@echo -e "$(CYAN)🧪 Birim Testleri derleniyor...$(RESET)"
	@$(CC) $(CFLAGS) -I src test/test_temel.c $(CORE_SRCS) -o test_temel
	@echo -e "$(GREEN)   ✔ Başarılı!$(RESET)"

# Hedef 5: Performans Benchmark Testleri
test_benchmark: $(CORE_SRCS) test/test_benchmark.c
	@echo -e "$(CYAN)⏱️  Performans Testleri derleniyor...$(RESET)"
	@$(CC) $(CFLAGS) -I src test/test_benchmark.c $(CORE_SRCS) -o test_benchmark
	@echo -e "$(GREEN)   ✔ Başarılı!$(RESET)"

test: test_temel test_benchmark
	@./test_temel
	@./test_benchmark

bitis:
	@echo -e "\n$(GREEN)🚀 BÜTÜN SİSTEMLER AKTİF!$(RESET)"
	@echo -e "$(YELLOW)👉 Terminal testi için :$(RESET) ./ministream"
	@echo -e "$(YELLOW)👉 Veri üretmek için   :$(RESET) ./generator 5000000"
	@echo -e "$(YELLOW)👉 Web Arayüzü için    :$(RESET) python3 backend/server.py\n"

# Temizlik Komutu
clean:
	@echo -e "$(RED)🗑️  Eski derleme dosyaları temizleniyor...$(RESET)"
	@rm -f ministream ministream.so generator benchmark test_temel test_benchmark
	@echo -e "$(GREEN)✨ Tertemiz!$(RESET)\n"