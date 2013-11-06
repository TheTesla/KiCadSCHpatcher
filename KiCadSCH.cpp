#include "KiCadSCH.h"



KiCadSCH::KiCadSCH()
{

}

KiCadSCH::~KiCadSCH()
{
    if(iSCHfile.is_open()) iSCHfile.close();
}

int KiCadSCH::readSCHfile(ifstream &file)
{
    if(!file.is_open()) return -1;
    tab.loadTable(file);
    file.clear();
    file.seekg(0, ios::beg);
    return 0;
}

int KiCadSCH::readSCHfile(string filename)
{
    iSCHfile.open(filename.c_str());
    return readSCHfile(iSCHfile);
}

int KiCadSCH::findrow(string fieldname, string fieldentry, int startrow, unsigned subpart)
{
    int row;
    row = tab.findrow("$Comp",startrow);
    while(-1!=row){
        if((tab.Tableread(row,10)=="\""+fieldname+"\"")&&tab.Tableread(row,0)=="F") {
            if(to_string(subpart) == tab.getUnitNbr(row)) return row;
        }
        row = tab.findrow("\""+fieldentry+"\"", 2, row+1);
    }
    return -1;
}

int KiCadSCH::findrow(KiCadStdfn_et fieldname, string fieldentry, int startrow, unsigned subpart)
{
    int row;
    row = tab.findrow("$Comp", 0, startrow);
    while(-1!=row){
        if((tab.Tableread(row,1)==to_string(fieldname))&&tab.Tableread(row,0)=="F"){
            if(to_string(subpart) == tab.getUnitNbr(row)) return row;
        }
        row = tab.findrow("\""+fieldentry+"\"", 2, row+1);
    }
    return -1;
}

int KiCadSCH::getCompbeginrow(int row)
{
    row = tab.findrow_bw("$Comp",0,row);
    return row;
}

int KiCadSCH::getCompendrow(int row)
{
    row = tab.findrow("$EndComp",0,row);
    return row;
}

int KiCadSCH::getLastentryrow(int row)
{
    int startrow, endrow;
    startrow = getCompbeginrow(row);
    endrow = getCompendrow(row);
    row = tab.findrow_bw("F",0,endrow);
    if(row<startrow) return -1;
    return row;
}

int KiCadSCH::getEntryrow(int row, string fieldname)
{
    int startrow, endrow;
    startrow = getCompbeginrow(row);
    endrow = getCompendrow(row);
    row = tab.findrow("\""+fieldname+"\"",10,startrow);
    if(row>endrow) return -1;
    return row;
}

int KiCadSCH::getEntryrow(int row, KiCadStdfn_et fieldname)
{
    int startrow, endrow;
    startrow = getCompbeginrow(row);
    endrow = getCompendrow(row);
    row = startrow;
    while(tab.Tableread(row,0)!="F"){
        row = tab.findrow(to_string(fieldname),1,row+1);
        if(-1==row) return -1;
    }
    if(row>endrow) return -1;
    return row;
}


string KiCadSCH::getEntry(int row, string fieldname)
{
    row = getEntryrow(row, fieldname);
    if(-1==row) return "";
    return tab.Tableread(row,2);
}

string KiCadSCH::getEntry(int row, KiCadStdfn_et fieldname)
{
    row = getEntryrow(row, fieldname);
    if(-1==row) return "";
    return tab.Tableread(row,2);
}

int KiCadSCH::getUnitrow(int row)
{
    int startrow, endrow;
    startrow = getCompbeginrow(row);
    endrow = getCompendrow(row);
    row = tab.findrow("U",0,startrow);
    if(row>endrow) return -1;
    return row;
}

string KiCadSCH::getUnitNbr(int row)
{
    row = getUnitrow(row);
    if(-1==row) return "";
    return tab.Tableread(row,1);
}

int KiCadSCH::getKoordrow(int row)
{
    int startrow, endrow;
    startrow = getCompbeginrow(row);
    endrow = getCompendrow(row);
    row = tab.findrow("P",0,startrow);
    if(row>endrow) return -1;
    return row;
}


int KiCadSCH::patchFile(ofstream &oFile)
{
    string iline, oline;
    modiFile_t currentpatch;
    int line_n;
    unsigned i;
    if(!iSCHfile.is_open()) return -1;
    if(!oFile.is_open()) return -2;

    for(line_n=0; oFile.good()&&iSCHfile.good(); line_n++){
        getline(iSCHfile, iline);
        oline = iline;
        currentpatch.add = false;
        currentpatch.del = false;
        currentpatch.line = "";
        currentpatch.lineNbr = 0;
        for(i=0;i<patchvec.size();i++){
            if(line_n == patchvec[i].lineNbr){
                currentpatch = patchvec[i];
                if(currentpatch.add){
                    oFile << currentpatch.line << endl;
                }
            }
        }
        if(!currentpatch.del){
            oFile << oline << endl;
        }
    }
    return 0;
}

int KiCadSCH::addEntryGen(string entrycontent, int row, int entryrow, string lastcol, bool overwrite, bool resetparams)
{
    int startrow, endrow, lastentryrow, koordrow;
    int lastentryNbr;
    modiFile_t patch;
    string line;


    koordrow = getKoordrow(row);
    if(-1==koordrow) return -1;
    // Eintrag nicht vorhanden
    if(-1 == entryrow){
        lastentryrow = getLastentryrow(row);
        lastentryNbr = stoi(tab.Tableread(lastentryrow,1));
        patch.del = false;
        patch.add = true;
        patch.lineNbr = lastentryrow + 1;

        patch.line = "F " + to_string(lastentryNbr + 1) + " \"" + entrycontent + "\"" + " H " + tab.Tableread(koordrow,1) + " " + tab.Tableread(koordrow,2) + " 60 0000 C CNN" + lastcol;
    // Eintrag bereits vorhanden
    }else{
        if(!overwrite){// nicht ueberschreiben
            if(""!=tab.Tableread(entryrow, 10)) return -1;
        } // Eintrag vorhanden, aber leer - Ergaenzen
        row = entryrow;
        patch.del = true;
        patch.add = true;
        patch.lineNbr = row;
        if(resetparams){
            patch.line = "F " + tab.Tableread(row, 1) + " \"" + entrycontent + "\"" + " H " + tab.Tableread(koordrow,1) + " " + tab.Tableread(koordrow,2) + " 60 0000 C CNN" + lastcol;
        }else{ // nur den Inhalt anpassen, Koordinaten, Ausrichtung, Sichtbarkeit (Anzeige) usw. beibehalten
            patch.line = "F " + tab.Tableread(row, 1) + " \"" + entrycontent + "\" " + tab.Tableread(row,3) + " " + tab.Tableread(row,4) + " " + tab.Tableread(row,5) + " " + tab.Tableread(row,6) + " " + tab.Tableread(row,7) + " " + tab.Tableread(row,8) + " " + tab.Tableread(row,9) + " " + tab.Tableread(row,10);
        }
    }
    patchvec.push_back(patch);
    return 0;
}

int KiCadSCH::addEntry(string entryname, string entrycontent, int row, bool overwrite, bool resetparams)
{
    int entryrow;
    entryrow = tab.getEntryrow(row, entryname);
    return addEntryGen(entrycontent, row, entryrow, " \""+entryname+"\"", overwrite, resetparams);
}

int KiCadSCH::addEntry(KiCadStdfn_et entryname, string entrycontent, int row, bool overwrite, bool resetparams)
{
    int entryrow;
    entryrow = tab.getStdAttribrow(to_string(entryname), row);
    return addEntryGen(entrycontent, row, entryrow, "", overwrite, resetparams);
}

