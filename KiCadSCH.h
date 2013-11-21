/* Klasse verwaltet KiCad-sch-Dateien
 *
 * Copyright (C) 2013 Stefan Helmert <stefan.helmert@gmx.net>
 *
 *
 * Benutzt Table.h, Dient zum Einlesen und veraendern von KiCad SCH-Dateien um Bateil-Parameter zu bearbeiten.

 KiCadSCH-class
 - methods to search in sch-file
 - methods to read sch-file like a database
 - methids to add entries to sch-file

 */

#ifndef KICADSCH_H_INCLUDED
#define KICADSCH_H_INCLUDED

#include "Table.h"
#include <string>
#include <vector>
#include <fstream>
#include "extraops.h"

using namespace std;

enum KiCadStdfn_et
{
    notStd = -1,
    REF = 0,
    VAL = 1,
    FP = 2,
    DS = 3
};

typedef struct modiFile_t
{
    bool del;   // true: delete the line
    bool add;   // true: add new line
    int lineNbr;// the corresponding line-number of the input-file
    int entryNbr; // the number after "F "
    string fieldname;
    string line;// content of the new line (if line is added)
} modiFile_t;

class KiCadSCH
{
    public:
        KiCadSCH();
        ~KiCadSCH();
        int readSCHfile(ifstream &file);
        int readSCHfile(string filename);
        int findrow(string fieldname, string fieldentry, int startrow = 0, unsigned subpart = 1);
        int findrow(KiCadStdfn_et fieldname, string fieldentry, int startrow = 0, unsigned subpart = 1);
        int getCompbeginrow(int row);
        int getCompendrow(int row);
        int getLastentryrow(int row);
        int getLastentryNbr(modiFile_t patch);
        int getPatchindex(modiFile_t patch);
        void updatePatchEntryNbr(void);


        int getEntryrow(int row, string fieldname, bool namecontains = false, bool strcontainsname = false);
        int getEntryrow(int row, KiCadStdfn_et fieldname);
        string getEntry(int row, string fieldname, bool namecontains = false, bool strcontainsname = false);
        string getEntry(int row, KiCadStdfn_et fieldname);
        void getEntrys(int row, vector<datapair_t> &datavec);
        int getUnitrow(int row);
        string getUnitNbr(int row);
        int getKoordrow(int row);

        int patchFile(ofstream &oFile);
        int addEntryGen(string entrycontent, int row, int entryrow, string lastcol = "", bool overwrite = true, bool resetparams = false);
        int addEntry(string entryname, string entrycontent, int row = 0, bool overwrite = true, bool resetparams = false);
        int addEntry(KiCadStdfn_et entryname, string entrycontent, int row = 0, bool overwrite = true, bool resetparams = false);
        int addEntrys(vector<datapair_t> newdata, int row = 0);
        unsigned getpatchsize(void);

    private:
        ifstream iSCHfile;
        Table tab;
        vector<modiFile_t> patchvec;
        KiCadStdfn_et convfieldnameStdfield(string fieldname, bool namecontains = false, bool strcontainsname = false);

};

#endif // KICADSCH_H_INCLUDED
