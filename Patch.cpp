
#include "Patch.h"
#include <string>
#include <iostream>

using namespace std;

Patch::Patch()
{
}

Patch::~Patch()
{
}

int Patch::addEntryGen(string entrycontent, int row, int entryrow, string lastcol, bool overwrite, bool resetparams)
{
    int startrow, endrow, lastentryrow, koordrow;
    int lastentryNbr;
    modiFile_t patch;
    string line;


    koordrow = iKiCadSCHtab.getKoordrow(row);
    if(-1==koordrow) return -1;
    // Eintrag nicht vorhanden
    if(-1 == entryrow){
        lastentryrow = iKiCadSCHtab.getLastentryrow(row);
        lastentryNbr = stoi(iKiCadSCHtab.Tableread(lastentryrow,1));
        patch.del = false;
        patch.add = true;
        patch.lineNbr = lastentryrow + 1;

        patch.line = "F " + to_string(lastentryNbr + 1) + " \"" + entrycontent + "\"" + " H " + iKiCadSCHtab.Tableread(koordrow,1) + " " + iKiCadSCHtab.Tableread(koordrow,2) + " 60 0000 C CNN" + lastcol;
    // Eintrag bereits vorhanden
    }else{
        if(!overwrite){// nicht ueberschreiben
            if(""!=iKiCadSCHtab.Tableread(entryrow, 10)) return -1;
        } // Eintrag vorhanden, aber leer - Ergaenzen
        row = entryrow;
        patch.del = true;
        patch.add = true;
        patch.lineNbr = row;
        if(resetparams){
            patch.line = "F " + iKiCadSCHtab.Tableread(row, 1) + " \"" + entrycontent + "\"" + " H " + iKiCadSCHtab.Tableread(koordrow,1) + " " + iKiCadSCHtab.Tableread(koordrow,2) + " 60 0000 C CNN" + lastcol;
        }else{ // nur den Inhalt anpassen, Koordinaten, Ausrichtung, Sichtbarkeit (Anzeige) usw. beibehalten
            patch.line = "F " + iKiCadSCHtab.Tableread(row, 1) + " \"" + entrycontent + "\" " + iKiCadSCHtab.Tableread(row,3) + " " + iKiCadSCHtab.Tableread(row,4) + " " + iKiCadSCHtab.Tableread(row,5) + " " + iKiCadSCHtab.Tableread(row,6) + " " + iKiCadSCHtab.Tableread(row,7) + " " + iKiCadSCHtab.Tableread(row,8) + " " + iKiCadSCHtab.Tableread(row,9) + " " + iKiCadSCHtab.Tableread(row,10);
        }
    }
    patchvec.push_back(patch);
    return 0;
}

int Patch::addEntry(string entryname, string entrycontent, int row, bool overwrite, bool resetparams)
{
    int entryrow;
    entryrow = iKiCadSCHtab.getEntryrow(row, entryname);
    return addEntryGen(entrycontent, row, entryrow, " \""+entryname+"\"", overwrite, resetparams);
}

