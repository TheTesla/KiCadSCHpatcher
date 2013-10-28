
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

using namespace std;



typedef struct modiFile_t
{
    bool del;   // true: delete the line
    bool add;   // true: add new line
    int lineNbr;// the corresponding line-number of the input-file
    string line;// content of the new line (if line is added)
} modiFile_t;

int patchFile(ifstream &iFile, ofstream &oFile, modiFile_t* patch)
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
        for(i=0;i<5;i++){
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
    string KiCadSCHFilename;
    Table KiCadSCHTab;

    KiCadSCHFilename = "ATUC256L4U.sch";
    ifstream KiCadSCHFile;
    KiCadSCHFile.open(KiCadSCHFilename.c_str());
    KiCadSCHTab.loadTable(KiCadSCHFile);

    int i, row;

    row = KiCadSCHTab.findKiCadSCHVal("CONN_25", 124);
    cout << row << endl;
    cout << KiCadSCHTab.getVal(row)<< endl;

    ofstream oFile;
    oFile.open("test.sch");
    KiCadSCHFile.clear();
    KiCadSCHFile.seekg(0, ios::beg);

    modiFile_t patch[5];
    patch[0].del = true;
    patch[0].add = false;
    patch[0].lineNbr = 0;
    patch[1].del = true;
    patch[1].add = false;
    patch[1].lineNbr = 1;
    patch[2].del = true;
    patch[2].add = true;
    patch[2].lineNbr = 2;
    patch[2].line = "2";
    patch[3].del = false;
    patch[3].add = true;
    patch[3].lineNbr = 2;
    patch[3].line = "3";
    patch[4].del = true;
    patch[4].add = false;
    patch[4].lineNbr = 4;
    cout << patchFile(KiCadSCHFile, oFile, patch) << endl;

    oFile.close();

    for(i=row;i<10;i++){

        cout << KiCadSCHTab.Tableread(i,0) << endl;

    }


    return EXIT_SUCCESS;
}
