#ifndef PANEL_DEBUG_CPP
#define PANEL_DEBUG_CPP
//----------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------
#include "panel_debug.h"
//----------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------
PanelDebug::PanelDebug() : PanelWindow()
{
	logf( (char*)"Contructeur PanelDebug::PanelDebug() %d", __LINE__ );
	WindowsManager& wm = WindowsManager::getInstance();
	//setPosAndSize( 10, 10, DX, 14 );
	wm.add( this );
	idx = 0;
	setVisible(false);
	setBackground( (char*)"images/background.tga" );
	tab_size = 50;
	if ( bNuit )		uColor = 0xff0000ff;
	else				uColor = 0xffFFffFF;
}
//----------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------
PanelDebug::~PanelDebug()
{
	WindowsManager& wm = WindowsManager::getInstance();
	wm.sup( this );

	logf( (char*)"Destructeur PanelDebug::~PanelDebug() %d", __LINE__ );
	
	int nb = pText.size();
	logf( (char*)"|  destruction de %d PanelText", nb );
	for( int i=0; i<nb; i++ )
	{
		//logf( (char*)"%02d/%02d PanelText", i, pText.size()  );
		delete pText[i];
		pText[i] = NULL;
		//pText.erase( pText.begin()+ i );
	}

	//pText.clear();
}
//----------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------
void PanelDebug::add_text( char* p)
{
	//logf( (char*)"PanelDebug::add(%s)", p );
	
	if ( idx >= pText.size() )
	{
		PanelText* pPanelText =  new PanelText((char*)p ,  PanelText::NORMAL_FONT, 5,  7 + idx*14 );
		pText.push_back( pPanelText );
		pPanelText->setTabSize(tab_size);
		add( pPanelText );
		int __DY = (idx+1) * 14 + 14;
		setSize( getDX(), __DY );
	}
	else
	{
		pText[idx]->changeText( p );
		//pText[idx]->buildString();
	}
	
	pText[idx]->setColor( uColor );

	idx ++;
}
//----------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------
void PanelDebug::add_textf( char* fmt, ...)
{
    char chaine[1024];
    
    va_list arglist;

    va_start( arglist, fmt );
    vsnprintf( chaine, sizeof(chaine), fmt, arglist );
    va_end( arglist );
    
    add_text( (char*)chaine );
}
//----------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------
void PanelDebug::reset_list()
{
	int nb = pText.size();
	for( int i=idx; i<nb; i++ )
	{
		pText[i]->changeText((char*)"");
	}
	idx = 0;
}
//----------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------
void PanelDebug::setTabSize(unsigned u)
{
	tab_size = u;

	int nb = pText.size();
	for( int i=0; i<nb; i++ )
	{
		pText[i]->setTabSize(tab_size);
	}
}
//----------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------
void PanelDebug::setColor( uint32_t c )
{
	PanelWindow::setColor( c );

	int nb = pText.size();
	for( int i=0; i<nb; i++ )
	{
		pText[i]->setColor( c );
	}
	
	uColor = c;
}
//----------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------
#endif

