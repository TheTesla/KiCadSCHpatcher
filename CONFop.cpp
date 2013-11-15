#include "CONFop.h"

#include <iostream>

CONFop::CONFop()
{
    //ctor
}

CONFop::~CONFop()
{
    if(iCONFfile.is_open()) iCONFfile.close();

}

int CONFop::readCONFfile(ifstream &file)
{
    if(!file.is_open()) return -1;
    tab.loadTable(file, ",", " \t", " \t");
    tab.rmquotmarks();
    tab.eraseemptyrows();
    file.clear();
    file.seekg(0, ios::beg);
    return 0;
}

int CONFop::readCONFfile(string filename)
{
    iCONFfile.open(filename.c_str());
    return readCONFfile(iCONFfile);
}

CONFreadstate_et CONFop::getBlock(int &row, CSVparams_t &csv, vector<datapair_t> &searchvec, vector<datapair_t> &updatevec)
{
    int col;
    string str;
    CONFreadstate_et state;
    col = 0;
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
                    csv.filename = "Database.csv";
                    csv.delim = "\t";
                    csv.ignoreafter = " ";
                    csv.ignorebefore = " ";
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
                if("namecontains"==str){
                    entry.namecontains = true;
                    entry.strcontainsname = false;
                }
                if("strcontainsname"==str){
                    entry.namecontains = false;
                    entry.strcontainsname = true;
                }
                entry.fieldname = tab.Tableread(row, 1);

                if("entrycontains"==tab.Tableread(row, 2)){
                    entry.entrycontains = true;
                    entry.strcontainsentry = false;
                }
                if("strcontainsentry"==tab.Tableread(row, 2)){
                    entry.entrycontains = false;
                    entry.strcontainsentry = true;
                }
                if("takeDatabasefieldname"==tab.Tableread(row, 3)){
                    entry.takeDatabasefieldname = true;
                }
                if(SEARCH==state){
                    if(""!=entry.fieldname) searchvec.push_back(entry);
                }
                if(UPDATE==state){
                    if(""!=entry.fieldname) updatevec.push_back(entry);
                }
                break;

        }

    row++;
    }
    state = EOFile;
    return state;
}
