// util.cpp - utilitas string, tanggal, dan pembacaan masukan
#include "sipus.h"
#include <iostream>
#include <sstream>
#include <cctype>
using namespace std;

const string NAMA_KATEGORI[JML_KATEGORI] = {
    "Pemrograman", "Basis Data", "Jaringan", "Matematika", "Umum"
};
const string NAMA_BULAN[JML_BULAN] = {
    "Jan", "Feb", "Mar", "Apr", "Mei", "Jun",
    "Jul", "Agu", "Sep", "Okt", "Nov", "Des"
};

static const size_t MAKS_TEKS = 100;

// ---------------------------------------------------------------- string
string keHurufKecil(const string& s) {
    string h = s;
    for (size_t i = 0; i < h.size(); i++)
        h[i] = (char)tolower((unsigned char)h[i]);
    return h;
}

string keHurufBesar(const string& s) {
    string h = s;
    for (size_t i = 0; i < h.size(); i++)
        h[i] = (char)toupper((unsigned char)h[i]);
    return h;
}

string pangkas(const string& s) {
    size_t awal = 0, akhir = s.size();
    while (awal < akhir && isspace((unsigned char)s[awal])) awal++;
    while (akhir > awal && isspace((unsigned char)s[akhir - 1])) akhir--;
    return s.substr(awal, akhir - awal);
}

string potong(const string& s, size_t lebar) {
    if (s.size() <= lebar) return s;
    if (lebar <= 2) return s.substr(0, lebar);
    return s.substr(0, lebar - 2) + "..";
}

string formatRupiah(int nilai) {
    string angka = to_string(nilai < 0 ? -nilai : nilai);
    string hasil;
    int hitung = 0;
    for (int i = (int)angka.size() - 1; i >= 0; i--) {
        hasil.insert(hasil.begin(), angka[i]);
        hitung++;
        if (hitung % 3 == 0 && i > 0) hasil.insert(hasil.begin(), '.');
    }
    return string(nilai < 0 ? "-Rp" : "Rp") + hasil;
}

bool mengandungKata(const string& teks, const string& kata) {
    return keHurufKecil(teks).find(keHurufKecil(kata)) != string::npos;
}

static bool formatKode(const string& s, const char* awalan) {
    if (s.size() != 6) return false;
    if (s.compare(0, 3, awalan) != 0) return false;
    for (int i = 3; i < 6; i++)
        if (!isdigit((unsigned char)s[i])) return false;
    return true;
}

bool formatKodeBuku(const string& s) { return formatKode(s, "BK-"); }
bool formatIdAnggota(const string& s) { return formatKode(s, "AG-"); }

// Teks yang boleh disimpan: tidak kosong, tidak memuat '|' (pemisah berkas)
// atau karakter kontrol, dan maksimal MAKS_TEKS karakter.
bool teksAman(const string& s) {
    string t = pangkas(s);
    if (t.empty() || t.size() > MAKS_TEKS) return false;
    for (size_t i = 0; i < t.size(); i++)
        if (t[i] == '|' || iscntrl((unsigned char)t[i])) return false;
    return true;
}

bool parseInt(const string& s, int& hasil) {
    string t = pangkas(s);
    if (t.empty()) return false;
    size_t i = 0;
    bool negatif = false;
    if (t[0] == '-' || t[0] == '+') {
        negatif = (t[0] == '-');
        i = 1;
    }
    if (i >= t.size() || t.size() - i > 9) return false;
    long nilai = 0;
    for (; i < t.size(); i++) {
        if (!isdigit((unsigned char)t[i])) return false;
        nilai = nilai * 10 + (t[i] - '0');
    }
    hasil = (int)(negatif ? -nilai : nilai);
    return true;
}

// --------------------------------------------------------------- tanggal
static bool kabisat(int tahun) {
    return (tahun % 4 == 0 && tahun % 100 != 0) || tahun % 400 == 0;
}

static int hariDalamBulan(int bulan, int tahun) {
    static const int hari[JML_BULAN] = {31, 28, 31, 30, 31, 30,
                                        31, 31, 30, 31, 30, 31};
    if (bulan == 2 && kabisat(tahun)) return 29;
    return hari[bulan - 1];
}

bool tanggalValid(const Tanggal& t) {
    if (t.tahun < 1900 || t.tahun > 2100) return false;
    if (t.bulan < 1 || t.bulan > 12) return false;
    return t.hari >= 1 && t.hari <= hariDalamBulan(t.bulan, t.tahun);
}

// Konversi tanggal -> jumlah hari sejak 1970-01-01 (algoritma days_from_civil).
int hariKeAngka(const Tanggal& t) {
    int y = t.tahun, m = t.bulan, d = t.hari;
    if (m <= 2) y -= 1;
    int era = (y >= 0 ? y : y - 399) / 400;
    int yoe = y - era * 400;
    int doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + doe - 719468;
}

// Kebalikan hariKeAngka (algoritma civil_from_days).
Tanggal angkaKeTanggal(int z) {
    z += 719468;
    int era = (z >= 0 ? z : z - 146096) / 146097;
    int doe = z - era * 146097;
    int yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    int y = yoe + era * 400;
    int doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    int mp = (5 * doy + 2) / 153;
    Tanggal t;
    t.hari = doy - (153 * mp + 2) / 5 + 1;
    t.bulan = mp < 10 ? mp + 3 : mp - 9;
    t.tahun = y + (t.bulan <= 2 ? 1 : 0);
    return t;
}

Tanggal tambahHari(const Tanggal& t, int n) {
    return angkaKeTanggal(hariKeAngka(t) + n);
}

string formatTanggal(const Tanggal& t) {
    string h = to_string(t.hari), b = to_string(t.bulan);
    if (h.size() < 2) h = "0" + h;
    if (b.size() < 2) b = "0" + b;
    return h + "/" + b + "/" + to_string(t.tahun);
}

// Menerima "3 11 2026", "3/11/2026", atau "3-11-2026".
bool parseTanggal(const string& s, Tanggal& t) {
    string x = s;
    for (size_t i = 0; i < x.size(); i++)
        if (x[i] == '/' || x[i] == '-') x[i] = ' ';
    istringstream iss(x);
    string a, b, c, lebih;
    if (!(iss >> a >> b >> c)) return false;
    if (iss >> lebih) return false;
    Tanggal tmp = {0, 0, 0};
    if (!parseInt(a, tmp.hari) || !parseInt(b, tmp.bulan) ||
        !parseInt(c, tmp.tahun))
        return false;
    if (!tanggalValid(tmp)) return false;
    t = tmp;
    return true;
}

// --------------------------------------------------------------- masukan
bool bacaBaris(const string& prompt, string& hasil) {
    cout << prompt;
    string baris;
    if (!getline(cin, baris)) return false;
    baris = pangkas(baris);
    if (keHurufKecil(baris) == "batal") {
        cout << ">> Dibatalkan.\n";
        return false;
    }
    hasil = baris;
    return true;
}

bool bacaTeks(const string& prompt, string& hasil) {
    string s;
    while (true) {
        if (!bacaBaris(prompt, s)) return false;
        if (teksAman(s)) {
            hasil = s;
            return true;
        }
        cout << ">> Teks tidak boleh kosong, tidak boleh memuat '|', "
                "dan maksimal 100 karakter.\n";
    }
}

bool bacaAngka(const string& prompt, int& hasil, int min, int maks) {
    string s;
    int nilai = 0;
    while (true) {
        if (!bacaBaris(prompt, s)) return false;
        if (parseInt(s, nilai) && nilai >= min && nilai <= maks) {
            hasil = nilai;
            return true;
        }
        cout << ">> Masukan tidak valid. Isi angka antara " << min
             << " dan " << maks << ".\n";
    }
}

bool bacaKodeBuku(const string& prompt, string& hasil) {
    string s;
    while (true) {
        if (!bacaBaris(prompt, s)) return false;
        s = keHurufBesar(s);
        if (formatKodeBuku(s)) {
            hasil = s;
            return true;
        }
        cout << ">> Format kode harus BK-xxx (contoh: BK-001).\n";
    }
}

bool bacaIdAnggota(const string& prompt, string& hasil) {
    string s;
    while (true) {
        if (!bacaBaris(prompt, s)) return false;
        s = keHurufBesar(s);
        if (formatIdAnggota(s)) {
            hasil = s;
            return true;
        }
        cout << ">> Format ID harus AG-xxx (contoh: AG-001).\n";
    }
}

bool bacaTanggal(const string& prompt, Tanggal& hasil) {
    string s;
    while (true) {
        if (!bacaBaris(prompt, s)) return false;
        if (parseTanggal(s, hasil)) return true;
        cout << ">> Tanggal tidak valid. Gunakan format: hari bulan tahun "
                "(contoh: 3 11 2026).\n";
    }
}

bool konfirmasi(const string& prompt) {
    string s;
    while (true) {
        if (!bacaBaris(prompt + " (y/n): ", s)) return false;
        s = keHurufKecil(s);
        if (s == "y" || s == "ya") return true;
        if (s == "n" || s == "t" || s == "tidak") return false;
        cout << ">> Jawab dengan y atau n.\n";
    }
}
