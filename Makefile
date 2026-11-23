CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
MODUL = buku.cpp anggota.cpp transaksi.cpp laporan.cpp berkas.cpp util.cpp

all: sipus

sipus: main.cpp menu.cpp $(MODUL) sipus.h
	$(CXX) $(CXXFLAGS) -o sipus main.cpp menu.cpp $(MODUL)

uji: uji.cpp $(MODUL) sipus.h
	$(CXX) $(CXXFLAGS) -o uji uji.cpp $(MODUL)
	./uji

asan: main.cpp menu.cpp uji.cpp $(MODUL) sipus.h
	$(CXX) $(CXXFLAGS) -g -fsanitize=address,undefined -o uji_asan uji.cpp $(MODUL)
	$(CXX) $(CXXFLAGS) -g -fsanitize=address,undefined -o sipus_asan main.cpp menu.cpp $(MODUL)
	./uji_asan

clean:
	rm -f sipus uji uji_asan sipus_asan sipus_data.txt
