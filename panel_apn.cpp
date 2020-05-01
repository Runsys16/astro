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
    setExtraString( "panelApn" );
    
    setPos( 1000, 200 );
    
 	setBackground((char*)"images/background.tga");

    int p   =   0;
    int dy  =  14;
    int x   =   5;
    int dx  =  90;
    float gris = 0.7;
    vec4 c  = vec4(gris, gris, gris, 1.0);
	
	add( new PanelText( (char*)"Temps (s)\t:",		PanelText::NORMAL_FONT, x, p*dy, get_color(c)) );
    pTime = new PanelSpinEditText();
    pTime->set( 0, 60*15, 1, 1 );
    pTime->set_delta( 20, 8 );
    pTime->changeText( "200" );
    pTime->setPos( x+dx, p*dy );
    pTime->setSize( 200, dy );
    pTime->setExtraString( "pTime" );
    p++;
	add( new PanelText( (char*)"ISO\t\t:",		    PanelText::NORMAL_FONT, x, p*dy, get_color(c) ) );
    pIso = new PanelSpinEditText();
    pIso->set( 100, 3200, 1, 1 );
    pIso->set_delta( 20, 8 );
    pIso->changeText( "800" );
    pIso->setPos( x+dx, p*dy );
    pIso->setSize( 200, dy );
    pIso->setExtraString( "pIso" );
    p++;
	add( new PanelText( (char*)"Frames\t:",		    PanelText::NORMAL_FONT, x, p*dy, get_color(c) ) );
    pFrames = new PanelSpinEditText();
    pFrames->set( 0, 200, 1, 1 );
    pFrames->set_delta( 20, 8 );
    pFrames->changeText( "1" );
    pFrames->setPos( x+dx, p*dy );
    pFrames->setSize( 200, dy );
    pFrames->setExtraString( "pFrames" );
    p++;
	add( new PanelText( (char*)"TimeOut\t:",		PanelText::NORMAL_FONT, x, p*dy, get_color(c) ) );
    pTimeOut = new PanelSpinEditText();
    pTimeOut->set( 1, 10, 1, 1);
    pTimeOut->set_delta( 20, 8 );
    pTimeOut->changeText( "3" );
    pTimeOut->setPos( x+dx, p*dy );
    pTimeOut->setSize( 200, dy );
    pTimeOut->setExtraString( "pTimeOut" );
    p++;
	add( new PanelText( (char*)"Num\t\t:",		    PanelText::NORMAL_FONT, x, p*dy, get_color(c) ) );
    pNum = new PanelSpinEditText();
    pNum->set( 1, 1000, 1 , 1 );
    pNum->set_delta( 20, 8 );
    pNum->changeText( "0" );
    pNum->setPos( x+dx, p*dy );
    pNum->setSize( 200, dy );
    pNum->setExtraString( "pNum" );
    p++;
    
    setSize( 160, p*dy );
    
    add(pTime);
    add(pIso);
    add(pFrames);
    add(pTimeOut);
    add(pNum);

    if( var.existe("APN_Time") )                sTime       = string( *var.gets( "APN_Time" ) );
    else                                        sTime       = "2";

    if( var.existe("APN_Iso") )                 iIso        = var.geti( "APN_Iso" );
    else                                        iIso        = 400;

    if( var.existe("APN_Frames") )              iFrames     = var.geti( "APN_Frames" );
    else                                        iFrames     = 1;

    if( var.existe("APN_TimeOut") )             iTimeOut    = var.geti( "APN_TimeOut" );
    else                                        iTimeOut    = 3;

    if( var.existe("APN_Num") )                 iNum        = var.geti( "APN_Num" );
    else                                        iNum        = 0;

    pTime->changeText( sTime );
    pIso->changeText( to_string(iIso) );
    pFrames->changeText( to_string(iFrames) );
    pTimeOut->changeText( to_string(iTimeOut) );
    pNum->changeText( to_string(iNum) );
    
    wm.onTop(this);
    wm.changeCapture(this);
    wm.changeFocus(pTime);
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
void PanelApn::setVisible(bool b)
{
    logf( (char*)"PanelApn::setVisible(%s)", BOOL2STR(b) );

    PanelWindow::setVisible(b);
    WindowsManager& wm = WindowsManager::getInstance();
    if (b)
    {
        wm.onTop(this);
        wm.changeCapture(this);
        wm.changeFocus(pTime);
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelApn::supCallBacks()
{
    WindowsManager& wm = WindowsManager::getInstance();

    while( wm.is_call_back_keyboard( pTime ) )      wm.sup_call_back_keyboard( pTime );
    while( wm.is_call_back_keyboard( pIso ) )       wm.sup_call_back_keyboard( pIso );
    while( wm.is_call_back_keyboard( pFrames ) )    wm.sup_call_back_keyboard( pFrames );
    while( wm.is_call_back_keyboard( pTimeOut ) )   wm.sup_call_back_keyboard( pTimeOut );
    while( wm.is_call_back_keyboard( pNum ) )       wm.sup_call_back_keyboard( pNum );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool PanelApn::keyboard(char key, int x, int y)
{
    WindowsManager& wm = WindowsManager::getInstance();
   
    Panel* p = wm.getFocus();
    
    /*
    if ( p!= NULL && p->getID() == 9010 )
        logf( (char*)"Focus %s", (char*)p->getExtraString().c_str() );
    
    if      ( p == this )            logf( (char*)"PanelApn::keyboard() focus panel console" );
    else if ( p == NULL )            logf( (char*)"PanelApn::keyboard() focus NULL" );
    else                             logf( (char*)"PanelApn::keyboard() focus autre chose ID=%d", p->getID() );
    */
    
    if ( p == pTime || p == pIso || p == pFrames || p == pTimeOut || p == pNum )
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

    //logf( (char*)"PanelApn::keyboard(%d) %c", key, key );

    if (   0 <= key &&  key <  32  )                wm.keyboardFunc( key, x, y);
    if (        key ==  '.'        )                wm.keyboardFunc( key, x, y);
    if ( '0' <= key &&  key <= '9' )                wm.keyboardFunc( key, x, y);
    if ( ' ' <= key &&  key <= 126 )                return true;


    switch(key){
    case 27:
        {
        logf( (char*)"Echappe" );
        setVisible( false );
        wm.changeFocus(NULL);
        supCallBacks();
        wm.stopKeyboard();
        }
        return true;
    case '\r':
        {
        logf( (char*)"Touche Entree" );
        setVisible( false );
        wm.changeFocus(NULL);
        saveValues();
        supCallBacks();
        wm.stopKeyboard();
        photo();
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
void PanelApn::saveValues()
{
    VarManager&         var = VarManager::getInstance();
    
    sTime = string( pTime->getText() );
    var.set( "APN_Time", sTime );
    logf( (char*)"Time = %s", (char*)sTime.c_str() );    
    
    iIso = stoi( pIso->getText(), NULL, 10 );
    var.set( "APN_Iso", iIso );
    logf( (char*)"ISO = %d", iIso );    
    
    iFrames = stoi( pFrames->getText(), NULL, 10 );
    var.set( "APN_Frames", iFrames );
    logf( (char*)"Frames = %d", iFrames );    
    
    iTimeOut = stoi( pTimeOut->getText(), NULL, 10 );
    var.set( "APN_TimeOut", iTimeOut );
    logf( (char*)"TimeOut = %d", iTimeOut );    
    
    iNum = stoi( pNum->getText(), NULL, 10 );
    var.set( "APN_Num", iNum );
    logf( (char*)"Num = %d", iNum );    
    
}
thread thread_pentax;
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelApn::commande_photo()
{
    VarManager&         var = VarManager::getInstance();

    string filename = " -o /home/rene/Documents/astronomie/logiciel/script/image/atmp/2000-01-01/k200d/test-"
                        + to_string(iNum++);
    string iso = " -i " + pIso->getText();
    string stime = " -t " + pTime->getText();
    string frames = " -F " + pFrames->getText();
    string timeout = " --timeout=" + pTimeOut->getText();
    string focus = " -f";
    string mode = " -m M";
    string command = "/home/rene/Documents/astronomie/logiciel/k200d/pktriggercord-0.84.04/pktriggercord-cli";

    command = command + filename + iso + stime + timeout + mode;


    pNum->changeText( to_string(iNum) );
    var.set( "APN_Num", iNum );

    logf( (char*) command.c_str() );

    
    int ret = system( (char*) command.c_str() );
    if ( ret != 0 )
    {
        string mes = "Erreur de commande Pentax Valeur retourne : " +  to_string(ret);
        alertBox(mes);
        logf( (char*)mes.c_str() );
    }
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelApn::photo()
{
    string dir = "/home/rene/Documents/astronomie/logiciel/script/image/atmp/2000-01-01/k200d/";

    thread( &PanelApn::commande_photo, this ).detach();
    Surveillance::getInstance().start(dir);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

