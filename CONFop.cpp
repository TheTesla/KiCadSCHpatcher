#include "CONFop.h"

#include <iostream>

CONFop::CONFop()
{
}

CONFop::~CONFop()
{
    if(iCONFfile.is_open()) iCONFfile.close();
}

int CONFop::readCONFfile(ifstream &file)
{
    int err;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    if(!file.is_open()) return -1;
    err = tab.loadTable(file, ",", " \t", " \t");
    if(0!=err) return err;
    tab.rmquotmarks();
    tab.eraseemptyrows();

    return 0;
}

int CONFop::readCONFfile(string filename)
{
    iCONFfile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try{
        iCONFfile.open(filename.c_str());
    }
    catch(std::ifstream::failure e) {
        return -1; // Oeffenen fehlgeschlagen
    }
    return readCONFfile(iCONFfile);
}

CONFreadstate_et CONFop::getBlock(int &row, CSVparams_t &csv, vector<datapair_t> &searchvec, vector<datapair_t> &updatevec)
{
    string str;
    CONFreadstate_et state;
    state = NONE;
    datapair_t entry;
    while(tab.OK){
        str = tab.Tableread(row, 0);
        switch(state){
            case NONE:
                if("DATABASE"==str){
                    state = DATABASE;
                }
                if("SEARCH"==str){
                    state = SEARCH;
                }
                if("UPDATE"==str){
                    state = ERROR;
                    return state;
                }
                break;
            case DATABASE:
                if("SEARCH"==str){
                    state = SEARCH;
                }
                if("file"==str){
                    csv.filename = tab.Tableread(row, 1);
                }
                if("ignorebefore"==str){
                    csv.ignorebefore = tab.Tableread(row, 1);
                }
                if("ignoreafter"==str){
                    csv.ignoreafter = tab.Tableread(row, 1);
                }
                if("delim"==str){
                    csv.delim = tab.Tableread(row, 1);
                }
                break;
            case SEARCH:
                if("UPDATE"==str){
                    state = UPDATE;
                    break;
                }
            case UPDATE:
                if("DATABASE"==str || "SEARCH"==str || "UPDATE"==str){
                    state = READY;
                    return state;
                }
                entry.namecontains = false;
                entry.strcontainsname = false;
                entry.entrycontains = false;
                entry.strcontainsentry = false;
                entry.takeDatabasefieldname = false;
                entry.resetparams = false;
                entry.overwrite = false;
                entry.allowemptyentries = false;
                if("namecontains"==str){
                    entry.namecontains = true;
                    entry.strcontainsname = false;
                }
                if("strcontainsname"==str){
                    entry.namecontains = false;
                    entry.strcontainsname = true;
                }
                entry.fieldname = tab.Tableread(row, 1);
                if('$'==entry.fieldname[0]) {
                    entry.valuesearch = true;
                    entry.fieldname = entry.fieldname.substr(1, std::string::npos);
                }
                if("entrycontains"==tab.Tableread(row, 2)){
                    entry.entrycontains = true;
                    entry.strcontainsentry = false;
                }
                if("strcontainsentry"==tab.Tableread(row, 2)){
                    entry.entrycontains = false;
                    entry.strcontainsentry = true;
                }
                if(string::npos!=tab.Tableread(row, 3).find("takeDatabasefieldname")){
                    entry.takeDatabasefieldname = true;
                }
                if(string::npos!=tab.Tableread(row, 3).find("resetparams")){
                    entry.resetparams = true;
                }
                if(string::npos!=tab.Tableread(row, 3).find("overwrite")){
                    entry.overwrite = true;
                }
                if(string::npos!=tab.Tableread(row, 3).find("allowemptyentries")){
                    entry.allowemptyentries = true;
                }
                if(SEARCH==state){
                    if(""!=entry.fieldname) searchvec.push_back(entry);
                }
                if(UPDATE==state){
                    if(""!=entry.fieldname) updatevec.push_back(entry);
                }
                break;
            default:
                return state;


        }

    row++;
    }
    state = EOFile;
    return state;
}
