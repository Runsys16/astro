#ifndef STAR_CATALOG_H
#define STAR_CATALOG_H  1

#include "main.h"
#include "Mathlib.h"

#include <WindowsManager.h>
#include <thread>


using namespace std;

class StarCatalog 
{
public :
        double          fRA;
        double          fDE;
        double          fMag;
        string          name;
        PanelText *     pInfo;
        char            p_sInfo[225];

public :
        StarCatalog( double, double, double, string );
        ~StarCatalog();
        
inline  double          getRA()             {return fRA;}
inline  double          getDE()             {return fDE;}
inline  PanelText*      getInfo()           {return pInfo;}
};

#endif
