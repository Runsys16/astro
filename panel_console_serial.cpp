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
    VarManager& var= VarManager::getInstance();
    int x = var.geti("xPanelSerial");
    int y = var.geti("yPanelSerial");
    int dx = var.geti("dxPanelSerial");
    int dy = var.geti("dyPanelSerial");
    if ( x<= 0 )        x = 10;
    if ( dx<= 100 )     dx = 500;
    if ( y<= 0 )        y = 10;
    if ( dy<= 100 )     dy = 700;
	pw->setPosAndSize( x, y, dx, dy );
	
	pc->setPosAndSize( 0, 0, dx, dy);
	pc->setPrompt( "arduino> " );

    pc->setCallBackCmd( cb );
    pc->setTabSize( 60 );
    
    pc->setBackground((char*)"images/background.tga");

    WindowsManager& wm = WindowsManager::getInstance();
    
    wm.add(pw);
    wm.sup_call_back_keyboard( pc );
    
    ad = -1.0;
    dc = -1.0;
    ad_change = -1.0;
    dc_change = -1.0;


    if ( var.getb("bNuit") )    pc->setColor( 0xffff0000 );
    else                        pc->setColor( 0xffffffff );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool PanelConsoleSerial::keyboard(char key, int x, int y)
{
    WindowsManager& wm = WindowsManager::getInstance();

    Panel* p = wm.getFocus();
    if ( p != pc )
    {
        while ( wm.is_call_back_keyboard(pc) )
        {
            logf( (char*)"PanelConsoleSerial::keyboard() suppression callback" );
            wm.sup_call_back_keyboard( pc );
        }    
        wm.sup_call_back_keyboard( pc );
        return false;
    }

    if ( !wm.is_call_back_keyboard( pc ) )
    {
        logf( (char*)"PanelConsoleSerial::keyboard()   Ajout callback" );
        wm.call_back_keyboard( pc );
        wm.startKeyboard();
    }


    wm.startKeyboard();
    WindowsManager::getInstance().keyboardFunc( key, x, y);
    //logf( (char*)"Traitement PanelConsoleSerial::keyboard()" );
    
	switch(key){ 
	
	case 27:
	    {
        logf( (char*)"Echappe" );
        wm.sup_call_back_keyboard( pc );
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
void PanelConsoleSerial::writeln(char* str)
{
    if ( str[0] == '-' )
    {
        if ( str[1] == 'a' )
        {
            ad_change = strtod(&str[2],NULL);
            //logf( (char*)"AD" );
        }
        else if ( str[1] == 'd' )
        {
            dc_change = strtod(&str[2],NULL);
            //logf( (char*)"DEC" );
        }

        time_t t = time(0);
        //cout << t << endl;
        //long long tl = (unsigned long long)t * 1000000LL;
        long long tl = (unsigned long long)t;
        
        unsigned char buff[24];
        memset( buff, 0, 24);
        buff[0] = 24;
        
        unsigned char *          conv = (unsigned char*)&tl;
        
        
        buff[4] = conv[0];
        buff[5] = conv[1];
        buff[6] = conv[2];
        buff[7] = conv[3];
        buff[8] = conv[4];
        buff[9] = conv[5];
        buff[10] = conv[6];
        buff[11] = conv[7];
        
        
        float fa, fd;
        
        fa = ad_change;// / M_PI * 2147483648;
        fd = dc_change;//) / M_PI * 2147483648;

/*
        if self.ad <0.0:      
            self.ad = 180.0 + self.ad
            self.dc = 180.0 - self.dc

        if self.dc >180.0:      
            self.dc = -self.dc  + 180.0
            if self.ad < 180.0:
                self.ad = -180.0 + self.ad
*/        
        if ( fa <0.0 )
        {      
            fa = 180.0 + fa;
            fd = 180.0 - fd;
        }
        
        if ( fd >180.0)
        {      
            fd = -fd + 180.0;
            if ( fa < 180.0 )       fa = -180.0 + fa;
        }
        
        fa = DEG2RAD(fa) / M_PI * 2147483648;
        fd = DEG2RAD(fd) / M_PI * 2147483648;

        int a, d;
        
        a = fa;
        d = fd;
        
        conv = (unsigned char*)&a;
        buff[12] = conv[0];
        buff[13] = conv[1];
        buff[14] = conv[2];
        buff[15] = conv[3];

        conv = (unsigned char*)&d;
        buff[16] = conv[0];
        buff[17] = conv[1];
        buff[18] = conv[2];
        buff[19] = conv[3];

        Serveur_mgr::getInstance().write_stellarium( (char*)buff );

        return;
    }
    pc->affiche( str );
    //logf( (char*)str );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelConsoleSerial::idleGL()
{
    if ( pw->getHaveMove() )
    {
        pw->resetHaveMove();
        VarManager& var= VarManager::getInstance();

        var.set("xPanelSerial",  pw->getX() );
        var.set("yPanelSerial",  pw->getY() );
        var.set("dxPanelSerial", pw->getDX() );
        var.set("dyPanelSerial", pw->getDY() );
    }
    
    
    if ( ad != ad_change )
    {
        ad = ad_change;
        change_ad( ad );
    }

    if ( dc != dc_change )
    {
        dc = dc_change;
        change_dc( dc );
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------


