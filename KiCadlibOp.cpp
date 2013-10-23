
#include "KiCadlibOp.h"



// Ausgabe für die lib-Datei
string ICSymprint(ICdevSym Sym)
{
    unsigned long i;
    stringstream ICprint;
    
    ICprint << "# " << Sym.name << endl << "#" << endl;
    ICprint << "DEF " << Sym.name << " " << Sym.reference << " 0 " << "20 Y Y 1 F N"<< endl;
    ICprint << "F0 \"" << Sym.reference << "\" " << Sym.x1 << " " << Sym.y1 + signed(Sym.height) + 30<< " 50 H V L BNN" << endl;
    ICprint << "F1 \"" << Sym.name << "\" " << Sym.x1 + signed(Sym.width) << " " << Sym.y1 - 100<< " 50 H V R BNN" << endl;
    ICprint << "F2 \"" << Sym.Package << "\" " << Sym.x1 << " " << Sym.y1 - 50<< " 50 H V L CNN" << endl;
    ICprint << "DRAW" << endl;
    ICprint << "S " << Sym.x1 << " " << Sym.y1 << " " << Sym.x1 + signed(Sym.width) << " " << Sym.y1 + signed(Sym.height) << " 0 1 0 f" << endl; 
    for(i=0;i<Sym.Pincount;i++){
        ICprint << "X " << Sym.Pins[i].name << " " << Sym.Pins[i].num << " " << Sym.Pins[i].posx << " " << Sym.Pins[i].posy << " " << Sym.Pins[i].length << " " << Sym.Pins[i].direction << " 40 40 1 1 " << Sym.Pins[i].electrical_type << " " << Sym.Pins[i].pin_type << " " << endl;
    }
    ICprint << "ENDDRAW" << endl;
    ICprint << "ENDDEF" << endl;
    ICprint << "#" << endl; 
    return ICprint.str();
}

void KiCadlibRemoveSym(string libFilename, string Symname)
{
    ofstream olibFile;
    ifstream ilibFile;
    string tmpFilename;
    string line;
    tmpFilename = "~";
    tmpFilename=libFilename+"~";
    olibFile.open(tmpFilename.c_str());
    if(!olibFile.is_open()) goto oFileFailed;
    ilibFile.open(libFilename.c_str());
    if(!ilibFile.is_open()) goto iFileFailed;
    
    
    while(ilibFile.good()){
        getline(ilibFile, line);
        // Kommentarblock am Kopf des Eintrags entfernen
        if((0 == line.find("# "+Symname)) && (line.length()==Symname.length()+2)){
            cout << "Removed lib-comment" << endl;
            while(ilibFile.good()){
                getline(ilibFile, line);
                if(0 == line.find("#")) continue;
                break;
            }
        }
        
        // gesamten Eintrag entfernen 
        if(0 == line.find("DEF "+Symname+" ")){
            cout << "Removed lib-entry" << endl;
            while(ilibFile.good()){
                getline(ilibFile, line);
                if(0 == line.find("ENDDEF")) break;
            }            
            while(ilibFile.good()){
                getline(ilibFile, line);
                if(0 == line.compare("#")) continue;
                break;
            }              
        }
        
        olibFile << line;
        // erneute Prüfung nach getline(), bevor Zeilenumbruch  angehöngt wird
        // sonst wird die Datei immer eine Zeile länger

        if(ilibFile.good()) olibFile << endl;    
    }

    ilibFile.close();
    iFileFailed:           
    olibFile.close();
    remove(libFilename.c_str());
    rename(tmpFilename.c_str(), libFilename.c_str());
    oFileFailed:
    return;
}


void KiCadlibInsertSym(string libFilename, string Symname, string content)
{
    ofstream olibFile;
    ifstream ilibFile;
    string tmpFilename;
    string line;
    string firstline_of_next_entry;
    int linenum, linenum_prev_def;
    bool libempty;
    tmpFilename=libFilename+"~";
    olibFile.open(tmpFilename.c_str());
    if(!olibFile.is_open()) goto oFileFailed;
    ilibFile.open(libFilename.c_str());
    if(!ilibFile.is_open()) goto iFileFailed;    
    
    // Zeile ermitteln, wo der letzte Eintrag vor dem neuen Eintrag beginnt.
    linenum = 0;
    linenum_prev_def = 0;
    libempty = true;
    while(ilibFile.good()){
        getline(ilibFile, line);
        if(0 == line.find("DEF ")){
            libempty = false;
            if(line.substr(4) > Symname) break;
            else{
                linenum_prev_def = linenum;
            }
        }
        linenum++;
    }
    
    // Zurückspulen
    ilibFile.clear();
    ilibFile.seekg(0,ios::beg);

    // Sonderfall lib-Header vorhanden, aber kein Bauteil-Eintrag
    if(libempty){
        while(ilibFile.good()){
            getline(ilibFile, line);
            olibFile << line << endl;
            if(0 == line.find("EESchema-LIBRARY")){
                olibFile << "#encoding utf-8" << endl;
                olibFile << "#" << endl;
                while(ilibFile.good()){// Rest verwerfen
                    getline(ilibFile, line);
                }
                break;
            }
        }
        cout << "Added entry to empty lib with existing header" << endl;        
    }
    // Sonderfall: neuer Eintrag vor dem ersten Eintrag
    else if(0 == linenum_prev_def){
        while(ilibFile.good()){
            getline(ilibFile, line);
            if(0 == line.find("# ") && 2 < line.size()){
                firstline_of_next_entry = line;
                break;
            }
            else{
                olibFile << line << endl;
            }
        }
        cout << "Added entry to lib at first place" << endl;        
    }    
    else{// Normalfall: neuer Eintrag dazwischen oder am Ende
        
        // Kopieren bis zum Ende des letzten Eintrags vor dem neuen Eintrag
        linenum = 0;
        while(ilibFile.good()){
            // Kopieren bis Anfang des letzten Eintrags
            getline(ilibFile, line);
            olibFile << line << endl;
            //Kopiere den Rest bis Ende des letzten Eintrags
            if(linenum++>linenum_prev_def){
                if(0 == line.find("ENDDEF")){
                    // leere Kommentare am Ende des letzten Eintrags mit kopieren
                    while(ilibFile.good()){
                        getline(ilibFile, line); 
                        if(line == "#"){
                            olibFile << line << endl;
                            continue;    
                        }
                        break;
                    }
                    firstline_of_next_entry = line; // bereits Zeile des folgenden Eintrags
                    break;
                }
            }
        }
        cout << "Added entry to lib" << endl;        
    }
    // Einfügen des neuen Eintrags    
    olibFile << content;
    
    if(false == libempty){
        olibFile << firstline_of_next_entry << endl;
        
        while(ilibFile.good()){
            getline(ilibFile, line);
            olibFile << line;
            if(ilibFile.good()) olibFile << endl;
        }
    }
    else{
        olibFile << "#End Library" << endl;
    }
    

    ilibFile.close();
    iFileFailed:           
    olibFile.close();
    remove(libFilename.c_str());
    rename(tmpFilename.c_str(), libFilename.c_str());
    oFileFailed:
    return;
}

int checkKiCadlibexists(string libFilename)
{
    ifstream ilibFile;
    string line;
    ilibFile.open(libFilename.c_str());
    if(ilibFile.is_open()){
        while(ilibFile.good()){
            getline(ilibFile, line);
            if(0 == line.find("EESchema-LIBRARY")) return 1;
        }
    }
    return 0;
}

void KiCadinitlib(string libFilename, string content)
{
    ofstream olibFile;
    string line;
    olibFile.open(libFilename.c_str());
    time_t now = time(0);
    tm* localtm = localtime(&now);
    olibFile << "EESchema-LIBRARY Version 1.0  Date: " << setfill('0') << setw(2) << localtm->tm_mday << "." << setfill('0') << setw(2) << localtm->tm_mon+1 << "." << localtm->tm_year+1900 << " "<< setfill('0') << setw(2) << localtm->tm_hour << ":"<< setfill('0') << setw(2) << localtm->tm_min << ":"<< setfill('0') << setw(2) << localtm->tm_sec << endl;
    olibFile << "#encoding utf-8" << endl;
    olibFile << "#" << endl;
    olibFile << content;
    olibFile << "#End Library" << endl;
    olibFile.close();
}

