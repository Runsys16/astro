#include "file_browser.h"
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
FileBrowser* FileBrowser::s_pInstance;	// Instance de la classe


#define DY  15
#define DXFile  250
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool fct_tri(string i, string j)
{
    return (i.compare(j) < 0);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
vector<string> explode(const string& s, const char& c)
{
	string buff = "";
	vector<string> v;
	//cout << "explode" << endl;
	for(int i=0; i<s.size(); i++)
	{
		if(s[i] != c)
	    {
		    buff = buff + s[i];
		    //cout << buff << endl;
		}

		if(s[i] == c && buff != "")
		{
		    //cout << buff << endl;
		    v.push_back(buff);
		    buff = "";
		}
	}
	if(buff != "") v.push_back(buff);
	
	return v;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
FileBrowser::FileBrowser()
{
    logf( (char*)"----------- Constructeur filebrowser() -----------" );

    dirScroll  = 0;
    fileScroll = 0;

    workingDir = "/home/rene/programmes/";
    workingDir = "/home/rene/programmes/opengl/";
    workingDir = "/home/rene/Documents/astronomie/logiciel/script/image/atmp/2019-06-30/";
    filtre     = "";
    
    currentDir = string(workingDir);

    pW              = new PanelWindow();
	//panelDirName    = new PanelText( (char*)workingDir.c_str(),		PanelText::NORMAL_FONT, 20, 10 );
    panelDir        = new PanelDir(this);
    panelFile       = new PanelFile(this);
    panelOK         = new ButtonOK(this);
    panelQuit       = new ButtonQUIT(this);
    panelFilename   = new PanelEditText();
	panelDirName    = new PanelEditText();
	panelDirName->changeText( (char*)workingDir.c_str() );//		PanelText::NORMAL_FONT, 20, 10 );
    
    panelFilename->changeText( "" );
    pW->setDisplayGL(displayGLnuit_cb);
    //WindowsManager::getInstance().call_back_keyboard( panelFilename );
    pW->setExtraString( "panelFileBrowser" );

    
    dx = 700;
    dy = 600;
    x = (getScreenDX()-dx)/2;
    y = (getScreenDY()-dy)/2;
    
    pW->setPosAndSize(x, y, dx, dy);
    pW->setBackground( (char*)"images/background.tga" );
    cache();
    
	pW->add(panelDirName);
    pW->add(panelDir);
    pW->add(panelFile);
	pW->add(panelOK);
	pW->add(panelQuit);
	pW->add(panelFilename);

	pW->add( new PanelText( (char*)"Filename : ",		PanelText::NORMAL_FONT, 5, 0 ) );
	pW->add( new PanelText( (char*)"Directory : ",		PanelText::NORMAL_FONT, 5, 15 ) );


    
    panelDirName->setPosAndSize( 80, 15, dx, DY);
    panelDir->setPosAndSize( 0, 20+30, 150, dy-25-50-30);
    panelFile->setPosAndSize( 150, 20+30, dx-150, dy-25-20-30);
    
    panelOK->setPosAndSize( (dx)/3-54/2, dy-20, 54, 20);
    panelQuit->setPosAndSize( 2*(dx)/3-54/2, dy-20, 54, 20);
    
    panelFilename->setPosAndSize( 80, 0, 400, 20);

    panelDir->setScissor(true);
    panelFile->setScissor(true);
    
    panelOK->setUp(   (char*)"images/ok_over.tga" );
    panelOK->setDown( (char*)"images/ok_over.tga" );
    panelOK->setOver( (char*)"images/ok_down.tga" );

    panelQuit->setUp(   (char*)"images/quit_over.tga" );
    panelQuit->setDown( (char*)"images/quit_over.tga" );
    panelQuit->setOver( (char*)"images/quit_down.tga" );


    explore_dir();
    
    WindowsManager& wm = WindowsManager::getInstance();
    wm.add( pW );
    wm.sup_call_back_keyboard( panelFilename );

    bNewline = false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FileBrowser::explore_dir()
{
    struct dirent *lecture;
    DIR *rep;
    PanelText *     pT;

    rep = opendir( (char*)workingDir.c_str() );
    panelDirName->changeText( string(workingDir) );
    if ( rep==NULL )        return;

    int xd, yd, xf, yf;
    xd = xf = 8+16;
    yd = yf = 0;
    
    while ((lecture = readdir(rep))) {

        string s;
        int found;
        s = lecture->d_name;
        //if ( s[0] == '.' )      continue;
         
        if ( lecture->d_type == 8 )
        {
            if ( filtre == "" || s.find(filtre)!=string::npos )
                tFileNames.push_back( s );
        }
        else
        {
            tDirNames.push_back( s );
        }
    }

    closedir(rep);
    
    int n;
    n = tFileNames.size();
    sort( tFileNames.begin(), tFileNames.end(), fct_tri );
    sort( tDirNames.begin(), tDirNames.end(), fct_tri );
    
    //logf( (char*)"------ tFileNames -------"  );
    dy = panelFile->getPosDY();
     
    for( int i=0; i<tFileNames.size(); i++ )
    {
        //logf( (char*)"%s", (char*)tFileNames[i].c_str() );
        pT = new PanelText( (char*)tFileNames[i].c_str(),		PanelText::NORMAL_FONT, xf, yf );
        panelFile->add( pT );
        addImage( "images/file.png", panelFile, xf-4-16, yf );
        
        yf += DY;
        if ( yf >= dy )      { xf += DXFile; yf = 0; }
    }
            
    for( int i=0; i<tDirNames.size(); i++ )
    {
        pT = new PanelText( (char*)tDirNames[i].c_str(),		PanelText::NORMAL_FONT, xd, yd );
        panelDir->add( pT );
        addImage( "images/dir.png", panelDir, 4, yd );
        yd += DY;
    }
        
      
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FileBrowser::change_dir( int n )
{
    
    //logf( (char*)" Numero %d -> '%s'", n, (char*)tDirNames[n].c_str() );
    string dirname = tDirNames[n];
    
    if ( tDirNames[n].size() ==2 && tDirNames[n][0] == '.' && tDirNames[n][1] == '.' )
    {
        string s = tDirNames[n];
        
        vector<string> split = explode(workingDir, '/');

        dirname = "";
        for( int i=0; i<split.size()-1; i++ ) { dirname = dirname + "/" + split[i]; }
        dirname = dirname + "/";
        
    }
    else
        dirname = workingDir + dirname + "/";
    
    logf( (char*)"FileBrowser::change_dir() nouveau repertoire '%s'", (char*)dirname.c_str() );

    //-----------------------------------------    
       
    panelDir->deleteChilds();
    panelFile->deleteChilds();
    
    //-----------------------------------------    

    tDirNames.clear();
    tFileNames.clear();
    
    dirScroll = fileScroll = 0;
        
    logf( (char*)" Explore %s", (char*)dirname.c_str() );
    workingDir = dirname;
    explore_dir(  );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FileBrowser::change_dir( string dirname )
{
    logf( (char*)"FileBrowser::change_dir('%s')", (char*)dirname.c_str() );

    //-----------------------------------------    
       
    panelDir->deleteChilds();
    panelFile->deleteChilds();
    
    //-----------------------------------------    

    tDirNames.clear();
    tFileNames.clear();
        
    dirScroll = fileScroll = 0;

    logf( (char*)" Explore %s", (char*)dirname.c_str() );
    workingDir = dirname;
    explore_dir(  );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool FileBrowser::isInsideDir( int x, int y )
{
    logf( (char*)"FileBrowser::isInsideDir(%d, %d)", x, y );
    log_tab(true);

    int Y = panelDir->Screen2y(y);
    int n = Y / DY;
    
    if ( n >= tDirNames.size() )
    {
        logf( (char*)"NOK" );
        log_tab(false);
        logf( (char*)"FileBrowser::isInsideDir(%d, %d)", x, y );
        return false;
    }

    n -= dirScroll;
    logf( (char*)"Choix rep : %d  (scroll=%d)", n, dirScroll );
    logf( (char*)"OK %s ", (char*)tDirNames[n].c_str() );
    
    change_dir( n );        
    
    dirScroll = 0;

    log_tab(false);
    logf( (char*)"FileBrowser::isInsideDir(%d, %d)", x, y );
    return true;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool FileBrowser::isInsideFile( int x, int y )
{
    logf( (char*)"FileBrowser::isInsideFile(%d, %d)", x, y );
    log_tab(true);

    int Y = panelFile->Screen2y(y) / DY;
    int X = panelFile->Screen2x(x) / DXFile; 
    
    int m = panelFile->getPosDY() / DY;
    int n = m*X + Y;
    n -= fileScroll;

    logf( (char*)"--- X=%d Y=%d m=%d n=%d/%d", X, Y, m, n, tFileNames.size() );
    
    if ( n >= tFileNames.size() )
    {
        logf( (char*)"NOK X=%d Y=%d m=%d n=%d", X, Y, m, n );
        logf( (char*)"FileScroll = %d", fileScroll );
        log_tab(false);
        logf( (char*)"FileBrowser::isInsideFile()" );
        return false;
    }
    //logf( (char*)"OK %s ", (char*)tFileNames[n].c_str() );
    
    panelFilename->changeText( string(tFileNames[n]) );
    
    if ( panelOK->getCallback() == NULL ){
        charge_image( workingDir, tFileNames[n] );
    }
    else
    {
        string s = getWorkingDir() + getFilename();
        if (panelOK->getExtra() != 10)      panelOK->getCallback()->callback( true, 0, (char*)s.c_str() );
    }

    //charge_image( workingDir, tFileNames[n] );
    WindowsManager::getInstance().onTop(pW);
    
    log_tab(false);
    logf( (char*)"FileBrowser::isInsideFile()" );
    return true;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FileBrowser::addImage( string s, PanelSimple* p, int x, int y)
{
    PanelSimple* pImage = new PanelSimple();

    pImage->setPosAndSize( x, y, 16, 16 );
    pImage->setBackground( (char*)s.c_str() );
    p->add(pImage);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FileBrowser::affiche()
{
    WindowsManager& wm = WindowsManager::getInstance();

    pW->setVisible(true);
    wm.onTop( pW );
    wm.changeCapture( panelFilename );

    if ( !wm.is_call_back_keyboard( panelFilename ) )
    {
        logf( (char*)"FileBrowser::affiche() Ajout callback keyboard" );
        wm.changeFocus( panelFilename );
        wm.call_back_keyboard( panelFilename );
        wm.startKeyboard();
    }

    setColor(color);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FileBrowser::cache()
{
    WindowsManager& wm = WindowsManager::getInstance();

    pW->setVisible(false);

    while ( wm.is_call_back_keyboard(panelFilename) )
        wm.sup_call_back_keyboard( panelFilename );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FileBrowser::supCallBacks()
{
    WindowsManager& wm = WindowsManager::getInstance();

    while( wm.is_call_back_keyboard( panelFilename ) )      wm.sup_call_back_keyboard( panelFilename );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool FileBrowser::keyboard(char key, int x, int y)
{
  /*
    //logf( (char*)"Traitement FileBrowser::keyboard()" );
    WindowsManager& wm = WindowsManager::getInstance();
    Panel* p = wm.getCapture();
    
    if ( p != panelFilename )
    {
        //wm.stopKeyboard();
        while ( wm.is_call_back_keyboard(panelFilename) )
        {
            wm.sup_call_back_keyboard( panelFilename );
            logf( (char*)"FileBrowser::keyboard() Suppression callback keyboard" );
        }
        return false;
    }
    
    if ( !wm.is_call_back_keyboard( panelFilename ) )
    {
        logf( (char*)"FileBrowser::keyboard() Ajout callback keyboard" );
        wm.call_back_keyboard( panelFilename );
        wm.startKeyboard();
    }

    wm.startKeyboard();
    WindowsManager::getInstance().keyboardFunc( key, x, y);
    //logf( (char*)"Traitement PanelConsoleSerial::keyboard()" );
*/
   
    WindowsManager& wm = WindowsManager::getInstance();
    //wm.changeFocus( panelFilename );
    Panel* p = wm.getFocus();
    
    
    if ( p != NULL )        logf( (char*)"Focus : %d", p->getID() );
    /*
    if      ( p == this )            logf( (char*)"PanelApn::keyboard() focus panel console" );
    else if ( p == NULL )            logf( (char*)"PanelApn::keyboard() focus NULL" );
    else                             logf( (char*)"PanelApn::keyboard() focus autre chose ID=%d", p->getID() );
    */
    
    if ( p == panelFilename   )
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

    logf( (char*)"PanelApn::keyboard(%d) %c", key, key );

    if (   0 <= key &&  key <  'z' )                wm.keyboardFunc( key, x, y);
    
    
	switch(key){ 
	
	case 27:
	    {
            logf( (char*)"Echappe" );
            cache();
        return false;
	    }
	    break;
	case 13:
	    {
            logf( (char*)"FileBrowser::keyboard() KEY RETRUN ***" );
            if ( bNewline )
            {
                string s = getWorkingDir() + getFilename();
                logf( (char*)"  %s", (char*)s.c_str() );

                if ( panelOK->getCallback() != NULL )
                {
                    logf( (char*)"  callback NON null extra : %d", panelOK->getCallback()->getExtra() );

                    switch(panelOK->getCallback()->getExtra())
                    {
                    case 0:
                    {
                        panelOK->getCallback()->callback( true, 1, (char*)s.c_str() );
                    }
                    break;
                    case 10:
                    {
                        panelOK->getCallback()->callback( true, 10, (char*)s.c_str() );
                        logf( (char*)"  Extra %d", panelOK->getCallback()->getExtra() ); 
                    }    
                    break;
                    case 11:
                    {
                        panelOK->getCallback()->callback( true, 11, (char*)s.c_str() );
                        logf( (char*)"  Extra %d", panelOK->getCallback()->getExtra() ); 
                    }    
                    break;
                    case 12:
                    {
                        panelOK->getCallback()->callback( true, 12, (char*)s.c_str() );
                        logf( (char*)"  Extra %d", panelOK->getCallback()->getExtra() );
                        cache();
                    }    
                    break;
                    }
                }
                else
                {
                    logf( (char*)"  callback null " );
                }
                
                //if ( panelOK->getCallback() != NULL &&  panelOK->getCallback()->getExtra() == 10)       
                //    panelOK->getCallback()->callback( true, 1, (char*)s.c_str() );
            }
            else
            {
                logf( (char*)"Retour chariot non pris en compte bNewline=%s", BOOL2STR(bNewline) ); 
            }
            //cb_ok_release_left(0,0);
            
	    }
	    break;
    /*
	case 'a':
	    {
            scrollDir( 1 );
	    }
	    break;

	case 'A':
	    {
            scrollDir( -1 );
	    }
	    break;

	case 'b':
	    {
            scrollFile( 1 );
	    }
	    break;

	case 'B':
	    {
            scrollFile( -1 );
	    }
	    break;
    */
    default:
        {
            logf((char*)"FileBrowser key: %d", key);
        }
        break;
	}
    return true;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FileBrowser::scrollDir( int n )
{
    if ( n!=1 && n!=-1 )            return;
    
    dirScroll += n;
    
    vector<Panel*>& childs = panelDir->getChilds();
    for ( int i=0; i<childs.size(); i++ )
    {
        int x = childs[i]->getPosX();
        int y = childs[i]->getPosY() + n * DY;
        childs[i]->setPos( x, y );
        //logf( (char*)"Changement %d,%d", x, y );
    }

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FileBrowser::scrollFile( int n )
{
    if ( n!=1 && n!=-1 )            return;
    
    fileScroll += n;

    vector<Panel*>& childs = panelFile->getChilds();
    int max =  panelFile->getPosDY() / DY - 1;

    for ( int i=0; i<childs.size(); i++ )
    {
        int x = childs[i]->getPosX();
        int y = childs[i]->getPosY() + n * DY;
        if ( y < 0 )
        {
            y = max * DY;
            x -= DXFile;
        }
        if ( y > (max*DY) )
        {
            y = 0;
            x += DXFile;
        }
        
        childs[i]->setPos( x, y );
        logf( (char*)"Changement %d (%d, %d)  max=%d", i, x, y, (max*DY) );
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FileBrowser::setCallBackOK(ButtonCallBack* p)
{
    panelOK->setCallBack(p);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FileBrowser::setCallBackQUIT(ButtonCallBack* p)
{
    panelQuit->setCallBack(p);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FileBrowser::setCallBack(ButtonCallBack* p)
{
    panelOK->setCallBack(p);
    panelQuit->setCallBack(p);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FileBrowser::setExtra( int ii)
{
    panelOK->setExtra( ii );
    panelQuit->setExtra( ii );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FileBrowser::setColor( unsigned long c)
{
    color = c;
    pW->setColor( color );
    panelDir->setColor( color );
    panelFile->setColor( color );
    panelDirName->setColor(color);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FileBrowser::setFocus()
{
    WindowsManager& wm = WindowsManager::getInstance();
    wm.changeFocus( panelFilename );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FileBrowser::setFilename( string s )
{
    panelFilename->changeText( s );
}
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//
// Class ButtonOK
//
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
ButtonOK::ButtonOK( FileBrowser* p )
{
    pFB = p;
    pCallBack = NULL;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void ButtonOK::releaseLeft( int xm, int ym )
{
    logf( (char*)"ButtonOK::releaseLeft(xm, ym) " );
    
    string s = pFB->getWorkingDir() + pFB->getFilename();
    logf( (char*)"  extra: %d '%s'", getExtra(), (char*)s.c_str() );
    
    if ( pCallBack == NULL )                logf( (char*)"  callback null " );
    else                                    logf( (char*)"  callback NON null " );

    
    logf( (char*)"  Extra %d", getExtra() );
    switch(getExtra())
    {
    case 0:
    {
        if ( pCallBack == NULL )                setCurrentDirectory( s );
        else                                    pCallBack->callback( true, 1, (char*)s.c_str() );
    }
    break;
    case 10:
    {
        pCallBack->callback( true, 10, (char*)s.c_str() );
    }    
    break;
    case 11:
    {
        pCallBack->callback( true, 11, (char*)s.c_str() );
    }    
    break;
    case 12:
    {
        pCallBack->callback( true, 12, (char*)s.c_str() );
    }    
    break;
    }

    pFB->cache();
}
//--------------------------------------------------------------------------------------------------------------------
//
// Class ButtonQUIT
//
//--------------------------------------------------------------------------------------------------------------------
ButtonQUIT::ButtonQUIT( FileBrowser* p )
{
    pFB = p;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void ButtonQUIT::releaseLeft( int xm, int ym )
{
    if ( pCallBack != NULL )                pCallBack->callback( false, -1, (char*)"" );

    pFB->cache();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------








