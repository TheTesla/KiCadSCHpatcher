
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
    int row, rowfound;
    vector<int> rowsfound, bestrowsfound;
    ofstream oFile;
    string search_fieldname;
    string entry, newentry;
    string update_fieldname;
    datapair_t searchentry;
    datapair_t updateentry;
    vector<datapair_t> searchvec;
    vector<datapair_t> updatevec;
    vector<datapair_t> ordervec;
    vector<oplog_t> oplog;
    oplog_t oplogentr;
    CSVop* Database_ptr;
    int csvrow;
    CSVparams_t csvparam;
    CONFreadstate_et confstate;
    int notoverwritten, NoParts, NoEntrperPart, round, NoMatches, Nonewentr;
    int err;

    err = kicadsch.readSCHfile(iSCHfilename);
    if(-1==err) return -1;
    if(-11==err) return -11;
    oFile.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    try{
        oFile.open(oSCHfilename.c_str());
    }
    catch(std::ofstream::failure e){
        if(oFile.bad()) return -12;
        return -2;
    }

    err = conf.readCONFfile(CONFfilename);
    if(-1==err) return -3;
    if(-11==err) return -13;



    csvrow = 0;
    round = 0;
    oplog.clear();
    while(EOFile!=confstate){
        searchvec.clear();
        updatevec.clear();
        ordervec.clear();
        confstate = conf.getBlock(csvrow, csvparam, searchvec, ordervec, updatevec);
        if(!checkCONF(searchvec)){
            cout << "Error: value with tolerance in last position of configuration file - no tolerance specification following." << endl << endl;
            continue;
        }
        Database_ptr = new(CSVop);
        Database_ptr->CSVparams = csvparam;
        err = Database_ptr->readCSVfile();
        if(-1==err) return -4;
        if(-11==err) return -14;

        row = 0;
        NoEntrperPart = updatevec.size();
        NoParts = 0;
        notoverwritten = 0;
        NoMatches = 0;
        Nonewentr = 0;
        while(1){
            row = kicadsch.getCompendrow(row+1);
            kicadsch.getEntrys(row, searchvec); // searchvec mit Eintraegen aus SCH-file anreichern
            rmquotmarks(searchvec);
            //rowfound = Database.findrow(searchvec);
            rowsfound = Database_ptr->findrows(searchvec);

            if(0<rowsfound.size()) {
                kicadsch.getEntrys(row, ordervec);
                bestrowsfound = Database_ptr->findbestrows(rowsfound,ordervec);
                rowfound = bestrowsfound[0];

                NoMatches++; // zaehle erfolgreiche Treffer
                Database_ptr->getEntrys(rowfound, updatevec); // updatevec mit Eintraegen aus der Datenbank anreichern
                oplogentr.patchstartindex = kicadsch.getPatchsize();
                notoverwritten += kicadsch.addEntrys(updatevec, row);
                oplogentr.SCHrow = row;
                oplogentr.DBrow = rowfound;
                oplogentr.DBrows = rowsfound;
                oplogentr.DBbestrows = bestrowsfound;
                oplogentr.searchv = searchvec;
                oplogentr.orderv = ordervec;
                oplogentr.updatev = updatevec;
                oplogentr.NoPatchentr = kicadsch.getPatchsize() - oplogentr.patchstartindex;
                Nonewentr += oplogentr.NoPatchentr;
                oplog.push_back(oplogentr);
            }
            if(-1==row) break;
            NoParts++;
        }
        delete(Database_ptr);
        round++;
        cout << "round: " << round << endl;
        cout << "    database used: " << csvparam.filename << endl;
        cout << "    number of matches: " << NoMatches << endl;
        cout << "    number of parts: " << NoParts << endl;
        cout << "    entries per part: " << NoEntrperPart << endl;
        cout << "    total entries (including empty): " << NoParts*NoEntrperPart << endl;
        cout << "    number of entries not overwritten (excluding empty): " << notoverwritten << endl;
        cout << "    number of new entries: " << Nonewentr << endl << endl;
    }

    kicadsch.updatePatchEntryNbr(); // Eintraege mit laufender Nummer versehen und an Ausgabezeile anfuegen

    err = kicadsch.patchFile(oFile);
    kicadsch.printoplog(oplog);

    //kicadsch.printPatch();
    if(oFile.is_open()) oFile.close();
    if(0==err) return 10*err;
        cout << " total number of new entries: " << kicadsch.getpatchsize() << endl;


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
            cerr << "ERROR: input file not specified" << endl;
            ok = false;
        }
        if(""==out){
            cerr << "ERROR: output file not specified" << endl;
            ok = false;
        }
        if(""==conf){
            cerr << "ERROR: config file not specified" << endl;
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
            if(-1==err) cerr << "could not open input file" << endl;;
            if(-11==err) cerr << "read error on input file" << endl;;
            if(-2==err) cerr << "could not open/create output file" << endl;
            if(-12==err) cerr << "could not open/create output file - file bad" << endl;
            if(-3==err) cerr << "could not open config file" << endl;;
            if(-13==err) cerr << "read error on config file" << endl;;
            if(-4==err) cerr << "could not open database file" << endl;
            if(-14==err) cerr << "read error on database file" << endl;
            if(-10==err) cerr << "could not write to output file" << endl;
            if(-20==err) cerr << "read error on input file while writing output file" << endl;
        }
    }else{
        cout << endl << "This is KiCadSCHpatcher. It is designed to update the parameters" << endl << "of the components of KiCad schematic file from a database (csv)." << endl;
        cout << endl << "Copyright (C) 2013 Stefan Helmert" << endl << endl;
        cout << "usage: KiCadSCHpatcher -iInputFILE.sch -oOutputFILE.sch -cConfigFILE.conf" << endl;
    }


    return EXIT_SUCCESS;
}

