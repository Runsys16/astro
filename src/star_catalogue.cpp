#include "star_catalogue.h"

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
StarCatalog::~StarCatalog()
{
    //logf( (char*)"Destructeur StarCatalog() %s", (char*)name.c_str() );
    if (pInfo != NULL)
    {
        Panel*  pp = pInfo->getParent();
        if ( pp != NULL )     pp->sup(pInfo);

        delete pInfo;
        pInfo = NULL;
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
StarCatalog::StarCatalog( double ra, double de, double mag, string n )
{
    //logf( (char*)"Constructeur StarCatalog()" );
    fRA = ra;
    fDE = de;
    fMag = mag;
    name = string( n );
    
    //sprintf((char*)p_sInfo, "%s %0.2f", (char*)name.c_str(), (float)mag);
    sprintf((char*)p_sInfo, "m=%0.4f", (float)mag);
    
    pInfo       = new PanelText( (char*)p_sInfo,		PanelText::NORMAL_FONT );
    pInfo->setExtraString( "Star pInfo" );
    
    unsigned long color;
    if (bNuit)                  color = 0xFFFF0000;
    else                        color = 0xFFFFFFFF;

    pInfo->setColor(color);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

