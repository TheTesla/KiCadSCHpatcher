#ifndef CSVOP_H_INCLUDED
#define CSVOP_H_INCLUDED

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include "Table.h"

using namespace std;


typedef struct datapair_t
{
    string fieldname;
    string fieldentry;
    bool namecontains; // fieldname ist in Tabelleneintrag enthalten
    bool entrycontains;  // fieldentry ist in Tabelleneintrag enthalten
    bool strcontainsname; // Tabelleneintrag ist in fieldname enthalten
    bool strcontainsentry; // Tabelleneintrag ist in fieldentry enthalten
} datapair_t;

class CSVop
{
    public:
        CSVop();
        ~CSVop();
        int readCSVfile(ifstream &file);
        int readCSVfile(string filename);
        int findrow(string fieldname, string fieldentry, int startrow = 0, bool namecontains = false, bool entrycontains = false, bool strcontainsname = false, bool strcontainsentry = false, unsigned subpart = 1);
        int findrow(vector<datapair_t> entrypairs, int startrow, int Nbr2find);
        int findrow(vector<datapair_t> entrypairs, int startrow, bool namecontains = false, bool entrycontains = false, bool strcontainsname = false, bool strcontainsentry = false);

        string getEntry(int row, string fieldname, bool namecontains = false, bool strcontainsname = false);

    private:
        ifstream iCSVfile;
        Table tab;
};


#endif // CSVOP_H_INCLUDED
