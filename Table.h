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

using namespace std;

class Table
{
    public:
        Table();
        ~Table();
        void loadTable(ifstream &File);
        string Tableread(int row, int col);
        int findrow(string find, int col=0, int firstrow=0);
        int findcol(string find, int row=0, int firstcol=0);
        void eraseemptyrows(void);
        int findrow_bw(string find, int col, int lastrow);
        int findKiCadSCHrow(string fieldname, string fieldentry);
        int findKiCadSCHRef(string fieldentry);
        int findKiCadSCHVal(string fieldentry);
        int findKiCadSCHFP(string fieldentry);
        int findKiCadSCHDS(string fieldentry);

        int getCompbeginrow(int row);
        int getCompendrow(int row);
        string getEntry(int row, string fieldname);
        string getRef(int row);
        string getVal(int row);
        string getFP(int row);
        string getDS(int row);
    private:
        string* tabstr;
        int cols;
        int rows;
        void findtablesize(ifstream &File);

};

#endif
