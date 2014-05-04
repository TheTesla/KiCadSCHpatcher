/* Klasse verwaltet csv-Dateien
 *
 * Copyright (C) 2013 Stefan Helmert <stefan.helmert@gmx.net>
 *
 *
 * Benutzt Table.h, Dient zum Einlesen und Abfragen von CSV-Dateien.

 CSVop-class
 - methods to find entry by fieldname
 - methods to read entries
 */



#ifndef CSVOP_H_INCLUDED
#define CSVOP_H_INCLUDED

#include <string>
#include <fstream>
#include <vector>
#include "Table.h"
#include "extraops.h"

using namespace std;




class CSVop
{
    public:
        CSVop();
        ~CSVop();

        CSVparams_t CSVparams;

        int readCSVfile(ifstream &file);
        int readCSVfile(string filename);
        int readCSVfile(void);
        int findrow(string fieldname, string fieldentry, int startrow = 0, bool namecontains = false, bool entrycontains = false, bool strcontainsname = false, bool strcontainsentry = false, bool valuesearch = false, unsigned subpart = 1, double precision = 0);
        int findrow(vector<datapair_t> entrypairs, int startrow, int Nbr2find);
        int findrow(vector<datapair_t> entrypairs, int startrow = 1);

        string getEntry(int row, string fieldname, bool namecontains = false, bool strcontainsname = false);
        void getEntrys(int row, vector<datapair_t> &data2update);
        string getfullName(string fieldname, bool namecontains = false, bool strcontainsname = false);
        int getNorows(void);

    private:
        ifstream iCSVfile;
        Table tab;
};


#endif // CSVOP_H_INCLUDED
