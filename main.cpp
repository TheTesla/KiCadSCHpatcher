
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



int doit(string iSCHfilename, string oSCHfilename, string CONFfilename)
{

    KiCadSCH kicadsch;
    CONFop conf;
    int row;
    ofstream oFile;
    string search_fieldname;
    string entry, newentry;
    string update_fieldname;
    datapair_t searchentry;
    datapair_t updateentry;
    vector<datapair_t> searchvec;
    vector<datapair_t> updatevec;
    CSVop Database;
    int csvrow;
    CSVparams_t csvparam;
    CONFreadstate_et confstate;
    int notoverwritten, NoParts, NoEntrperPart, round;

    if(-1==kicadsch.readSCHfile(iSCHfilename)) return -1;
    oFile.open(oSCHfilename.c_str());
    if(!oFile.is_open()) return -2;
    if(-1==conf.readCONFfile(CONFfilename)) return -3;



    csvrow = 0;
    round = 0;

    while(EOFile!=confstate){
        searchvec.clear();
        updatevec.clear();
        confstate = conf.getBlock(csvrow, csvparam, searchvec, updatevec);
        Database.CSVparams = csvparam;
        if(-1==Database.readCSVfile()) return -4;
        row = 0;
        NoEntrperPart = updatevec.size();
        NoParts = 0;
        notoverwritten = 0;
        while(1){
            row = kicadsch.getCompendrow(row+1);
            kicadsch.getEntrys(row, searchvec); // searchvec mit Eintraegen aus SCH-file anreichern
            rmquotmarks(searchvec);
            Database.getEntrys(Database.findrow(searchvec), updatevec); // updatevec mit Eintraegen aus der Datenbank anreichern
            notoverwritten += kicadsch.addEntrys(updatevec, row);
            if(-1==row) break;
            NoParts++;
        }
        round++;
        cout << "round: " << round << endl;
        cout << "    database used: " << csvparam.filename << endl;
        cout << "    number of Parts: " << NoParts << endl;
        cout << "    entries per Part: " << NoEntrperPart << endl;
        cout << "    total entries (including empty): " << NoParts*NoEntrperPart << endl;
        cout << "    number of Entries not overwritten (excluding empty): " << notoverwritten << endl << endl;
    }

    kicadsch.updatePatchEntryNbr(); // Eintraege mit laufender Nummer versehen und an Ausgabezeile anfuegen
    kicadsch.patchFile(oFile);
    oFile.close();

    return 0;
}


int main(int argc, char *argv[])
{
    string str, inp, out, conf;
    int i;
    bool ok;
    int err;
    ok = true;
    if(1<argc){
        for(i=1;i<argc;i++){
        str = argv[i];
            if(0==str.find("-i")) inp = str.substr(2, str.size()-2);
            if(0==str.find("-o")) out = str.substr(2, str.size()-2);
            if(0==str.find("-c")) conf = str.substr(2, str.size()-2);
        }
        if(""==inp){
            cout << "ERROR: input file not specified" << endl;
            ok = false;
        }
        if(""==out){
            cout << "ERROR: output file not specified" << endl;
            ok = false;
        }
        if(""==conf){
            cout << "ERROR: config file not specified" << endl;
            ok = false;
        }
        if(ok){
            if(inp==out){
                out = "~" + out;
                err = doit(inp, out, conf);
                if(0==err){
                    ifstream ifile(out.c_str(), std::ios::binary);
                    ofstream ofile(inp.c_str(), std::ios::binary);
                    ofile << ifile.rdbuf();
                    remove(out.c_str());
                }
            }else{
                err = doit(inp, out, conf);
            }
            if(-1==err) cout << "could not open input file" << endl;
            if(-2==err) cout << "could not open output file" << endl;
            if(-3==err) cout << "could not open config file" << endl;
            if(-4==err) cout << "could not open database file" << endl;
        }
    }else{
        cout << endl << "This is KiCadSCHpatcher. It is designed to update the parameters" << endl << "of the components of KiCad schematic file from a database (csv)." << endl;
        cout << endl << "Copyright (C) 2013 Stefan Helmert" << endl << endl;
        cout << "usage: KiCadSCHpatcher -iInputFILE.sch -oOutputFILE.sch -cConfigFILE.conf" << endl;
    }


    return EXIT_SUCCESS;
}
