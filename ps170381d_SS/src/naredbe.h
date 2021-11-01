#include "tabSimbola.h"
unsigned char dohvatiOC(string mnemo);

void naredbaBezOp(string instr);
void naredbaJedanOp(string instr,string operand1);

void naredbaDvaOp( string instr,string operand1, string operand2);
void naredbaSkoka(string instr, string operand1);

extern string obradiSimbol(string mojSimbol, string direktiva);


unsigned int prviBajt(unsigned int mojBroj);

unsigned int drugiBajt(unsigned int mojBroj);

string odrediAdresiranje(string kod, string& s, string& s1);