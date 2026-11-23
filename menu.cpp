// menu.cpp - menu interaktif (semua masukan divalidasi lewat util.cpp)
// Ketik "batal" pada isian mana pun untuk membatalkan operasi.
#include "sipus.h"
#include <iostream>
using namespace std;

static const char* BERKAS_DATA = "sipus_data.txt";

static void cetakDaftarKategori() {
    cout << "Kategori: ";
    for (int i = 0; i < JML_KATEGORI; i++)
        cout << i << "=" << NAMA_KATEGORI[i] << (i + 1 < JML_KATEGORI ? "  " : "\n");
}

// ----------------------------------------------------------- kelola buku
static void tambahBukuInteraktif(DaftarBuku& b) {
    Buku baru = {"", "", "", 0, 0, 0, 0};
    if (!bacaKodeBuku("Kode buku (BK-xxx) : ", baru.kode)) return;
    if (cariKode(b, baru.kode) != nullptr) {
        cout << ">> Gagal: kode " << baru.kode << " sudah dipakai.\n";
        return;
    }
    if (!bacaTeks("Judul              : ", baru.judul)) return;
    if (!bacaTeks("Penulis            : ", baru.penulis)) return;
    cetakDaftarKategori();
    if (!bacaAngka("Kategori           : ", baru.kategori, 0, JML_KATEGORI - 1)) return;
    if (!bacaAngka("Tahun terbit       : ", baru.tahun, 1000, 9999)) return;
    if (!bacaAngka("Stok               : ", baru.stok, 0, 9999)) return;
    if (tambahBuku(b, baru))
        cout << ">> Buku ditambahkan. (isi " << b.n << " / kapasitas " << b.kapasitas << ")\n";
    else
        cout << ">> Gagal menambahkan buku.\n";
}

static void ubahBukuInteraktif(DaftarBuku& b) {
    string kode;
    if (!bacaKodeBuku("Kode buku yang diubah : ", kode)) return;
    const Buku* lama = cariKode(b, kode);
    if (lama == nullptr) {
        cout << ">> Buku tidak ditemukan.\n";
        return;
    }
    cetakHeaderBuku();
    cetakBarisBuku(*lama);
    Buku baru = *lama;
    if (!bacaTeks("Judul baru     : ", baru.judul)) return;
    if (!bacaTeks("Penulis baru   : ", baru.penulis)) return;
    cetakDaftarKategori();
    if (!bacaAngka("Kategori baru  : ", baru.kategori, 0, JML_KATEGORI - 1)) return;
    if (!bacaAngka("Tahun baru     : ", baru.tahun, 1000, 9999)) return;
    if (!bacaAngka("Stok baru      : ", baru.stok, 0, 9999)) return;
    cout << (ubahBuku(b, kode, baru) ? ">> Data buku diperbarui.\n"
                                     : ">> Gagal memperbarui buku.\n");
}

static void hapusBukuInteraktif(DaftarBuku& b, const DaftarTransaksi& t) {
    string kode;
    if (!bacaKodeBuku("Kode buku yang dihapus : ", kode)) return;
    if (cariKode(b, kode) == nullptr) {
        cout << ">> Buku tidak ditemukan.\n";
        return;
    }
    if (bukuSedangDipinjam(t, kode)) {
        cout << ">> Ditolak: buku masih dipinjam.\n";
        return;
    }
    if (!konfirmasi("Yakin hapus buku " + kode + "?")) {
        cout << ">> Penghapusan dibatalkan.\n";
        return;
    }
    cout << (hapusBuku(b, kode) ? ">> Buku dihapus.\n" : ">> Gagal menghapus buku.\n");
}

void menuBuku(DaftarBuku& b, const DaftarTransaksi& t) {
    int pilihan = -1;
    do {
        cout << "\n--- Kelola Buku ---\n1. Tambah buku\n2. Ubah buku\n"
                "3. Hapus buku\n4. Tampilkan semua buku\n0. Kembali\n";
        if (!bacaAngka("Pilihan: ", pilihan, 0, 4)) {
            if (cin.fail()) return;
            pilihan = -1;
            continue;
        }
        switch (pilihan) {
            case 1: tambahBukuInteraktif(b); break;
            case 2: ubahBukuInteraktif(b); break;
            case 3: hapusBukuInteraktif(b, t); break;
            case 4: tampilBuku(b); break;
        }
    } while (pilihan != 0);
}

// -------------------------------------------------------- kelola anggota
static void tambahAnggotaInteraktif(DaftarAnggota& a) {
    Anggota baru = {"", "", "", 0};
    if (!bacaIdAnggota("ID anggota (AG-xxx) : ", baru.id)) return;
    if (cariAnggota(a, baru.id) != nullptr) {
        cout << ">> Gagal: ID " << baru.id << " sudah dipakai.\n";
        return;
    }
    if (!bacaTeks("Nama                : ", baru.nama)) return;
    if (!bacaTeks("Program studi       : ", baru.prodi)) return;
    if (tambahAnggota(a, baru))
        cout << ">> Anggota ditambahkan. (isi " << a.n << " / kapasitas " << a.kapasitas << ")\n";
    else
        cout << ">> Gagal menambahkan anggota.\n";
}

static void ubahAnggotaInteraktif(DaftarAnggota& a) {
    string id, nama, prodi;
    if (!bacaIdAnggota("ID anggota yang diubah : ", id)) return;
    if (cariAnggota(a, id) == nullptr) {
        cout << ">> Anggota tidak ditemukan.\n";
        return;
    }
    if (!bacaTeks("Nama baru           : ", nama)) return;
    if (!bacaTeks("Program studi baru  : ", prodi)) return;
    cout << (ubahAnggota(a, id, nama, prodi) ? ">> Data anggota diperbarui.\n"
                                             : ">> Gagal memperbarui anggota.\n");
}

void menuAnggota(DaftarAnggota& a) {
    int pilihan = -1;
    do {
        cout << "\n--- Kelola Anggota ---\n1. Tambah anggota\n2. Ubah anggota\n"
                "3. Tampilkan semua anggota\n0. Kembali\n";
        if (!bacaAngka("Pilihan: ", pilihan, 0, 3)) {
            if (cin.fail()) return;
            pilihan = -1;
            continue;
        }
        switch (pilihan) {
            case 1: tambahAnggotaInteraktif(a); break;
            case 2: ubahAnggotaInteraktif(a); break;
            case 3: tampilAnggota(a); break;
        }
    } while (pilihan != 0);
}

// ----------------------------------------------------- daftar buku (F6)
void menuDaftarBuku(DaftarBuku& b) {
    cout << "\nUrutkan berdasarkan:\n1. Kode\n2. Judul\n"
            "3. Jumlah dipinjam (terbanyak dulu)\n0. Tanpa pengurutan\n";
    int pilihan = 0;
    if (!bacaAngka("Pilihan: ", pilihan, 0, 3)) return;
    if (pilihan == 1) urutkanKode(b);
    else if (pilihan == 2) urutkanJudul(b);
    else if (pilihan == 3) urutkanDipinjam(b);
    tampilBuku(b);
}

// -------------------------------------------------------- pinjam (F3)
void menuPinjam(DaftarBuku& b, DaftarAnggota& a, DaftarTransaksi& t) {
    cout << "\n--- Pinjam Buku ---\n";
    string idAg, kodeBk;
    Tanggal tgl = {0, 0, 0};
    if (!bacaIdAnggota("ID anggota                  : ", idAg)) return;
    if (!bacaKodeBuku("Kode buku                   : ", kodeBk)) return;
    if (!bacaTanggal("Tanggal (hari bulan tahun) : ", tgl)) return;

    string alasan = alasanPinjamDitolak(b, a, idAg, kodeBk, tgl);
    if (!alasan.empty()) {
        cout << ">> Ditolak: " << alasan << "\n";
        return;
    }
    if (pinjamBuku(t, b, a, idAg, kodeBk, tgl)) {
        cout << ">> Peminjaman berhasil. Nomor transaksi: " << t.data[t.n - 1].nomor << "\n"
             << ">> Batas kembali: " << formatTanggal(tambahHari(tgl, LAMA_PINJAM))
             << " (" << LAMA_PINJAM << " hari)\n";
    } else {
        cout << ">> Peminjaman gagal.\n";
    }
}

// ------------------------------------------------------- kembali (F4)
void menuKembali(DaftarBuku& b, DaftarAnggota& a, DaftarTransaksi& t) {
    cout << "\n--- Kembalikan Buku ---\nBuku yang sedang dipinjam:\n";
    tampilPinjamanAktif(t);
    if (t.n == 0) return;

    int nomor = 0;
    Tanggal tgl = {0, 0, 0};
    if (!bacaAngka("Nomor transaksi              : ", nomor, 1, 999999)) return;
    Transaksi* tr = cariTransaksi(t, nomor);
    if (tr == nullptr) {
        cout << ">> Nomor transaksi tidak ditemukan.\n";
        return;
    }
    if (tr->selesai) {
        cout << ">> Transaksi ini sudah dikembalikan.\n";
        return;
    }
    if (!bacaTanggal("Tanggal kembali (hari bulan tahun) : ", tgl)) return;
    if (selisihHari(tr->tglPinjam, tgl) < 0) {
        cout << ">> Ditolak: tanggal kembali sebelum tanggal pinjam ("
             << formatTanggal(tr->tglPinjam) << ").\n";
        return;
    }

    int denda = 0, terlambat = 0;
    if (kembalikanBuku(t, b, a, nomor, tgl, denda, terlambat)) {
        cout << ">> Buku dikembalikan.\n";
        if (terlambat > 0)
            cout << ">> Terlambat " << terlambat << " hari. Denda: " << formatRupiah(denda) << "\n";
        else
            cout << ">> Tepat waktu. Denda: " << formatRupiah(0) << "\n";
    } else {
        cout << ">> Pengembalian gagal.\n";
    }
}

// --------------------------------------------------------- cari (F5)
void menuCari(DaftarBuku& b) {
    cout << "\n--- Cari Buku ---\n1. Berdasarkan judul (sebagian kata)\n"
            "2. Berdasarkan kode (binary search)\n0. Kembali\n";
    int pilihan = 0;
    if (!bacaAngka("Pilihan: ", pilihan, 0, 2)) return;
    if (pilihan == 1) {
        string kata;
        if (!bacaTeks("Kata kunci judul : ", kata)) return;
        cariJudul(b, kata);
    } else if (pilihan == 2) {
        string kode;
        if (!bacaKodeBuku("Kode buku : ", kode)) return;
        urutkanKode(b);     // binary search mensyaratkan array terurut menurut kode
        int idx = binarySearchKode(b.data, 0, b.n - 1, kode);
        if (idx < 0) {
            cout << "Buku dengan kode " << kode << " tidak ditemukan.\n";
        } else {
            cout << "Ditemukan pada indeks " << idx << " (setelah diurutkan menurut kode):\n";
            cetakHeaderBuku();
            cetakBarisBuku(b.data[idx]);
        }
    }
}

// --------------------------------------------------- laporan (F7 & F8)
void menuLaporan(const DaftarBuku& b, const DaftarAnggota& a,
                 const DaftarTransaksi& t) {
    int pilihan = -1;
    do {
        cout << "\n--- Laporan ---\n1. Rekap peminjaman per kategori x bulan\n"
                "2. Total denda seluruh transaksi\n"
                "3. Riwayat transaksi satu anggota\n0. Kembali\n";
        if (!bacaAngka("Pilihan: ", pilihan, 0, 3)) {
            if (cin.fail()) return;
            pilihan = -1;
            continue;
        }
        if (pilihan == 1) {
            int tahun = 0;
            if (!bacaAngka("Tahun (1900-2100, 0 = semua tahun) : ", tahun, 0, 2100)) continue;
            if (tahun != 0 && tahun < 1900) {
                cout << ">> Tahun harus 0 atau antara 1900 dan 2100.\n";
                continue;
            }
            int rekap[JML_KATEGORI][JML_BULAN];
            isiRekap(t, b, rekap, tahun);
            tampilRekap(rekap, NAMA_KATEGORI, tahun);
        } else if (pilihan == 2) {
            cout << "Total denda dari " << t.n << " transaksi: "
                 << formatRupiah(totalDendaRekursif(t.data, t.n)) << "\n";
        } else if (pilihan == 3) {
            string id;
            if (!bacaIdAnggota("ID anggota : ", id)) continue;
            const Anggota* pa = cariAnggota(a, id);
            if (pa == nullptr) {
                cout << ">> Anggota tidak ditemukan.\n";
                continue;
            }
            cout << "\nRiwayat " << pa->id << " - " << pa->nama << "\n"
                 << "No    Buku    Tgl pinjam   Tgl kembali  Denda\n"
                 << string(52, '-') << "\n";
            int jumlah = riwayatAnggotaRekursif(t.data, t.n, id);
            if (jumlah == 0) cout << "(belum ada transaksi)\n";
            else cout << jumlah << " transaksi.\n";
        }
    } while (pilihan != 0);
}

// -------------------------------------------- berkas & data contoh
void menuBerkas(DaftarBuku& b, DaftarAnggota& a, DaftarTransaksi& t) {
    int pilihan = -1;
    do {
        cout << "\n--- Berkas & Data Contoh ---\n1. Simpan data ke " << BERKAS_DATA
             << "\n2. Muat data dari " << BERKAS_DATA << "\n3. Isi data contoh\n0. Kembali\n";
        if (!bacaAngka("Pilihan: ", pilihan, 0, 3)) {
            if (cin.fail()) return;
            pilihan = -1;
            continue;
        }
        if (pilihan == 1) {
            cout << (simpanData(b, a, t, BERKAS_DATA) ? ">> Data disimpan.\n"
                                                      : ">> Gagal menyimpan data.\n");
        } else if (pilihan == 2) {
            if (!konfirmasi("Data yang sedang ada akan diganti. Lanjut?")) continue;
            cout << (muatData(b, a, t, BERKAS_DATA)
                         ? ">> Data dimuat.\n"
                         : ">> Gagal memuat: berkas tidak ada atau isinya tidak valid. "
                           "Data lama tidak berubah.\n");
        } else if (pilihan == 3) {
            cout << (isiDataContoh(b, a, t)
                         ? ">> Data contoh diisi (8 buku, 4 anggota, 10 transaksi).\n"
                         : ">> Data contoh hanya bisa diisi saat semua data masih kosong.\n");
        }
    } while (pilihan != 0);
}
