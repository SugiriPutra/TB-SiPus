// main.cpp - titik masuk program SiPus
#include "sipus.h"
#include <iostream>
#include <fstream>
using namespace std;

static const char* BERKAS_DATA = "sipus_data.txt";

int main() {
    DaftarBuku buku;
    DaftarAnggota anggota;
    DaftarTransaksi transaksi;
    inisialisasi(buku, KAPASITAS_AWAL);
    inisialisasi(anggota, KAPASITAS_AWAL);
    inisialisasi(transaksi, KAPASITAS_AWAL);

    // Muat data tersimpan jika ada.
    ifstream cek(BERKAS_DATA);
    if (cek) {
        cek.close();
        if (muatData(buku, anggota, transaksi, BERKAS_DATA))
            cout << "Data dimuat dari " << BERKAS_DATA << ".\n";
        else
            cout << "Peringatan: " << BERKAS_DATA
                 << " tidak valid, memulai dengan data kosong.\n";
    }

    int pilihan = -1;
    do {
        cout << "\n=== SiPus - Sistem Peminjaman Perpustakaan Mini ===\n"
                "1. Kelola Buku\n"
                "2. Kelola Anggota\n"
                "3. Daftar Buku (dengan pengurutan)\n"
                "4. Pinjam Buku\n"
                "5. Kembalikan Buku\n"
                "6. Cari Buku\n"
                "7. Laporan\n"
                "8. Berkas & Data Contoh\n"
                "0. Keluar\n";
        if (!bacaAngka("Pilihan: ", pilihan, 0, 8)) {
            if (cin.fail()) break;      // input habis (EOF)
            pilihan = -1;
            continue;
        }
        switch (pilihan) {
            case 1: menuBuku(buku, transaksi); break;
            case 2: menuAnggota(anggota); break;
            case 3: menuDaftarBuku(buku); break;
            case 4: menuPinjam(buku, anggota, transaksi); break;
            case 5: menuKembali(buku, anggota, transaksi); break;
            case 6: menuCari(buku); break;
            case 7: menuLaporan(buku, anggota, transaksi); break;
            case 8: menuBerkas(buku, anggota, transaksi); break;
            case 0:
                if (konfirmasi("Simpan data sebelum keluar?")) {
                    cout << (simpanData(buku, anggota, transaksi, BERKAS_DATA)
                                 ? ">> Data disimpan.\n"
                                 : ">> Gagal menyimpan data.\n");
                }
                break;
        }
    } while (pilihan != 0);

    // Membebaskan seluruh memori dinamis.
    bebaskan(buku);
    bebaskan(anggota);
    bebaskan(transaksi);
    cout << "Terima kasih.\n";
    return 0;
}
