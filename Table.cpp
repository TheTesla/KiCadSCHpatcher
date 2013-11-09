/* Klasse verwaltet string-Tabellen
 *
 * Copyright (C) 2012 Stefan Helmert <stefan.helmert@gmx.net>
 *
 *
 * loadTable() liest Datensatz aus Textdatei ein, Komma als Spaltentrenner
 * Leerzeichen am Anfang und Ende eines Eintrags werden entfernt
 */

#include "Table.h"
#include <iostream>
#include <string>
#include "KiCadSCH.h"

Table::Table()
{
}

Table::~Table()
{
}

void Table::loadTable(ifstream &File, string delim, string ignorebefore, string ignoreafter)
{
    int col, row, i, pos, oldpos, entrystartpos, qmpos, entrbegpos, entrendpos;
    string entry, line;
    bool in_quotation_marks = false;

    findtablesize(File); // wegen dynamischer Speicherverwaltung
    tabstr = new string[rows*cols];
    col = 0;
    row = 0;
    if(File.is_open()){
        while(File.good()){
            getline(File, line);
            col = 0;
            pos = 0;
            oldpos = 0;
            qmpos = 0;
            entrystartpos = 0;
            in_quotation_marks = false;
            while(line.size()!=pos){
                pos = line.find_first_of(delim, oldpos);
                // Leerzeichen zwischen Anfuehrungszeichen zaehlen nicht
                qmpos = oldpos;
                if(string::npos==pos) pos = line.size();
                while(1){ // weil zwei Anfuehrungszeichen in einem Eintrag sein koennen
                    qmpos = line.find_first_of("\"", qmpos);
                    if((string::npos==qmpos)||(qmpos>pos)) break;
                    qmpos++;
                    in_quotation_marks = !in_quotation_marks;
                }
                if(!in_quotation_marks){
                    // zwischen zwei Delimiter befindet sich der Eintrag
                    if((oldpos<pos)||((ignorebefore!=delim)&&(ignoreafter!=delim))){
                            /* Besonderheit: Delimitterzeichen ist zu ignorierendes Zeichen -
                                             keine Unterscheidung zwischen leeren Eintraegen
                                             und mehreren zu ignorierenden Fuellzeichen moeglich
                            */

                        entry = line.substr(entrystartpos, pos-entrystartpos);
                        entrbegpos = entry.find_first_not_of(ignorebefore); // zu ignorierende Zeichen am Anfang, ...
                        entrendpos = entry.find_last_not_of(ignoreafter); // zu ignorierende Zeichen am Ende, ...
                        if((string::npos!=entrbegpos)&&(string::npos!=entrendpos)){
                            entry = entry.substr(entrbegpos, entrendpos-entrbegpos+1); // ... entfernen
                        }else{
                            entry = "";
                        }
                        tabstr[col+row*cols] = entry;
                        col++;
                    }
                    entrystartpos = pos+1; // weil oldpos auch innerhalb der Anfuehrungszeichen unterteilt
                }
                oldpos = pos+1;
            }
            row++;
        }
    }
}

string Table::Tableread(int row, int col)
{
    if(row>rows || col>cols || row<0 || col<0) return "";
    return tabstr[col + row*cols];
}


// Trennzeichen ist Leerzeichen, zwischen Anfuehrungszeuchen nicht mitgezaehlt
void Table::findtablesize(ifstream &File)
{
    string line;
    size_t pos, oldpos, qmpos;
    bool in_quotation_marks = false;
    rows = 0;
    cols = 0;
    int tmp;
    if(File.is_open()){
        while(File.good()){
            getline(File, line);
            rows++;
            tmp = 0;
            pos = 0;
            oldpos = 0;
            qmpos = 0;
            in_quotation_marks = false;
            while(string::npos!=pos){
                pos = line.find_first_of(" ", oldpos);
                // Leerzeichen zwischen Anfuehrungszeichen zaehlen nicht
                qmpos = oldpos;
                while(1){ // weil zwei Anfuehrungszeichen in einem Eintrag sein koennen
                    qmpos = line.find_first_of("\"", qmpos);
                    if((string::npos==qmpos)||(qmpos>pos)) break;
                    qmpos++;
                    in_quotation_marks = !in_quotation_marks;
                }
                if(!in_quotation_marks){
                    if(oldpos<pos) tmp++;
                }
                oldpos = pos+1;
            }
            if(tmp>cols) cols = tmp;
        }
    File.clear();
    File.seekg(0,ios::beg);
    }
}

int Table::findrow(string find, int col, int firstrow)
{
    int row;
    string str;
    for(row=firstrow;row<rows;row++){
        str = this->Tableread(row,col);
        if(0 == str.compare(find)) return row;
    }
    return -1;
}

int Table::findrow_bw(string find, int col, int lastrow)
{
    int row;
    string str;
    for(row=lastrow;row>=0;row--){
        str = this->Tableread(row,col);
        if(0 == str.compare(find)) return row;
    }
    return -1;
}

int Table::findcol(string find, int row, int firstcol)
{
    int col;
    string str;
    for(col=firstcol;col<cols;col++){
        str = this->Tableread(row,col);
        if(0 == str.compare(find)) return col;
    }
    return -1;
}

void Table::eraseemptyrows(void)
{
    int rrow, wrow, col;
    bool colempty;
    wrow = 0;
    string str;
    for(rrow=0;rrow<rows;rrow++){
        colempty = true;
        for(col=0;col<cols;col++){
            str = this->Tableread(rrow, col);
            tabstr[col + wrow*cols] = str;
            if(0 != str.compare("")) colempty = false;
        }
        if(false == colempty) wrow++;
    }
}
