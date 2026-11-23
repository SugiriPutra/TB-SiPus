// berkas.cpp - Fitur tambahan: simpan/muat data ke berkas teks (<fstream>)
//
// Format (satu record per baris, dipisah tanda '|'):
//   BUKU|kode|judul|penulis|kategori|tahun|stok|totalDipinjam
//   ANGGOTA|id|nama|prodi|sedangPinjam
//   TRANSAKSI|nomor|idAnggota|kodeBuku|hP|bP|tP|hK|bK|tK|selesai|denda
#include "sipus.h"
#include <fstream>
using namespace std;

bool simpanData(const DaftarBuku& b, const DaftarAnggota& a,
                const DaftarTransaksi& t, const string& namaBerkas) {
    ofstream f(namaBerkas.c_str());
    if (!f) return false;
    f << "# SiPus data v1\n";
    for (int i = 0; i < b.n; i++) {
        const Buku& x = b.data[i];
        f << "BUKU|" << x.kode << '|' << x.judul << '|' << x.penulis << '|'
          << x.kategori << '|' << x.tahun << '|' << x.stok << '|'
          << x.totalDipinjam << '\n';
    }
    for (int i = 0; i < a.n; i++) {
        const Anggota& x = a.data[i];
        f << "ANGGOTA|" << x.id << '|' << x.nama << '|' << x.prodi << '|'
          << x.sedangPinjam << '\n';
    }
    for (int i = 0; i < t.n; i++) {
        const Transaksi& x = t.data[i];
        f << "TRANSAKSI|" << x.nomor << '|' << x.idAnggota << '|'
          << x.kodeBuku << '|' << x.tglPinjam.hari << '|' << x.tglPinjam.bulan
          << '|' << x.tglPinjam.tahun << '|' << x.tglKembali.hari << '|'
          << x.tglKembali.bulan << '|' << x.tglKembali.tahun << '|'
          << (x.selesai ? 1 : 0) << '|' << x.denda << '\n';
    }
    f.close();
    return !f.fail();
}

// Memecah 'baris' pada '|'; mengembalikan jumlah bagian (maksimal 'maks').
static int pecah(const string& baris, string bagian[], int maks) {
    int jumlah = 0;
    size_t awal = 0;
    while (jumlah < maks) {
        size_t pos = baris.find('|', awal);
        if (pos == string::npos) {
            bagian[jumlah++] = baris.substr(awal);
            break;
        }
        bagian[jumlah++] = baris.substr(awal, pos - awal);
        awal = pos + 1;
    }
    return jumlah;
}

static bool parseBuku(const string bagian[], Buku& hasil) {
    Buku x = {"", "", "", 0, 0, 0, 0};
    x.kode = bagian[1];
    x.judul = bagian[2];
    x.penulis = bagian[3];
    if (!parseInt(bagian[4], x.kategori) || !parseInt(bagian[5], x.tahun) ||
        !parseInt(bagian[6], x.stok) || !parseInt(bagian[7], x.totalDipinjam))
        return false;
    hasil = x;
    return true;
}

static bool parseAnggota(const string bagian[], Anggota& hasil) {
    Anggota x = {"", "", "", 0};
    x.id = bagian[1];
    x.nama = bagian[2];
    x.prodi = bagian[3];
    if (!parseInt(bagian[4], x.sedangPinjam)) return false;
    hasil = x;
    return true;
}

static bool parseTransaksi(const string bagian[], Transaksi& hasil) {
    Transaksi x = {};
    int selesai = 0;
    x.idAnggota = bagian[2];
    x.kodeBuku = bagian[3];
    if (!parseInt(bagian[1], x.nomor) ||
        !parseInt(bagian[4], x.tglPinjam.hari) ||
        !parseInt(bagian[5], x.tglPinjam.bulan) ||
        !parseInt(bagian[6], x.tglPinjam.tahun) ||
        !parseInt(bagian[7], x.tglKembali.hari) ||
        !parseInt(bagian[8], x.tglKembali.bulan) ||
        !parseInt(bagian[9], x.tglKembali.tahun) ||
        !parseInt(bagian[10], selesai) || !parseInt(bagian[11], x.denda))
        return false;
    if (x.nomor < 1 || x.denda < 0 || (selesai != 0 && selesai != 1)) return false;
    if (!formatIdAnggota(x.idAnggota) || !formatKodeBuku(x.kodeBuku)) return false;
    if (!tanggalValid(x.tglPinjam)) return false;
    x.selesai = (selesai == 1);
    if (x.selesai) {
        if (!tanggalValid(x.tglKembali)) return false;
    } else {
        x.tglKembali = Tanggal{0, 0, 0};
    }
    hasil = x;
    return true;
}

// Data dibaca ke daftar sementara dulu. Daftar asli baru diganti jika seluruh
// berkas valid, sehingga berkas rusak tidak merusak data yang sedang dipakai.
bool muatData(DaftarBuku& b, DaftarAnggota& a, DaftarTransaksi& t,
              const string& namaBerkas) {
    ifstream f(namaBerkas.c_str());
    if (!f) return false;

    DaftarBuku tb;
    DaftarAnggota ta;
    DaftarTransaksi tt;
    inisialisasi(tb, KAPASITAS_AWAL);
    inisialisasi(ta, KAPASITAS_AWAL);
    inisialisasi(tt, KAPASITAS_AWAL);

    bool ok = true;
    string baris;
    string bagian[13];
    while (ok && getline(f, baris)) {
        baris = pangkas(baris);
        if (baris.empty() || baris[0] == '#') continue;
        int jml = pecah(baris, bagian, 13);
        if (bagian[0] == "BUKU" && jml == 8) {
            Buku x = {"", "", "", 0, 0, 0, 0};
            ok = parseBuku(bagian, x) && tambahBuku(tb, x);
        } else if (bagian[0] == "ANGGOTA" && jml == 5) {
            Anggota x = {"", "", "", 0};
            ok = parseAnggota(bagian, x) && tambahAnggota(ta, x);
        } else if (bagian[0] == "TRANSAKSI" && jml == 12) {
            Transaksi x = {};
            ok = parseTransaksi(bagian, x) &&
                 cariTransaksi(tt, x.nomor) == nullptr && tambahTransaksi(tt, x);
        } else {
            ok = false;
        }
    }

    if (ok) {
        bebaskan(b);
        bebaskan(a);
        bebaskan(t);
        b = tb;
        a = ta;
        t = tt;
    } else {
        bebaskan(tb);
        bebaskan(ta);
        bebaskan(tt);
    }
    return ok;
}

// ------------------------------------------------------------ data contoh
struct ContohPinjam {
    const char* idAnggota;
    const char* kodeBuku;
    Tanggal pinjam;
    Tanggal kembali;    // hari = 0 -> belum dikembalikan
};

// Hanya diisi saat ketiga daftar masih kosong. Dipakai untuk demo dan uji manual.
bool isiDataContoh(DaftarBuku& b, DaftarAnggota& a, DaftarTransaksi& t) {
    if (b.n > 0 || a.n > 0 || t.n > 0) return false;

    const Buku buku[] = {
        {"BK-001", "Algoritma dan Pemrograman", "Budi Santoso", 0, 2023, 3, 0},
        {"BK-002", "Struktur Data dengan C++", "Wati Lestari", 0, 2024, 2, 0},
        {"BK-003", "Basis Data Relasional", "Sari Dewi", 1, 2022, 2, 0},
        {"BK-004", "SQL untuk Pemula", "Agus Pratama", 1, 2021, 1, 0},
        {"BK-005", "Jaringan Komputer", "Made Wijaya", 2, 2020, 2, 0},
        {"BK-006", "Kalkulus Dasar", "Ketut Arya", 3, 2019, 3, 0},
        {"BK-007", "Aljabar Linier", "Putu Ayu", 3, 2022, 1, 0},
        {"BK-008", "Etika Profesi TI", "Komang Adi", 4, 2023, 2, 0},
    };
    const Anggota anggota[] = {
        {"AG-001", "Ayu Lestari", "Teknologi Informasi", 0},
        {"AG-002", "Gede Pratama", "Teknologi Informasi", 0},
        {"AG-003", "Made Dewi", "Sistem Informasi", 0},
        {"AG-004", "Ketut Wirawan", "Teknologi Informasi", 0},
    };
    const ContohPinjam pinjam[] = {
        {"AG-001", "BK-001", {5, 1, 2026}, {10, 1, 2026}},
        {"AG-002", "BK-003", {12, 2, 2026}, {25, 2, 2026}},   // terlambat 6 hari
        {"AG-001", "BK-005", {3, 3, 2026}, {9, 3, 2026}},
        {"AG-003", "BK-006", {15, 4, 2026}, {22, 4, 2026}},
        {"AG-004", "BK-002", {2, 5, 2026}, {14, 5, 2026}},    // terlambat 5 hari
        {"AG-002", "BK-001", {20, 6, 2026}, {25, 6, 2026}},
        {"AG-001", "BK-001", {1, 9, 2026}, {8, 9, 2026}},
        {"AG-003", "BK-004", {10, 10, 2026}, {20, 10, 2026}}, // terlambat 3 hari
        {"AG-004", "BK-006", {3, 11, 2026}, {0, 0, 0}},       // masih dipinjam
        {"AG-002", "BK-008", {5, 11, 2026}, {0, 0, 0}},       // masih dipinjam
    };

    for (size_t i = 0; i < sizeof(buku) / sizeof(buku[0]); i++)
        tambahBuku(b, buku[i]);
    for (size_t i = 0; i < sizeof(anggota) / sizeof(anggota[0]); i++)
        tambahAnggota(a, anggota[i]);
    for (size_t i = 0; i < sizeof(pinjam) / sizeof(pinjam[0]); i++) {
        const ContohPinjam& c = pinjam[i];
        if (!pinjamBuku(t, b, a, c.idAnggota, c.kodeBuku, c.pinjam)) continue;
        if (c.kembali.hari != 0)
            kembalikanBuku(t, b, a, t.data[t.n - 1].nomor, c.kembali);
    }
    return true;
}
