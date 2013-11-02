#ifndef PATCH_H_INCLUDED
#define PATCH_H_INCLUDED

#include "Table.h"
#include <vector>
#include <string>

using namespace std;

typedef struct modiFile_t
{
    bool del;   // true: delete the line
    bool add;   // true: add new line
    int lineNbr;// the corresponding line-number of the input-file
    string line;// content of the new line (if line is added)
} modiFile_t;

class Patch
{
    public:
        Patch();
        ~Patch();
        vector<modiFile_t> patchvec;
        Table iKiCadSCHtab;
        int addEntry(string entryname, string entrycontent, int row = 0, bool overwrite = true, bool resetparams = false);
    private:
};

#endif // PATCH_H_INCLUDED
