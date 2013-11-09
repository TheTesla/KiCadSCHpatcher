#ifndef CSVOP_H_INCLUDED
#define CSVOP_H_INCLUDED

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include "Table.h"

using namespace std;

class CSVop
{
    public:
        CSVop();
        ~CSVop();
        int readCSVfile(ifstream &file);
        int readCSVfile(string filename);
        int findrow(string fieldname, string fieldentry, int startrow = 0, unsigned subpart = 1);
        //int findrow(KiCadStdfn_et fieldname, string fieldentry, int startrow = 0, unsigned subpart = 1);
        string getEntry(int row, string fieldname);

    private:
        ifstream iCSVfile;
        Table tab;
};


#endif // CSVOP_H_INCLUDED
