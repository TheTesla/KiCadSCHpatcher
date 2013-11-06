#ifndef PATCH_H_INCLUDED
#define PATCH_H_INCLUDED

#include "Table.h"
#include <vector>
#include <string>

using namespace std;



class Patch
{
    public:
        Patch();
        ~Patch();
        Table iKiCadSCHtab;
        int addEntryGen(string entrycontent, int row, int entryrow, string lastcol = "", bool overwrite = true, bool resetparams = false);
        int addEntry(string entryname, string entrycontent, int row = 0, bool overwrite = true, bool resetparams = false);
        int addVal(string entrycontent, int row, bool overwrite = true, bool resetparams = false);
    private:
};

#endif // PATCH_H_INCLUDED
