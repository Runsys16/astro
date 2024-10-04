#include "panel_fits_correction.h"
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelCorrectionFits::~PanelCorrectionFits()
{
	logf( (char*)"Destructeur PanelCorrectionFits()");

    sup(pCDELT1plus);
    delete pCDELT1plus;
    pCDELT1plus = 0; 

    sup(pCDELT2plus);
    delete pCDELT2plus;
    pCDELT2plus = 0; 

    WindowsManager& wm = WindowsManager::getInstance();
    wm.sup( this );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelCorrectionFits::PanelCorrectionFits()
{
	logf( (char*)"Constructeur PanelCorrectionFits()");

    VarManager&         var = VarManager::getInstance();

    setExtraString( "PanelCorrectionFits" );
    setSize( 160, 200 );
    
 	setBackground((char*)"images/background.tga");

    int p   =   0;
    int dy  =  14;
    int x   =   5;
    int dx  =  90;
    float gris = 0.7;
    vcf4 c  = vcf4(gris, gris, gris, 1.0);

	double min=-0.01, max=0.01, step=0.02, ntour=20.0;

    this->setExtraString( (char*)"PanelCorrectionFits" );
    this->setBackground( (char*)"images/background.tga" );
    //this->setCanMove( true );

	pCDELT1plus = new PanelSpinEditText();
	add( new PanelText( (char*)"Asc. droite\t:",		PanelText::NORMAL_FONT, x, p*dy, get_color(c)) );
	add( pCDELT1plus );
    pCDELT1plus->set( min, max, step, ntour );
    pCDELT1plus->set_ndecimal(6);
    pCDELT1plus->set_delta( 20, 8 );
    pCDELT1plus->setPos( x+dx, p*dy );
    pCDELT1plus->setSize( 160-dx, dy );
    pCDELT1plus->setExtraString( "pCDELT1plus" );
    p++;

	pCDELT2plus = new PanelSpinEditText();
	add( new PanelText( (char*)"Declinaison\t:",		    PanelText::NORMAL_FONT, x, p*dy, get_color(c) ) );
	add( pCDELT2plus );
    pCDELT2plus->set( min, max, step, ntour );
    pCDELT2plus->set_ndecimal(6);
    pCDELT2plus->set_delta( 20, 8 );
    pCDELT2plus->setPos( x+dx, p*dy );
    pCDELT2plus->setSize( 160-dx, dy );
    pCDELT2plus->setExtraString( "pCDELT2plus" );
    p++;

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCorrectionFits::setVisible(bool b)
{
    //logf( (char*)"PanelCorrectionFits::setVisible(%s)", BOOL2STR(b) );

    PanelWindow::setVisible(b);
    WindowsManager& wm = WindowsManager::getInstance();
    if (b)
    {
        wm.onTop(this);
        //wm.changeCapture(this);
        //wm.changeFocus(pCDELT1plus);
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCorrectionFits::supCallBacks()
{
    WindowsManager& wm = WindowsManager::getInstance();

    while( wm.is_call_back_keyboard( pCDELT1plus ) )	wm.sup_call_back_keyboard( pCDELT1plus );
    while( wm.is_call_back_keyboard( pCDELT2plus ) )	wm.sup_call_back_keyboard( pCDELT2plus );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool PanelCorrectionFits::keyboard(char key, int x, int y)
{
    WindowsManager& wm = WindowsManager::getInstance();
   
    Panel* p = wm.getFocus();
    
    if ( p == pCDELT1plus || p == pCDELT2plus )
    {
        supCallBacks();
        wm.call_back_keyboard( p );
    }
    else
    {
        wm.stopKeyboard();
        return false;
    }
        

    wm.startKeyboard();
    //wm.keyboard(key, int x, int y)

    //logf( (char*)"PanelCorrectionFits::keyboard(%d) %c", key, key );

    //if (   0 <= key &&  key <  32  )                wm.keyboardFunc( key, x, y);
    if (        key ==  '.'        )                wm.keyboardFunc( key, x, y);
    if ( '0' <= key &&  key <= '9' )                wm.keyboardFunc( key, x, y);
    if ( ' ' <= key &&  key <= 126 )                return true;


    switch(key){
    case 27:
        {
        logf( (char*)"Echappe" );
        wm.stopKeyboard();
        setVisible( false );
        wm.changeFocus(NULL);
        supCallBacks();
        saveValues();
        }
        return false;
    case '\r':
        {
        logf( (char*)"Touche Entree" );
        wm.stopKeyboard();
        setVisible( false );
        wm.changeFocus(NULL);
        saveValues();
        supCallBacks();
        }
        return true;
    case 17:
        return false;
    }
    return true;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCorrectionFits::saveValues()
{
    VarManager&         var = VarManager::getInstance();
    
    logf( (char*)"PanelCorrectionFits::saveValues()" );    
    
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCorrectionFits::displayGL()
{
    VarManager&         var = VarManager::getInstance();
    unsigned int color; 
    if ( var.getb("bNuit") )        color = 0xffff0000;
    else                            color = 0xffffffff;
    
    setColor( color );
    pCDELT1plus->setColor( color );
    pCDELT2plus->setColor( color );
    
    
    PanelWindow::displayGL();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCorrectionFits::setParent()
{
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCorrectionFits::onTop()
{
    WindowsManager& wm = WindowsManager::getInstance();
	wm.onTop( this );
    wm.onTop( pCDELT1plus );
    wm.onTop( pCDELT2plus );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
















