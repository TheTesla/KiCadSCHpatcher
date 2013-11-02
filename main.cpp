
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




int main(int argc, char *argv[])
{
    cout << "blub"  << endl;
    string KiCadSCHFilename;
    Table KiCadSCHTab;

    KiCadSCHFilename = "ATUC256L4U.sch";
    ifstream KiCadSCHFile;
    KiCadSCHFile.open(KiCadSCHFilename.c_str());
    KiCadSCHTab.loadTable(KiCadSCHFile);

    int i, row;

    row = KiCadSCHTab.findKiCadSCHVal("CONN_25", 120);
    cout << row << endl;
    cout << KiCadSCHTab.getVal(row)<< endl;

    ofstream oFile;
    oFile.open("test.sch");
    KiCadSCHFile.clear();
    KiCadSCHFile.seekg(0, ios::beg);


    Patch SCHpatch;
    SCHpatch.iKiCadSCHtab = KiCadSCHTab;
    SCHpatch.addEntry("Digi-Key Part Number", "12345", row);


    cout << patchFile(KiCadSCHFile, oFile, SCHpatch.patchvec) << endl;

    oFile.close();

    for(i=row;i<10;i++){

        cout << KiCadSCHTab.Tableread(i,0) << endl;

    }


    return EXIT_SUCCESS;
}
