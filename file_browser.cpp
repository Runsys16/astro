#include "file_browser.h"
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------


#define DY  15
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
    logf( (char*)"Click (%d,%d)", x, y );
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
    logf( (char*)"Click (%d,%d)", x, y );
    FileBrowser& fb = FileBrowser::getInstance();
    
    int n = y/DY;

    fb.isInsideDir(x, y);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void cb_file_release_left( int x, int y )
{
    logf( (char*)"Click (%d,%d)", x, y );
    FileBrowser& fb = FileBrowser::getInstance();
    
    int n = y/DY;

    fb.isInsideFile(x, y);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void cb_ok_release_left( int x, int y )
{
    logf( (char*)"Nouveau repertoire d'image : " );
    
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

    workingDir = "/home/rene/Documents/astronomie/logiciel/script/image/";
    //workingDir = "/home/rene/programmes/";
    currentDir = string(workingDir);

    pW              = new PanelWindow();
	panelDirName    = new PanelText( (char*)workingDir.c_str(),		PanelText::NORMAL_FONT, 20, 10 );
    panelDir        = new PanelSimple();
    panelFile       = new PanelSimple();
    panelOK         = new PanelButton();
    panelQuit       = new PanelButton();
    panelFilename   = new PanelEditText();
    
    //WindowsManager::getInstance().call_back_keyboard( panelFilename );

    
    dx = 700;
    dy = 600;
    x = (getScreenDX()-dx)/2;
    y = (getScreenDY()-dy)/2;
    
    pW->setPosAndSize(x, y, dx, dy);
    pW->setBackground( (char*)"background.tga" );
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
    
    panelOK->setUp(   (char*)"ok_over.tga" );
    panelOK->setDown( (char*)"ok_over.tga" );
    panelOK->setOver( (char*)"ok_down.tga" );

    panelQuit->setUp(   (char*)"quit_over.tga" );
    panelQuit->setDown( (char*)"quit_over.tga" );
    panelQuit->setOver( (char*)"quit_down.tga" );

    panelDir->setClickLeft( (click_left_cb_t) &cb_dir_click_left );
    panelDir->setReleaseLeft( (click_left_cb_t) &cb_dir_release_left );
    panelFile->setReleaseLeft( (click_left_cb_t) &cb_file_release_left );
    
    panelQuit->setReleaseLeft( (release_left_cb_t) &cb_quit_release_left );
    panelOK->setReleaseLeft( (release_left_cb_t) &cb_ok_release_left );

    explore_dir();
    
    WindowsManager::getInstance().add( pW );
    
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
    
    logf( (char*)"------ tFileNames -------"  );

    for( int i=0; i<tFileNames.size(); i++ )
    {
        logf( (char*)"%s", (char*)tFileNames[i].c_str() );
        pT = new PanelText( (char*)tFileNames[i].c_str(),		PanelText::NORMAL_FONT, xf, yf );
        panelFile->add( pT );
        addImage( "file.png", panelFile, xf-4-16, yf );
        
        yf += DY;
        if ( yf > dy )      { xf += 250; yf = 0; }
    }
            
    for( int i=0; i<tDirNames.size(); i++ )
    {
        pT = new PanelText( (char*)tDirNames[i].c_str(),		PanelText::NORMAL_FONT, xd, yd );
        panelDir->add( pT );
        addImage( "dir.png", panelDir, 4, yd );
        yd += DY;
    }
        
      
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FileBrowser::change_dir( int n )
{
    
    logf( (char*)" Numero %d -> '%s'", n, (char*)tDirNames[n].c_str() );
    string dirname = tDirNames[n];
    
    if ( tDirNames[n].size() ==2 && tDirNames[n][0] == '.' && tDirNames[n][1] == '.' )
    {
        string s = tDirNames[n];
        
        vector<string> split = explode(workingDir, '/');

        dirname = "";
        for( int i=0; i<split.size()-1; i++ ) { dirname = dirname + "/" + split[i]; cout <<  dirname << endl;}
        dirname = dirname + "/";
        
    }
    else
        dirname = workingDir + dirname + "/";
    
    logf( (char*)"'%s'", (char*)dirname.c_str() );

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
    logf( (char*)"OK %s ", (char*)tDirNames[n].c_str() );
    
    change_dir( n );        
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool FileBrowser::isInsideFile( int x, int y )
{
    int Y = panelFile->Screen2y(y);
    int n = Y / DY;
    
    if ( n >= tFileNames.size() )
    {
        logf( (char*)"NOK" );
        return false;
    }
    logf( (char*)"OK %s ", (char*)tFileNames[n].c_str() );
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
    pW->setVisible(true);
    WindowsManager::getInstance().onTop( pW );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool FileBrowser::keyboard(char key, int x, int y)
{
    logf( (char*)"Traitement FileBrowser::keyboard()" );
    WindowsManager& wm = WindowsManager::getInstance();

    /*
    if ( wm.getFocus() != pW )
    {
        wm.stopKeyboard();
        //logf( (char*)"PAS Traitement PanelConsoleSerial::keyboard()" );
        return false;
    }
    */
    
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
        logf( (char*)"Echappe" );
        cb_ok_release_left(0,0);
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







