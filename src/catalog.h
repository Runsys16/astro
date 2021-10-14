#ifndef CATALOG_H
#define CATALOG_H 1

#include "main.h"
#include <WindowsManager.h>
#include <vector>
#include "star_catalogue.h"



//class StarCatalog;


class Catalog
{
private:
    string                          sFilename;
    string                          sName;
    vector<StarCatalog*>            stars;

public:
    Catalog();
    Catalog(string);
    Catalog(string, string);
    ~Catalog();
    
    void                            add( StarCatalog* );
    void                            sauve();
    void                            charge();
    
inline int                          size()                      { return stars.size(); }
inline StarCatalog*                 get(int i)                  { return stars[i]; }
inline void                         clear()                     { stars.clear(); }
};


#endif
