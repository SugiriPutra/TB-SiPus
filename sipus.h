// sipus.h - definisi tipe data dan prototipe fungsi
// SiPus: Sistem Peminjaman Perpustakaan Mini

#ifndef SIPUS_H
#define SIPUS_H

#include <string>
using namespace std;

const int JML_KATEGORI = 5;     // Ukuran baris rekap
const int JML_BULAN = 12;       // Ukuran kolom rekap
const int MAKS_PINJAM = 3;      // Batas buku/anggota
const int DENDA_HARIAN = 1000;  // Rupiah per hari
const int LAMA_PINJAM = 7;      // Hari
const int KAPASITAS_AWAL = 4;   // Kapasitas awal array dinamis (maks. 4)

extern const string NAMA_KATEGORI[JML_KATEGORI];
extern const string NAMA_BULAN[JML_BULAN];

struct Tanggal {
    int hari, bulan, tahun;
};

struct Buku {
    string kode;        // Format "BK-001"
    string judul;
    string penulis;
    int kategori;       // 0..JML_KATEGORI-1
    int tahun;
    int stok;           // Eksemplar tersedia
    int totalDipinjam;  // Untuk statistik
};

struct Anggota {
    string id;          // Format "AG-001"
    string nama;
    string prodi;
    int sedangPinjam;   // Maksimum MAKS_PINJAM
};

struct Transaksi {
    int nomor;
    string idAnggota;
    string kodeBuku;
    Tanggal tglPinjam;
    Tanggal tglKembali; // Diisi saat buku kembali
    bool selesai;
    int denda;
};

// Pembungkus array dinamis: alamat, isi, kapasitas
struct DaftarBuku {
    Buku* data; int n, kapasitas;
};

struct DaftarAnggota {
    Anggota* data; int n, kapasitas;
};

struct DaftarTransaksi {
    Transaksi* data; int n, kapasitas;
};

// ======================================================================
// Modul Buku (Mahasiswa A)  -> buku.cpp
// ======================================================================
void inisialisasi(DaftarBuku& d, int kapasitasAwal);
void bebaskan(DaftarBuku& d);
bool tambahBuku(DaftarBuku& d, const Buku& b);
bool ubahBuku(DaftarBuku& d, const string& kode, const Buku& baru);
bool hapusBuku(DaftarBuku& d, const string& kode);
Buku* cariKode(DaftarBuku& d, const string& kode);
const Buku* cariKode(const DaftarBuku& d, const string& kode);
void cariJudul(const DaftarBuku& d, const string& kata);
int hitungJudulCocok(const DaftarBuku& d, const string& kata);
void urutkanKode(DaftarBuku& d);
void urutkanJudul(DaftarBuku& d);
void urutkanDipinjam(DaftarBuku& d);   // terbanyak dipinjam lebih dulu
int binarySearchKode(const Buku arr[], int kiri,
                     int kanan, const string& kode);
void cetakHeaderBuku();
void cetakBarisBuku(const Buku& b);
void tampilBuku(const DaftarBuku& d);

// ======================================================================
// Modul Anggota & Transaksi (Mahasiswa B)  -> anggota.cpp, transaksi.cpp
// ======================================================================
void inisialisasi(DaftarAnggota& d, int kapasitasAwal);
void bebaskan(DaftarAnggota& d);
bool tambahAnggota(DaftarAnggota& d, const Anggota& a);
bool ubahAnggota(DaftarAnggota& d, const string& id,
                 const string& nama, const string& prodi);
Anggota* cariAnggota(DaftarAnggota& d, const string& id);
const Anggota* cariAnggota(const DaftarAnggota& d, const string& id);
void tampilAnggota(const DaftarAnggota& d);

void inisialisasi(DaftarTransaksi& d, int kapasitasAwal);
void bebaskan(DaftarTransaksi& d);
bool tambahTransaksi(DaftarTransaksi& d, const Transaksi& tr);
int nomorBerikut(const DaftarTransaksi& d);
Transaksi* cariTransaksi(DaftarTransaksi& d, int nomor);
bool bukuSedangDipinjam(const DaftarTransaksi& t, const string& kode);
void tampilPinjamanAktif(const DaftarTransaksi& t);

// Mengembalikan "" jika peminjaman boleh dilakukan, selain itu alasan penolakan.
string alasanPinjamDitolak(const DaftarBuku& b, const DaftarAnggota& a,
                           const string& idAg, const string& kodeBk,
                           const Tanggal& tgl);
bool pinjamBuku(DaftarTransaksi& t, DaftarBuku& b,
                DaftarAnggota& a, const string& idAg,
                const string& kodeBk, const Tanggal& tgl);
bool kembalikanBuku(DaftarTransaksi& t, DaftarBuku& b,
                    DaftarAnggota& a, int nomor,
                    const Tanggal& tgl);
// Versi dengan parameter keluaran: denda dan jumlah hari terlambat.
bool kembalikanBuku(DaftarTransaksi& t, DaftarBuku& b,
                    DaftarAnggota& a, int nomor,
                    const Tanggal& tgl, int& denda, int& hariTerlambat);
int selisihHari(const Tanggal& awal,
                const Tanggal& akhir);
int hitungDenda(const Tanggal& pinjam, const Tanggal& kembali,
                int& hariTerlambat);
int totalDendaRekursif(const Transaksi arr[], int n);
// Mencetak (jika cetak=true) dan menghitung transaksi milik satu anggota.
int riwayatAnggotaRekursif(const Transaksi arr[], int n,
                           const string& idAg, bool cetak = true);

// ======================================================================
// Modul Laporan (Mahasiswa B)  -> laporan.cpp
// ======================================================================
// tahun = 0 berarti semua tahun dihitung.
void isiRekap(const DaftarTransaksi& t,
              const DaftarBuku& b, int rekap[][JML_BULAN], int tahun = 0);
int kategoriTerpopuler(const int rekap[][JML_BULAN]);  // -1 jika kosong
void tampilRekap(const int rekap[][JML_BULAN],
                 const string namaKategori[], int tahun = 0);

// ======================================================================
// Penyimpanan berkas (fitur tambahan)  -> berkas.cpp
// ======================================================================
bool simpanData(const DaftarBuku& b, const DaftarAnggota& a,
                const DaftarTransaksi& t, const string& namaBerkas);
bool muatData(DaftarBuku& b, DaftarAnggota& a,
              DaftarTransaksi& t, const string& namaBerkas);
bool isiDataContoh(DaftarBuku& b, DaftarAnggota& a, DaftarTransaksi& t);

// ======================================================================
// Utilitas (string, tanggal, masukan)  -> util.cpp
// ======================================================================
string keHurufKecil(const string& s);
string keHurufBesar(const string& s);
string pangkas(const string& s);
string potong(const string& s, size_t lebar);
string formatRupiah(int nilai);
bool mengandungKata(const string& teks, const string& kata);
bool formatKodeBuku(const string& s);
bool formatIdAnggota(const string& s);
bool teksAman(const string& s);
bool parseInt(const string& s, int& hasil);

bool tanggalValid(const Tanggal& t);
int hariKeAngka(const Tanggal& t);
Tanggal angkaKeTanggal(int hari);
Tanggal tambahHari(const Tanggal& t, int n);
string formatTanggal(const Tanggal& t);
bool parseTanggal(const string& s, Tanggal& t);

// Fungsi baca: mengembalikan false jika pengguna mengetik "batal" atau input habis (EOF).
bool bacaBaris(const string& prompt, string& hasil);
bool bacaTeks(const string& prompt, string& hasil);
bool bacaAngka(const string& prompt, int& hasil, int min, int maks);
bool bacaKodeBuku(const string& prompt, string& hasil);
bool bacaIdAnggota(const string& prompt, string& hasil);
bool bacaTanggal(const string& prompt, Tanggal& hasil);
bool konfirmasi(const string& prompt);

// ======================================================================
// Menu interaktif  -> menu.cpp
// ======================================================================
void menuBuku(DaftarBuku& b, const DaftarTransaksi& t);
void menuAnggota(DaftarAnggota& a);
void menuDaftarBuku(DaftarBuku& b);
void menuPinjam(DaftarBuku& b, DaftarAnggota& a, DaftarTransaksi& t);
void menuKembali(DaftarBuku& b, DaftarAnggota& a, DaftarTransaksi& t);
void menuCari(DaftarBuku& b);
void menuLaporan(const DaftarBuku& b, const DaftarAnggota& a,
                 const DaftarTransaksi& t);
void menuBerkas(DaftarBuku& b, DaftarAnggota& a, DaftarTransaksi& t);

#endif
