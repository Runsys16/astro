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
    bool							bSauve;

public:
    Catalog();
    Catalog(bool);
    Catalog(string);
    Catalog(string, string);
    ~Catalog();
    
    void                            efface();
    void                            add( StarCatalog* );
    void                            sauve();
    void                            charge();
    void                            list();
    
inline int                          size()                      { return stars.size(); }
inline StarCatalog*                 get(int i)                  { return stars[i]; }
inline void                         clear()                     { stars.clear(); }
};


#endif
