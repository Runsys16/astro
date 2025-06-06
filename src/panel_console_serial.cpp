#include "panel_console_serial.h"



//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelConsoleSerial::PanelConsoleSerial()
{
    pw = new PanelWindow();
    pc = new PanelConsole( 100, 5);
    cb = new Console();
    pw->setExtraString("PanelConsoleSerial");
    
    pw->setVisible(true);
    pw->add(pc);
    
	pw->setPosAndSize( 10, 10, 500, 700);
    pw->setDisplayGL(displayGLnuit_cb);
    pw->loadSkin(PanelWindow::STD);

    VarManager& var= VarManager::getInstance();
    int x, y, dx, dy;

    if ( var.existe("xPanelSerial"))			x = var.geti("xPanelSerial");
    else										x = 0;
    if ( var.existe("yPanelSerial"))			y = var.geti("yPanelSerial");
    else										y = 0;
    if ( var.existe("dxPanelSerial"))			dx = var.geti("dxPanelSerial");
    else										dx = 0;
    if ( var.existe("dyPanelSerial"))			dy = var.geti("dyPanelSerial");
    else										dy = 0;
    
    if ( x<= 0 )        x = 10;
    if ( dx<= 100 )     dx = 500;
    if ( y<= 0 )        y = 10;
    if ( dy<= 100 )     dy = 700;
	pw->setPosAndSize( x, y, dx, dy );
	
	pc->setPosAndSize( 0, 0, dx, dy);
	pc->setPrompt( "No connect> " );

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

    if ( !var.existe("bNuit") )              var.set( "bNuit", false );

    if ( var.getb("bNuit") )    pc->setColor( 0xffff0000 );
    else                        pc->setColor( 0xffffffff );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool PanelConsoleSerial::keyboard(char key, int x, int y)
{
    WindowsManager& wm = WindowsManager::getInstance();

    Panel* p = wm.getCapture();
    /*
    if      ( p == pc )              logf( (char*)"PanelConsoleSerial::keyboard() focus panel console" );
    else if ( p == pw )              logf( (char*)"PanelConsoleSerial::keyboard() focus panel window" );
    else if ( p == NULL )            logf( (char*)"PanelConsoleSerial::keyboard() focus NULL" );
    else                             logf( (char*)"PanelConsoleSerial::keyboard() focus autre chose ID=%d", p->getID() );
    */
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
	
	// CTRL q
	case 17:
	case 27:
	    {
        logf( (char*)"Echappe ou  Ctrl+D" );
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
    if ( str[0] == 0 || str[1] == 0)
    {
        logf( (char*)"[WARNING] PanelConsoleSerial::writeln chaine trop petite" );
        return;
    }
    
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

        if ( ad_change!=0.0 && dc_change!=0.0 )
        {
            Serveur_mgr::getInstance().write_stellarium( (char*)buff );
            //logf( (char*)"Em Stellarium Ad=%0.8f Dc=%0.8f", ad_change, dc_change );
        }
        return;
    }
    else
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
    
    
    if ( ad_change!=0.0 && dc_change!=0.0 )
    {
        if ( ad != ad_change )
        {
            ad = ad_change;
            change_ad_status( ad );
        }

        if ( dc != dc_change )
        {
            dc = dc_change;
            change_dc_status( dc );
        }
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelConsoleSerial::setVisible(bool b)
{
    WindowsManager& wm = WindowsManager::getInstance();

    bVisible=b;
    pw->setVisible(b);

    logf( (char*)"Donne le Focus a la console" );

    
    if ( b)
    {
        wm.onTop(pw);
        wm.changeFocus( pc );
        wm.changeCapture( pc );
    }
    else {
        wm.changeFocus(NULL);
        wm.changeCapture( NULL );
        wm.onBottom(pw);
    }
        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------



