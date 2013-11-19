/* Klasse verwaltet string-Tabellen
 *
 * Copyright (C) 2013 Stefan Helmert <stefan.helmert@gmx.net>
 *
 *
 * loadTable() liest Datensatz aus Textdatei ein, Beliebiger Spaltentrenner
 * definierte Zeichen am Anfang und Ende eines Eintrags werden entfernt
 * Ist Spaltentrenner und zu ignorierendes Zeichen gleich, so wird bei einer
 * Folge dieser Zeichen ein Zeichen als Trenner und die restlichen Zeichen
 * als zu ignorierende Zeichen gewertet.
 * Innerhalb von ""-Zeichen darf Trennzeichen vorkommen.

 table-class
 - read from csv-file (pin-file, conf-file, sch-file)
 - search-methods
 - method to erase empty rows
 - method to remove quotationsmarks arround the entry
 - write-method

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
        int loadTable(ifstream &File, string delim = ",", string ignorebefore = " ", string ignoreafter = " ", bool findtabsize = true);
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
