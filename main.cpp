
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





int main(int argc, char *argv[])
{
    string KiCadSCHFilename;
    Table KiCadSCHTab;

    KiCadSCHFilename = "ATUC256L4U.sch";
    ifstream KiCadSCHFile;
    KiCadSCHFile.open(KiCadSCHFilename.c_str());
    KiCadSCHTab.loadTable(KiCadSCHFile);

    int i, row;

    row = KiCadSCHTab.findKiCadSCHVal("1k5");
    cout << KiCadSCHTab.getDS(row)<< endl;

    for(i=row;i<10;i++){

        cout << KiCadSCHTab.Tableread(i,0) << endl;

    }


    return EXIT_SUCCESS;
}
