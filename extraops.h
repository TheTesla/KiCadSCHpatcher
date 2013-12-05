/* Zusatzfunktionen und -Strukturen, die in fast allen Komponenten gebraucht werden
 *
 * Copyright (C) 2013 Stefan Helmert <stefan.helmert@gmx.net>
 *
 *
 *

 */

#ifndef EXTRAOPS_H_INCLUDED
#define EXTRAOPS_H_INCLUDED

#include <vector>
#include <string>
#include <cmath>

using namespace std;

typedef struct datapair_t
{
    string fieldname;
    string fieldentry;
    bool namecontains = false; // fieldname ist in Tabelleneintrag enthalten
    bool entrycontains = false;  // fieldentry ist in Tabelleneintrag enthalten
    bool strcontainsname = false; // Tabelleneintrag ist in fieldname enthalten
    bool strcontainsentry = false; // Tabelleneintrag ist in fieldentry enthalten
    bool valuesearch = false; // Interpretation von Prefixen (k, M usw.)  bei Werten
    bool takeDatabasefieldname = false; // Vollstaendigen fieldname aus Datenbank in SCH-file uebernehmen, ansonsten Suchwort aus config-file
    bool overwrite = false;
    bool resetparams = false;
    bool allowemptyentries = false;
} datapair_t;

typedef struct CSVparams_t
{
    string filename = "Database.csv";
    string delim = "\t";
    string ignorebefore = " ";
    string ignoreafter = " ";
} CSVparams_t;

string rmquotmarks(string str);
void rmquotmarks(vector<datapair_t> &data);
float norm_value(string str);
void norm_value_search(vector<datapair_t> &searchvec);


#endif // EXTRAOPS_H_INCLUDED
