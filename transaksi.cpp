// transaksi.cpp - Modul Transaksi: F3 (pinjam), F4 (kembali + denda),
// F8 (total denda & riwayat anggota, rekursif)
#include "sipus.h"
#include <iostream>
#include <iomanip>
using namespace std;

void inisialisasi(DaftarTransaksi& d, int kapasitasAwal) {
    if (kapasitasAwal < 1) kapasitasAwal = 1;
    d.data = new Transaksi[kapasitasAwal]();
    d.n = 0;
    d.kapasitas = kapasitasAwal;
}

void bebaskan(DaftarTransaksi& d) {
    delete[] d.data;
    d.data = nullptr;
    d.n = 0;
    d.kapasitas = 0;
}

static void perbesar(DaftarTransaksi& d) {
    int kapasitasBaru = d.kapasitas > 0 ? d.kapasitas * 2 : 1;
    Transaksi* baru = new Transaksi[kapasitasBaru]();
    for (int i = 0; i < d.n; i++) baru[i] = d.data[i];
    delete[] d.data;
    d.data = baru;
    d.kapasitas = kapasitasBaru;
}

bool tambahTransaksi(DaftarTransaksi& d, const Transaksi& tr) {
    if (d.n == d.kapasitas) perbesar(d);
    d.data[d.n] = tr;
    d.n++;
    return true;
}

// Transaksi tidak pernah dihapus, tetapi nomor dicari dari yang terbesar
// agar tetap aman setelah data dimuat dari berkas.
int nomorBerikut(const DaftarTransaksi& d) {
    int maks = 0;
    for (int i = 0; i < d.n; i++)
        if (d.data[i].nomor > maks) maks = d.data[i].nomor;
    return maks + 1;
}

Transaksi* cariTransaksi(DaftarTransaksi& d, int nomor) {
    for (int i = 0; i < d.n; i++)
        if (d.data[i].nomor == nomor) return &d.data[i];
    return nullptr;
}

bool bukuSedangDipinjam(const DaftarTransaksi& t, const string& kode) {
    for (int i = 0; i < t.n; i++)
        if (!t.data[i].selesai && t.data[i].kodeBuku == kode) return true;
    return false;
}

void tampilPinjamanAktif(const DaftarTransaksi& t) {
    int jumlah = 0;
    for (int i = 0; i < t.n; i++) {
        const Transaksi& tr = t.data[i];
        if (tr.selesai) continue;
        if (jumlah == 0) {
            cout << left << setw(7) << "No" << setw(8) << "Anggota"
                 << setw(8) << "Buku" << setw(13) << "Tgl pinjam"
                 << "Batas kembali\n";
            cout << string(50, '-') << "\n";
        }
        cout << left << setw(7) << tr.nomor << setw(8) << tr.idAnggota
             << setw(8) << tr.kodeBuku << setw(13) << formatTanggal(tr.tglPinjam)
             << formatTanggal(tambahHari(tr.tglPinjam, LAMA_PINJAM)) << "\n";
        jumlah++;
    }
    if (jumlah == 0) cout << "(tidak ada buku yang sedang dipinjam)\n";
}

// ------------------------------------------------------------- F3: pinjam
string alasanPinjamDitolak(const DaftarBuku& b, const DaftarAnggota& a,
                           const string& idAg, const string& kodeBk,
                           const Tanggal& tgl) {
    if (!tanggalValid(tgl)) return "Tanggal tidak valid.";
    const Anggota* pa = cariAnggota(a, idAg);
    if (pa == nullptr) return "Anggota tidak ditemukan.";
    const Buku* pb = cariKode(b, kodeBk);
    if (pb == nullptr) return "Buku tidak ditemukan.";
    if (pb->stok <= 0) return "Stok buku habis.";
    if (pa->sedangPinjam >= MAKS_PINJAM)
        return "Anggota sudah mencapai batas peminjaman (maksimal " +
               to_string(MAKS_PINJAM) + " buku).";
    return "";
}

bool pinjamBuku(DaftarTransaksi& t, DaftarBuku& b, DaftarAnggota& a,
                const string& idAg, const string& kodeBk, const Tanggal& tgl) {
    if (!alasanPinjamDitolak(b, a, idAg, kodeBk, tgl).empty()) return false;
    Buku* pb = cariKode(b, kodeBk);
    Anggota* pa = cariAnggota(a, idAg);

    Transaksi tr = {};
    tr.nomor = nomorBerikut(t);
    tr.idAnggota = idAg;
    tr.kodeBuku = kodeBk;
    tr.tglPinjam = tgl;
    tr.tglKembali = Tanggal{0, 0, 0};
    tr.selesai = false;
    tr.denda = 0;
    if (!tambahTransaksi(t, tr)) return false;

    pb->stok--;
    pb->totalDipinjam++;
    pa->sedangPinjam++;
    return true;
}

// ------------------------------------------------------- F4: kembali
int selisihHari(const Tanggal& awal, const Tanggal& akhir) {
    return hariKeAngka(akhir) - hariKeAngka(awal);
}

// Parameter keluaran hariTerlambat; nilai kembalian adalah denda (Rupiah).
int hitungDenda(const Tanggal& pinjam, const Tanggal& kembali, int& hariTerlambat) {
    int lama = selisihHari(pinjam, kembali);
    hariTerlambat = lama > LAMA_PINJAM ? lama - LAMA_PINJAM : 0;
    return hariTerlambat * DENDA_HARIAN;
}

bool kembalikanBuku(DaftarTransaksi& t, DaftarBuku& b, DaftarAnggota& a,
                    int nomor, const Tanggal& tgl, int& denda, int& hariTerlambat) {
    denda = 0;
    hariTerlambat = 0;
    if (!tanggalValid(tgl)) return false;
    Transaksi* tr = cariTransaksi(t, nomor);
    if (tr == nullptr || tr->selesai) return false;
    if (selisihHari(tr->tglPinjam, tgl) < 0) return false;   // tidak boleh mundur
    Buku* pb = cariKode(b, tr->kodeBuku);
    Anggota* pa = cariAnggota(a, tr->idAnggota);
    if (pb == nullptr || pa == nullptr) return false;

    denda = hitungDenda(tr->tglPinjam, tgl, hariTerlambat);
    tr->tglKembali = tgl;
    tr->selesai = true;
    tr->denda = denda;
    pb->stok++;
    if (pa->sedangPinjam > 0) pa->sedangPinjam--;
    return true;
}

bool kembalikanBuku(DaftarTransaksi& t, DaftarBuku& b, DaftarAnggota& a,
                    int nomor, const Tanggal& tgl) {
    int denda = 0, terlambat = 0;
    return kembalikanBuku(t, b, a, nomor, tgl, denda, terlambat);
}

// -------------------------------------------------------- F8: rekursif
// Jumlah denda arr[0..n-1] = denda elemen terakhir + jumlah sisanya.
int totalDendaRekursif(const Transaksi arr[], int n) {
    if (n <= 0) return 0;                                   // basis
    return arr[n - 1].denda + totalDendaRekursif(arr, n - 1);
}

// Rekursi turun dulu ke n = 0, baru mencetak saat naik kembali, sehingga
// riwayat tercetak berurutan dari transaksi terlama ke terbaru.
int riwayatAnggotaRekursif(const Transaksi arr[], int n,
                           const string& idAg, bool cetak) {
    if (n <= 0) return 0;                                   // basis
    int jumlah = riwayatAnggotaRekursif(arr, n - 1, idAg, cetak);
    if (arr[n - 1].idAnggota == idAg) {
        jumlah++;
        if (cetak) {
            const Transaksi& tr = arr[n - 1];
            cout << left << setw(6) << tr.nomor << setw(8) << tr.kodeBuku
                 << setw(13) << formatTanggal(tr.tglPinjam);
            if (tr.selesai)
                cout << setw(13) << formatTanggal(tr.tglKembali)
                     << formatRupiah(tr.denda) << "\n";
            else
                cout << "(belum kembali)\n";
        }
    }
    return jumlah;
}
