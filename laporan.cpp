// laporan.cpp - Modul Laporan: F7 (rekap kategori x bulan, array 2D)
#include "sipus.h"
#include <iostream>
#include <iomanip>
using namespace std;

// rekap[kategori][bulan-1] = jumlah peminjaman. tahun = 0 -> semua tahun.
// Transaksi yang bukunya sudah dihapus dilewati (kategorinya tidak diketahui).
void isiRekap(const DaftarTransaksi& t, const DaftarBuku& b,
              int rekap[][JML_BULAN], int tahun) {
    for (int i = 0; i < JML_KATEGORI; i++)
        for (int j = 0; j < JML_BULAN; j++) rekap[i][j] = 0;

    for (int k = 0; k < t.n; k++) {
        const Transaksi& tr = t.data[k];
        if (tahun > 0 && tr.tglPinjam.tahun != tahun) continue;
        if (tr.tglPinjam.bulan < 1 || tr.tglPinjam.bulan > JML_BULAN) continue;
        const Buku* pb = cariKode(b, tr.kodeBuku);
        if (pb == nullptr) continue;
        if (pb->kategori < 0 || pb->kategori >= JML_KATEGORI) continue;
        rekap[pb->kategori][tr.tglPinjam.bulan - 1]++;
    }
}

// Jika ada dua kategori seri, kategori berindeks lebih kecil yang dipilih.
int kategoriTerpopuler(const int rekap[][JML_BULAN]) {
    int terbaik = -1, maks = 0;
    for (int i = 0; i < JML_KATEGORI; i++) {
        int total = 0;
        for (int j = 0; j < JML_BULAN; j++) total += rekap[i][j];
        if (total > maks) {
            maks = total;
            terbaik = i;
        }
    }
    return terbaik;
}

void tampilRekap(const int rekap[][JML_BULAN], const string namaKategori[],
                 int tahun) {
    cout << "\nREKAP PEMINJAMAN";
    if (tahun > 0) cout << " " << tahun;
    cout << "\n";

    cout << left << setw(14) << "Kategori";
    for (int j = 0; j < JML_BULAN; j++) cout << right << setw(5) << NAMA_BULAN[j];
    cout << right << setw(7) << "Total" << "\n";
    cout << string(14 + 5 * JML_BULAN + 7, '-') << "\n";

    int totalBulan[JML_BULAN] = {0};
    int totalSemua = 0;
    for (int i = 0; i < JML_KATEGORI; i++) {
        int totalBaris = 0;
        cout << left << setw(14) << namaKategori[i];
        for (int j = 0; j < JML_BULAN; j++) {
            cout << right << setw(5) << rekap[i][j];
            totalBaris += rekap[i][j];
            totalBulan[j] += rekap[i][j];
        }
        cout << right << setw(7) << totalBaris << "\n";
        totalSemua += totalBaris;
    }

    cout << string(14 + 5 * JML_BULAN + 7, '-') << "\n";
    cout << left << setw(14) << "Total";
    for (int j = 0; j < JML_BULAN; j++) cout << right << setw(5) << totalBulan[j];
    cout << right << setw(7) << totalSemua << "\n";

    int populer = kategoriTerpopuler(rekap);
    if (populer < 0) {
        cout << "Kategori terpopuler: - (belum ada data peminjaman)\n";
    } else {
        int total = 0;
        for (int j = 0; j < JML_BULAN; j++) total += rekap[populer][j];
        cout << "Kategori terpopuler: " << namaKategori[populer]
             << " (" << total << " peminjaman)\n";
    }
}
