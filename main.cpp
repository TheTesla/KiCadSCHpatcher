
/*
  Name: KiCadSCHpatcher
  Copyright: 2013 Stefan Helmert
  Author: Stefan Helmert
  Date: 15.11.2013
  Description: update Component-Entries from Database - BOM-import
*/



#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <stdio.h>
#include "KiCadSCH.h"
#include "CSVop.h"
#include "extraops.h"
#include "CONFop.h"

using namespace std;



int main(int argc, char *argv[])
{
    string KiCadSCHFilename;
    KiCadSCH kicadsch;
    CONFop conf;

    KiCadSCHFilename = "ATUC256L4U.sch";
    kicadsch.readSCHfile(KiCadSCHFilename);


    int i, row;
    ofstream oFile;
    oFile.open("test.sch");

    string search_fieldname;
    string entry, newentry;
    string update_fieldname;

    datapair_t searchentry;
    datapair_t updateentry;
    vector<datapair_t> searchvec;
    vector<datapair_t> updatevec;



    CSVop Database;


    conf.readCONFfile("KiCadSCHpatcher.conf");


    int x;
    x = 3;
    int csvrow;
    CSVparams_t csvparam;
    CONFreadstate_et confstate;
    row = 0;
    csvrow = 0;
    while(EOFile!=confstate){
        searchvec.clear();
        updatevec.clear();
        confstate = conf.getBlock(csvrow, csvparam, searchvec, updatevec);
        Database.CSVparams = csvparam;
        Database.readCSVfile();
        row = 0;
        while(1){

            row = kicadsch.getCompendrow(row+1);
            kicadsch.getEntrys(row, searchvec); // searchvec mit Eintraegen aus SCH-file anreichern
            rmquotmarks(searchvec);

            Database.getEntrys(Database.findrow(searchvec), updatevec); // updatevec mit Eintraegen aus der Datenbank anreichern
            kicadsch.addEntrys(updatevec, row);
            if(-1==row) break;
        }

    }

    kicadsch.updatePatchEntryNbr(); // Eintraege mit laufender Nummer versehen und an Ausgabezeile anfuegen

    cout << kicadsch.patchFile(oFile) << endl;

    oFile.close();




    return EXIT_SUCCESS;
}
