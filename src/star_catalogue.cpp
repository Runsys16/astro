#include "star_catalogue.h"
//--------------------------------------------------------------------------------------------------------------------
//#define COORD
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
//#define COORD
    //logf( (char*)"Constructeur StarCatalog()" );
    fRA = ra;
    fDE = de;
    fMag = mag;
    name = string( n );
    
#ifdef COORD
    struct hms HMS;
    struct dms DMS;
    deg2hms( ra, HMS );
    deg2dms( de, DMS );
    sprintf((char*)p_sInfo, "m=%0.2f %02dh %02d' %2.2f\" %02d° %02d' %02.2f\"", (float)mag, (int)HMS.h, (int)HMS.m, HMS.s, (int)DMS.d, (int)DMS.m,  DMS.s   );
    //sprintf((char*)p_sInfo, "m=%0.4f %2f %f", (float)mag, ra, de   );
#else    
    //sprintf((char*)p_sInfo, "m=%0.4f", (float)mag );
    sprintf((char*)p_sInfo, "%0.2f", (float)mag );
#endif

    pInfo       = new PanelText( (char*)p_sInfo,		PanelText::NORMAL_FONT );
    pInfo->setExtraString( "Star pInfo" );
    
    unsigned long color;
    if (bNuit)                  color = 0xFFFF0000;
    else                        color = 0xFFFFFFFF;

    pInfo->setColor(color);
    //WindowsManager::getInstance().add(pInfo);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void StarCatalog::affiche_position()
{
    struct hms HMS;
    struct dms DMS;
    deg2hms( fRA, HMS );
    deg2dms( fDE, DMS );
    sprintf((char*)p_sInfo, "%0.2f AD=%02dh %02d' %2.2f\" DE=%02d° %02d' %02.2f\"", (float)fMag, (int)HMS.h, (int)HMS.m, HMS.s, (int)DMS.d, (int)DMS.m,  DMS.s   );

    pInfo->changeText( (char*)p_sInfo );
    
    unsigned long color;
    if (bNuit)                  color = 0xFFFF0000;
    else                        color = 0xFFFFFFFF;

    pInfo->setColor(color);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void StarCatalog::affiche_magnitude()
{
    sprintf((char*)p_sInfo, "%0.2f", (float)fMag );

    pInfo->changeText( (char*)p_sInfo );
    
    unsigned long color;
    if (bNuit)                  color = 0xFFFF0000;
    else                        color = 0xFFFFFFFF;

    pInfo->setColor(color);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

