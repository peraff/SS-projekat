#pragma once 

#include <list> 
#include <map> 
#include <iterator>
#include <string>
#include <iostream>

using namespace std; 

class IzrazZapis{ 
public:
     string simbol; 
     char znak; 
     IzrazZapis(string s="greska", char c='/'): simbol(s), znak(c) {} 

};

class EquZapis {
public: 
     string simbol; 
     string izraz; 
     EquZapis(string s, string i) : simbol(s), izraz(i) {}
};

class ZapisOUNPCrel { 
 public:
     string simbol;  //simbol koji se referise unapred
     int pozicija;  //pozicija u nizu sa sadrzajem sekcije koju treba prepraviti

     int velicina; // broj bajtova koje treba prepraviti pocev od lokacije pozicija
     short sekcija_koriscenja; 
     ZapisOUNPCrel(string s, int poz, int vel, short ss): simbol(s), pozicija(poz), velicina(vel), sekcija_koriscenja(ss){};
};


class ZapisOUN { 
 public:
     string simbol;  //simbol koji se referise unapred
     int pozicija;  //pozicija u nizu sa sadrzajem sekcije koju treba prepraviti

     char op; // operator + ili -
     int velicina; // broj bajtova koje treba prepraviti pocev od lokacije pozicija
     short sekcija_koriscenja; 
     ZapisOUN(string s, int poz, int vel, short ss, char t='/'): simbol(s), pozicija(poz), op(t), velicina(vel), sekcija_koriscenja(ss){};
};

class RelZapis { 
 public:
     int pozicija;  //pozicija u nizu sa sadrzajem sekcije koju treba prepraviti
     string simbol;  //simbol na osnovu kojeg treba prepraviti ostavljeni zapis(ili simbol koji predstavlja sekciju za lokalne ili sam taj simbol ako je G)
                    // OVDE OSTAVLJAS SIMBOLICKO IME SIMBOLA PA KAD RAZRESIS SVA OBRACANJA UNAPRED UPISI REDNI BROJ!

     char tip; // A - apsolutno R-relativno
     int velicina; // broj bajtova koje treba prepraviti pocev od lokacije pozicija
     int sekcija; //na koju sekciju se ondosi rel zapis - koristim pri ispisu 

     RelZapis(string s, int poz, char t, int vel, int se): simbol(s), pozicija(poz), tip(t), velicina(vel), sekcija(se) {};
};

typedef struct TabelaSimbola {
     string labela; 
     short sekcija; 
     short vrednost; 
     bool lokalni; // 0 - globalni; 1 - lokalni
     short rBr; 
     int velicina = -1;
     unsigned char* niz=nullptr; 

     TabelaSimbola(string lab="", short sek=-1, short vr=-1, short rb=0, bool l=true) : labela(lab), sekcija(sek), vrednost(vr), rBr(rb), lokalni(l){ 
          //dodavanje u tabelu simbola

     }
} TabelaSimbola;

extern short rBr; 
extern TabelaSimbola* tekucaSekcija; 
extern int lc; 
extern int brojacSekcija;
extern list<string> globalni;



extern map<string, TabelaSimbola> tabelaSimbola; 
extern list<RelZapis> tabelaRelokacija; 
extern list<ZapisOUN> tabelaOUN;
extern list<EquZapis> equLista; 
extern list<ZapisOUNPCrel> tabelaOUNPCrel;

int dodajUSimTab(string labela, short vrednost, short sekc= tekucaSekcija->rBr, bool l=true);
int dodajSekUSimTab(TabelaSimbola* ts);
int dodajURelTab(string sim, int poz, char t, int vel, int sek);
string nadjiSimbolZaRbr(short redni_broj);