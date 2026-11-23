// uji.cpp - pengujian otomatis SiPus (F1-F8 + fitur berkas + memori)
// Kompilasi: g++ -std=c++17 -Wall -o uji uji.cpp buku.cpp anggota.cpp
//            transaksi.cpp laporan.cpp berkas.cpp util.cpp
#include "sipus.h"
#include <iostream>
#include <cstdio>
using namespace std;

static int jumlahUji = 0, jumlahLulus = 0;

static void cek(const string& nama, bool kondisi) {
    jumlahUji++;
    if (kondisi) jumlahLulus++;
    cout << (kondisi ? "[LULUS] " : "[GAGAL] ") << nama << "\n";
}

static void judul(const string& s) { cout << "\n== " << s << " ==\n"; }

static Buku mk(const char* kode, const char* jdl, const char* penulis,
               int kat, int tahun, int stok) {
    Buku b = {kode, jdl, penulis, kat, tahun, stok, 0};
    return b;
}

static Anggota ag(const char* id, const char* nama) {
    Anggota a = {id, nama, "Teknologi Informasi", 0};
    return a;
}

int main() {
    DaftarBuku buku;
    DaftarAnggota anggota;
    DaftarTransaksi trans;
    inisialisasi(buku, 2);          // kapasitas kecil agar pembesaran teruji
    inisialisasi(anggota, 2);
    inisialisasi(trans, 2);

    // ------------------------------------------------------------- F1
    judul("F1: Kelola buku");
    cek("F1.1 tambah buku pertama berhasil",
        tambahBuku(buku, mk("BK-002", "Struktur Data", "Wati", 0, 2024, 3)));
    tambahBuku(buku, mk("BK-001", "Algoritma Dasar", "Budi", 0, 2023, 1));
    cek("F1.2 array membesar otomatis (3 buku, kapasitas awal 2)",
        tambahBuku(buku, mk("BK-003", "Basis Data", "Sari", 1, 2022, 2)) &&
            buku.n == 3 && buku.kapasitas >= 3);
    cek("F1.3 kode ganda ditolak",
        !tambahBuku(buku, mk("BK-001", "Duplikat", "X", 0, 2020, 1)) && buku.n == 3);
    cek("F1.4 format kode salah ditolak",
        !tambahBuku(buku, mk("BUKU-9", "Salah", "X", 0, 2020, 1)));
    cek("F1.5 kategori di luar rentang ditolak",
        !tambahBuku(buku, mk("BK-010", "Salah", "X", 9, 2020, 1)));
    cek("F1.6 stok negatif ditolak",
        !tambahBuku(buku, mk("BK-011", "Salah", "X", 0, 2020, -1)));
    cek("F1.7 judul memuat '|' ditolak",
        !tambahBuku(buku, mk("BK-012", "A|B", "X", 0, 2020, 1)));
    cek("F1.8 ubahBuku mengganti judul",
        ubahBuku(buku, "BK-003", mk("XX", "Basis Data Lanjut", "Sari", 1, 2022, 2)) &&
            cariKode(buku, "BK-003")->judul == "Basis Data Lanjut");
    cek("F1.9 ubahBuku mempertahankan kode",
        cariKode(buku, "BK-003") != nullptr && cariKode(buku, "XX") == nullptr);
    cek("F1.10 hapusBuku kode tidak ada -> false", !hapusBuku(buku, "BK-999"));
    tambahBuku(buku, mk("BK-004", "Sementara", "Z", 4, 2021, 1));
    cek("F1.11 hapusBuku berhasil dan n berkurang",
        hapusBuku(buku, "BK-004") && buku.n == 3 && cariKode(buku, "BK-004") == nullptr);

    // ------------------------------------------------------------- F2
    judul("F2: Kelola anggota");
    cek("F2.1 tambah anggota berhasil", tambahAnggota(anggota, ag("AG-001", "Ayu")));
    tambahAnggota(anggota, ag("AG-002", "Gede"));
    cek("F2.2 array anggota membesar otomatis",
        tambahAnggota(anggota, ag("AG-003", "Made")) && anggota.n == 3 &&
            anggota.kapasitas >= 3);
    cek("F2.3 ID ganda ditolak", !tambahAnggota(anggota, ag("AG-001", "Lain")));
    cek("F2.4 format ID salah ditolak", !tambahAnggota(anggota, ag("A1", "Salah")));
    cek("F2.5 cariAnggota tidak ada -> nullptr", cariAnggota(anggota, "AG-999") == nullptr);
    cek("F2.6 ubahAnggota mengganti nama",
        ubahAnggota(anggota, "AG-002", "Gede Pratama", "Sistem Informasi") &&
            cariAnggota(anggota, "AG-002")->nama == "Gede Pratama");

    // ------------------------------------------------------------- F3
    judul("F3: Peminjaman");
    Tanggal t1 = {3, 11, 2026};
    cek("F3.1 pinjam berhasil", pinjamBuku(trans, buku, anggota, "AG-001", "BK-002", t1));
    cek("F3.2 stok berkurang, sedangPinjam & totalDipinjam bertambah",
        cariKode(buku, "BK-002")->stok == 2 && cariKode(buku, "BK-002")->totalDipinjam == 1 &&
            cariAnggota(anggota, "AG-001")->sedangPinjam == 1 && trans.n == 1);
    cek("F3.3 bukuSedangDipinjam true untuk buku dipinjam", bukuSedangDipinjam(trans, "BK-002"));
    cek("F3.4 anggota tidak ada ditolak",
        !pinjamBuku(trans, buku, anggota, "AG-999", "BK-002", t1));
    cek("F3.5 buku tidak ada ditolak",
        !pinjamBuku(trans, buku, anggota, "AG-001", "BK-999", t1));
    cek("F3.6 tanggal tidak valid ditolak (30 Feb)",
        !pinjamBuku(trans, buku, anggota, "AG-001", "BK-001", Tanggal{30, 2, 2026}));
    cek("F3.7 pinjam buku ke-2 dan ke-3 berhasil",
        pinjamBuku(trans, buku, anggota, "AG-001", "BK-001", t1) &&
            pinjamBuku(trans, buku, anggota, "AG-001", "BK-003", t1));
    tambahBuku(buku, mk("BK-005", "Jaringan", "Wira", 2, 2020, 5));
    cek("F3.8 buku ke-4 ditolak (batas 3)",
        !pinjamBuku(trans, buku, anggota, "AG-001", "BK-005", t1) &&
            !alasanPinjamDitolak(buku, anggota, "AG-001", "BK-005", t1).empty());
    cek("F3.9 stok 0 ditolak (BK-001 stok 1 sudah habis)",
        cariKode(buku, "BK-001")->stok == 0 &&
            !pinjamBuku(trans, buku, anggota, "AG-002", "BK-001", t1));

    // ------------------------------------------------------------- F4
    judul("F4: Pengembalian & denda");
    cek("F4.1 selisihHari lintas tahun (30/12/2026 -> 2/1/2027 = 3)",
        selisihHari(Tanggal{30, 12, 2026}, Tanggal{2, 1, 2027}) == 3);
    cek("F4.2 selisihHari tahun kabisat (28/2/2028 -> 1/3/2028 = 2)",
        selisihHari(Tanggal{28, 2, 2028}, Tanggal{1, 3, 2028}) == 2);
    cek("F4.3 selisihHari tahun non-kabisat (28/2/2027 -> 1/3/2027 = 1)",
        selisihHari(Tanggal{28, 2, 2027}, Tanggal{1, 3, 2027}) == 1);
    cek("F4.4 tambahHari 3/11/2026 + 7 = 10/11/2026",
        formatTanggal(tambahHari(t1, LAMA_PINJAM)) == "10/11/2026");
    int denda = -1, terlambat = -1;
    cek("F4.5 kembali tepat waktu (hari ke-7): denda 0",
        kembalikanBuku(trans, buku, anggota, 1, Tanggal{10, 11, 2026}, denda, terlambat) &&
            denda == 0 && terlambat == 0);
    cek("F4.6 stok & sedangPinjam pulih setelah kembali",
        cariKode(buku, "BK-002")->stok == 3 && cariAnggota(anggota, "AG-001")->sedangPinjam == 2 &&
            trans.data[0].selesai);
    cek("F4.7 terlambat 3 hari: denda Rp3.000 (parameter keluaran)",
        kembalikanBuku(trans, buku, anggota, 2, Tanggal{13, 11, 2026}, denda, terlambat) &&
            terlambat == 3 && denda == 3000 && trans.data[1].denda == 3000);
    cek("F4.8 kembali dua kali ditolak",
        !kembalikanBuku(trans, buku, anggota, 2, Tanggal{14, 11, 2026}));
    cek("F4.9 nomor transaksi tidak ada ditolak",
        !kembalikanBuku(trans, buku, anggota, 99, Tanggal{14, 11, 2026}));
    cek("F4.10 tanggal kembali sebelum tanggal pinjam ditolak",
        !kembalikanBuku(trans, buku, anggota, 3, Tanggal{1, 11, 2026}) && !trans.data[2].selesai);
    cek("F4.11 bukuSedangDipinjam false setelah dikembalikan", !bukuSedangDipinjam(trans, "BK-002"));

    // ------------------------------------------------------------- F5
    judul("F5: Pencarian");
    cek("F5.1 cariKode ketemu", cariKode(buku, "BK-005") != nullptr);
    cek("F5.2 cariKode tidak ada -> nullptr", cariKode(buku, "BK-777") == nullptr);
    cek("F5.3 cari judul tidak peka huruf besar/kecil",
        hitungJudulCocok(buku, "STRUKTUR") == 1 && hitungJudulCocok(buku, "data") == 2);
    cek("F5.4 cari judul tidak ada -> 0", hitungJudulCocok(buku, "zzzz") == 0);
    urutkanKode(buku);
    cek("F5.5 binarySearchKode ketemu pada indeks benar",
        binarySearchKode(buku.data, 0, buku.n - 1, "BK-003") == 2);
    cek("F5.6 binarySearchKode di ujung awal & akhir",
        binarySearchKode(buku.data, 0, buku.n - 1, "BK-001") == 0 &&
            binarySearchKode(buku.data, 0, buku.n - 1, "BK-005") == buku.n - 1);
    cek("F5.7 binarySearchKode tidak ada -> -1",
        binarySearchKode(buku.data, 0, buku.n - 1, "BK-777") == -1);
    cek("F5.8 binarySearchKode pada array kosong -> -1",
        binarySearchKode(buku.data, 0, -1, "BK-001") == -1);

    // ------------------------------------------------------------- F6
    judul("F6: Pengurutan");
    bool terurut = true;
    for (int i = 0; i + 1 < buku.n; i++)
        if (buku.data[i].kode > buku.data[i + 1].kode) terurut = false;
    cek("F6.1 urutkanKode menaik", terurut);
    urutkanJudul(buku);
    terurut = true;
    for (int i = 0; i + 1 < buku.n; i++)
        if (keHurufKecil(buku.data[i].judul) > keHurufKecil(buku.data[i + 1].judul)) terurut = false;
    cek("F6.2 urutkanJudul menaik (alfabetis)", terurut);
    urutkanDipinjam(buku);
    terurut = true;
    for (int i = 0; i + 1 < buku.n; i++)
        if (buku.data[i].totalDipinjam < buku.data[i + 1].totalDipinjam) terurut = false;
    cek("F6.3 urutkanDipinjam menurun", terurut);
    cek("F6.4 record utuh ikut berpindah (kode & judul tetap cocok)",
        cariKode(buku, "BK-003")->judul == "Basis Data Lanjut" &&
            cariKode(buku, "BK-002")->judul == "Struktur Data");

    // ------------------------------------------------------------- F7
    judul("F7: Rekap kategori x bulan");
    int rekap[JML_KATEGORI][JML_BULAN];
    isiRekap(trans, buku, rekap);
    // Semua 3 transaksi dipinjam November 2026 (bulan ke-11 -> indeks 10).
    // BK-002 kat 0, BK-001 kat 0, BK-003 kat 1.
    cek("F7.1 sel [Pemrograman][Nov] = 2", rekap[0][10] == 2);
    cek("F7.2 sel [Basis Data][Nov] = 1", rekap[1][10] == 1);
    cek("F7.3 sel lain bernilai 0", rekap[2][10] == 0 && rekap[0][0] == 0);
    int totalSemua = 0;
    for (int i = 0; i < JML_KATEGORI; i++)
        for (int j = 0; j < JML_BULAN; j++) totalSemua += rekap[i][j];
    cek("F7.4 jumlah seluruh sel = jumlah transaksi", totalSemua == trans.n);
    cek("F7.5 kategori terpopuler = Pemrograman (indeks 0)", kategoriTerpopuler(rekap) == 0);
    isiRekap(trans, buku, rekap, 2025);
    cek("F7.6 filter tahun tanpa data -> semua 0 dan tidak ada terpopuler",
        kategoriTerpopuler(rekap) == -1);

    // ------------------------------------------------------------- F8
    judul("F8: Total denda & riwayat (rekursif)");
    cek("F8.1 totalDendaRekursif = Rp3.000", totalDendaRekursif(trans.data, trans.n) == 3000);
    cek("F8.2 totalDendaRekursif array kosong = 0", totalDendaRekursif(trans.data, 0) == 0);
    cek("F8.3 riwayat AG-001 = 3 transaksi", riwayatAnggotaRekursif(trans.data, trans.n, "AG-001", false) == 3);
    cek("F8.4 riwayat anggota tanpa transaksi = 0",
        riwayatAnggotaRekursif(trans.data, trans.n, "AG-003", false) == 0);

    // ------------------------------------------------- fitur tambahan: berkas
    judul("Fitur tambahan: simpan/muat berkas");
    const string berkas = "uji_tmp_sipus.txt";
    cek("B.1 simpanData berhasil", simpanData(buku, anggota, trans, berkas));
    DaftarBuku b2;
    DaftarAnggota a2;
    DaftarTransaksi t2;
    inisialisasi(b2, 1);
    inisialisasi(a2, 1);
    inisialisasi(t2, 1);
    cek("B.2 muatData berhasil", muatData(b2, a2, t2, berkas));
    cek("B.3 jumlah data sama setelah dimuat", b2.n == buku.n && a2.n == anggota.n && t2.n == trans.n);
    cek("B.4 isi data sama (judul, denda, tanggal kembali)",
        cariKode(b2, "BK-003") != nullptr && cariKode(b2, "BK-003")->judul == "Basis Data Lanjut" &&
            t2.data[1].denda == 3000 && formatTanggal(t2.data[1].tglKembali) == "13/11/2026");
    cek("B.5 muatData berkas tidak ada -> false dan data lama utuh",
        !muatData(b2, a2, t2, "tidak_ada.txt") && b2.n == buku.n);
    {
        FILE* f = fopen(berkas.c_str(), "a");
        if (f) { fputs("BUKU|BK-900|rusak\n", f); fclose(f); }
    }
    cek("B.6 berkas rusak ditolak dan data lama tidak berubah",
        !muatData(b2, a2, t2, berkas) && b2.n == buku.n);
    remove(berkas.c_str());
    bebaskan(b2);
    bebaskan(a2);
    bebaskan(t2);

    // ------------------------------------------------------------ memori
    judul("Memori dinamis");
    bebaskan(buku);
    bebaskan(anggota);
    bebaskan(trans);
    cek("M.1 bebaskan mengosongkan pointer",
        buku.data == nullptr && anggota.data == nullptr && trans.data == nullptr);
    cek("M.2 bebaskan mengosongkan n dan kapasitas", buku.n == 0 && buku.kapasitas == 0);
    cek("M.3 isiDataContoh mengisi data kosong dan menolak data tidak kosong", [] {
        DaftarBuku b; DaftarAnggota a; DaftarTransaksi t;
        inisialisasi(b, KAPASITAS_AWAL); inisialisasi(a, KAPASITAS_AWAL);
        inisialisasi(t, KAPASITAS_AWAL);
        bool ok = isiDataContoh(b, a, t) && b.n == 8 && a.n == 4 && t.n == 10 &&
                  !isiDataContoh(b, a, t) && totalDendaRekursif(t.data, t.n) == 14000;
        bebaskan(b); bebaskan(a); bebaskan(t);
        return ok;
    }());

    cout << "\n===============================\nHasil: " << jumlahLulus << " / "
         << jumlahUji << " kasus uji lulus\n";
    return jumlahLulus == jumlahUji ? 0 : 1;
}
