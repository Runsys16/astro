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
    string                          sName;
    vector<StarCatalog*>            stars;

public:
    Catalog();
    ~Catalog();
    
    void                            add( StarCatalog* );
    
inline int                          size()                      { return stars.size(); }
inline StarCatalog*                 get(int i)                  { return stars[i]; }
inline void                         clear()                     { stars.clear(); }
};


#endif
