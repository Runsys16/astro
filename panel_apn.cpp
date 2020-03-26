#include "panel_apn.h"

//--------------------------------------------------------------------------------------------------------------------
//
/*
    string filename = " -o /home/rene/Documents/astronomie/logiciel/script/image/atmp/2000-01-01/k200d/test";
    string iso = " -i 400";
    string stime = " -t " + to_string(timePentax);
    string frames = " -F 1";
    string timeout = " --timeout=3";
    string focus = " -f";
    string command = "/home/rene/Documents/astronomie/logiciel/k200d/pktriggercord-0.84.04/pktriggercord-cli";
*/

//--------------------------------------------------------------------------------------------------------------------
PanelApn::PanelApn()
{
	WindowsManager&     wm  = WindowsManager::getInstance();
    VarManager&         var = VarManager::getInstance();

    wm.add(this);
    
    setPosAndSize( 1400, 200, 200, 200 );
    
 	setBackground((char*)"images/background.tga");

    int p   =  0;
    int dy  = 12;
    int x   =  5;
	
	add( new PanelText( (char*)"Temps (s)\t: ",		PanelText::NORMAL_FONT, x, p*dy) );
    pTime = new PanelEditText();
    pTime->changeText( "200" );
    pTime->setPos( x+100, p*dy );
    p++;
	add( new PanelText( (char*)"ISO      \t: ",		PanelText::NORMAL_FONT, x, p*dy ) );
    pIso = new PanelEditText();
    pIso->changeText( "800" );
    pIso->setPos( x+100, p*dy );
    p++;
	add( new PanelText( (char*)"Frames   \t: ",		PanelText::NORMAL_FONT, x, p*dy ) );
    pFrames = new PanelEditText();
    pFrames->changeText( "1" );
    pFrames->setPos( x+100, p*dy );
    p++;
	add( new PanelText( (char*)"TimeOut  \t: ",		PanelText::NORMAL_FONT, x, p*dy ) );
    pTimeOut = new PanelEditText();
    pTimeOut->changeText( "3" );
    pTimeOut->setPos( x+100, p*dy );
    
    add(pTime);
    add(pIso);
    add(pFrames);
    add(pTimeOut);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelApn::~PanelApn()
{
    sup(pTime);
    delete pTime;
    
    pTime = 0; 
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool PanelApn::keyboard(char key, int x, int y)
{
    WindowsManager& wm = WindowsManager::getInstance();
   
    Panel* p = wm.getFocus();
    /*
    if      ( p == this )              logf( (char*)"PanelConsoleSerial::keyboard() focus panel console" );
    else if ( p == pw )              logf( (char*)"PanelConsoleSerial::keyboard() focus panel window" );
    else if ( p == NULL )            logf( (char*)"PanelConsoleSerial::keyboard() focus NULL" );
    else                             logf( (char*)"PanelConsoleSerial::keyboard() focus autre chose ID=%d", p->getID() );
    */
    if ( p != this )
    {
        while ( wm.is_call_back_keyboard(pTime) )
        {
            logf( (char*)"PanelApn::keyboard() suppression callback" );
            wm.sup_call_back_keyboard( pTime );
        }    
        wm.sup_call_back_keyboard( pTime );
        return false;
    }

    if ( !wm.is_call_back_keyboard( pTime ) )
    {
        logf( (char*)"PanelApn::keyboard()   Ajout callback" );
        wm.call_back_keyboard( pTime );
        wm.startKeyboard();
    }


    wm.startKeyboard();
    WindowsManager::getInstance().keyboardFunc( key, x, y);


	switch(key){ 
	
	case 27:
	    {
        logf( (char*)"Echappe" );
        wm.sup_call_back_keyboard( pTime );
        return false;
	    }
	    break;

    default:
        {
        //logf((char*)"key: %d", key);
        }
        break;
	}
    
    return true;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

