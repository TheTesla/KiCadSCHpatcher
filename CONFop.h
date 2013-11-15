#ifndef CONFOP_H
#define CONFOP_H

#include "Table.h"
#include "extraops.h"


enum CONFreadstate_et
{
    EOFile = -2,
    ERROR = -1,
    NONE = 0,
    DATABASE = 1,
    SEARCH = 2,
    UPDATE = 3,
    READY = 127
};

class CONFop
{
    public:
        CONFop();
        ~CONFop();
        int readCONFfile(ifstream &file);
        int readCONFfile(string filename);
        CONFreadstate_et getBlock(int &row, CSVparams_t &csv, vector<datapair_t> &searchvec, vector<datapair_t> &updatevec);

    private:
        ifstream iCONFfile;
        Table tab;
};

#endif // CONFOP_H
