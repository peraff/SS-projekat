#include "tabSimbola.h"

map<string, TabelaSimbola> tabelaSimbola;
list<RelZapis> tabelaRelokacija; 
list<ZapisOUN> tabelaOUN;
list<EquZapis> equLista; 
list<ZapisOUNPCrel> tabelaOUNPCrel;

list<string> globalni;



short rBr=0;
int lc = 0;
int brojacSekcija=1;

TabelaSimbola* tekucaSekcija = new TabelaSimbola("UND", 0, 0, rBr++); 

int dodajURelTab(string sim, int poz, char t, int vel, int sek) { 
     RelZapis rz(sim, poz, t, vel, sek);
     tabelaRelokacija.push_back(rz);
     return 1;
}

int dodajUSimTab(string labela, short vrednost,short sekc, bool l){ 

     TabelaSimbola noviSimbol(labela, sekc, vrednost, rBr++,l);

     //Kad sam ostavljao zapis o relokaciji za lokalni simbol koji jos uvek nije bio definisan!
     //prodji kroz zapise o relokaciji i ako naidjes na ovaj simbol i pritom da nije u listi globalnih onda ga prepravi na ime sekcije!
     bool glob=false;
     list<int> listaPozicijaZaBrisanje;    //BRISEM RELOKACIONE ZAPISE KOJI SE ODNOSE NA KONSTANTE - vrednost konstante se ugradjuje na mesto koriscenje u obradi liste OUN
     int brojacPetlje=0;
     list<RelZapis>::iterator ita= tabelaRelokacija.begin();
     while(ita!=tabelaRelokacija.end()) { 
          //cout<<"SIMBOL U TABELI REL" <<a.simbol<<endl;
          if(ita->simbol==noviSimbol.labela) {
               for(auto& b: globalni) { 
                    if(b == noviSimbol.labela) {
                         glob=true; 
                         break;
                    }
               }
               if(!glob){
                    //cout<<"Prepravljen simbol: "<<a.simbol<<" , na simbol:"<< tekucaSekcija->labela<<endl;
                    ita->simbol=tekucaSekcija->labela;  //prepravi jer si ostavio rel zapis u odnosu na simbol, a treba na sekciju
                    if(sekc==-2) {
                         tabelaRelokacija.erase(ita++);
                         continue;
                    }
               } 
          
          }
          
          ita++;
     }

 /*    list<RelZapis>::iterator ita= tabelaRelokacija.begin();     //OBRISI SVE REL_ZAPISE ZA CONST!
     for(auto& a: listaPozicijaZaBrisanje) { 
          cout<<"brisanje "<<a<<endl<<flush;
          advance(ita, a);
          tabelaRelokacija.erase(ita);
          ita=tabelaRelokacija.begin();
          cout<<"obrisan "<<a<<endl<<flush;
     }
*/
     tabelaSimbola.insert(pair<string, TabelaSimbola>(labela, noviSimbol));
     //cout<<"Ubacen novi simbol"<<labela<<endl<<flush;
     return 1; 
}

int dodajSekUSimTab(TabelaSimbola* ts){
     if(ts->rBr!=0 && ts->labela!="bss" && ts->labela!=".bss" && ts->labela!=".section bss" && ts->labela!=".section .bss") { 
          ts->niz= new unsigned char[200];
     }
     ts->sekcija=ts->rBr;                               //PAZI OVDE STA RADIS AKO NEKAD BUDES MENJAO MORAS I OVDE DA PROMENIS! SIMBOLI KOJI SU SEKCIJE IMAJU ISTI BROJ KAO REDNI BROJ
     tabelaSimbola.insert(pair<string, TabelaSimbola>(ts->labela, *ts));
     return 1;
}

string nadjiSimbolZaRbr(short redni_broj) { 
     for(auto& a: tabelaSimbola) { 
          if(a.second.sekcija==redni_broj && a.second.rBr==redni_broj) return a.second.labela;
     }
     cout<<"Nema ovog simbola u tabeli simbola";
     return "Nema ovog simbola u tabeli simbola";
}