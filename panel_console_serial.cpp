#include "panel_console_serial.h"


//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelConsoleSerial::PanelConsoleSerial()
{
    pw = new PanelWindow();
    pc = new PanelConsole( 100, 5);
    cb = new Console();
    
    pw->setVisible(true);
    pw->add(pc);
    
	pw->setPosAndSize( 10, 10, 500, 700);
	pc->setPosAndSize( 0, 0, 500, 700);
	pc->setPrompt( "arduino> " );

    pc->setCallBackCmd( cb );
    pc->setTabSize( 60 );
    
    pc->setBackground((char*)"background.tga");

    WindowsManager::getInstance().add(pw);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool PanelConsoleSerial::keyboard(char key, int x, int y)
{
    WindowsManager& wm = WindowsManager::getInstance();

    if ( wm.getFocus() != pc )
    {
        wm.stopKeyboard();
        //logf( (char*)"PAS Traitement PanelConsoleSerial::keyboard()" );
        return false;
    }
    
    wm.startKeyboard();
    WindowsManager::getInstance().keyboardFunc( key, x, y);
    //logf( (char*)"Traitement PanelConsoleSerial::keyboard()" );
    
	switch(key){ 
	
	case 27:
	    {
        logf( (char*)"Echappe" );
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
void PanelConsoleSerial::writeln(char* str)
{
    if ( str[0] == '-' )
    {
        if ( str[1] == 'a' )
        {
            float ad = strtod(&str[2],NULL);
            change_ad( ad );
            //logf( (char*)"AD" );
        }
        else if ( str[1] == 'd' )
        {
            float dc = strtod(&str[2],NULL);
            change_dc( dc );
            //logf( (char*)"DEC" );
        }
        return;
    }
    pc->affiche( str );
    logf( (char*)str );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------


