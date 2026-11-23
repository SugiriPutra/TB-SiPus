SiPus - Sistem Peminjaman Perpustakaan Mini
Tugas Besar Proyek Pemrograman dan Struktur Data

Kelompok  : [isi nomor kelompok]
Anggota 1 : I Komang Gede Sugiri Putra - 260530911009
Anggota 2 : [isi nama - NIM]

------------------------------------------------------------------
1. Berkas
------------------------------------------------------------------
sipus.h        Konstanta, struct, dan prototipe seluruh fungsi
buku.cpp       Modul Buku      : F1 (kelola), F5 (cari), F6 (urut), binary search rekursif
anggota.cpp    Modul Anggota   : F2
transaksi.cpp  Modul Transaksi : F3 (pinjam), F4 (kembali + denda), F8 (rekursif)
laporan.cpp    Modul Laporan   : F7 (rekap kategori x bulan, array 2D)
berkas.cpp     Fitur tambahan  : simpan/muat data ke berkas teks (<fstream>) + data contoh
util.cpp       Utilitas string, tanggal, dan pembacaan masukan tervalidasi
menu.cpp       Menu interaktif
main.cpp       Menu utama
uji.cpp        Pengujian otomatis (68 kasus uji)
Makefile       Perintah kompilasi (opsional)

------------------------------------------------------------------
2. Kompilasi (g++ dengan dukungan C++17)
------------------------------------------------------------------
Program utama:
  g++ -std=c++17 -Wall -Wextra -o sipus main.cpp menu.cpp buku.cpp anggota.cpp transaksi.cpp laporan.cpp berkas.cpp util.cpp

Program uji:
  g++ -std=c++17 -Wall -Wextra -o uji uji.cpp buku.cpp anggota.cpp transaksi.cpp laporan.cpp berkas.cpp util.cpp

Dengan AddressSanitizer (Linux / macOS / WSL), tambahkan: -g -fsanitize=address,undefined
Alternatif: valgrind --leak-check=full ./sipus   (atau ./uji)

Jika make tersedia: make, make uji, make asan, make clean.

------------------------------------------------------------------
3. Menjalankan
------------------------------------------------------------------
  ./sipus     (Windows: sipus.exe)
  ./uji       (keluaran akhir: "68 / 68 kasus uji lulus")

Mencoba cepat: pilih menu 8 -> 3 (isi data contoh), lalu coba menu lainnya.
Ketik "batal" pada isian mana pun untuk membatalkan operasi.

Data disimpan di sipus_data.txt (folder tempat program dijalankan) dan
dimuat otomatis saat program dimulai.

------------------------------------------------------------------
4. Aturan yang diterapkan
------------------------------------------------------------------
- Lama pinjam 7 hari, denda Rp1.000 per hari keterlambatan, maksimal 3 buku per anggota
  (konstanta LAMA_PINJAM, DENDA_HARIAN, MAKS_PINJAM di sipus.h).
- Kode buku berformat BK-xxx, ID anggota AG-xxx (3 digit), dan harus unik.
- Buku yang masih dipinjam tidak dapat dihapus.
- Pencarian kode memakai binary search rekursif; array diurutkan menurut kode lebih dulu.
- Pengurutan memakai selection sort manual (tanpa std::sort) dan menukar record utuh.
- Kapasitas awal array dinamis 4 dan digandakan otomatis saat penuh.
- Teks tidak boleh memuat karakter '|' (pemisah pada berkas data).
- Transaksi yang bukunya sudah dihapus tidak ikut dihitung pada rekap F7.
