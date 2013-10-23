
/*
  Name: KiCadICgen - editing symbol
  Copyright: 2012 Stefan Helmert
  Author: Stefan Helmert
  Date: 07.12.12 18:07
  Description: 
    - stretches symbol-rectangle
    - places pins around the symbol
    - grid-feature: rounds coordinates to stepsize
*/


#ifndef ICSYMBOL_H
#define ICSYMBOL_H

#include <string>


using namespace std;

// Grobe Pin-Beschreibung -> Pin-Position etc. wird daraus erzeugt
// Ein Array dieser Strukturen pro Symbolkante
// Array-Eintrag 0 ist kein Pin -> Abstandsdefinition zum Symbolrand
struct PinDescr
{
    string name;
    unsigned long num;
    unsigned long RelSpace;
    unsigned long AbsSpace;
    string electrical_type;
    string pin_type;
};

// Struktur beschreibt Pin am Bauteil detailiert (genaue Lage) im Schaltplansymbol
struct ICpinSym
{
    string name;
    unsigned long num;
    signed long posx;
    signed long posy;
    unsigned long length;
    char direction;
    string electrical_type;
    string pin_type;
};

// Struktur beschreibt Bauteil (Schaltplan-Symbol)
struct ICdevSym
{
    string name;
    string reference;
    string Package;
    string Library;
    unsigned long height;
    unsigned long width;
    signed long x1, y1;
    ICpinSym* Pins;
    unsigned long Pincount;        
};

signed long intround(signed long x, unsigned long stepsize);
void roundICcoords(ICdevSym &Sym, unsigned long PinEntryNum, unsigned long stepsize);
void updateICSymSize(ICdevSym &Sym, PinDescr* PinList, unsigned long PinEntryNum, char PinDir);
void ICSymPinPlace(ICdevSym &Sym, PinDescr* PinList, unsigned long PinEntryNum, char PinDir, unsigned long PinLength);
#endif
