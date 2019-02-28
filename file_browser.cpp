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
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
vector<string> explode(const string& s, const char& c)
{
	string buff = "";
	vector<string> v;
	cout << "expplode" << endl;
	for(int i=0; i<s.size(); i++)
	{
		if(s[i] != c)
	    {
		    buff = buff + s[i];
		    //cout << buff << endl;
		}

		if(s[i] == c && buff != "")
		{
		    cout << buff << endl;
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

    currentDir = "/home/rene/Documents/astronomie/logiciel/script/image/";

    pW              = new PanelWindow();
	panelDirName    = new PanelText( (char*)currentDir.c_str(),		PanelText::NORMAL_FONT, 20, 10 );
    panelDir        = new PanelSimple();
    panelFile       = new PanelSimple();
    panelOK         = new PanelButton();
    panelQuit       = new PanelButton();
    
    
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
    
    panelDirName->setPosAndSize( 0, 0, dx, DY);
    panelDir->setPosAndSize( 0, 20, 150, dy-20-50);
    panelFile->setPosAndSize( 150, 20, dx-150, dy-20-20);
    panelOK->setPosAndSize( 10, dy-20, 100, 20);
    panelQuit->setPosAndSize( 200, dy-20, 100, 20);
    
    panelOK->setUp(   (char*)"over.tga" );
    panelOK->setDown( (char*)"over.tga" );
    panelOK->setOver( (char*)"down.tga" );

    panelQuit->setUp(   (char*)"over.tga" );
    panelQuit->setDown( (char*)"over.tga" );
    panelQuit->setOver( (char*)"down.tga" );

    panelDir->setClickLeft( (click_left_cb_t) &cb_dir_click_left );
    panelDir->setReleaseLeft( (click_left_cb_t) &cb_dir_release_left );
    panelFile->setReleaseLeft( (click_left_cb_t) &cb_file_release_left );
    
    explore_dir( currentDir );
    
    WindowsManager::getInstance().add( pW );
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FileBrowser::explore_dir( string dirname )
{
    struct dirent *lecture;
    DIR *rep;
    PanelText *     pT;

    currentDir = dirname;
    setCurrentDirectory( dirname );
    panelDirName->changeText( dirname );
    rep = opendir( dirname.c_str() );

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
        
        vector<string> split = explode(currentDir, '/');

        dirname = "";
        for( int i=0; i<split.size()-1; i++ ) { dirname = dirname + "/" + split[i]; cout <<  dirname << endl;}
        dirname = dirname + "/";
        
    }
    else
        dirname = currentDir + dirname + "/";
    
    logf( (char*)"'%s'", (char*)dirname.c_str() );

    //-----------------------------------------    
       
    panelDir->deleteChilds();
    panelFile->deleteChilds();
    
    //-----------------------------------------    

    tDirNames.clear();
    tFileNames.clear();
        
    logf( (char*)" Explore %s", (char*)dirname.c_str() );
    explore_dir( dirname );
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

















