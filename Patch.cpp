
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

int Patch::addEntry(string entryname, string entrycontent, int row, bool overwrite, bool resetparams)
{
    int startrow, endrow, entryrow, lastentryrow, koordrow;
    int lastentryNbr;
    modiFile_t patch;
    string line;

    startrow = iKiCadSCHtab.getCompbeginrow(row);
    endrow = iKiCadSCHtab.getCompendrow(row);

    cout << startrow << endl << endrow << endl;

    koordrow = iKiCadSCHtab.getKoordrow(row);
    entryrow = iKiCadSCHtab.getEntryrow(row, entryname);
    // Eintrag nicht vorhanden
    if(-1 == entryrow){
        lastentryrow = iKiCadSCHtab.getLastentryrow(row);
        lastentryNbr = stoi(iKiCadSCHtab.Tableread(lastentryrow,1));
        patch.del = false;
        patch.add = true;
        patch.lineNbr = lastentryrow + 1;
        cout << koordrow << endl;
        cout << lastentryrow << endl;
        patch.line = "F " + to_string(lastentryNbr + 1) + " \"" + entrycontent + "\"" + " H " + iKiCadSCHtab.Tableread(koordrow,1) + " " + iKiCadSCHtab.Tableread(koordrow,2) + " 60 0000 C CNN " + "\"" + entryname + "\"";
    // Eintrag bereits vorhanden
    }else{
        if(!overwrite){// nicht ueberschreiben
            if(""!=iKiCadSCHtab.getEntry(row,entryname)) return -1;
        } // Eintrag vorhanden, aber leer - Ergaenzen
        row = iKiCadSCHtab.getEntryrow(row,entryname);
        patch.del = true;
        patch.add = true;
        patch.lineNbr = row;
        if(resetparams){
            patch.line = "F " + iKiCadSCHtab.Tableread(row, 1) + "\"" + entrycontent + "\"" + " H " + iKiCadSCHtab.Tableread(koordrow,1) + " " + iKiCadSCHtab.Tableread(koordrow,2) + " 60 0000 C CNN " + "\"" + entryname + "\"";
        }else{ // nur den Inhalt anpassen, Koordinaten, Ausrichtung, Sichtbarkeit (Anzeige) usw. beibehalten
            patch.line = "F " + iKiCadSCHtab.Tableread(row, 1) + " \"" + entrycontent + "\" " + iKiCadSCHtab.Tableread(row,3) + " " + iKiCadSCHtab.Tableread(row,4) + " " + iKiCadSCHtab.Tableread(row,5) + " " + iKiCadSCHtab.Tableread(row,6) + " " + iKiCadSCHtab.Tableread(row,7) + " " + iKiCadSCHtab.Tableread(row,8) + " " + iKiCadSCHtab.Tableread(row,9) + " " + iKiCadSCHtab.Tableread(row,10);
        }
    }
    patchvec.push_back(patch);
    return 0;
}
