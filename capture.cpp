#include "capture.h"



static int                      num;
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool fct_tri_capture(string i, string j)
{
    return (i.compare(j) < 0);
}
string old_dir = "";
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Capture::Capture()
{
    logf((char*)"Constructeur Capture() -----------" );
    log_tab(true);
    
    logf((char*)"old_dir : %s", (char*)old_dir.c_str() );

    if ( getCurrentDirectory() == "" )
    {
        logf( (char*)"[Erreur] Impossible de charger le repertoire : " );
        logf( (char*)"[Erreur] Veuillez fermer la fenetre touch'8'" );
        create_preview();
        log_tab(false);
        return;
    }

    if ( old_dir != getCurrentDirectory() )
    {
        old_dir = getCurrentDirectory();
        num = 0;
    }
    
    pooling();

    if ( filenames.size() != 0 )
    {
        num = num % filenames.size();
        filename = string( filenames[num] );
        num++;
    }
    else
    {
        filename = "";
        num = -1;
    }

    //logf( (char*)"image : %s", filename.c_str() );
    vector<string> res = split(filename,"/");
    dirname = "";
    
    int nb = res.size();
    for( int i=0; i<nb-1; i++ )
    {
        dirname += "/" + res[i];
    }
    basename = res[nb-1];
    //logf( (char*)"   dirname  : %s", (char*)dirname.c_str() );
    //logf( (char*)"   basename : %s", (char*)basename.c_str() );
    
    create_preview();

    bIcone = false;
    bFullScreen = false;

    setExtraString( "Capture ..." );

    log_tab(false);
    logf((char*)"Constructeur Capture() -----------" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Capture::Capture(string dirname, string name)
{
    logf((char*)"Constructeur Capture(dirname, %s) -----------", (char*)name.c_str() );
    log_tab(true);
    
    filename = dirname + name;
    dirname = dirname;
    basename = name;
    bIcone = false;
    bFullScreen = false;

    logf( (char*)"image : %s", filename.c_str() );
    create_preview();

    setExtraString( "Capture ..." );

    log_tab(false);
    logf((char*)"Constructeur Capture(%s, %s) -----------", (char*)dirname.c_str(), (char*)name.c_str() );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Capture::Capture(string f )
{
    logf((char*)"Constructeur Capture(%s) -----------", (char*)f.c_str() );
    log_tab(true);
    
    filename = f;

    vector<string> res = split(f,"/");
    dirname = "";
    
    int nb = res.size();
    for( int i=0; i<nb-1; i++ )
    {
        dirname += "/" + res[i];
    }

    basename = res[nb-1];

    logf( (char*)"basename : %s", basename.c_str() );
    create_preview();
    create_icones();

    setExtraString( "Capture ..." );

    log_tab(false);
    logf((char*)"Constructeur Capture(%s) -----------", (char*)f.c_str() );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Capture::~Capture()
{
    logf((char*)"Destructeur Capture() -----------" );
    log_tab(true);

	WindowsManager::getInstance().sup( this );
	
    sup(pTitre);
    sup(panelPreview);
    sup(pNbStars);

	delete panelPreview;
	delete pTitre;
	delete pNbStars;
	
	filenames.clear();

    log_tab(false);
    logf((char*)"Destructeur Capture() -----------" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::pooling()
{
    struct dirent *lecture;
    DIR *rep;
    string dirname = getCurrentDirectory();
    rep = opendir( dirname.c_str() );
    if ( rep == NULL )
    {
        logf( (char*)"[Erreur] Impossible de charger le repertoire : " );
        logf( (char*)"[Erreur] %s", (char*)dirname.c_str() );
    }
    
    filenames.clear();
    
    
    while ((lecture = readdir(rep))) {

        string s;
        int found;
        s = lecture->d_name;
        
        if (        s.find(".png") != string::npos
               ||   s.find(".fits")!= string::npos
               ||   s.find(".jpg") != string::npos
               ||   s.find(".tga") != string::npos
               ||   s.find(".raw") != string::npos)
        {
            string  f =  dirname + s;
            //logf( (char*)"%s", (char*)f.c_str() );
            filenames.push_back( f );
        }
        
    }

    sort( filenames.begin(), filenames.end(), fct_tri_capture );

    closedir(rep);    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::update()
{
    panelPreview->setRB( &readBgr );
    //panelPreview->updatePos();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::updatePos()
{
    PanelSimple::updatePos();

    int dx = getDX();

    pFermer->setPos(    dx - 20*3, 2);
    pMaximiser->setPos( dx - 20*2, 2);
    pIconiser->setPos(  dx - 20*1, 2);

    //pFermer->updatePos();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::clickLeft(int xm, int ym)
{
    logf( (char*)"Capture::clickLeft ..." );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::releaseLeft(int xm, int ym)
{
    logf( (char*)"Capture::releaseLeft ..." );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::create_icones()
{
    pFermer = new PanelSimple();
    pFermer->setBackground((char*)"images/fermer.tga");
    pFermer->setSize( 16, -14);
    pFermer->setPos(10+20*1, 2);
    add(pFermer);
    //pFermer->setPanelClickLeft( this );
    pFermer->setExtraString( "pFermer ..." );

    pMaximiser = new PanelSimple();
    pMaximiser->setBackground((char*)"images/maximiser.tga");
    pMaximiser->setSize( 16, 14);
    pMaximiser->setPos(10+20*2, 2);
    add(pMaximiser);
    pMaximiser->setExtraString( "pMaximiser ..." );

    pIconiser = new PanelSimple();
    pIconiser->setBackground((char*)"images/iconiser.tga");
    pIconiser->setSize( 16, 14);
    pIconiser->setPos(10+20*3, 2);
    add(pIconiser);
    pIconiser->setExtraString( "pIconiser ..." );

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::create_preview()	{
    logf((char*)"Capture::CreatePreview -------------" );
    log_tab(true);
    
    create_icones();
    logf((char*)"fichier = %s", (char*)filename.c_str() );
    
	WindowsManager& wm = WindowsManager::getInstance();

	//panelPreview = new PanelSimple();
	//panelPreview = new PanelWindow();
    setDisplayGL(displayGLnuit_cb);

    bFits = false;
    if ( filename.find( ".fits" ) != std::string::npos )
    {
        logf((char*)"Fichier fits %s", (char*)filename.c_str() );
        fits = new Fits(filename);
        fits->getRB(&readBgr);
        bFits = true;
    }
    else
    {
        readBgr.ptr = WindowsManager::OpenImage( (const std::string)filename, readBgr.w, readBgr.h, readBgr.d );
    }

	panelPreview = new PanelCapture(NULL, this);
	if ( readBgr.ptr == NULL )
	{
	    logf( (char*)"[Erreur]Pointeur sur background readBgr.ptr == NULL");
        panelPreview->setRB( NULL );
    }
    else
    {
	    logf( (char*)"setBackground( ..., %d, %d, %d)", readBgr.w, readBgr.h, readBgr.d);
        panelPreview->setBackground( readBgr.ptr, readBgr.w, readBgr.h, readBgr.d);
        panelPreview->setRB( &readBgr );
    }
    //panelPreview->findAllStar();

    add(panelPreview);

    //create_icones();

    resize( getWidth(), getHeight() );

    char * pS = (char*)filename.c_str();
    char * filenameShort = NULL;
    int nb = filename.size();
    
    for( int i=nb; i>0; i-- )
    {
        int j = i-1;
        if ( pS[j]=='/' )       { filenameShort = pS+j+1; break; }
    }

	//pTitre = new PanelText( (char*)filename.c_str(),		PanelText::LARGE_FONT, 20, 10 );
	pTitre = new PanelText( (char*)filenameShort,		PanelText::LARGE_FONT, 20, 10 );
	add( pTitre );
	
	pNbStars = new PanelText( (char*)"0",		PanelText::LARGE_FONT, getWidth()-50, 10 );
	panelPreview->add( pNbStars );
	panelPreview->getStars()->setPanelNbStars( pNbStars );
	
 	wm.add( this );


    


    log_tab(false);
    logf((char*)"Capture::CreatePreview -------------" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::resize(int w, int h )
{
    int x, y, dx, dy;

    dx = w ;
    dy = h ;

    if ( filename.length() !=  0 )
    {
	
        float ratioX = (float)dx / (float)readBgr.w;
        float ratioY = (float)dy / (float)readBgr.h;
        if  ( ratioX < ratioY ) 
        {
            dx = readBgr.w * ratioX;
            dy = readBgr.h * ratioX;
        }
        else
        {
            dx = readBgr.w * ratioY;
            dy = readBgr.h * ratioY;
        }
    }
    else
    {
        dx = 800;
        dy = 30;
        filename = "pas de fichier dans " + getCurrentDirectory();
    }
    x = (getWidth()-dx)/2;
    y = (getHeight()-dy)/2;
    
    setPosAndSize( x, y, dx, dy );
    //pMaximiser->setPos(dx-32*2, y-12);

    panelPreview->setPosAndSize( 0, 0, dx, dy );
    //panelPreview->setEchelle( (float)readBgr.w / (float)dx );
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::resize(int x, int y, int w, int h )
{
    int dx, dy;

    dx = w;
    dy = h;

    if ( filename.length() !=  0 )
    {
        float ratioX = (float)dx / (float)readBgr.w;
        float ratioY = (float)dy / (float)readBgr.h;
        if  ( ratioX < ratioY ) 
        {
            dx = (float)readBgr.w * ratioX;
            dy = (float)readBgr.h * ratioX;

            y += (h-dy)/2;
            //dx = readBgr.w * ratioX;
            //dy = readBgr.h * ratioX;
        }
        else
        {
            dx = (float)readBgr.w * ratioY;
            dy = (float)readBgr.h * ratioY;

            x += (w-dx)/2;
        }
    }
    else
    {
        dx = 800;
        dy = 30;
        filename = "pas de fichier dans " + getCurrentDirectory();
    }
    
    setPosAndSize( x, y, dx, dy );
    //pMaximiser->setPos( dx-(32*2), y-12 );
    panelPreview->setPosAndSize( 0, 0, dx, dy );
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::fullscreen()
{
    int dx = WindowsManager::getInstance().getWidth();
    int dy = WindowsManager::getInstance().getHeight();
    logf((char*)"Capture::fullscreen()  dx=%d dy=%d", dx, dy);

    //resize( 0, 0, dx, dy);
    setPosAndSize(0, 0, dx, dy);
    bIcone = false;
    bFullScreen = true;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::onTop()
{
    WindowsManager::getInstance().onTop( this );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::addStar( int x, int y )
{
    panelPreview->addStar(x, y);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::show()
{
    setVisible( true );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::hide()
{
    setVisible( false );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::setColor(long c)
{
    pTitre->setColor( c);
    PanelWindow::setColor( c);
    panelPreview->setColor( c);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::afficheFits()
{
    if (fits)
    {
        fits->afficheDic();
        fits->afficheDatas();
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

