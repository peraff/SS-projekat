#include "naredbe.h"

bool jelLiteral(string op, int * mojBroj){ 
        char* pom;
        *mojBroj = (int)strtol(op.c_str(), &pom, 10);
        if(*pom == 0) {                      // LITERAL JE
          return true;
        } else {   //SIMBOL JE
          return false;
        }
}

unsigned int prviBajt(unsigned int mojBroj){ 
           unsigned int maska1=0xf; 
           unsigned int maska2= 0xf0;

           unsigned int prviBajt= mojBroj & maska1; 
           unsigned int drugiBajt= (mojBroj & maska2) >> 4;


           unsigned int prvi=prviBajt | (drugiBajt<<4);

           return prvi;
}

unsigned int drugiBajt(unsigned int mojBroj) { 
    
        unsigned int maska3= 0xf00;
        unsigned int maska4= 0xf000;
 
        unsigned int treciBajt= (mojBroj & maska3) >> 8;
        unsigned int cetvrtiBajt= (mojBroj & maska4) >> 12;

        unsigned int drugi=treciBajt | (cetvrtiBajt<<4);
        return drugi;
}


void obradiSimbolPCREL(string mojSimbol, int tekPC){ 
    int pomeraj;
     map<string, TabelaSimbola>::iterator i = tabelaSimbola.find(mojSimbol);
    if (i != tabelaSimbola.end()) {  // vec je definisan, nemam obracanja u napred
        if(i->second.sekcija==tekucaSekcija->rBr){ //ako je simbol definisan u istoj sekciji u kojoj je i koriscen PCrel adresiranjem ugradi potpuno vrednost i ne ostavljaj nikakve relZapise
            pomeraj=i->second.vrednost-tekPC;
            tekucaSekcija->niz[lc++]=prviBajt(pomeraj);
            tekucaSekcija->niz[lc++]=drugiBajt(pomeraj);
        } else { //def u drugoj sekciji nmg da ugradim vrednost! Za globalne ugradi samo pomeraj do sledece instrukcije a za lokalne jos dodaj tekucu vrednost na taj pom! Rel zap u odnosu na simbol i sekc
            for(auto& a: globalni){ 
                if(a==mojSimbol){
                    pomeraj=lc-tekPC;
                    dodajURelTab(mojSimbol, lc, 'R', 2, tekucaSekcija->sekcija);

                    tekucaSekcija->niz[lc++]=prviBajt(pomeraj);
                    tekucaSekcija->niz[lc++]=drugiBajt(pomeraj);
                    
                    return;
                }
            }
            //OVDE DOLAZI AKO JE LOKALNI!
            string naziv_sekcije_gde_je_definisan= nadjiSimbolZaRbr(i->second.sekcija);  //za L simbole rel zapis ostaje u odnosu na sekciju!
            dodajURelTab(naziv_sekcije_gde_je_definisan, lc, 'R', 2, tekucaSekcija->sekcija);
            pomeraj=i->second.vrednost + lc - tekPC;
            tekucaSekcija->niz[lc++]=prviBajt(pomeraj);
            tekucaSekcija->niz[lc++]=drugiBajt(pomeraj);
        }
    } else {   //NEMA GA U TABELI SIMBOLA - PRAVI OBRCANJE U NAPRED ZA PCREL!!!
            ZapisOUNPCrel zoun(mojSimbol, lc, 2, tekucaSekcija->sekcija); //1 za byte direktivu; 2 jer imam 2 reci koje treba prepraviti
            tabelaOUNPCrel.push_back(zoun);
            pomeraj= -tekPC;
            tekucaSekcija->niz[lc++]=prviBajt(pomeraj);
            tekucaSekcija->niz[lc++]=drugiBajt(pomeraj);

    }
}



void naredbaBezOp(string instr) { 
unsigned int oc=dohvatiOC(instr); 
oc<<=3;
//cout<<endl<<tekucaSekcija->labela<<flush;
tekucaSekcija->niz[lc++] = (unsigned char)oc;
}

void naredbaJedanOp(string instr,string operand1){ 
//cout<<"Naredba sa 1 operanada je:"<< instr<<"Operand je:"<< operand1<<endl; 
    if(instr[0]=='j' || instr == "int" || instr == "call") naredbaSkoka(instr, operand1); //PAZII AKO BUDE NEKA MODIFIKACIJA DA SE UVEDE NOVA NAREDBA KOJA POCINJE SA J AHAHHAHAHAHAHAHAHAHAHAHAHAH
    else { //ovde za sve ostale naredbe sa jednim operandom! ALI TO SU SAMO PUSH I POP LOL - ni one ne mogu da imaju sufikse niti registri mogu da se dele!
        unsigned int oc=dohvatiOC(instr); 
        oc<<=3;
        oc|=0x4;
        tekucaSekcija->niz[lc++] = (unsigned char)oc;  //UPISAN OC
        string sve_sto_mi_treba;
        string s2;
        string adresiranje = odrediAdresiranje(operand1, sve_sto_mi_treba, s2);

        //************************************************************************* IDENTICNA OBRADA KAO ZA DVA OPERANDA ***************************************************************
        if(adresiranje == "immed"){ 
        //cout<<"IMMED Literal/sim: "<<sve_sto_mi_treba<<endl;
        int literal; 
        unsigned char opdescr=0;
        tekucaSekcija->niz[lc++] = opdescr;
        if(jelLiteral(sve_sto_mi_treba, &literal)){ 
            tekucaSekcija->niz[lc++] = prviBajt(literal);
            tekucaSekcija->niz[lc++] = drugiBajt(literal);
        }else { //SIMBOL JE MORA RELOKACIJA I SVE
            obradiSimbol(sve_sto_mi_treba, "word");   
        }
    } else if(adresiranje == "memdir"){ 
        //cout<<" MEMDIR Liter/sim: "<<sve_sto_mi_treba<<endl;

        int literal; 
        unsigned char op1descr=4<<5;  ////0x4 - memdir; nema registar, vrednosti 0!
        tekucaSekcija->niz[lc++] = op1descr;
        if(jelLiteral(sve_sto_mi_treba, &literal)){ 
            tekucaSekcija->niz[lc++] = prviBajt(literal);
            tekucaSekcija->niz[lc++] = drugiBajt(literal);
        }else { //SIMBOL JE MORA RELOKACIJA I SVE
            obradiSimbol(sve_sto_mi_treba, "word"); //u svakom sluc upisujem na dva bajta jer je ovo simbol! Operand je u MEM!!
        }

    } else if(adresiranje == "regdir"){ 
        //cout<<"REGDIR reg: "<<sve_sto_mi_treba<<endl;

        if(sve_sto_mi_treba=="psw") sve_sto_mi_treba="r9";
        else if(sve_sto_mi_treba == "pc") sve_sto_mi_treba="r7";
        int registar=atoi((sve_sto_mi_treba.substr(1,1) + "\0").c_str());

        if(sve_sto_mi_treba=="r9") registar=15;
        unsigned char op1descr=1<<5; //0x1
        op1descr|=registar<<1;
        tekucaSekcija->niz[lc++] = op1descr;

    } else if(adresiranje == "regind"){ 
        //cout<<"REGIND reg: "<<sve_sto_mi_treba<<endl;

        if(sve_sto_mi_treba=="pc") sve_sto_mi_treba="r7";
        if(sve_sto_mi_treba=="psw") sve_sto_mi_treba="r9";
        int registar=atoi((sve_sto_mi_treba.substr(1,1) + "\0").c_str());
        if(sve_sto_mi_treba=="r9") registar=15;

        unsigned char op1descr=2<<5; //0x2
        op1descr|=registar<<1;
        tekucaSekcija->niz[lc++] = op1descr;


    } else if(adresiranje=="pcrel"){ 
        //cout<<"pomerajPCREL: "<<sve_sto_mi_treba<<endl;

        unsigned char op1descr=3<<5; //0x3
        op1descr|=7<<1;   //PC registar je r7!
        tekucaSekcija->niz[lc++] = op1descr;

        obradiSimbolPCREL(sve_sto_mi_treba, lc + 2);

    } else if(adresiranje=="regindpom"){ 
        if(s2=="psw") {cout<<"Kako psw-om da adresiram memoriju???"<<endl; exit(-5);}
        int regi=atoi((s2.substr(1,1) + "\0").c_str()); //u s2 je sacuvan registar koji je koriscen a u svestomitreba je pomeraj 
        unsigned char op1descr=3<<5; //0x3
        op1descr|=regi<<1;
        tekucaSekcija->niz[lc++] = op1descr;

        int literal; 

        if(jelLiteral(sve_sto_mi_treba, &literal)){ 
            tekucaSekcija->niz[lc++] = prviBajt(literal);
            tekucaSekcija->niz[lc++] = drugiBajt(literal);
        }else { //SIMBOL JE MORA RELOKACIJA I SVE
            obradiSimbol(sve_sto_mi_treba, "word");
        }
    }

    }

}

void naredbaDvaOp( string instr,string operand1, string operand2) {
   // cout<<"Naredba sa 2 operanada je:"<< instr<<"Operand je:"<< operand1<<"Drugi je:"<< operand2<<endl; 

    unsigned char S=1;
    if(instr[instr.size()-1] =='b') { 
        if(instr != "sub") { //ovde ce uci i ako je subb!
            S=0; // velicina operanda je 1B inace uvek podrazumevam da je velciina 2B
            instr=instr.substr(0, instr.size()-1); //skini sufiks!
        }
    }
    if(instr[instr.size()-1] =='w') instr=instr.substr(0, instr.size()-1); //skini sufiks!
    
    unsigned int oc=dohvatiOC(instr); 
    oc<<=3;
    oc|=S<<2;
    tekucaSekcija->niz[lc++] = (unsigned char)oc;
    string sve_sto_mi_treba;                                             //ISPARSIRAN CIST OPERAND BEZ ZNAKOVA ZA DETERMINACIJU ADRESIRANJA
    string pomkodregindpom;
    string adresiranje1 = odrediAdresiranje(operand1, sve_sto_mi_treba, pomkodregindpom);

    // OVO SPAKUJ U FUNKCIJU I POZIVAJ SA PARAMETROM DAL JE PRVI ILI DRUGI OPERAND!

    if(adresiranje1 == "immed"){ 
        //cout<<"IMMED Literal/sim: "<<sve_sto_mi_treba<<endl;
        int literal; 
        unsigned char op1descr=0;
        tekucaSekcija->niz[lc++] = op1descr;
        if(jelLiteral(sve_sto_mi_treba, &literal)){ 
            if(S==0){  //OPERAND JE VELICINE SAMO JEDAN BAJT
                tekucaSekcija->niz[lc++] = literal;
            } else { //OPERAND VELICINE DVA BAJTA IMAM OBA IM/DI/AD bajta!
                tekucaSekcija->niz[lc++] = prviBajt(literal);
                tekucaSekcija->niz[lc++] = drugiBajt(literal);
            }
        }else { //SIMBOL JE MORA RELOKACIJA I SVE
            if(S==0) obradiSimbol(sve_sto_mi_treba, "byte");             //OVO PROVERI OBAVEZNOOO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            else obradiSimbol(sve_sto_mi_treba, "word");   
        }
    } else if(adresiranje1 == "memdir"){ 
        //cout<<" MEMDIR Liter/sim: "<<sve_sto_mi_treba<<endl;

        int literal; 
        unsigned char op1descr=4<<5;  ////0x4 - memdir; nema registar, vrednosti 0!
        tekucaSekcija->niz[lc++] = op1descr;
        if(jelLiteral(sve_sto_mi_treba, &literal)){ 
            tekucaSekcija->niz[lc++] = prviBajt(literal);
            tekucaSekcija->niz[lc++] = drugiBajt(literal);
        }else { //SIMBOL JE MORA RELOKACIJA I SVE
            obradiSimbol(sve_sto_mi_treba, "word"); //u svakom sluc upisujem na dva bajta jer je ovo simbol! Operand je u MEM!!
        }

    } else if(adresiranje1 == "regdir"){ 
        //cout<<"REGDIR reg: "<<sve_sto_mi_treba<<endl;

        if(sve_sto_mi_treba=="psw") sve_sto_mi_treba="r9";
        else if(sve_sto_mi_treba == "pc") sve_sto_mi_treba="r7";
        int registar=atoi((sve_sto_mi_treba.substr(1,1) + "\0").c_str());
        char lowHigh=0; 
        if(sve_sto_mi_treba.size()==3) { // ZNACI DA IMA SUFIKS NA REGISTRU
            if(S == 1) { cout<<"OPERAND JE VELICINE 2B, NE MOZE SE NAVESTI SUFIKS REGISTRA!"<<endl; exit(-7);}
            else { 
                if(sve_sto_mi_treba[2]=='l') lowHigh=0;
                else if(sve_sto_mi_treba[2]=='h') lowHigh=1;
                else { cout<<"GRESKA U INSTRUKCIJI"<<instr<<endl; exit(-4);}
            }
        }
        if(sve_sto_mi_treba=="r9") registar=15;
        unsigned char op1descr=1<<5; //0x1
        op1descr|=registar<<1;
        op1descr|=lowHigh;
        tekucaSekcija->niz[lc++] = op1descr;

    } else if(adresiranje1 == "regind"){ 
        //cout<<"REGIND reg: "<<sve_sto_mi_treba<<endl;

        if(sve_sto_mi_treba=="pc") sve_sto_mi_treba="r7";
        if(sve_sto_mi_treba=="psw") sve_sto_mi_treba="r9";
        int registar=atoi((sve_sto_mi_treba.substr(1,1) + "\0").c_str());
        if(sve_sto_mi_treba=="r9") registar=15;

        unsigned char op1descr=2<<5; //0x2
        op1descr|=registar<<1;
        tekucaSekcija->niz[lc++] = op1descr;


    } else if(adresiranje1=="pcrel"){ 
        //cout<<"pomerajPCREL: "<<sve_sto_mi_treba<<endl;

        //OVO TI OSTAJE ZA UJUTRO DA IZRESAVAS!!!!!!!!!!!!!!!!!!!!
        //*************************SAD CU NAPISATI JER IMAM IDEJU PA SUTRA PROVERI DETALJNO!!!!************************
        string pomp1, pomp2; 
        string res;
        int cnt2=0;

        res=odrediAdresiranje(operand2, pomp1, pomp2);  //OVDE MOZES ODMAH DA OBRADIS I DRUGI OPERAND PA SAMO NA IZLASKU PROVERI DAL JE adresiranje1==pcrel ako jeste vec imas obradjen drugi operand!!!

        if(res=="immed") { 
            cnt2=S?2:1;
        } else if(res=="memdir") { 
            cnt2=2;
        } else if(res=="regdir" || res=="regind"){
            cnt2=0;
        } else if(res=="regindpom" || res=="pcrel"){ 
            cnt2=2;
        } else {cout<<"Greska u adreisranju drugog parametra!"; exit(-5);}

        cnt2+=3; //OC OP1DESC lc: ? ? OP2DESC cnt2

        unsigned char op1descr=3<<5; //0x3
        op1descr|=7<<1;   //PC registar je r7!
        tekucaSekcija->niz[lc++] = op1descr;

        obradiSimbolPCREL(sve_sto_mi_treba, lc + cnt2);

    } else if(adresiranje1=="regindpom"){ 
        if(pomkodregindpom=="psw") {cout<<"Kako pswom da adresiram???"<<endl; exit(-5);}
        int regi=atoi((pomkodregindpom.substr(1,1) + "\0").c_str()); //u pomkodregindpom je sacuvan registar koji je koriscen a u svestomitreba je pomeraj 
        unsigned char op1descr=3<<5; //0x3
        op1descr|=regi<<1;
        tekucaSekcija->niz[lc++] = op1descr;

        int literal; 

        if(jelLiteral(sve_sto_mi_treba, &literal)){ 
            tekucaSekcija->niz[lc++] = prviBajt(literal);
            tekucaSekcija->niz[lc++] = drugiBajt(literal);
        }else { //SIMBOL JE MORA RELOKACIJA I SVE
            obradiSimbol(sve_sto_mi_treba, "word");
        }
    }
    
    //**************************************** OBRADJEN PRVI OEPRAND  - SAD IDENTICNO ZA DRUGI(pc rel malo drugacije) ************************************//

    string adresiranje2 = odrediAdresiranje(operand2, sve_sto_mi_treba, pomkodregindpom);

    // OVO SPAKUJ U FUNKCIJU I POZIVAJ SA PARAMETROM DAL JE PRVI ILI DRUGI OPERAND!

    if(adresiranje2 == "immed"){ 
        //cout<<"IMMED Literal/sim: "<<sve_sto_mi_treba<<endl;
        int literal; 
        unsigned char op2descr=0;
        tekucaSekcija->niz[lc++] = op2descr;
        if(jelLiteral(sve_sto_mi_treba, &literal)){ 
            if(S==0){  //OPERAND JE VELICINE SAMO JEDAN BAJT
                tekucaSekcija->niz[lc++] = literal;
            } else { //OPERAND VELICINE DVA BAJTA IMAM OBA IM/DI/AD bajta!
                tekucaSekcija->niz[lc++] = prviBajt(literal);
                tekucaSekcija->niz[lc++] = drugiBajt(literal);
            }
        }else { //SIMBOL JE MORA RELOKACIJA I SVE
            if(S==0) obradiSimbol(sve_sto_mi_treba, "byte");             //OVO PROVERI OBAVEZNOOO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            else obradiSimbol(sve_sto_mi_treba, "word");   
        }
    } else if(adresiranje2 == "memdir"){ 
        //cout<<" MEMDIR Liter/sim: "<<sve_sto_mi_treba<<endl;

        int literal; 
        unsigned char op1descr=4<<5;  ////0x4 - memdir; nema registar, vrednosti 0!
        tekucaSekcija->niz[lc++] = op1descr;
        if(jelLiteral(sve_sto_mi_treba, &literal)){ 
            tekucaSekcija->niz[lc++] = prviBajt(literal);
            tekucaSekcija->niz[lc++] = drugiBajt(literal);
        }else { //SIMBOL JE MORA RELOKACIJA I SVE
            obradiSimbol(sve_sto_mi_treba, "word"); //u svakom sluc upisujem na dva bajta jer je ovo simbol! Operand je u MEM!!
        }

    } else if(adresiranje2 == "regdir"){ 
        //cout<<"REGDIR reg: "<<sve_sto_mi_treba<<endl;

        if(sve_sto_mi_treba=="psw") sve_sto_mi_treba="r9";
        else if(sve_sto_mi_treba == "pc") sve_sto_mi_treba="r7";
        int registar=atoi((sve_sto_mi_treba.substr(1,1) + "\0").c_str());
        char lowHigh=0; 
        if(sve_sto_mi_treba.size()==3) { // ZNACI DA IMA SUFIKS NA REGISTRU
            if(S == 1) { cout<<"OPERAND JE VELICINE 2B, NE MOZE SE NAVESTI SUFIKS REGISTRA!"<<endl; exit(-7);}
            else { 
                if(sve_sto_mi_treba[2]=='l') lowHigh=0;
                else if(sve_sto_mi_treba[2]=='h') lowHigh=1;
                else { cout<<"GRESKA U INSTRUKCIJI"<<instr<<endl; exit(-4);}
            }
        }
        if(sve_sto_mi_treba=="r9") registar=15;
        unsigned char op1descr=1<<5; //0x1
        op1descr|=registar<<1;
        op1descr|=lowHigh;
        tekucaSekcija->niz[lc++] = op1descr;

    } else if(adresiranje2 == "regind"){ 
        //cout<<"REGIND reg: "<<sve_sto_mi_treba<<endl;

        if(sve_sto_mi_treba=="pc") sve_sto_mi_treba="r7";
        if(sve_sto_mi_treba=="psw") sve_sto_mi_treba="r9";
        int registar=atoi((sve_sto_mi_treba.substr(1,1) + "\0").c_str());
        if(sve_sto_mi_treba=="r9") registar=15;

        unsigned char op1descr=2<<5; //0x2
        op1descr|=registar<<1;
        tekucaSekcija->niz[lc++] = op1descr;


    } else if(adresiranje2=="pcrel"){ 
        //cout<<"pomerajPCREL: "<<sve_sto_mi_treba<<endl;

        //OVO TI OSTAJE ZA UJUTRO DA IZRESAVAS!!!!!!!!!!!!!!!!!!!!
        //*************************SAD CU NAPISATI JER IMAM IDEJU PA SUTRA PROVERI DETALJNO!!!!************************

        unsigned char op1descr=3<<5; //0x3
        op1descr|=7<<1;   //PC registar je r7!
        tekucaSekcija->niz[lc++] = op1descr;

        obradiSimbolPCREL(sve_sto_mi_treba, lc + 2);

    } else if(adresiranje2=="regindpom"){ 
        if(pomkodregindpom=="psw") {cout<<"Kako psw-om da adresiram memoriju???"<<endl; exit(-5);}
        int regi=atoi((pomkodregindpom.substr(1,1) + "\0").c_str()); //u pomkodregindpom je sacuvan registar koji je koriscen a u svestomitreba je pomeraj 
        unsigned char op1descr=3<<5; //0x3
        op1descr|=regi<<1;
        tekucaSekcija->niz[lc++] = op1descr;

        int literal; 

        if(jelLiteral(sve_sto_mi_treba, &literal)){ 
            tekucaSekcija->niz[lc++] = prviBajt(literal);
            tekucaSekcija->niz[lc++] = drugiBajt(literal);
        }else { //SIMBOL JE MORA RELOKACIJA I SVE
            obradiSimbol(sve_sto_mi_treba, "word");
        }
    }

}

void naredbaSkoka(string instr, string operand1){ 
    unsigned int oc=dohvatiOC(instr); 
    oc<<=3;
    oc|=0x4;
    tekucaSekcija->niz[lc++] = (unsigned char)oc;  //UPISAN OC
    int posZvezda=0;
    if((posZvezda=operand1.find('*')) != string::npos){
        int posProcenat=-1; 
        if((posProcenat=operand1.find('%')) != string::npos){ 
            if(operand1[posProcenat-1]=='('){ 
                if(operand1[posProcenat-2] == '*'){ //samo regind - bez pomeraja 0x2. IDENTICNO KAO ZA REGDIR SAMO DRUGI kod za adresiranje!!
                    operand1=operand1.substr(posProcenat+1, 2);
                    if(operand1=="pc") operand1="r7";
                    if(operand1=="ps") {cout<<"na psw da skace..."<<endl; exit(-76);}
                    int registar=atoi((operand1.substr(1,1) + "\0").c_str());
                    unsigned char op1descr=2<<5; //0x2
                    op1descr|=registar<<1;
                    tekucaSekcija->niz[lc++] = op1descr;
                } 

                else{ //NAJSLOZENIJE MOGUCE regindpom(MOGUCE I PCREL ZA REGISTAR r7/pc)

                    //prvo izvuci registar koji se koristi ako je pc, onda je pc relativno!
                    string registar=operand1.substr(posProcenat+1, 2);
                    if(registar=="r7" || registar =="pc"){  // PCREL ADRESIRANJE!!!!!!!!!!! Smatram da ne moze da se pojavi literal za pomeraj ako je naveden pc registar 
                        string pomeraj = operand1.substr(posZvezda+1, posProcenat-posZvezda-2);
                        registar="r7";
                        int regi=atoi((registar.substr(1,1) + "\0").c_str());
                        unsigned char op1descr=3<<5; //0x3
                        op1descr|=regi<<1;
                        tekucaSekcija->niz[lc++] = op1descr;

                        obradiSimbolPCREL(pomeraj, lc+2);
                    } 

                    else { //OBICNO REGINDPOM adresiranje, radi istu stvar kao i apsolutno adresiranje!
                        string pomeraj = operand1.substr(posZvezda+1, posProcenat-posZvezda-2);
                        
                        int regi=atoi((registar.substr(1,1) + "\0").c_str());
                        unsigned char op1descr=3<<5; //0x3
                        op1descr|=regi<<1;
                        tekucaSekcija->niz[lc++] = op1descr;

                        int literal; 
        
                        if(jelLiteral(pomeraj, &literal)){ 
                            tekucaSekcija->niz[lc++] = prviBajt(literal);
                            tekucaSekcija->niz[lc++] = drugiBajt(literal);
                        }else { //SIMBOL JE MORA RELOKACIJA I SVE
                            obradiSimbol(pomeraj, "word");
                        }
                    }

                }


            } else { //NEMA ZAGRADE - REGDIR!!!0x1
                operand1=operand1.substr(posProcenat+1, 2);
                if(operand1=="pc") operand1="r7";
                if(operand1=="ps") {cout<<"na psw  skaces..."<<endl; exit(-76);}
                int registar=atoi((operand1.substr(1,1) + "\0").c_str());
                unsigned char op1descr=1<<5; //0x1
                op1descr|=registar<<1;
                tekucaSekcija->niz[lc++] = op1descr;
                }

        } else { // NEMA PROCENAT IMA ZVEZDU - MEMDIR!!0x4
            operand1=operand1.substr(posZvezda+1, -1);
            int literal; 
            unsigned char op1descr=4<<5;  ////0x4 - memdir; nema registar, vrednosti 0!
            tekucaSekcija->niz[lc++] = op1descr;
            if(jelLiteral(operand1, &literal)){ 
                tekucaSekcija->niz[lc++] = prviBajt(literal);
                tekucaSekcija->niz[lc++] = drugiBajt(literal);
            }else { //SIMBOL JE MORA RELOKACIJA I SVE
                obradiSimbol(operand1, "word");
            }
        }

    } else { //NEMA ZVEZDICE - IMMED ADRESIRANJE 0x0
        int literal; 
        unsigned char op1descr=0;
        tekucaSekcija->niz[lc++] = op1descr;
        if(jelLiteral(operand1, &literal)){ 
            tekucaSekcija->niz[lc++] = prviBajt(literal);
            tekucaSekcija->niz[lc++] = drugiBajt(literal);
        }else { //SIMBOL JE MORA RELOKACIJA I SVE
            obradiSimbol(operand1, "word");
        }
    }
    
}


string odrediAdresiranje(string kod, string& s, string& pomm) {  //odredjuje vrstu adresiranja i vraca kao povratnu vrednost a sve potrebne info za to adresiranje upisuje u parametar S


    int posDolar=0;
    int posProcenat=0; 
    if((posDolar = kod.find('$')) != string::npos) {  //NEPOSREDNO ADRESIRANJE
        s=kod.substr(1,-1);
        return "immed";
    } else if((posProcenat = kod.find('%')) == string::npos) { //NEMA DOLARA NEMA NI % ZNACI DA JE MEMORIJSKO(APSOLUTNO)
        s=kod; 
        return "memdir";
    } else { // POSTOJI PROCENAT ALI NE I $ --> koriscen registar!
        if(posProcenat==0) { //REGISTARSKO DIREKTNO
            s=kod.substr(1,-1);
            return "regdir";
        } else if(kod[0]=='('){ //REGIND
            int dokle=kod.find(')');
            s=kod.substr(2, dokle - 2);
            return "regind";

        } else {  //REGINDPOM moze biti pcRel - izvuci registar 

            //prvo izvuci registar koji se koristi ako je pc, onda je pc relativno!
            string registar=kod.substr(posProcenat+1, 3);
            if(registar[2] == ')') registar = registar.substr(0,2);   //AKO NIJE PSW IZBACI ZAGRADU KOJU SI SUVISNU PROCITAO
            if(registar=="r7" || registar =="pc"){  // PCREL ADRESIRANJE!!!!!!!!!!! Smatram da ne moze da se pojavi literal za pomeraj ako je naveden pc registar 
                string pomeraj = kod.substr(0, posProcenat-1);
                s=pomeraj; 
                return "pcrel";
            } 

            else { //OBICNO REGINDPOM adresiranje, radi istu stvar kao i apsolutno adresiranje!
                string pomeraj = kod.substr(0, posProcenat-1);
                
                s=pomeraj; 
                pomm=registar;
                
                return "regindpom";
            }
        }
    }

    return kod;
}

unsigned char dohvatiOC(string mnemo){ 

    if(mnemo=="shr") return 24; 
    if(mnemo=="shl") return 23; 
    if(mnemo=="test") return 22; 
    if(mnemo=="xor") return 21; 
    if(mnemo=="or") return 20;
    if(mnemo=="and") return 19; 
    if(mnemo=="not") return 18; 
    if(mnemo=="cmp") return 17;
    if(mnemo=="div") return 16;
    if(mnemo=="mul") return 15;
    if(mnemo=="sub") return 14;
    if(mnemo=="add") return 13; 
    if(mnemo=="mov") return 12; 
    if(mnemo=="xchg") return 11; 
    if(mnemo=="pop") return 10; 
    if(mnemo=="push") return 9; 
    if(mnemo=="jgt") return 8; 
    if(mnemo=="jne") return 7; 
    if(mnemo=="jeq") return 6; 
    if(mnemo=="jmp") return 5;
    if(mnemo=="call") return 4;
    if(mnemo=="int") return 3;
    if(mnemo=="ret") return 2;
    if(mnemo=="iret") return 1;
    if(mnemo=="halt") return 0;
    else return -1;
}
