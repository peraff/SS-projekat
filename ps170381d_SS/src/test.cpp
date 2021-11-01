#include "tabSimbola.h"
#include <cstring>
#include <iomanip>   
#include <sstream>

#include "naredbe.h"
#include <fstream>



using namespace std;

//OBRISI ODAVDE I DODAJ U NOVI FAJL!
bool uSekciji();

string obradiSimbol(string mojSimbol, string direktiva);
list<IzrazZapis> parsirajEqu(string operandi);

string nekaDirektiva(string direktiva, string operandi)
{
  //proveravaj redom sve direktive koje mogu da se pojave i generisi OC i uvecavaj LC
  if (direktiva == "data" || direktiva == "text" || direktiva == "bss" || direktiva == "section")
  {

    string naziv = direktiva;
    map<string, TabelaSimbola>::iterator i = tabelaSimbola.find(tekucaSekcija->labela);
    if (i != tabelaSimbola.end())
    {
      i->second.velicina = lc;
    }
    delete tekucaSekcija;
    lc = 0;
    // *******************-+ Formiranje sekcije ako nije neka od osnovne 3. U ops je naziv sekcije
    if (direktiva == "section")
    {
      naziv = operandi;
    }
    else
    {
      naziv = "." + direktiva;
    }

    // *********************Formirana - isparsiran naziv
    tekucaSekcija = new TabelaSimbola(naziv, brojacSekcija++, lc, rBr++);
    dodajSekUSimTab(tekucaSekcija);
    return "ok";
  }
  else if (direktiva == "end")
  {
    map<string, TabelaSimbola>::iterator i = tabelaSimbola.find(tekucaSekcija->labela);
    if (i != tabelaSimbola.end())
    {
      i->second.velicina = lc;
      delete tekucaSekcija;
      tekucaSekcija=nullptr;
      lc = 0;
      return "end";
    } else { cout<<"Greska u tabeli simbola kod end direktive!"<<endl; exit(-41);}

  }
  else if (direktiva == "global")
  {
    if (operandi == "")
    {
      cout << "Direktiva global mora imati argumente" << endl;
      exit(-3);
    }
    int pos;
    if ((pos = operandi.find_first_of(',')) != string::npos)
    {
      char *tmp = strtok((char *)(operandi.c_str()), ",");
      for (int j = 0; tmp; j++)
      {
        globalni.push_back(tmp);
        tmp = strtok(NULL, ",");
      }
    }
    else
    {
      globalni.push_back(operandi);
    }
    /* PROLAZAK KROZ LISTU GLOBALNIH SIMBOLA! 
    cout<<"Globalni:"<<endl; 
    for(auto& a: globalni) cout<<a;
    cout<<endl;
    */
    return "ok";
  }
  else if (direktiva == "extern")
  {
    if (operandi == "")
    {
      cout << "Direktiva extern mora imati argumente" << endl;
      exit(-3);
    }
    int pos;
    if ((pos = operandi.find_first_of(',')) != string::npos)
    {
      char *tmp = strtok((char *)(operandi.c_str()), ",");
      for (int j = 0; tmp; j++)
      {
        string novi = tmp;
        globalni.push_back(novi);
        dodajUSimTab(novi, lc, 0, false);
        tmp = strtok(NULL, ",");
      }
    }
    else
    {
      globalni.push_back(operandi);
      dodajUSimTab(operandi, lc, 0, false);
    }
    return "ok";
  }
  else if (direktiva == "byte" || direktiva == "word")
  {

    if (!uSekciji())
    {
      cout << "Direktiva ili naredba koja generise sadrzaj mora biti u okviru sekcije koja ima sadrzaj!!" << endl;
      exit(-3);
    }

    // provera broja operanada (1 ili vise) - trazim zarez
    int pos;
    int niz_literala[20];
    int brojLiterala=0;
    string niz_simbola[20];
    int brojSimbola=0;
    bool broj=false;
    unsigned int mojBroj; 
    string mojSimbol;

    if ((pos = operandi.find_first_of(',')) != string::npos)
    {
      char *tmp = strtok((char *)(operandi.c_str()), ",");
   
        string novi = tmp;
        

        char* pom;
         mojBroj = (int)strtol(novi.c_str(), &pom, 10);
        if(*pom == 0) {                      // LITERALI SU
          broj = true; 
          niz_literala[0]=mojBroj;
          brojLiterala++;
          
          int i=1;
          while(tmp) {
            tmp = strtok(NULL, ",");
            if(!tmp) break;
            novi=tmp;
            char* pom;
            mojBroj = (int)strtol(novi.c_str(), &pom, 10);
            niz_literala[i++] = mojBroj;
            brojLiterala++;
          }
        
        } else {            //SIMBOLI SU
          broj=false;
          niz_simbola[0]=novi;
          brojSimbola++;
          int i=1;
          while(tmp) {
            tmp = strtok(NULL, ",");
            if(!tmp) break;
            novi=tmp;
            niz_simbola[i++] = novi;
            brojSimbola++;
          }
        }  
    }  // kraj if-a gde pitam dal je 1 ili vise operanada
    else {  // znaci da je samo jedan operand
        char* pom;
        mojBroj = (int)strtol(operandi.c_str(), &pom, 10);
        if(*pom == 0) {                      // LITERAL JE
          broj = true; 
        } else {   //SIMBOL JE
          mojSimbol=operandi;
        }
    }


     if(broj) {
       if(brojLiterala != 0) { 
         //obradi niz_literala koji ima brojLiterala elemenata

         for(int i=0; i < brojLiterala; i++) { 
           if(direktiva == "byte") tekucaSekcija->niz[lc++] = niz_literala[i];
          else if(direktiva == "word") {
            mojBroj=niz_literala[i];
           unsigned int maska1=0xf; 
           unsigned int maska2= 0xf0;
            unsigned int maska3= 0xf00;
             unsigned int maska4= 0xf000;
           unsigned int prviBajt= mojBroj & maska1; 
           unsigned int drugiBajt= (mojBroj & maska2) >> 4;
           unsigned int treciBajt= (mojBroj & maska3) >> 8;
           unsigned int cetvrtiBajt= (mojBroj & maska4) >> 12;

           unsigned int prvi=prviBajt | (drugiBajt<<4);
           unsigned int drugi=treciBajt | (cetvrtiBajt<<4);

           tekucaSekcija->niz[lc++] = prvi;
           tekucaSekcija->niz[lc++] = drugi;
         }
         }
       } else { 
         //samo jedan operand, broj, u prom mojBroj!
         if(direktiva == "byte") tekucaSekcija->niz[lc++] = mojBroj;
         else if(direktiva == "word") {
           unsigned int maska1=0xf; 
           unsigned int maska2= 0xf0;
            unsigned int maska3= 0xf00;
             unsigned int maska4= 0xf000;
           unsigned int prviBajt= mojBroj & maska1; 
           unsigned int drugiBajt= (mojBroj & maska2) >> 4;
           unsigned int treciBajt= (mojBroj & maska3) >> 8;
           unsigned int cetvrtiBajt= (mojBroj & maska4) >> 12;

           unsigned int prvi=prviBajt | (drugiBajt<<4);
           unsigned int drugi=treciBajt | (cetvrtiBajt<<4);

           tekucaSekcija->niz[lc++] = prvi;
           tekucaSekcija->niz[lc++] = drugi;
         }
       }
     } else {                                       // nije broj nego simbol!
       if(brojSimbola>0) { 
         for (size_t i = 0; i < brojSimbola; i++){
           string ret =obradiSimbol(niz_simbola[i], direktiva);
         }
        
       } else { 
         string ret= obradiSimbol(mojSimbol, direktiva);
       }
     }


    // ********************************************** BYTE I WORD GOTOVE! *********************************************** //
    return "gotove byte i word!";
  }
  else if (direktiva == "skip")
  {
    if (tekucaSekcija->labela=="UND")
    {
      cout << "Direktiva ili naredba koja generise sadrzaj mora biti u okviru sekcije!" << endl;
      exit(-3);
    }
    stringstream ss(operandi);
    int k;
    ss >> k;
    if (tekucaSekcija->labela == ".bss" || tekucaSekcija->labela == ".section .bss" || tekucaSekcija->labela == "bss" ||
        tekucaSekcija->labela == ".section bss" || tekucaSekcija->labela == "UND")
    {
      lc += k;
    }
    else
    {
      for (int i = 0; i < k; i++)
        tekucaSekcija->niz[lc++] = 0;
    }
    return "skip";
  }
  else if(direktiva == "equ") { 
    int pos;
    if ((pos = operandi.find_first_of(',')) != string::npos){

      char *tmp = strtok((char *)(operandi.c_str()), ",");

      string pom=operandi.substr(pos+1, -1);

      EquZapis ez(tmp, pom);

      equLista.push_back(ez);

     // cout<<"Simbol: "<<tmp<<" Operandi: " <<pom<<endl;
    
     // parsirajEqu(pom); NE ZOVEM OVDE NEGO KAD OBRADJUJEM EQU DIREKIVU NA KRAJU PROLAZA!

    }
    else
    {
      cout<<"Direktiva equ mora biti u formatu: .equ simbol, izraz"<<endl;
      exit(-3);
    }

    return "equ";
  }

  //*****************************************************************GOTOVE SVE DIREKTIVE AKO NISI NI JEDNU POGODIO VRATI GRESKU *****************************
  else{
    cout<<direktiva<<endl;
    return "greska";
  } 
}

/* **********************************************************************************************************************************************************
* **********************************************************************************************************************************************************
                                                                        MAIN 
* **********************************************************************************************************************************************************
* **********************************************************************************************************************************************************
*/


int main(int argc, char **argv)
{

  TabelaSimbola konstanta(".cons", -2, 0, -2);
  tabelaSimbola.insert(pair<string, TabelaSimbola>("CONST", konstanta));
  tabelaSimbola.insert(pair<string, TabelaSimbola>(tekucaSekcija->labela, *tekucaSekcija));

  string linija;
  ifstream ulaz(argv[1]);
  ofstream izlaz(argv[2]);

  if (ulaz.is_open())
  {

    while (getline(ulaz, linija))
    {

      string labela;

      if (linija[0] == ' ')
      {
        int pos = linija.find_first_not_of(' ');
        if (pos != string::npos)
        {
          linija = linija.substr(pos, linija.size() - pos);
        }
      }

      if (linija.find(':') != -1) //ako postoji labela, dodaj je u tabelu simbola i ukloni je iz linije
      {
        labela = linija.substr(0, linija.find_first_of(':'));

        dodajUSimTab(labela, lc);
        linija = linija.substr(linija.find(':') + 1, -1);
        if (linija.find_first_not_of(' ') != -1)
          linija = linija.substr(linija.find_first_not_of(' '), -1);
      }

      if (linija[0] == '.')                          //Znaci da je direktiva
      {
        //cout<<linija<<endl;
        string nkDir;
        string ops;
        int kraj;
        if ((kraj = linija.find_first_of(' ')) != -1)
        {
          nkDir = linija.substr(1, kraj - 1);
          if (linija.find_first_not_of(' ', kraj) != -1)
            ops = linija.substr(linija.find_first_not_of(' ', kraj), -1);
          else
            ops = "";
        }
        else
        {
          nkDir = linija.substr(1, -1);
          ops = "";
        }

        string vracenaDir = nekaDirektiva(nkDir, ops);
        if (vracenaDir == "end")
        {
          break;
        }
        else if (vracenaDir == "greska")
        {
          cout << "Nevalidan sadrzaj asemblerskog fajla - nepostojeca direktiva!"<<endl;
          exit(-1);
        } 
      } else if(linija[0]!=' ' && linija[0]!='\n' && linija[0]!='\0'){  // ZNACI DA JE NAREDBA POSTO NIJE DIREKTIVA!
        string instr;
        string operand1;
        string operand2;
        int kraj;
        int pos1;
        int pos2;

        if(!uSekciji()){
            cout<<"Naredba: "<< linija<< " mora biti u okviru sekcije koja ima sadrzaj!!"<<endl; 
            exit(-3);
        }

        if ((kraj = linija.find_first_of(' ')) != -1)
        {
          instr=linija.substr(0, kraj);
          if ((pos1=linija.find_first_not_of(' ', kraj)) != -1){

              if((pos2=linija.find_first_of(',', pos1)) != -1) {
                operand1 = linija.substr(pos1, pos2-pos1);  //NE SME RAZMAK POSLE ZAREZA!!!!
                operand2=linija.substr(pos2+1, -1);
                naredbaDvaOp(instr, operand1, operand2);
              } else {
                operand1 = linija.substr(pos1, -1);
                naredbaJedanOp(instr, operand1);
              }
      
          }
          else
            naredbaBezOp(instr);
        } else { //naredba bez operanda! halt; iret; ret;
            instr=linija.substr(0, kraj);
            naredbaBezOp(instr);
        }
      }
    } //KRAJ PROLASKA KROZ FAJL!
     // ************************************** ZAVRSI POSLEDNJU SEKCIJU AKO SE NE NAVEDE END ********************************//

    //ako nije navedeno end zavrsi poslednju sekciju!
      if(tekucaSekcija) {
       map<string, TabelaSimbola>::iterator i = tabelaSimbola.find(tekucaSekcija->labela);
        if (i != tabelaSimbola.end())
        {
          i->second.velicina = lc;
          delete tekucaSekcija;
          lc = 0;
        } else { cout<<"Greska u tabeli simbola kad nema .end direktive!"<<endl; exit(-41);}
      }




    // *************************************** OBRADA SIMBOLA UVEDENIH EQU DIREKTIVOM *************************************// 

    
    for(auto&a: equLista) { 
      list<IzrazZapis> listaOperanada = parsirajEqu(a.izraz); 

      int brS=0; //za svaki +(+1) za minus(-1), ako ostane ==0 znaci da je konstanta inace je relokatibilan simbol!

      short pocetnaVrednost=0;
      bool prvi=true; 
      short sekcija=-2;
      for(auto& b: listaOperanada){
          //cout<<b.znak<<b.simbol<<" ";
          //ovde proveri dal je broj, ako je broj samo ga uracunaj!

        char* pom;
        string pek=b.simbol;
        int brojIzIzraza = (int)strtol(pek.c_str(), &pom, 10);
        if(*pom == 0) {                      // BROJ JE A NE SIMBOL
          if(b.znak=='+') pocetnaVrednost+=brojIzIzraza;
            else if(b.znak=='-') pocetnaVrednost-=brojIzIzraza;
        } else {   //SIMBOL JE BIO U IZRAZU IDI DO TABELE SIMBOLA!
        
         map<string, TabelaSimbola>:: iterator it=tabelaSimbola.find(b.simbol);
          if(it==tabelaSimbola.end()) {  //ovaj simbol ne postoji u tabeli simbola. Napravi TNIS ubaci zapis i idi dalje
            cout<<"Nije podrzano koriscenje neizracunljivih simbola u equ!(Simbol u izrazu mora biti definisan na kraju prolaza)"<<endl;
            exit(2);
          } else { 
            if(it->second.sekcija!=-2){
              if(prvi) {
                sekcija=it->second.sekcija;
                prvi=false;
              } else { 
                if(it->second.sekcija != sekcija) {
                  if(it->second.sekcija!=-2){
                    cout<<"U equ mogu da se koriste simboli samo iz iste sekcije!"<<endl;
                    exit(3);
                  }
                }
              }
              if(it->second.sekcija != -2) {  //ZA KONSTANTE NE DIRAS brS! 
                if(b.znak=='+') brS++;
                else if(b.znak=='-') brS--;
              }
            }
            if(b.znak=='+') pocetnaVrednost+=it->second.vrednost;
            else if(b.znak=='-') pocetnaVrednost-=it->second.vrednost;
          }
        }
      } 
      //dodaj simbol u tabelu simbola sa izracunatom pocentom vrednoscu umesto LC! Za sekciju stavi sekciju simbola koji je prvi definisan

          if(brS==0) sekcija=-2;            //Ako su se potrli svi simboli onda simbol postaje apsolutan!
          else if (brS!=1) { 
            cout<<"Nekorektan izraz u okviru equ direktive za simbol: "<<a.simbol<<". Indeks klasifikacije je: "<<brS<<endl;
            exit(-3);
          }

          if(sekcija==0){   //AKO uvodim novi simbol koji je jednak eksternom simbolu i njega oznacim kao globalnog
            globalni.push_back(a.simbol);
          }

          for(auto& tmp: tabelaSimbola){ //za interne potrebe napravim ovaj pokazivac da bih mogao da prepravim zapis o relokaciji da ukazuje na sekciju u kojoj je simbol definisan!
            if(tmp.second.rBr==sekcija ) tekucaSekcija = new TabelaSimbola(tmp.second.labela, tmp.second.sekcija, 0, -7); 
          }

          //u ovoj funkciji koristim pokazivac tekucaSekcija da bih promenio zapis o rel sa simbola na sekciju!
          dodajUSimTab(a.simbol, pocetnaVrednost, sekcija); 
          delete tekucaSekcija;
          tekucaSekcija=nullptr;
    }
    //cout<<"samo equ"<<endl; 
    //exit(-357);


    // ************************************* OBRADA LISTE OBRACANJA U NAPRED! ******************************//
    map<string, TabelaSimbola>::iterator it;
    for(auto& a: tabelaOUN) { 
      it=tabelaSimbola.find(a.simbol); //nadji simbol u tabeli simbola
      if(it==tabelaSimbola.end()){ 
        cout<<"Simbol " <<a.simbol<<" nije definisan!"<<endl;
        exit(-15);
      } else { //prodji kroz tabelu simbola da nadjes sekciju koja ti treba da joj prepravis sadrzaj i upisi vrednost tvog simbola
        for(auto& b: tabelaSimbola) {
          // b je sekcija u kojoj je simbol KORISCEN!;
          // it je simbol iz tabele simbola
          // a je zapis Obracanja u napred!
          if((b.second.sekcija == a.sekcija_koriscenja) && (b.second.velicina>0)){ 
            if(a.velicina==1) b.second.niz[a.pozicija]=it->second.vrednost;
            else { // ako je velicina 2B
                  int mojBroj=it->second.vrednost;
                  unsigned int maska1=0xf; 
                  unsigned int maska2= 0xf0;
                  unsigned int maska3= 0xf00;
                  unsigned int maska4= 0xf000;
                  unsigned int prviBajt= mojBroj & maska1; 
                  unsigned int drugiBajt= (mojBroj & maska2) >> 4;
                  unsigned int treciBajt= (mojBroj & maska3) >> 8;
                  unsigned int cetvrtiBajt= (mojBroj & maska4) >> 12;

                  unsigned int prvi=prviBajt | (drugiBajt<<4);
                  unsigned int drugi=treciBajt | (cetvrtiBajt<<4);

                  b.second.niz[a.pozicija] = prvi;
                  b.second.niz[a.pozicija + 1] = drugi;
        




            }
            break;
          }
        }
      }

    }

    // ************************************ OBRADA ZAPISAOUNPCrel *******************************************// 

    for(auto& a: tabelaOUNPCrel) { 
      it=tabelaSimbola.find(a.simbol); //nadji simbol u tabeli simbola
      if(it==tabelaSimbola.end()){ 
        cout<<"Simbol " <<a.simbol<<" nije definisan!"<<endl;
        exit(-15);
      } else { //prodji kroz tabelu simbola da nadjes sekciju koja ti treba da joj prepravis sadrzaj i upisi vrednost tvog simbola
        for(auto& b: tabelaSimbola) {
          // b je sekcija u kojoj je simbol KORISCEN!;
          // it je simbol iz tabele simbola
          // a je zapis Obracanja u napred!
          if((b.second.sekcija == a.sekcija_koriscenja) && (b.second.velicina>0)){ 
            
            if(it->second.sekcija==a.sekcija_koriscenja){ //AKO JE SIMBOL KORISCEN U ISTOJ SEKCIJI GDE JE DEF UGRADI VR I NEMA REL ZAPISA!
                if(a.velicina==1) b.second.niz[a.pozicija] +=it->second.vrednost;
                else {
                   unsigned int prviSabirakC=b.second.niz[a.pozicija];
                   unsigned int drugiSabirakC=b.second.niz[a.pozicija + 1];

                   short zbir= (drugiSabirakC<<8) | prviSabirakC;

                  zbir+=it->second.vrednost;
                  b.second.niz[a.pozicija] = prviBajt(zbir);
                  b.second.niz[a.pozicija + 1] = drugiBajt(zbir);
                }
            } else { // AKO JE DEFINISAN U NEKOJ DRUGOJ SEKCIJI OSTAVI RELOKACIONI ZAPIS U ZAVISNOSTI DAL JE SIMBOL LOKALNI ILI GLOBALNI! ZA CONST NE OSTAVLJAJ REL ZAPISE!!!!!!
                bool globus=false;
                for(auto& g:globalni){ 
                  if(g==a.simbol) { //globalni je
                    globus=true;
                    
                    if(a.velicina==1){
                        RelZapis rzz(a.simbol, a.pozicija, 'R', 1, a.sekcija_koriscenja);
                        tabelaRelokacija.push_back(rzz);
                        b.second.niz[a.pozicija]+=a.pozicija;
                    } 
                    else {
                        RelZapis rzz(a.simbol, a.pozicija, 'R', 2, a.sekcija_koriscenja);
                        tabelaRelokacija.push_back(rzz);
                       unsigned int prviSabirakC=b.second.niz[a.pozicija];
                       unsigned int drugiSabirakC=b.second.niz[a.pozicija + 1];

                       short zbir= (drugiSabirakC<<8) | prviSabirakC;
                       zbir+=a.pozicija;
                       b.second.niz[a.pozicija] = prviBajt(zbir);
                       b.second.niz[a.pozicija + 1] = drugiBajt(zbir);
                    }
                  }
                }

                if(!globus){
                   string naziv_seke=nadjiSimbolZaRbr(it->second.sekcija);
                    if(a.velicina==1){
                        RelZapis rzz(naziv_seke, a.pozicija, 'R', 1, a.sekcija_koriscenja);
                        //if(it->second.sekcija!=-2)
                          tabelaRelokacija.push_back(rzz);  //AKO JE KONSTANTTNI SIMBOL UGRADI MU VREDNOST AL NE OSTAVLJAJ REL ZAPIS
                        b.second.niz[a.pozicija]+=a.pozicija + it->second.vrednost;
                    } 
                    else {
                       string naziv_seke=nadjiSimbolZaRbr(it->second.sekcija);
                        RelZapis rzz(naziv_seke, a.pozicija, 'R', 2, a.sekcija_koriscenja);
                       // if(it->second.sekcija!=-2) 
                        tabelaRelokacija.push_back(rzz);   //AKO JE KONSTANTTNI SIMBOL UGRADI MU VREDNOST AL NE OSTAVLJAJ REL ZAPIS
                        unsigned int prviSabirakC=b.second.niz[a.pozicija];
                        unsigned int drugiSabirakC=b.second.niz[a.pozicija + 1];

                        short zbir= (drugiSabirakC<<8) | prviSabirakC;
                        zbir+=a.pozicija + it->second.vrednost;
                       b.second.niz[a.pozicija] = prviBajt(zbir);
                       b.second.niz[a.pozicija + 1] = drugiBajt(zbir);
                    }  
                }
                
            }
            
            break;
          }
        }
      }

    }
    // *********************************** ISPIS TABELE SIMBOLA *********************************************//
    izlaz << "#tabela simbola" << endl;
    izlaz << "#ime    sek.    vr.hd    vid.    r.b.    vel(dec)  " << endl;
    for (it = tabelaSimbola.begin(); it != tabelaSimbola.end(); ++it)
    {
      for (auto &a : globalni)
        if (it->second.labela == a)
          it->second.lokalni = 0;

      izlaz<<setw(8)<<left;
      izlaz << it->second.labela;
      izlaz<<setw(8);
      izlaz << it->second.sekcija << "  \t";
      izlaz<<setw(8);
      izlaz.setf(ios::hex, ios::basefield);
      izlaz << it->second.vrednost;
      izlaz<<setw(8);
      izlaz << (it->second.lokalni ? "l" : "g");
      izlaz.unsetf(ios::hex);
      izlaz<<setw(8);
      izlaz << it->second.rBr;
      izlaz<<setw(8);
      izlaz<<it->second.velicina << endl;
    }
    //************************************* ISPIS SADRRZAJA SEKCIJA ******************************//
    izlaz.setf(ios::hex, ios::basefield);
    char mask1 = 0x0f;
    char mask2 = 0xf0;
    izlaz << endl
         << "#sadrzaj sekcija" << endl;
    for (it = tabelaSimbola.begin(); it != tabelaSimbola.end(); ++it)
    {
      if (it->second.velicina != -1 && it->second.niz != nullptr && it->second.velicina != 0)
      {
        izlaz << "#" + it->second.labela << endl;
        for (int i = 0; i < it->second.velicina; i++)
        {
          unsigned int prviB = ((unsigned int)it->second.niz[i]) & mask1;
          unsigned int drugiB = (((unsigned int)it->second.niz[i]) & mask2) >> 4;
          izlaz << drugiB << prviB << " ";
        }
        izlaz << endl;
      }
    }
    izlaz.unsetf(ios::hex);
    // **************************************** ISPIS TABELE RELOKACIJA *********************************//
    bool prvi=true;
    izlaz.setf(ios::hex, ios::basefield);
    list<RelZapis>::iterator iti;
    izlaz << endl
         << "#tabela_relokacija" 
         << endl;
    for (it = tabelaSimbola.begin(); it != tabelaSimbola.end(); ++it)
    {
      if (it->second.velicina != -1 && it->second.niz != nullptr && it->second.velicina != 0)
      {
        
         for (iti = tabelaRelokacija.begin(); iti != tabelaRelokacija.end(); ++iti)
        {
          if(iti->sekcija == it->second.sekcija) {
            if(prvi) {
                izlaz << "#rel" + it->second.labela << endl;
                izlaz <<"#offset    tip        ref.simbol"<<endl;
                prvi=false;
            }
            izlaz<<setw(11);
            izlaz<<iti->pozicija;
            izlaz<<setw(11);
            izlaz<< iti->tip;
            izlaz<<setw(11);
            izlaz<< iti->simbol<<endl;
          }
          //unsigned int prviB = ((unsigned int)it->second.niz[i]) & mask1;
          //unsigned int drugiB = (((unsigned int)it->second.niz[i]) & mask2) >> 4;
          //cout << drugiB << prviB << " ";
        }
        prvi=true;
        delete it->second.niz; 
      }

    }
    izlaz.unsetf(ios::hex);

    // ***************************************************************************************************//
    izlaz << '\n';

    ulaz.close();
    izlaz.close();
  }
  else
    cout << "Ne moze fajl otvorit2342244221" << endl;
}

bool uSekciji()
{
  if (tekucaSekcija->rBr != 0 && tekucaSekcija->labela!="bss" && tekucaSekcija->labela!=".bss" && tekucaSekcija->labela!="section bss" && tekucaSekcija->labela!="section .bss")
    return true;
  return false;
}


string obradiSimbol(string mojSimbol, string direktiva){ 
         //samo jedan simbol u promenljivoj mojSimbol.

         for(auto& a: globalni) {            //ako je simbol globalni onda ugradi 0 i ostavi rel zapis ka ovom simbolu
            if(a == mojSimbol) {
              if(direktiva=="byte"){
                dodajURelTab(mojSimbol, lc, 'A', 1, tekucaSekcija->sekcija);    //tekuca sekcija sluzi da bih znao kojoj sekciji pripada rel zapis kad ispisujem
                tekucaSekcija->niz[lc++]=0;
                return "Zavrsio";
              } 
              else { //direktiva word, zauzmi 2 reci
                dodajURelTab(mojSimbol, lc, 'A', 2, tekucaSekcija->sekcija);
                tekucaSekcija->niz[lc++]=0;
                tekucaSekcija->niz[lc++]=0;
                return "Zavrsio";
              }
              
            }
         }
         //Ovde dolazi samo ako simbol nije G, tj ako ga ne pronadje u listi globalnih! Za lokalne rel zapis u odnosu na sekciju! Prvi parametar je sekcija u kojoj je definisan simbol
         
         if(direktiva=="byte"){
           map<string, TabelaSimbola>::iterator i = tabelaSimbola.find(mojSimbol);
              if (i != tabelaSimbola.end()) {
                  string naziv_sekcije_gde_je_definisan= nadjiSimbolZaRbr(i->second.sekcija);  //za L simbole rel zapis ostaje u odnosu na sekciju!
                  dodajURelTab(naziv_sekcije_gde_je_definisan, lc, 'A', 1, tekucaSekcija->sekcija);
                  tekucaSekcija->niz[lc++]=i->second.vrednost;
              } else { //ako ga nema u tabeli simbola napravi obracanje u napred!
                //Formiraj zapis obracanja u napred i dodaj ga u tabelu. 
                ZapisOUN zoun(mojSimbol, lc, 1, tekucaSekcija->sekcija); //1 za byte direktivu
                tabelaOUN.push_back(zoun);

                dodajURelTab(mojSimbol, lc, 'A', 1, tekucaSekcija->sekcija);  //ostavim zapis u odnosu na taj simbol, pa kad ga dodam u TabSim izmenim!

                tekucaSekcija->niz[lc++]=0;    //preskacem sadrzaj u nizu, ugradicu ga na kraju fajla!
              }
         } else {      //ako je word 
            map<string, TabelaSimbola>::iterator i = tabelaSimbola.find(mojSimbol);
              if (i != tabelaSimbola.end()) {
                  string naziv_sekcije_gde_je_definisan= nadjiSimbolZaRbr(i->second.sekcija);
                  dodajURelTab(naziv_sekcije_gde_je_definisan, lc, 'A', 2, tekucaSekcija->sekcija);
                  int mojBroj=i->second.vrednost;
                  unsigned int maska1=0xf; 
                  unsigned int maska2= 0xf0;
                  unsigned int maska3= 0xf00;
                  unsigned int maska4= 0xf000;
                  unsigned int prviBajt= mojBroj & maska1; 
                  unsigned int drugiBajt= (mojBroj & maska2) >> 4;
                  unsigned int treciBajt= (mojBroj & maska3) >> 8;
                  unsigned int cetvrtiBajt= (mojBroj & maska4) >> 12;

                  unsigned int prvi=prviBajt | (drugiBajt<<4);
                  unsigned int drugi=treciBajt | (cetvrtiBajt<<4);

                  tekucaSekcija->niz[lc++] = prvi;
                  tekucaSekcija->niz[lc++] = drugi;

              } else { //ako ga nema u tabeli simbola napravi obracanje u napred!
                //Formiraj zapis obracanja u napred i dodaj ga u tabelu. 
                ZapisOUN zoun(mojSimbol, lc, 2, tekucaSekcija->sekcija); //1 za byte direktivu
                tabelaOUN.push_back(zoun);

                dodajURelTab(mojSimbol, lc, 'A', 2, tekucaSekcija->sekcija);  //ostavim zapis u odnosu na taj simbol, pa kad ga dodam u TabSim izmenim!

                tekucaSekcija->niz[lc++]=0;
                tekucaSekcija->niz[lc++]=0;     //preskacem sadrzaj u nizu, ugradicu ga na kraju fajla!
              }
         }
         return "ok";
}



//Parsiranje equ direktive  CELA FUNKCIJA KOJA RADI PROVERENO OVDE SACUVANA IZMENE SU DOLE
/*void parsirajEquA(string operandi) { 
    int pos;
    if ((pos = operandi.find_first_of(',')) != string::npos){

      char *tmp = strtok((char *)(operandi.c_str()), ",");
      cout<<"Simbol: "<<tmp<<" Operandi: ";

      string pom=operandi.substr(pos+1, -1);

      int traziOd=1; //Ako je znak prvog broja u izrazu minus on je na poziciji 0, kreni od prvog znaka i trazi sledeci +/-

      tmp = strtok(nullptr, "+-\n\r");

      if(tmp) {                       //kupim prvi broj sa predznakom! Ovo mora postojati. 
          if(pom[0]!='-') cout<<"+";
          else cout<<"-";
          cout<<tmp<<" ";
      } else {
        cout<<"Greska u formatu equ dirketive"<<endl;
        exit(-31);
      }
      for (int j = 0; tmp; j++)
      {
        
        int poz=pom.find_first_of("+-", traziOd);
        if(poz== string::npos) {         //Pokupljen prvi broj iz izraza, ako ne naidjem na + ili - u nastavku to je kraj izraza!
          cout<<"Prosao kroz sve argumente";
          exit(1);
        }
        if(pom[poz]=='+'){
          tmp = strtok(NULL, "+-\r\n");         //svakako kupi prvi naredni broj
          if(tmp)
          cout<<"+"<<tmp<<" ";
        } else if (pom[poz]=='-')  {
          tmp = strtok(nullptr, "+-\r\n");
          if(tmp)
          cout<<"-"<<tmp<< " ";
        } else {
          cout<<"Greska"; 
          exit(-32);
        }
        traziOd=poz+1;
      }
    }
    else
    {
      cout<<"Direktiva equ mora biti u formatu: .equ simbol, izraz"<<endl;
      exit(-3);
    }
}*/ 

list<IzrazZapis> parsirajEqu(string operandi) { 

      list<IzrazZapis> izrazLista; 

      int traziOd=1; //Ako je znak prvog broja u izrazu minus on je na poziciji 0, kreni od prvog znaka i trazi sledeci +/-

      string pomocni = operandi;
      char *tmp = strtok((char *)(pomocni.c_str()), "+-\n\r");

      IzrazZapis jedanOperand; 

      if(tmp) {                       //kupim prvi broj U IZRAZU sa predznakom! Ovo mora postojati. 
          if(operandi[0]!='-') jedanOperand.znak='+';
          else jedanOperand.znak='-';
          jedanOperand.simbol=tmp;
          izrazLista.push_back(jedanOperand);
      } else {
        cout<<"Direktiva equ mora biti u formatu: .equ simbol, izraz"<<endl;
        exit(-31);
      }
      for (int j = 0; tmp; j++)
      {
        int poz=operandi.find_first_of("+-", traziOd);
        if(poz== string::npos) {         //Pokupljen prvi broj iz izraza, ako ne naidjem na + ili - u nastavku to je kraj izraza!
          //cout<<"Prosao kroz sve argumente";
           return izrazLista;
        }
        if(operandi[poz]=='+'){
          tmp = strtok(NULL, "+-\n\r");         //svakako kupi prvi naredni broj
          if(tmp){
            jedanOperand.znak='+';
            jedanOperand.simbol=tmp;
            izrazLista.push_back(jedanOperand);
          }
        } else if (operandi[poz]=='-')  {
          tmp = strtok(NULL, "+-\n\r");
          if(tmp){
            jedanOperand.znak='-';
            jedanOperand.simbol=tmp;
            izrazLista.push_back(jedanOperand);
          }
        } else {
          cout<<"Greska"; 
          exit(-32);
        }
        traziOd=poz+1;
      }
      return izrazLista;
}

