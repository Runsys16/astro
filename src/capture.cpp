#include "capture.h"
#include "captures.h"



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
    logf((char*)"Constructeur Capture() -----------%d", __LINE__ );
    log_tab(true);
    
    logf((char*)"old_dir : %s", (char*)old_dir.c_str() );
    
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

    vector<string> res = split(filename,"/");
    dirname = "";
    
    int nb = res.size();
    for( int i=0; i<nb-1; i++ )
    {
        dirname += "/" + res[i];
    }
    basename = res[nb-1];
    
    bIconized = false;
    bFullScreen = false;

    setExtraString( "Capture" );
	bAfficheInfoFits = false;

    create_preview();

    log_tab(false);
    logf((char*)"Constructeur Capture() ----END----" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Capture::Capture(string dirname, string name)
{
    logf((char*)"Constructeur Capture(dirname, %s) -----------%d", (char*)name.c_str(), __LINE__ );
    log_tab(true);
    
    filename = dirname + name;
    dirname = dirname;
    old_dir = string( dirname );
    basename = name;
    bIconized = false;
    bFullScreen = false;

    logf( (char*)"image : %s", filename.c_str() );

    setExtraString( "Capture :"+basename );
	bAfficheInfoFits = false;

    create_preview();

    log_tab(false);
    logf((char*)"Constructeur Capture(%s, %s) -----END------", (char*)dirname.c_str(), (char*)name.c_str() );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Capture::Capture(string f )
{
    logf((char*)"Constructeur Capture(%s) -----------%d", (char*)f.c_str(), __LINE__ );
    log_tab(true);
    
    filename = f;

    vector<string> res = split(f,"/");
    dirname = "";
    
    int nb = res.size();
    for( int i=0; i<nb-1; i++ )
    {
        dirname += "/" + res[i];
    }
    
    old_dir = string( dirname + "/" );

    basename = res[nb-1];

    logf( (char*)"basename : %s", basename.c_str() );
    setExtraString( "Capture :"+basename );


    setExtraString( "Capture" );
	bAfficheInfoFits = false;

    create_preview();

    log_tab(false);
    logf((char*)"Constructeur Capture(%s) -----END------", (char*)f.c_str() );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Capture::~Capture()
{
    logf((char*)"Destructeur Capture::~Capture() -----------" );
    log_tab(true);

    sup(pTitre);
    sup(panelCapture);
    sup(pNbStars);

    panelCapture->sup(pFermer);
    panelCapture->sup(pIconiser);
    panelCapture->sup(pMaximiser);

    sup( panelCapture );

    //if ( isFits() )     delete fits;

    logf((char*)"Capture::~Capture() delete icons de fenetre" );
	delete		pFermer;
	delete		pIconiser;
	delete		pMaximiser;

    logf((char*)"Capture::~Capture() delete le titre" );
	delete pTitre;
    logf((char*)"Capture::~Capture() delete pNbStars" );
	delete pNbStars;
    logf((char*)"Capture::~Capture() delete pNbVizier" );
	delete pNbVizier;
    logf((char*)"Capture::~Capture() delete le panelCapture" );
	delete panelCapture;
	
	filenames.clear();
    logf((char*)"Destructeur Capture::~Capture() reste %d fenetre", getNbPanel() );
    
	WindowsManager::getInstance().sup( this );
	
    log_tab(false);
    logf((char*)"Destructeur Capture::~Capture() -----END------" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::pooling()
{
    struct dirent *lecture;
    DIR *rep;
    //string dirname = getCurrentDirectory();
    string dirname = old_dir;
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
    panelCapture->setRB( &readBgr );
    //panelCapture->updatePos();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::updatePos()
{
    Panel::updatePos();
    updatePosIcones();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::updatePosIcones()
{
    int dx = getDX();
    int DX = panelCapture->getPosX();
    int DY = panelCapture->getPosY();


    pFermer->setPos(    dx - 20*3 -DX, 2 -DY);
    pMaximiser->setPos( dx - 20*2 -DX, 2 -DY);
    pIconiser->setPos(  dx - 20*1 -DX, 2 -DY);
    pNbStars->setPos(	dx - 20*5 -DX, 2 -DY + 20);
    pNbVizier->setPos(	dx - 20*5 -DX, 2 -DY + 40);

	pFermer->updatePos();
	pIconiser->updatePos();
	pMaximiser->updatePos();
	pNbStars->updatePos();
	pNbVizier->updatePos();
    //logf( (char*)"Captures::update() ..." );
    return;
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

    logf( (char*)"Capture::releaseLeft( %d, %d)", xm, ym );
    log_tab(true);

    if ( pFermer == pFermer->isMouseOver(xm, ym) )
    {
        logf( (char*)"Capture::releaseLeft() Fermeture" );
        Captures::getInstance().setCurrent(this);
        Captures::getInstance().supprime();
    }
    else
    if ( pMaximiser == pMaximiser->isMouseOver(xm, ym ) )
    {
        logf( (char*)"Capture::releaseLeft() Maximiser" );
        if ( bIconized )
        {
            Captures::getInstance().setCurrent(this);
            Captures::getInstance().onTop(this);
        }
        else
        {
            Captures::getInstance().setCurrent(this);
            Captures::getInstance().fullscreen();
        }
    }
    else
    if ( pIconiser == pIconiser->isMouseOver(xm, ym ) )
    {
       logf( (char*)"Capture::releaseLeft() Iconiser" );
       iconize( getDX(), getDY());
       Captures::getInstance().rotate_capture_plus(true);
       if ( bFits )	fits->afficheInfoFits(true);
    }
	/*
    if ( bFits )	{
		if ( !bIconized && bAfficheInfoFits )			fits->afficheInfoFits(true);
		else											fits->afficheInfoFits(false);
	}
	*/

    log_tab(false);
    logf( (char*)"Capture::releaseLeft(...)   ------END---------" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::create_icones()
{
    pFermer = new PanelSimple();
    pFermer->setBackground((char*)"images/fermer.tga");
    pFermer->setSize( 16, 14);
    pFermer->setPos(10+20*1, 2);
    pFermer->setExtraString( "pFermer" );
    pFermer->setPanelReleaseLeft(this);
    
    panelCapture->add(pFermer);

    pMaximiser = new PanelSimple();
    pMaximiser->setBackground((char*)"images/maximiser.tga");
    pMaximiser->setSize( 16, 14);
    pMaximiser->setPos(10+20*2, 2);
    pMaximiser->setExtraString( "pMaximiser" );
    pMaximiser->setPanelReleaseLeft(this);

    panelCapture->add(pMaximiser);

    pIconiser = new PanelSimple();
    pIconiser->setBackground((char*)"images/iconiser.tga");
    pIconiser->setSize( 16, 14);
    pIconiser->setPos(10+20*3, 2);
    pIconiser->setExtraString( "pIconiser" );
    pIconiser->setPanelReleaseLeft(this);

    panelCapture->add(pIconiser);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::create_preview()	{
    logf((char*)"Capture::CreatePreview -------------" );
    log_tab(true);
    

	loadSkinPath( "images/astro" );
	setBorderSize(8);

    //create_icones();
    logf((char*)"fichier = %s", (char*)filename.c_str() );
    
	WindowsManager& wm = WindowsManager::getInstance();

    setDisplayGL(displayGLnuit_cb);

	//-------------------------------------------------------------------------
	// Chargement de l(image dans la structure
	// struct readBackground   (main.h)
	// ------ readBackground.ptr = pointeur sur le tableau de couleur
	//
    bFits = false;
    if ( 	filename.find( ".fits" ) != std::string::npos  
    	||	filename.find( ".fit" ) != std::string::npos
    		)
    {
        bFits = true;
	}

	panelCapture = new PanelCapture(NULL, this);
    //bFits = false;
    if ( 	filename.find( ".fits" ) != std::string::npos  
    	||	filename.find( ".fit" ) != std::string::npos
    		)
    {
        logf((char*)"Fichier fits %s", (char*)filename.c_str() );
        fits = new Fits(filename, panelCapture );

        log((char*)"Chargement fichier" );
        log_tab(true);

        fits->chargeFits();
        fits->getPanelFits()->setParent(panelCapture);
        //fits->getPanelCorrectionFits()->setParent();
        fits->getRB(&readBgr);
        log_tab(false);
    }
    else
    {
        unsigned int w, h, d;
        readBgr.ptr = WindowsManager::OpenImage( (const std::string)filename, w, h, d );
        readBgr.w = w;
        readBgr.h = h,
        readBgr.d = d;
    }
	//-------------------------------------------------------------------------
	// Gestion des Erreurs
	if ( readBgr.ptr == NULL )
	{
	    logf( (char*)"[Erreur] Pointeur sur background readBgr.ptr == NULL");
        panelCapture->setRB( NULL );
    }
    else
    {
	    logf( (char*)"setBackground( ..., %d, %d, %d)", readBgr.w.load(), readBgr.h.load(), readBgr.d.load());
        panelCapture->setBackground( readBgr.ptr.load(), readBgr.w.load(), readBgr.h.load(), readBgr.d.load());
        panelCapture->setRB( &readBgr );
    }

    add(panelCapture);
    resize( getWidth(), getHeight() );

	//-------------------------------------------------------------------------
	// recuperarion du nom de fichier seul
    char * pS = (char*)filename.c_str();
    char * filenameShort = NULL;
    int nb = filename.size();
    
    for( int i=nb; i>0; i-- )
    {
        int j = i-1;
        if ( pS[j]=='/' )       { filenameShort = pS+j+1; break; }
    }

	//-----------------------------------------------------------------------
	// Ajoute les textes d'informations
	// nombre d'etoile (star)
	// nombre d'etoile (vizier)
	// Titre de la fenetre (nm de fichier)
	pTitre = new PanelText( (char*)filenameShort,		PanelText::LARGE_FONT, 20, 10 );
	pTitre->setExtraString( "PanelText Titre" );

	add( pTitre );
	//------------------------
	pNbStars = new PanelText( (char*)"",		PanelText::LARGE_FONT, getWidth()-50, 20 );
	pNbStars->setExtraString( "PanelText NbStar" );
	panelCapture->add( pNbStars );
	panelCapture->getStars()->setPanelNbStars( pNbStars );
	
 	wm.add( this );
	//------------------------
	pNbVizier = new PanelText( (char*)"",		PanelText::LARGE_FONT, getWidth()-50, 20 );
	pNbVizier->setExtraString( "PanelText NbVizier" );

	panelCapture->add( pNbVizier );
	//------------------------
    
    create_icones();
    panelCapture->onBottom();


    log_tab(false);
    logf((char*)"Capture::CreatePreview ------END-------" );
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
	
        double ratioX = (double)dx / (double)readBgr.w;
        double ratioY = (double)dy / (double)readBgr.h;
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

    //panelCapture->setPosAndSize( 0, 0, dx, dy );
    panelCapture->setSize( dx, dy );
    //panelCapture->setEchelle( (double)readBgr.w / (double)dx );
    
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
        double ratioX = (double)dx / (double)readBgr.w;
        double ratioY = (double)dy / (double)readBgr.h;
        if  ( ratioX < ratioY ) 
        {
            dx = (double)readBgr.w * ratioX;
            dy = (double)readBgr.h * ratioX;

            y += (h-dy)/2;
            //dx = readBgr.w * ratioX;
            //dy = readBgr.h * ratioX;
        }
        else
        {
            dx = (double)readBgr.w * ratioY;
            dy = (double)readBgr.h * ratioY;

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
    //panelCapture->setPosAndSize( 0, 0, dx, dy );
    panelCapture->setSize( dx, dy );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::fullscreen()
{
	logf( (char*)"Capture::fullscreen() %s", basename.c_str() );
    int dx = WindowsManager::getInstance().getWidth();
    int dy = WindowsManager::getInstance().getHeight();
    //logf((char*)"Capture::fullscreen()  dx=%d dy=%d", dx, dy );

    //logf((char*)" preview  dx=%d dy=%d", panelCapture->getDX(), panelCapture->getDY() );
    setPosAndSize(0, 0, dx, dy);
    updatePos();
    //logf((char*)" preview  dx=%d dy=%d", panelCapture->getDX(), panelCapture->getDY() );
    
    int X=0, Y=0;
    if ( panelCapture->getDX() > dx ) {
    	X = (panelCapture->getDX() - dx ) / 2;
    }
    if ( panelCapture->getDY() > dy ) {
    	Y = (panelCapture->getDY() - dy ) / 2;
    }
	//panelCapture->setPos(-X, -Y);
    bIconized = false;
    bFullScreen = true;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::iconize( int dxIcon, int dyIcon)
{
	if ( bIconized )	return;
	logf( (char*)"Capture::iconize() %s", basename.c_str() );
	log_tab(true);

    VarManager& 	var	= VarManager::getInstance();
	if ( var.getb("bShowIcones")	)			setVisible( true );
	else										setVisible( false );

	bIconized = true;
	bFullScreen = false;

	setSize( dxIcon, dyIcon );
	
	panelCapture->iconize();


	if ( bFits )	{
		fits->getPanelFits()->setVisible(false);
		fits->getPanelCorrectionFits()->setVisible(false);
	}
	log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::restaure()
{
	logf( (char*)"Capture::restaure() %s", basename.c_str() );
	bIconized 				= false;
    /*
    bAfficheInfoFits		= bInfo;
    bAfficheInfoSouris		= bGrille;
    bAfficheGrille			= bSouris;

	if ( bFits )	{
		fits->restaure( bAfficheInfoFits );
	}
	*/
	panelCapture->restaure();

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::onTop()
{
	logf( (char*)"Capture::onTop() %s", basename.c_str() );
    WindowsManager& wm	= WindowsManager::getInstance();
    VarManager& 	var	= VarManager::getInstance();

    wm.onTop( this );
    if ( isFits() )	{
		wm.onTop( fits->getPanelFits() );
		wm.onTop( fits->getPanelCorrectionFits() );

	}	
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::addStar( int x, int y )
{
    panelCapture->addStar(x, y);
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
    //panelCapture->setColor( c);
    pFermer->setColor( c);
    pIconiser->setColor( c);
    pMaximiser->setColor( c);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::afficheFitsDic()
{
	if ( isFits() )			fits->afficheDic();
	else
	{
		logf( (char*)"Ce n'est pas une image fits" );
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::afficheFits()
{
	//return;
    if (bFits)
    {
		if ( bAfficheInfoFits && !bIconized )			fits->afficheInfoFits(true);
		else											fits->afficheInfoFits(false);
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::afficheInfoFits()
{
    if (bFits)
    {
    	bAfficheInfoFits = !bAfficheInfoFits;
        if ( !bIconized && bAfficheInfoFits )			fits->afficheInfoFits(true);
        else											fits->afficheInfoFits(false);
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::afficheInfoFits(bool b)
{
	logf( (char*)"Capture::afficheInfoFits(%s)", BOOL2STR(b) );
    if (bFits && !bIconized )
    {
        fits->afficheInfoFits( b );
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::export_stars()
{
    Stars*            pStars = panelCapture->getStars();
    if ( pStars == NULL || pStars->size() == 0 )	{
    	log( (char*)"[ERREUR] Etoiles non chargees" );
    	return;
    }
    
	string filenameSauve( "/home/rene/.astropilot/export/export_stars.csv" );
    std::ofstream fichier;
    
    fichier.open(filenameSauve, std::ios_base::out);

    if ( !fichier ) 
    {
        logf( (char*)"[ERROR]impossible d'ouvrir : '%s'", (char*)filenameSauve.c_str() );
    }

    fichier << "Asc. Droite;Déclinaison;Magnitude" << "\n";

    for(int i=0; i<pStars->size(); i++)
    {
    	Star* pStar = pStars->get(i);
        fichier << "" << pStar->getAD() << "; " <<  pStar->getDE() << "; "<<  pStar->getMagnitude() << "\n";;
    }

    fichier.close();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::export_vizier()
{
    Catalog*            pVizier = panelCapture->getCatalog();
    if ( pVizier == NULL )	{
    	log( (char*)"[ERREUR] Etoiles Vizier non chargees" );
    	return;
    }
    
	string filenameSauve( "/home/rene/.astropilot/export/export_vizier.csv" );
    std::ofstream fichier;
    
    fichier.open(filenameSauve, std::ios_base::out);

    if ( !fichier )     {
        logf( (char*)"[ERROR]impossible d'ouvrir : '%s'", (char*)filenameSauve.c_str() );
    }

    fichier << "Asc. Droite;Déclinaison;Magnitude" << "\n";

    for(int i=0; i<pVizier->size(); i++)
    {
    	StarCatalog* pStar = pVizier->get(i);
        fichier << "" << pStar->fRA << "; " <<  pStar->fDE << "; "<<  pStar->fMag << "\n";;
    }

    fichier.close();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::setNbVizier( unsigned u )
{
    char t[] = "00000000000";  
    sprintf( t, "%d", (int)u );
    pNbVizier->changeText( t );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

