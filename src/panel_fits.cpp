#include "panel_fits.h"
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#define ECHELLE 10.0
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelFits::~PanelFits()
{
    WindowsManager& wm = WindowsManager::getInstance();

    logf( (char*)"Destructeur PanelFits()");

    int nb = fits_keys.size();
    for( int i=0; i<nb; i++ )		{
    	delete fits_keys[i];
    	delete fits_values[i];
    }
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
	
	fits_keys.push_back(   new PanelText((char*)k.c_str() ,  PanelText::NORMAL_FONT, 0,   i*14 ) );
	fits_values.push_back( new PanelText((char*)val.c_str() ,  PanelText::NORMAL_FONT, 100, i*14 ));
	
	//this->add( fits_keys[i] );
	//this->add( fits_values[i] );
	pScroll->add( fits_keys[i] );
	pScroll->add( fits_values[i] );
	
	int DX = this->getPosDX();
	int DY = this->getPosDY();
	
	i+=1;
	int ii = i;
	//if ( i>=30 )		 ii = 30;
	
	this->setSize( DX, ii*14 );
	pScroll->setSize( DX, ii*14 );
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
    PanelWindow::updatePos();
    int DX = getDX();
    int DY = getDY();
    pScroll->setSize(DX, DY);
    pScroll->updatePos();
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
    logf( (char*)"PanelFits::wheelUp(xm=%d, ym=%d)", xm, ym );
    pScroll->wheelUp(xm, ym);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelFits::wheelDown(int xm, int ym)
{
    logf( (char*)"PanelFits::wheelDown(xm=%d, ym=%d)", xm, ym );
    pScroll->wheelDown(xm, ym);
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

