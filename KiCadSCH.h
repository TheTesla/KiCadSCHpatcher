

#ifndef KICADSCH_H_INCLUDED
#define KICADSCH_H_INCLUDED

#include "Table.h"
#include <string>
#include <vector>
#include <fstream>


using namespace std;

enum KiCadStdfn_et
{
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
    string line;// content of the new line (if line is added)
} modiFile_t;

class KiCadSCH
{
    public:
        KiCadSCH();
        ~KiCadSCH();
        Table tab;
        int readSCHfile(ifstream &file);
        int readSCHfile(string filename);
        vector<modiFile_t> patchvec;
        int findrow(string fieldname, string fieldentry, int startrow = 0, unsigned subpart = 1);
        int findrow(KiCadStdfn_et fieldname, string fieldentry, int startrow = 0, unsigned subpart = 1);
        int getCompbeginrow(int row);
        int getCompendrow(int row);
        int getLastentryrow(int row);
        int getEntryrow(int row, string fieldname);
        int getEntryrow(int row, KiCadStdfn_et fieldname);
        string getEntry(int row, string fieldname);
        string getEntry(int row, KiCadStdfn_et fieldname);
        int getUnitrow(int row);
        string getUnitNbr(int row);
        int getKoordrow(int row);

        int patchFile(ofstream &oFile);
        int addEntryGen(string entrycontent, int row, int entryrow, string lastcol = "", bool overwrite = true, bool resetparams = false);
        int addEntry(string entryname, string entrycontent, int row = 0, bool overwrite = true, bool resetparams = false);
        int addEntry(KiCadStdfn_et entryname, string entrycontent, int row = 0, bool overwrite = true, bool resetparams = false);

    private:
        ifstream iSCHfile;
};

#endif // KICADSCH_H_INCLUDED