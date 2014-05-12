#include "KiCadSCH.h"
#include <iostream>

KiCadSCH::KiCadSCH()
{
}

KiCadSCH::~KiCadSCH()
{
    if(iSCHfile.is_open()) iSCHfile.close();
}

int KiCadSCH::readSCHfile(ifstream &file)
{
    int err;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    if(!file.is_open()) return -1;
    err = tab.loadTable(file, " ");
    if(0!=err) return err;
    tab.rmquotmarks();

    return 0;
}

int KiCadSCH::readSCHfile(string filename)
{
    iSCHfile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try{
        iSCHfile.open(filename.c_str());
    }
    catch(std::ifstream::failure e){
        return -1; //Oeffnen fehlgeschlagen
    }
    return readSCHfile(iSCHfile);
}

int KiCadSCH::findrow(string fieldname, string fieldentry, int startrow, unsigned subpart)
{
    int row;
    row = getCompbeginrow(startrow);
    while(-1!=row){
        if((tab.Tableread(row,10)==fieldname)&&(tab.Tableread(row,0)=="F")) {
            if(to_string(subpart) == getUnitNbr(row)) return row;
        }
        row = tab.findrow(fieldentry, 2, row+1);
    }
    return -1;
}

int KiCadSCH::findrow(KiCadStdfn_et fieldname, string fieldentry, int startrow, unsigned subpart)
{
    int row;
    row = getCompbeginrow(startrow);
    if(NAME==fieldname){
        while(-1!=row){
            if(tab.Tableread(row,0)=="L"){
                if(to_string(subpart) == getUnitNbr(row)) return row;
            }
            row = tab.findrow(fieldentry, 1, row+1);
        }
    }

    while(-1!=row){
        if((tab.Tableread(row,1)==to_string(fieldname))&&tab.Tableread(row,0)=="F"){
            if(to_string(subpart) == getUnitNbr(row)) return row;
        }
        row = tab.findrow(fieldentry, 2, row+1);
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

// Funktion ermoeglicht das Pruefen noch nicht auf die SCH-Datei uebertragener Aenderungen
// -> Ein Eintrag kann mehrfach ueberschrieben werden.
// WICHTIG: .del bedeutet nur, dass Zeile der Originaldatei ersetzte/geloescht wird
//          Es hat keine Einfluss auf Patch-Eintraegen.
int KiCadSCH::getPatchindex(modiFile_t patch)
{
    unsigned i;
    for(i=0;i<patchvec.size();i++){
        // Wenn .del gesetzt ist, dann ueberschreibt der Eintrag den alten Eintrag der original SCH-Datei
        // d. h. der ersetzende Eintrag beginnt eine Zeile frueher
        if(patchvec[i].lineNbr - (patchvec[i].del?0:1) == patch.lineNbr -(patch.del?0:1)){
            if(patchvec[i].fieldname==patch.fieldname){
                return i;
            }
        }
    }
    return -1;
}

void KiCadSCH::updatePatchEntryNbr(void)
{
    unsigned i, j;
    int lineNbr, tmp, entryNbr;
    lineNbr = -1;

    //Aeussere Schleife - Eintrag/Zeile finden (gleiche Zeile nicht mehrmals verarbeiten
    for(j=0;j<patchvec.size();j++){
        tmp = patchvec[j].lineNbr - (patchvec[j].del?0:1);
        if(tmp==lineNbr) continue;
        lineNbr = tmp;
        entryNbr = patchvec[j].entryNbr;
        // niedrigste Eintragsnummer finden
        for(i=0;i<patchvec.size();i++){
            if(patchvec[i].lineNbr-(patchvec[i].del?0:1) == lineNbr){
                entryNbr = min(entryNbr, patchvec[i].entryNbr);
            }
        }
        // Nummern aufsteigend zuordnen
        for(i=0;i<patchvec.size();i++){
            if(patchvec[i].lineNbr-(patchvec[i].del?0:1) == lineNbr){
                patchvec[i].entryNbr = entryNbr;
                entryNbr++;
            }
        }
    }

    // Ausgabezeile um Eintragsnummer ergaenzen
    for(i=0;i<patchvec.size();i++){
        patchvec[i].line = "F " + to_string(patchvec[i].entryNbr) + patchvec[i].line;
    }

}

int KiCadSCH::getLastentryNbr(modiFile_t patch)
{
    int entryNbr;
    unsigned i;
    entryNbr = patch.lineNbr;

    for(i=0;i<patchvec.size();i++){
        // weiterer Eintrag an selber Stelle der Originaldatei
        if(patchvec[i].lineNbr-(patchvec[i].del?0:1) == patch.lineNbr-(patch.del?0:1)){
                entryNbr++;
        }
    }

    return entryNbr;
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

KiCadStdfn_et KiCadSCH::convfieldnameStdfield(string fieldname, bool namecontains, bool strcontainsname)
{
    string strtmp;
    if(fieldname=="Name"||fieldname=="name") return NAME;
    if(fieldname=="Reference"||fieldname=="reference"||fieldname=="Ref"||fieldname=="ref") return REF;
    if(fieldname=="Value"||fieldname=="value"||fieldname=="Val"||fieldname=="val") return VAL;
    if(fieldname=="Datasheet"||fieldname=="datasheet"||fieldname=="DS"||fieldname=="ds") return DS;
    if(fieldname=="Footprint"||fieldname=="footprint"||fieldname=="FP"||fieldname=="fp") return FP;
    if(namecontains){
        strtmp = "Name";
        if(string::npos!=strtmp.find(fieldname)) return NAME;
        strtmp = "name";
        if(string::npos!=strtmp.find(fieldname)) return NAME;
        strtmp = "Reference";
        if(string::npos!=strtmp.find(fieldname)) return REF;
        strtmp = "reference";
        if(string::npos!=strtmp.find(fieldname)) return REF;
        strtmp = "Value";
        if(string::npos!=strtmp.find(fieldname)) return VAL;
        strtmp = "value";
        if(string::npos!=strtmp.find(fieldname)) return VAL;
        strtmp = "Datasheet";
        if(string::npos!=strtmp.find(fieldname)) return DS;
        strtmp = "datasheet";
        if(string::npos!=strtmp.find(fieldname)) return DS;
        strtmp = "DS";
        if(string::npos!=strtmp.find(fieldname)) return DS;
        strtmp = "ds";
        if(string::npos!=strtmp.find(fieldname)) return DS;
        strtmp = "Footprint";
        if(string::npos!=strtmp.find(fieldname)) return FP;
        strtmp = "footprint";
        if(string::npos!=strtmp.find(fieldname)) return FP;
        strtmp = "FP";
        if(string::npos!=strtmp.find(fieldname)) return FP;
        strtmp = "fp";
        if(string::npos!=strtmp.find(fieldname)) return FP;
    }
    if(strcontainsname){
        if(string::npos!=fieldname.find("Name")||string::npos!=fieldname.find("name")) return NAME;
        if(string::npos!=fieldname.find("Ref")||string::npos!=fieldname.find("ref")) return REF;
        if(string::npos!=fieldname.find("Val")||string::npos!=fieldname.find("val")) return VAL;
        if(string::npos!=fieldname.find("Datasheet")||string::npos!=fieldname.find("datasheet")||string::npos!=fieldname.find("DS")||string::npos!=fieldname.find("ds")) return DS;
        if(string::npos!=fieldname.find("Footprint")||string::npos!=fieldname.find("footprint")||string::npos!=fieldname.find("FP")||string::npos!=fieldname.find("fp")) return FP;
    }
    return notStd;
}

int KiCadSCH::getEntryrow(int row, string fieldname, bool namecontains, bool strcontainsname)
{
    int startrow, endrow;
    KiCadStdfn_et Stdfn;
    startrow = getCompbeginrow(row);
    endrow = getCompendrow(row);
    Stdfn = convfieldnameStdfield(fieldname, namecontains, strcontainsname);
    if(-1==Stdfn){
        row = tab.findrow(fieldname, 10, startrow, namecontains, strcontainsname);
    }
    else{
        row = getEntryrow(row, Stdfn);
    }
    if(row>endrow) return -1;
    return row;
}

int KiCadSCH::getEntryrow(int row, KiCadStdfn_et fieldname)
{
    int startrow, endrow;
    startrow = getCompbeginrow(row);
    endrow = getCompendrow(row);
    row = startrow;
    if(NAME==fieldname) return getHeadrow(row);
    while(tab.Tableread(row,0)!="F"){
        row = tab.findrow(to_string(fieldname),1,row+1);
        if(-1==row) return -1;
    }
    if(row>endrow) return -1;
    return row;
}

string KiCadSCH::getEntry(int row, string fieldname, bool namecontains, bool strcontainsname)
{
    row = getEntryrow(row, fieldname, namecontains, strcontainsname);
    if(-1==row) return "";
    if(NAME==convfieldnameStdfield(fieldname, namecontains, strcontainsname)) return tab.Tableread(row,1);
    return tab.Tableread(row,2);
}

string KiCadSCH::getEntry(int row, KiCadStdfn_et fieldname)
{
    row = getEntryrow(row, fieldname);
    if(-1==row) return "";
    if(NAME == fieldname) return tab.Tableread(row,1);
    return tab.Tableread(row,2);
}

void KiCadSCH::getEntrys(int row, vector<datapair_t> &datavec)
{
    unsigned i;
    for(i=0;i<datavec.size();i++){
        datavec[i].fieldentry = getEntry(row, datavec[i].fieldname, datavec[i].namecontains, datavec[i].strcontainsname);
    }
}

int KiCadSCH::getHeadrow(int row)
{
    int startrow, endrow;
    startrow = getCompbeginrow(row);
    endrow = getCompendrow(row);
    row = tab.findrow("L",0,startrow);
    if(row>endrow) return -1;
    return row;
}

string KiCadSCH::getName(int row)
{
    row = getHeadrow(row);
    if(-1==row) return "";
    return tab.Tableread(row,1);
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
#
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
    string iline, oline, last_oline;
    modiFile_t currentpatch;
    int line_n;
    unsigned i;
    unsigned olineNbr = 0;

    iSCHfile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    oFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try{
        last_oline = "";
        for(line_n=0; true; line_n++){
            getline(iSCHfile, iline);
            oline = iline;
            currentpatch.add = false;
            currentpatch.del = false;
            currentpatch.line = "";
            currentpatch.lineNbr = 0;
            for(i=0;i<patchvec.size();i++){
                if(line_n == patchvec[i].lineNbr){
                    currentpatch = patchvec[i];
                    patchvec[i].prevline = last_oline;
                    if(currentpatch.add){
                        oFile << currentpatch.line << endl;
                        olineNbr++;
                        patchvec[i].olineNbr = olineNbr;
                    }
                }
            }
            if(!currentpatch.del){
                oFile << oline << endl;
                last_oline = oline;
                olineNbr++;
                if(patchvec.size()>i){ // if patchvec is empty
                    patchvec[i].olineNbr = olineNbr;
                }
            }else{
                if(patchvec.size()<i) patchvec[i].deletedline = oline;
            }
        }
    }
    catch(std::fstream::failure e){
        if(iSCHfile.bad()) return -1;
        if(oFile.bad()) return -2;
        if(iSCHfile.fail()) return -1;
        if(oFile.fail()) return -2;
        if(iSCHfile.eof()){
            iSCHfile.clear();
            iSCHfile.seekg(0, ios::beg);
            return 0;
        }
        if(oFile.eof()) {
            oFile.clear();
            return -2;
        }
        return -255;
    }
    return 0;
}

int KiCadSCH::addEntryGen(string entrycontent, int row, int entryrow, string lastcol, bool overwrite, bool resetparams, unsigned uvi)
{
    int lastentryrow, koordrow;
    int lastentryNbr;
    int index;
    modiFile_t patch;

    patch.uvi = uvi;
    koordrow = getKoordrow(row);
    if(-1==koordrow) return -1;
    // Eintrag nicht vorhanden
    if(-1 == entryrow){
        lastentryrow = getLastentryrow(row);
        lastentryNbr = stoi(tab.Tableread(lastentryrow, 1));
        patch.del = false;
        patch.add = true;
        patch.fieldname = lastcol;
        patch.entryNbr = lastentryNbr + 1;
        patch.lineNbr = lastentryrow + 1; // bei Mehrfacheintraegen, mehrfach gleiche Zeile -> updatePatchEntryNbr()

        patch.line  = " \"" + entrycontent + "\"" + " H " + tab.Tableread(koordrow,1) + " " + tab.Tableread(koordrow,2)
                    + " 60 0000 C CNN" + lastcol;
    // Eintrag bereits vorhanden
    }else{
        if(!overwrite){// nicht ueberschreiben
            if(""!=tab.Tableread(entryrow, 2)) return -1;
        } // Eintrag vorhanden, aber leer - Ergaenzen
        row = entryrow;
        patch.del = true;
        patch.add = true;
        patch.fieldname = lastcol;
        patch.entryNbr = stoi(tab.Tableread(row, 1));
        patch.lineNbr = row; // bei Mehrfacheintraegen, mehrfach gleiche Zeile -> updatePatchEntryNbr()
        if(resetparams){
            patch.line  = " \"" + entrycontent + "\"" + " H " + tab.Tableread(koordrow,1) + " " + tab.Tableread(koordrow,2)
                        + " 60 0000 C CNN" + lastcol; // hier keine Anfuehrungszeichen hinzufuegen, damit der Eintrag nicht nur leer, sondern auch nicht vorhanden sein kann
        }else{ // nur den Inhalt anpassen, Koordinaten, Ausrichtung, Sichtbarkeit (Anzeige) usw. beibehalten
            patch.line  = " \"" + entrycontent + "\" " + tab.Tableread(row,3) + " " + tab.Tableread(row,4) + " "
                        + tab.Tableread(row,5) + " " + tab.Tableread(row,6) + " " + tab.Tableread(row,7) + " " + tab.Tableread(row,8) + " " + tab.Tableread(row,9)
                        + " \"" + tab.Tableread(row,10) + "\""; // Aus Tabelle wurden Anfuehrungszeichen entfernt - deshalb wieder hinzuuegen
        }
    }

    // Mehrfachueberschreiben von Eintraegen (bzw. nicht Ueberschreiben)
    index = getPatchindex(patch);
    if(-1!=index){
        if(overwrite){
            patchvec[index] = patch;
        }
    }else{
        patchvec.push_back(patch);
    }

    return 0;
}


int KiCadSCH::addEntry(string entryname, string entrycontent, int row, bool overwrite, bool resetparams, unsigned uvi)
{
    int entryrow;
    KiCadStdfn_et Stdfn;
    Stdfn = convfieldnameStdfield(entryname);
    entryrow = getEntryrow(row, entryname);
    if(notStd==Stdfn)   return addEntryGen(entrycontent, row, entryrow, " \""+entryname+"\"", overwrite, resetparams, uvi);
    else                return addEntryGen(entrycontent, row, entryrow, "", overwrite, resetparams, uvi);
}

int KiCadSCH::addEntry(KiCadStdfn_et entryname, string entrycontent, int row, bool overwrite, bool resetparams, unsigned uvi)
{
    int entryrow;
    entryrow = getEntryrow(row, entryname);
    return addEntryGen(entrycontent, row, entryrow, "", overwrite, resetparams, uvi);
}

int KiCadSCH::addEntrys(vector<datapair_t> newdata, int row)
{
    unsigned i;
    int entriesnotoverwritten;
    entriesnotoverwritten = 0;
    for(i=0;i<newdata.size();i++){
        if(""!=newdata[i].fieldentry||newdata[i].allowemptyentries){
            if(-1==addEntry(newdata[i].fieldname, newdata[i].fieldentry, row, newdata[i].overwrite, newdata[i].resetparams, i)) entriesnotoverwritten++;
        }
    }
    return entriesnotoverwritten;
}

unsigned KiCadSCH::getpatchsize(void)
{
    return patchvec.size();
}

int KiCadSCH::printPatch(void)
{
    modiFile_t c;
    unsigned i;

    for(i=0; i<patchvec.size(); i++){
        c = patchvec[i];
        cout << "  " << c.lineNbr-1 << ": " << c.prevline << endl;
        if(c.del) cout << "- " << c.lineNbr << ": " << c.deletedline << endl;
        if(c.add) cout << "+ " << c.lineNbr << " <- " << "(row)" << ": " << c.line << endl;
    }
    return i;
}

size_t KiCadSCH::getPatchsize(void)
{
    return patchvec.size();
}


void printrowvec(vector<int> rows)
{
    unsigned i;
    for(i=0;i<rows.size()-1;i++){
        cout << rows[i] << ", ";
    }
    cout << rows.back() << endl;
}


void KiCadSCH::printoplogentr(oplog_t entry)
{
    unsigned i, j;
    if(0==entry.updatev.size()) return;
    cout << "SEARCH:" << endl;
    for(i=0;i<entry.searchv.size();i++){
        cout << "  " << entry.searchv[i].fieldname << " = " << entry.searchv[i].fieldentry << endl;
    }
    cout << "  -> SCHrow = " << entry.SCHrow << endl;
    cout << "FOUND:" << endl;
    cout << "  DBrows = ";
    printrowvec(entry.DBrows);
    cout << "  DBrows (best) = ";
    printrowvec(entry.DBbestrows);
    cout << "SELECT:" << endl;
    cout << "  DBrow = " << entry.DBrow << endl;
    cout << "UPDATE:" << endl;
    j = entry.patchstartindex;
    for(i=0;i<entry.updatev.size();i++){
        if(patchvec[j].uvi == i){
            cout << "  " << patchvec[j].lineNbr << " -> " << patchvec[j].olineNbr << ": " << entry.updatev[i].fieldname << " = " << entry.updatev[i].fieldentry << endl;
            j++;
        }
    }
}

void KiCadSCH::printoplog(vector<oplog_t> entrys)
{
    unsigned i;
    for(i=0;i<entrys.size();i++){
        printoplogentr(entrys[i]);
        cout << endl;
    }
}

