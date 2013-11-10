
/*
  Name: KiCadICgen
  Copyright: 2012 Stefan Helmert
  Author: Stefan Helmert
  Date: 07.12.12 18:07
  Description: generates or modiefies KiCad-lib-file from a pin-file
*/



#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>
#include "Table.h"
#include "ICSymbol.h"
#include "ICProcTable.h"
#include <ctime>
#include <iomanip>
#include <stdio.h>
#include "KiCadlibOp.h"
#include "Patch.h"
#include "KiCadSCH.h"
#include "CSVop.h"

using namespace std;



int patchFile(ifstream &iFile, ofstream &oFile, vector<modiFile_t> patch)
{
    string iline, oline;
    modiFile_t currentpatch;
    int line_n;
    unsigned i;
    if(!iFile.is_open()) return -1;
    if(!oFile.is_open()) return -2;

    for(line_n=0; oFile.good()&&iFile.good(); line_n++){
        getline(iFile, iline);


        oline = iline;
        currentpatch.add = false;
        currentpatch.del = false;
        currentpatch.line = "";
        currentpatch.lineNbr = 0;
        for(i=0;i<patch.size();i++){
            if(line_n == patch[i].lineNbr){
                currentpatch = patch[i];
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

string rmquotmarks(string str)
{
    int spos, epos;
    spos = str.find_first_not_of("\"");
    epos = str.find_last_not_of("\"");
    if(spos!=string::npos&&epos!=string::npos) str = str.substr(spos, epos-spos+1);
    return str;
}

void rmquotmarks(vector<datapair_t> &data)
{
    int i;
    for(i=0;i<data.size();i++){
        data[i].fieldentry = rmquotmarks(data[i].fieldentry);
    }
}


int main(int argc, char *argv[])
{
    cout << "blub"  << endl;
    string KiCadSCHFilename;
    //Table KiCadSCHTab;
    KiCadSCH kicadsch;


    KiCadSCHFilename = "ATUC256L4U.sch";
    kicadsch.readSCHfile(KiCadSCHFilename);

    int i, row;

    row = kicadsch.findrow(VAL,"BAS70BRW", 120, 1);
    cout << row << endl;
    cout << kicadsch.getEntry(row, VAL)<< endl;

    ofstream oFile;
    oFile.open("test.sch");

    string search_fieldname;
    string entry, newentry;
    search_fieldname = "Digi-Key Part Number";
    string update_fieldname;
    update_fieldname = "Seeed OPL SKU";

    CSVop Database;

    Database.readCSVfile("ATXMega128_USB.csv");
    cout << Database.getEntry(Database.findrow("Color", "yellow"), "Reference") << endl;

    row = 0;
    while(1){
        row = kicadsch.getCompendrow(row+1);
        entry = kicadsch.getEntry(row, search_fieldname);
        entry = rmquotmarks(entry);

        newentry = Database.getEntry(Database.findrow(search_fieldname,entry), update_fieldname);
        kicadsch.addEntry(update_fieldname, newentry, row);
        if(-1==row) break;
    }

    //SCHpatch.addEntry("Digi-Key Part Number", "12345", row, true, true);
    kicadsch.addEntry(VAL, "12345", row, true, true);


    cout << kicadsch.patchFile(oFile) << endl;

    oFile.close();




    return EXIT_SUCCESS;
}
