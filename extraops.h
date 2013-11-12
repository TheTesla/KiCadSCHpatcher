#ifndef EXTRAOPS_H_INCLUDED
#define EXTRAOPS_H_INCLUDED

#include <vector>
#include <string>

using namespace std;

typedef struct datapair_t
{
    string fieldname;
    string fieldentry;
    bool namecontains = false; // fieldname ist in Tabelleneintrag enthalten
    bool entrycontains = false;  // fieldentry ist in Tabelleneintrag enthalten
    bool strcontainsname = false; // Tabelleneintrag ist in fieldname enthalten
    bool strcontainsentry = false; // Tabelleneintrag ist in fieldentry enthalten
    bool takeDatabasefieldname = false; // Vollstaendigen fieldname aus Datenbank in SCH-file uebernehmen, ansonsten Suchwort aus config-file
} datapair_t;

string rmquotmarks(string str);
void rmquotmarks(vector<datapair_t> &data);


#endif // EXTRAOPS_H_INCLUDED