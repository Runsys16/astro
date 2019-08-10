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
void cb_dir_click_left( int x, int y )
{
    //logf( (char*)"Click (%d,%d)", x, y );
    FileBrowser& fb = FileBrowser::getInstance();

    int n = y/DY;
    
    if ( n >= fb.getDirNames().size() )
    {
        logf( (char*)"[warning]n'existe pas (%d,%d)", x, y );
    }

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void cb_dir_release_left( int x, int y )
{
    //logf( (char*)"Click (%d,%d)", x, y );
    FileBrowser& fb = FileBrowser::getInstance();
    
    int n = y/DY;

    fb.isInsideDir(x, y);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void cb_file_release_left( int x, int y )
{
    //logf( (char*)"Click (%d,%d)", x, y );
    FileBrowser& fb = FileBrowser::getInstance();
    
    fb.isInsideFile(x, y);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void cb_ok_release_left( int x, int y )
{
    //logf( (char*)"Nouveau repertoire d'image : " );
    
    FileBrowser& fb = FileBrowser::getInstance();
    
    string s = fb.getWorkingDir();
    //fb.setCurrentDir( s );
    
    logf( (char*)"  %s", (char*)s.c_str() );
    setCurrentDirectory( s );
    
    fb.cache();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void cb_quit_release_left( int x, int y )
{
    FileBrowser::getInstance().cache();
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
    currentDir = string(workingDir);

    pW              = new PanelWindow();
	panelDirName    = new PanelText( (char*)workingDir.c_str(),		PanelText::NORMAL_FONT, 20, 10 );
    panelDir        = new PanelDir(this);
    panelFile       = new PanelFile(this);
    panelOK         = new PanelButton();
    panelQuit       = new PanelButton();
    panelFilename   = new PanelEditText();
    
    pW->setDisplayGL(displayGLnuit_cb);
    //WindowsManager::getInstance().call_back_keyboard( panelFilename );

    
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

    panelDir->setClickLeft( (click_left_cb_t) &cb_dir_click_left );
    panelDir->setReleaseLeft( (click_left_cb_t) &cb_dir_release_left );
    panelFile->setReleaseLeft( (click_left_cb_t) &cb_file_release_left );
    
    panelQuit->setReleaseLeft( (release_left_cb_t) &cb_quit_release_left );
    panelOK->setReleaseLeft( (release_left_cb_t) &cb_ok_release_left );

    explore_dir();
    
    WindowsManager& wm = WindowsManager::getInstance();
    wm.add( pW );
    wm.sup_call_back_keyboard( panelFilename );
    
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
    
    logf( (char*)"nouveau repertoire '%s'", (char*)dirname.c_str() );

    //-----------------------------------------    
       
    panelDir->deleteChilds();
    panelFile->deleteChilds();
    
    //-----------------------------------------    

    tDirNames.clear();
    tFileNames.clear();
        
    logf( (char*)" Explore %s", (char*)dirname.c_str() );
    workingDir = dirname;
    explore_dir(  );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool FileBrowser::isInsideDir( int x, int y )
{
    int Y = panelDir->Screen2y(y);
    int n = Y / DY;
    
    if ( n >= tDirNames.size() )
    {
        logf( (char*)"NOK" );
        return false;
    }

    n -= dirScroll;
    logf( (char*)"Choix rep : %d  (scroll=%d)", n, dirScroll );
    logf( (char*)"OK %s ", (char*)tDirNames[n].c_str() );
    
    change_dir( n );        
    
    dirScroll = 0;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool FileBrowser::isInsideFile( int x, int y )
{
    int Y = panelFile->Screen2y(y) / DY;
    int X = panelFile->Screen2x(x) / DXFile; 
    
    int m = panelFile->getPosDY() / DY;
    int n = m*X + Y;

    //logf( (char*)"--- X=%d Y=%d m=%d n=%d", X, Y, m, n );
    
    if ( n >= tFileNames.size() )
    {
        logf( (char*)"NOK X=%s Y=%d m=%d n=%d", X, Y, m, n );
        return false;
    }
    //logf( (char*)"OK %s ", (char*)tFileNames[n].c_str() );
    
    change_file( workingDir, tFileNames[n] );
    WindowsManager::getInstance().onTop(pW);
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
    wm.changeFocus( panelFilename );

    if ( !wm.is_call_back_keyboard( panelFilename ) )
    {
        logf( (char*)"Ajout callback" );
        wm.call_back_keyboard( panelFilename );
        wm.startKeyboard();
    }
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
bool FileBrowser::keyboard(char key, int x, int y)
{
    //logf( (char*)"Traitement FileBrowser::keyboard()" );
    WindowsManager& wm = WindowsManager::getInstance();
    Panel* p = wm.getFocus();
    
    if ( p != panelFilename )
    {
        //wm.stopKeyboard();
        while ( wm.is_call_back_keyboard(panelFilename) )
        {
            wm.sup_call_back_keyboard( panelFilename );
            logf( (char*)"FileBrowser::keyboard() Suppression callback" );
        }
        return false;
    }
    
    if ( !wm.is_call_back_keyboard( panelFilename ) )
    {
        logf( (char*)"FileBrowser::keyboard() Ajout callback" );
        wm.call_back_keyboard( panelFilename );
        wm.startKeyboard();
    }

    wm.startKeyboard();
    WindowsManager::getInstance().keyboardFunc( key, x, y);
    //logf( (char*)"Traitement PanelConsoleSerial::keyboard()" );
    
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
            logf( (char*)"ENTRE" );
            cb_ok_release_left(0,0);
	    }
	    break;

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







