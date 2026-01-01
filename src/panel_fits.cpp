#include "panel_fits.h"
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#define ECHELLE 10.0
#define FONTSIZE	10
#define INTERLIGNE	12
#define DY_MAX  (60*INTERLIGNE)
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelFits::~PanelFits()
{
    logf( (char*)"Destructeur PanelFits()");
    
    WindowsManager& wm = WindowsManager::getInstance();
    int nb = fits_keys.size();

    for( int i=0; i<nb; i++ )		{
    	pScroll->sup( fits_keys[i] );
    	pScroll->sup( fits_values[i] );
    	delete fits_keys[i];
    	delete fits_values[i];
    }
    sup( pScroll );
    delete pScroll;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelFits::PanelFits()
{
    WindowsManager& wm = WindowsManager::getInstance();
    pParentView = NULL;
    setExtraString( "PanelFits" );
    setBackground( (_Texture2D*)NULL);
    
    pScroll = new PanelScrollY();
    pScroll->setBackground( (char*)"images/background.tga" );
    pScroll->setDelta( INTERLIGNE );
	add(pScroll);
	pScroll->setPos(0,0);
    logf( (char*)"Constructeur PanelFits()");
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelFits::add_key_value(string k, string v )
{
    //logf( (char*)"PanelFits::add_key_value( '%s' , '%s')", k.c_str(), v.c_str());
	int i = fits_keys.size();
	string val = ": " + v;

	//fits_keys.push_back(   new PanelText((char*)k.c_str() ,  PanelText::NORMAL_FONT, 0,   i*14 ) );
	//fits_values.push_back( new PanelText((char*)val.c_str() ,  PanelText::NORMAL_FONT, 100, i*14 ));

	
	/*
	#define POLICE "/usr/share/fonts/truetype/ubuntu/UbuntuMono-B.ttf"
	#define POLICE "/usr/share/fonts/truetype/msttcorefonts/Andale_Mono.ttf"
	#define POLICE "/usr/share/fonts/truetype/msttcorefonts/andalemo.ttf"
	#define POLICE "/usr/share/fonts/truetype/ttf-bitstream-vera/VeraMono.ttf"
	#define POLICE "/usr/share/fonts/truetype/ttf-bitstream-vera/VeraMoBd.ttf"
	#define POLICE "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf"
	#define POLICE "/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf"
	#define POLICE "/usr/share/fonts/truetype/tlwg/TlwgMono.ttf"
	#define POLICE "/usr/share/fonts/truetype/tlwg/TlwgMono-Bold.ttf"
	#define POLICE "fonts/Nintendo-DS-BIOS.ttf"
	*/
	
	#define POLICE "fonts/DOS-VGA.ttf"
	//#define POLICE "fonts/UbuntuMono-R.ttf"
	PanelText* pTextKey =    new PanelText( (char*)k.c_str(), 		(char*)POLICE, 0,   i*INTERLIGNE, FONTSIZE, 0xFFFFFFFF );
	PanelText* pTextVal =    new PanelText( (char*)val.c_str(),  	(char*)POLICE, 100, i*INTERLIGNE, FONTSIZE, 0xFFFFFFFF );
	pTextKey->setDY( FONTSIZE );
	pTextVal->setDY( FONTSIZE );

	fits_keys.push_back(     pTextKey );
	fits_values.push_back(   pTextVal );
		
	pScroll->add( fits_keys[i] );
	pScroll->add( fits_values[i] );
	
	int DX = this->getPosDX();
	int DY = this->getPosDY();
	
	i+=1;
	int ii = i;
	//if ( i>=30 )		 ii = 30;
	
	
	if ( (ii*INTERLIGNE) < DY_MAX )	{
		this->setSize( DX, ii*INTERLIGNE + 2 );
		pScroll->setSize( DX, ii*INTERLIGNE );
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelFits::update()
{
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelFits::displayGL()
{
	if ( !visible )			return;
    VarManager&         var = VarManager::getInstance();

    if ( bNuit )		{
    	setColor( COLOR32(255, 0, 0, 255) );
    	pScroll->setColor( COLOR32(255, 0, 0, 255) );
    }
    else	{
    	setColor( COLOR32(255, 255, 255, 255) );    
    	pScroll->setColor( COLOR32(255, 255, 255, 255) );
    }

    PanelWindow::displayGL();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelFits::updatePos()
{
	/*
    int DY = pScroll->computeDY();// -  2* getBorderSize();
    if ( pScroll->getChildsSize() > 60 )		DY = 60*12;
    setPosDY( DY + 2 );// -  2* getBorderSize();
    pScroll->setPosDY( DY + 2 );// -  2* getBorderSize();
    */
    int DY = getDY();
    pScroll->setPosDY( DY );// -  2* getBorderSize();
	    
    PanelWindow::updatePos();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelFits::clickRight(int xm, int ym)
{
    logf( (char*)"PanelFits::clickRight(xm=%d, ym=%d)", xm, ym );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelFits::motionRight(int xm, int ym)
{
    logf( (char*)"PanelFits::motionRight(xm=%d, ym=%d)", xm, ym );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelFits::releaseRight(int xm, int ym)
{
    logf( (char*)"PanelFits::releaseRight(xm=%d, ym=%d)", xm, ym );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelFits::wheelUp(int xm, int ym)
{
    //logf( (char*)"PanelFits::wheelUp(xm=%d, ym=%d)", xm, ym );
    //pScroll->wheelUp(xm, ym);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelFits::wheelDown(int xm, int ym)
{
    //logf( (char*)"PanelFits::wheelDown(xm=%d, ym=%d)", xm, ym );
    //pScroll->wheelDown(xm, ym);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelFits::setParent(Panel* p )
{
	if ( pParentView == NULL )	{
		pParentView = p;
		WindowsManager::getInstance().add(this);
		WindowsManager::getInstance().onTop(this);
		//p->add(this);
		setPos( 10, 100 );
		setCanMove(true);
    	setExtraString( "PanelFits" );
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

