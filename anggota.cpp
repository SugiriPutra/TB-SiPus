// anggota.cpp - Modul Anggota: F2 (kelola anggota)
#include "sipus.h"
#include <iostream>
#include <iomanip>
using namespace std;

void inisialisasi(DaftarAnggota& d, int kapasitasAwal) {
    if (kapasitasAwal < 1) kapasitasAwal = 1;
    d.data = new Anggota[kapasitasAwal]();
    d.n = 0;
    d.kapasitas = kapasitasAwal;
}

void bebaskan(DaftarAnggota& d) {
    delete[] d.data;
    d.data = nullptr;
    d.n = 0;
    d.kapasitas = 0;
}

static void perbesar(DaftarAnggota& d) {
    int kapasitasBaru = d.kapasitas > 0 ? d.kapasitas * 2 : 1;
    Anggota* baru = new Anggota[kapasitasBaru]();
    for (int i = 0; i < d.n; i++) baru[i] = d.data[i];
    delete[] d.data;
    d.data = baru;
    d.kapasitas = kapasitasBaru;
}

static bool anggotaValid(const Anggota& a) {
    return formatIdAnggota(a.id) && teksAman(a.nama) && teksAman(a.prodi) &&
           a.sedangPinjam >= 0 && a.sedangPinjam <= MAKS_PINJAM;
}

bool tambahAnggota(DaftarAnggota& d, const Anggota& a) {
    if (!anggotaValid(a)) return false;
    if (cariAnggota(d, a.id) != nullptr) return false;  // ID harus unik
    if (d.n == d.kapasitas) perbesar(d);
    d.data[d.n] = a;
    d.n++;
    return true;
}

bool ubahAnggota(DaftarAnggota& d, const string& id,
                 const string& nama, const string& prodi) {
    Anggota* p = cariAnggota(d, id);
    if (p == nullptr) return false;
    if (!teksAman(nama) || !teksAman(prodi)) return false;
    p->nama = nama;
    p->prodi = prodi;
    return true;
}

Anggota* cariAnggota(DaftarAnggota& d, const string& id) {
    for (int i = 0; i < d.n; i++)
        if (d.data[i].id == id) return &d.data[i];
    return nullptr;
}

const Anggota* cariAnggota(const DaftarAnggota& d, const string& id) {
    for (int i = 0; i < d.n; i++)
        if (d.data[i].id == id) return &d.data[i];
    return nullptr;
}

void tampilAnggota(const DaftarAnggota& d) {
    if (d.n == 0) {
        cout << "(belum ada data anggota)\n";
        return;
    }
    cout << left << setw(8) << "ID" << setw(26) << "Nama" << setw(24)
         << "Prodi" << "Sedang pinjam\n";
    cout << string(70, '-') << "\n";
    for (int i = 0; i < d.n; i++) {
        cout << left << setw(8) << d.data[i].id
             << setw(26) << potong(d.data[i].nama, 24)
             << setw(24) << potong(d.data[i].prodi, 22)
             << d.data[i].sedangPinjam << "/" << MAKS_PINJAM << "\n";
    }
    cout << "Total " << d.n << " anggota (kapasitas array: " << d.kapasitas << ")\n";
}
