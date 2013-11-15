/* Klasse verwaltet string-Tabellen
 *
 * Copyright (C) 2012 Stefan Helmert <stefan.helmert@gmx.net
 *
 *
 * loadTable() liest Datensatz aus Textdatei ein, Komma als Spaltentrenner
 * Leerzeichen am Anfang und Ende eines Eintrags werden entfernt

 table-class
 - read from csv-file (pin-file)
 - search-methods
 - method to erase empty rows


 */

#ifndef TABLE_H
#define TABLE_H


#include <cstdlib>
#include <fstream>
#include <string>
#include <algorithm>
#include "extraops.h"

using namespace std;

class Table
{
    public:
        Table();
        ~Table();
        bool OK = true;
        void loadTable(ifstream &File, string delim = ",", string ignorebefore = " ", string ignoreafter = " ", bool findtabsize = true);
        string Tableread(int row, int col);
        int findrow(string findstr, int col = 0, int lastrow = 0, bool entrycontains = false, bool strcontainsentr = false);
        int findrow_bw(string findstr, int col, int lastrow, bool entrycontains = false, bool strcontainsentr = false);
        int findcol(string findstr, int row = 0, int firstcol = 0, bool entrycontains = false, bool strcontainsentr = false);
        void eraseemptyrows(void);
        int Tablewrite(int row, int col, string entry);
        void rmquotmarks(void);


    private:
        string* tabstr;
        int cols;
        int rows;

};

#endif
