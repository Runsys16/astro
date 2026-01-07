#include "star_catalogue.h"
//--------------------------------------------------------------------------------------------------------------------
//#define COORD
//#define AFFIDX
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
StarCatalog::StarCatalog( double _ra, double _de, double _mag, string _name, int _i )
{
	init( _ra, _de, _mag, -1.0, -1.0, -1.0, -1.0, _name, _i );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
StarCatalog::StarCatalog( 	double _ra, double _de, double _mag, double _pra, 
							double _era, double _pde, double _ede, string _name, int _i )
{
	init( _ra, _de, _mag, _pra, _era, _pde, _ede, _name, _i );
}

/*    //logf( (char*)"Constructeur StarCatalog()" );
    fpmRA	= _pra;
    feRA	= _era;
    fpmDE	= _pde;
    feDE	= _ede;
#endif

    fRA		= _ra;
    fDE		= _de;
    fMag	= _mag;
    
    name	= string( _name );
	idx		= _i;
    
	char            p_sInfo[225];


//#define COORD
#ifdef COORD
    struct hms HMS;
    struct dms DMS;
    deg2hms( fRA, HMS );
    deg2dms( fDE, DMS );
    //sprintf((char*)p_sInfo, "m=%0.2f %02dh %02d' %2.2f\" %02d° %02d' %02.2lf\"", mag, (int)HMS.h, (int)HMS.m, HMS.s, (int)DMS.d, (int)DMS.m,  DMS.s   );
    sprintf((char*)p_sInfo, "m=%0.4lf %.2lf %.2lf", fMag, fRA, fDE  );
#else    
    //sprintf((char*)p_sInfo, "m=%0.4f", (float)mag );
    sprintf((char*)p_sInfo, "%0.2lf", fMag );
#endif

    pInfo       = new PanelText( (char*)p_sInfo,		PanelText::NORMAL_FONT, 10, 10 );
    pInfo->setExtraString( "Star pInfo" );
    
    unsigned long color;
    if (bNuit)                  color = 0xFFFF0000;
    else                        color = 0xFFFFFFFF;

    pInfo->setColor(color);

}
*/
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void StarCatalog::init( 	double _ra, double _de, double _mag, double _pra, 
							double _era, double _pde, double _ede, string _name, int _i )
{
    //logf( (char*)"Constructeur StarCatalog()" );
    fpmRA	= _pra;
    feRA	= _era;
    fpmDE	= _pde;
    feDE	= _ede;

    fRA		= _ra;
    fDE		= _de;
    fMag	= _mag;
    
    name	= string( _name );
	idx		= _i;
    
	char            p_sInfo[225];


//#define COORD
#ifdef COORD
    struct hms HMS;
    struct dms DMS;
    deg2hms( fRA, HMS );
    deg2dms( fDE, DMS );
    //sprintf((char*)p_sInfo, "m=%0.2f %02dh %02d' %2.2f\" %02d° %02d' %02.2lf\"", mag, (int)HMS.h, (int)HMS.m, HMS.s, (int)DMS.d, (int)DMS.m,  DMS.s   );
    sprintf((char*)p_sInfo, "m=%0.4lf %.2lf %.2lf", fMag, fRA, fDE  );
#else    
    //sprintf((char*)p_sInfo, "m=%0.4f", (float)mag );
    sprintf((char*)p_sInfo, "%0.2lf", fMag );
#endif

    pInfo       = new PanelText( (char*)p_sInfo,		PanelText::NORMAL_FONT, 10, 10 );
    pInfo->setExtraString( "Star pInfo" );
    
    unsigned long color;
    if (bNuit)                  color = 0xFFFF0000;
    else                        color = 0xFFFFFFFF;

    pInfo->setColor(color);

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


	char            p_sInfo[225];

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
	char            p_sInfo[225];


	#ifdef AFFIDX
    sprintf((char*)p_sInfo, "%d-%0.2f", idx, (float)fMag );
	#else
    sprintf((char*)p_sInfo, "%0.2f", (float)fMag );
	#endif

    pInfo->changeText( (char*)p_sInfo );
    
    unsigned long color;
    if (bNuit)                  color = 0xFFFF0000;
    else                        color = 0xFFFFFFFF;

    pInfo->setColor(color);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

