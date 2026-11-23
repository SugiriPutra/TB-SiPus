// buku.cpp - Modul Buku: F1 (kelola buku), F5 (pencarian), F6 (pengurutan)
#include "sipus.h"
#include <iostream>
#include <iomanip>
using namespace std;

// ------------------------------------------------------ memori dinamis
void inisialisasi(DaftarBuku& d, int kapasitasAwal) {
    if (kapasitasAwal < 1) kapasitasAwal = 1;
    d.data = new Buku[kapasitasAwal]();
    d.n = 0;
    d.kapasitas = kapasitasAwal;
}

void bebaskan(DaftarBuku& d) {
    delete[] d.data;
    d.data = nullptr;
    d.n = 0;
    d.kapasitas = 0;
}

// Menggandakan kapasitas: buat array baru, salin isi, hapus array lama.
static void perbesar(DaftarBuku& d) {
    int kapasitasBaru = d.kapasitas > 0 ? d.kapasitas * 2 : 1;
    Buku* baru = new Buku[kapasitasBaru]();
    for (int i = 0; i < d.n; i++) baru[i] = d.data[i];
    delete[] d.data;
    d.data = baru;
    d.kapasitas = kapasitasBaru;
}

// ------------------------------------------------------------ F1: kelola
static bool bukuValid(const Buku& b) {
    return formatKodeBuku(b.kode) && teksAman(b.judul) &&
           teksAman(b.penulis) && b.kategori >= 0 &&
           b.kategori < JML_KATEGORI && b.tahun >= 1000 &&
           b.tahun <= 9999 && b.stok >= 0 && b.totalDipinjam >= 0;
}

bool tambahBuku(DaftarBuku& d, const Buku& b) {
    if (!bukuValid(b)) return false;
    if (cariKode(d, b.kode) != nullptr) return false;   // kode harus unik
    if (d.n == d.kapasitas) perbesar(d);
    d.data[d.n] = b;
    d.n++;
    return true;
}

// Kode dan totalDipinjam tidak berubah; sisanya diganti dengan 'baru'.
bool ubahBuku(DaftarBuku& d, const string& kode, const Buku& baru) {
    Buku* p = cariKode(d, kode);
    if (p == nullptr) return false;
    Buku salinan = baru;
    salinan.kode = p->kode;
    salinan.totalDipinjam = p->totalDipinjam;
    if (!bukuValid(salinan)) return false;
    *p = salinan;
    return true;
}

// Menggeser elemen setelahnya ke kiri. (Pengecekan "masih dipinjam"
// dilakukan pemanggil lewat bukuSedangDipinjam di modul transaksi.)
bool hapusBuku(DaftarBuku& d, const string& kode) {
    int idx = -1;
    for (int i = 0; i < d.n; i++) {
        if (d.data[i].kode == kode) {
            idx = i;
            break;
        }
    }
    if (idx < 0) return false;
    for (int i = idx; i < d.n - 1; i++) d.data[i] = d.data[i + 1];
    d.n--;
    return true;
}

// ---------------------------------------------------------- F5: pencarian
Buku* cariKode(DaftarBuku& d, const string& kode) {
    for (int i = 0; i < d.n; i++)
        if (d.data[i].kode == kode) return &d.data[i];
    return nullptr;
}

const Buku* cariKode(const DaftarBuku& d, const string& kode) {
    for (int i = 0; i < d.n; i++)
        if (d.data[i].kode == kode) return &d.data[i];
    return nullptr;
}

int hitungJudulCocok(const DaftarBuku& d, const string& kata) {
    int jumlah = 0;
    for (int i = 0; i < d.n; i++)
        if (mengandungKata(d.data[i].judul, kata)) jumlah++;
    return jumlah;
}

void cariJudul(const DaftarBuku& d, const string& kata) {
    int ditemukan = 0;
    for (int i = 0; i < d.n; i++) {
        if (mengandungKata(d.data[i].judul, kata)) {
            if (ditemukan == 0) cetakHeaderBuku();
            cetakBarisBuku(d.data[i]);
            ditemukan++;
        }
    }
    if (ditemukan == 0)
        cout << "Tidak ada buku dengan judul memuat \"" << kata << "\".\n";
    else
        cout << ditemukan << " buku ditemukan.\n";
}

// Syarat: arr[kiri..kanan] sudah terurut menaik berdasarkan kode.
int binarySearchKode(const Buku arr[], int kiri, int kanan, const string& kode) {
    if (kiri > kanan) return -1;                        // basis: tidak ada
    int tengah = kiri + (kanan - kiri) / 2;
    if (arr[tengah].kode == kode) return tengah;        // basis: ketemu
    if (kode < arr[tengah].kode)
        return binarySearchKode(arr, kiri, tengah - 1, kode);
    return binarySearchKode(arr, tengah + 1, kanan, kode);
}

// -------------------------------------------------------- F6: pengurutan
static bool kodeDahulu(const Buku& a, const Buku& b) {
    return a.kode < b.kode;
}

static bool judulDahulu(const Buku& a, const Buku& b) {
    string x = keHurufKecil(a.judul), y = keHurufKecil(b.judul);
    if (x != y) return x < y;
    return a.kode < b.kode;
}

static bool dipinjamDahulu(const Buku& a, const Buku& b) {
    if (a.totalDipinjam != b.totalDipinjam)
        return a.totalDipinjam > b.totalDipinjam;       // menurun
    return a.kode < b.kode;
}

// Selection sort manual; yang ditukar adalah record Buku utuh.
static void urutkanDengan(DaftarBuku& d, bool (*dahulu)(const Buku&, const Buku&)) {
    for (int i = 0; i < d.n - 1; i++) {
        int terpilih = i;
        for (int j = i + 1; j < d.n; j++)
            if (dahulu(d.data[j], d.data[terpilih])) terpilih = j;
        if (terpilih != i) {
            Buku tmp = d.data[i];
            d.data[i] = d.data[terpilih];
            d.data[terpilih] = tmp;
        }
    }
}

void urutkanKode(DaftarBuku& d) { urutkanDengan(d, kodeDahulu); }
void urutkanJudul(DaftarBuku& d) { urutkanDengan(d, judulDahulu); }
void urutkanDipinjam(DaftarBuku& d) { urutkanDengan(d, dipinjamDahulu); }

// ------------------------------------------------------------ tampilan
void cetakHeaderBuku() {
    cout << left << setw(8) << "Kode" << setw(28) << "Judul"
         << setw(18) << "Penulis" << setw(13) << "Kategori"
         << setw(7) << "Tahun" << setw(6) << "Stok" << "Dipinjam\n";
    cout << string(88, '-') << "\n";
}

void cetakBarisBuku(const Buku& b) {
    string kat = (b.kategori >= 0 && b.kategori < JML_KATEGORI)
                     ? NAMA_KATEGORI[b.kategori] : "?";
    cout << left << setw(8) << b.kode << setw(28) << potong(b.judul, 26)
         << setw(18) << potong(b.penulis, 16) << setw(13) << kat
         << setw(7) << b.tahun << setw(6) << b.stok << b.totalDipinjam << "\n";
}

void tampilBuku(const DaftarBuku& d) {
    if (d.n == 0) {
        cout << "(belum ada data buku)\n";
        return;
    }
    cetakHeaderBuku();
    for (int i = 0; i < d.n; i++) cetakBarisBuku(d.data[i]);
    cout << "Total " << d.n << " judul (kapasitas array: " << d.kapasitas << ")\n";
}
