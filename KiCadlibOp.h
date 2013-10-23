

/*
  Name: KiCadICgen - KiCad-lib-file access
  Copyright: 2012 Stefan Helmert
  Author: Stefan Helmert
  Date: 07.12.12 18:07
  Description: 
    - generate entry for lib-file
    - remove old entry
    - add new entry on the right place (sorted alphabetic)
    - check if lib-file exists
    - initialize new lib-file
*/



#ifndef KICADLIBOP_H
#define KICADLIBOP_H

#include "ICSymbol.h"
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <ctime>
#include <iomanip>

using namespace std;

string ICSymprint(ICdevSym Sym);
void KiCadlibRemoveSym(string libFilename, string Symname);
void KiCadlibInsertSym(string libFilename, string Symname, string content);
int checkKiCadlibexists(string libFilename);
void KiCadinitlib(string libFilename, string content);

#endif
