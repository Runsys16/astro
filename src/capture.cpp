#include "capture.h"
#include "captures.h"
#include "notification_capture.h"
//--------------------------------------------------------------------------------------------------------------------
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
    
    logf((char*)"Lit repertoire old_dir : %s", (char*)old_dir.c_str() );
    
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
    for( int i=0; i<nb-1; i++ )        dirname += "/" + res[i];
    dirname = dirname + "/";

    basename = res[nb-1];
    
	charge( dirname, basename );
	if ( bNuit ) 	setColor( VCF4_2_COLOR32(cRouge) );

    log_tab(false);
    logf((char*)"Constructeur Capture() ----END----" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Capture::Capture(string dir_name, string base_name)
{
    logf((char*)"Constructeur Capture(%s, %s) -----------%d", (char*)dir_name.c_str(), (char*)base_name.c_str(), __LINE__ );
    log_tab(true);
    charge( dir_name, base_name );
	if ( bNuit ) 	setColor( VCF4_2_COLOR32(cRouge) );
    log_tab(false);
    logf((char*)"Constructeur Capture(%s, %s) -----END------", (char*)dirname.c_str(), (char*)basename.c_str() );
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
    
    for( int i=0; i<nb-1; i++ )        dirname += "/" + res[i];
    dirname = dirname + "/";
    basename = res[nb-1];

    old_dir = string( dirname );

    logf( (char*)"basename : %s", basename.c_str() );
    setExtraString( "Capture :"+basename );

	charge( dirname, basename );

	if ( bNuit ) 	setColor( VCF4_2_COLOR32(cRouge) );
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

	WindowsManager& wm = WindowsManager::getInstance()	;

    sup(pTitre);
    sup(panelCapture);
    sup(pNbStars);

    if ( isFits() )     delete fits;

    panelCapture->sup(pFermer);
    panelCapture->sup(pIconiser);
    panelCapture->sup(pMaximiser);

    sup( panelCapture );


    logf((char*)"delete icons de fenetre" );
	delete		pFermer;
	delete		pIconiser;
	delete		pMaximiser;

    logf((char*)"delete le titre" );
	delete pTitre;
    logf((char*)"delete pNbStars" );
	delete pNbStars;
    logf((char*)"delete pNbVizier" );
	delete pNbVizier;
    logf((char*)"delete le panelCapture" );
	delete panelCapture;
	
	filenames.clear();
    logf((char*)"Reste %d fenetre", getNbPanel() );

    
	wm.sup( this );

    logf((char*)"delete pInfoGraph" );
	if ( pInfoGraph )	{
		if ( pGraph) 	pGraph->sup( pInfoGraph );
		delete pInfoGraph;
	}	
	
    logf((char*)"delete pGraph" );
	if ( pGraph )	{
		wm.sup( pGraph );
		delete pGraph;
	}
	
	delete_vars();

	if ( pFindStar )		delete pFindStar;

    log_tab(false);
    logf((char*)"Destructeur Capture::~Capture() -----END------" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::init()
{
	//-----------------------------------
	// Valeur d par defaut des variables
	//-----------------------------------
	bAfficheInfoFits	= false;
	bAfficheGraph		= false;
    bIconized			= false;
    bFullScreen			= false;

	pGraph				= NULL;
	pInfoGraph			= NULL;
	pFindStar			= NULL;

	sPosSvg.X			= -1;

	sPosGraph.X			= 10;
	sPosGraph.Y			= 10;
	sPosGraph.DX		= width/2 -40;
	sPosGraph.DY		= height/2 -40;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::charge(string dir_name, string base_name)
{
    logf((char*)"Capture::charge(%s, %s) -----------%d", (char*)dir_name.c_str(), (char*)base_name.c_str(), __LINE__ );
    log_tab(true);
    
    dirname		= dir_name;
    basename	= base_name;
    filename	= dirname + basename;
    old_dir		= string( dirname );

    logf( (char*)"image : %s", filename.c_str() );
    setExtraString( "Capture :"+ basename );

	init();
    create_preview();

    log_tab(false);
    logf((char*)"Capture::charge(%s, %s) -----END------", (char*)dirname.c_str(), (char*)basename.c_str() );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::charge_vars_findstar()
{
    logf((char*)"Capture::charge_findstar() -- %d", __LINE__ );
    log_tab(true);

	VarManager& var = VarManager::getInstance();
	bool bSauveVar = var.getSauve();
	var.stopSauve();
	
	int i = 0;
	char st[255];

	if ( pFindStar == NULL )	create_find_star();
	pFindStar->setName( getBasename() );

	logf( (char*)"Ech %0.2lf Ech_user %0.2lf Ech_geo %0.2lf", panelCapture->getEch(), panelCapture->getEch_user(), panelCapture->getEch_geo() );
	pFindStar->setConvert( panelCapture );
	
	snprintf( (char*)st, sizeof(st), (char*)"GRPH_Dis_%03d_li", ID );
	if ( var.existe( string(st) ) )
	{

																				int l  = var.geti( string(st) );
		snprintf( (char*)st, sizeof(st), (char*)"GRPH_Dis_%03d_X", ID );		int x  = var.geti( string(st) );
		snprintf( (char*)st, sizeof(st), (char*)"GRPH_Dis_%03d_Y", ID );		int y  = var.geti( string(st) );
		snprintf( (char*)st, sizeof(st), (char*)"GRPH_Dis_%03d_DX", ID );		int dx = var.geti( string(st) );
		snprintf( (char*)st, sizeof(st), (char*)"GRPH_Dis_%03d_DY", ID );		int dy = var.geti( string(st) );

		logf( (char*)"GRPH_Dis %d (%d, %d, %d, %d)  ligne=%d", ID, x, y, dx, dy, l );

		pFindStar->create_graph_distri( this, l, x, y, dx, dy );
	}
	


	while( true )
	{
		snprintf( (char*)st, sizeof(st), (char*)"GRPH_Lum_%03d-%03d_li", ID, i );
		
		if ( var.existe( string(st) ) )
		{
																						int l  = var.geti( string(st) );
			snprintf( (char*)st, sizeof(st), (char*)"GRPH_Lum_%03d-%03d_X", ID, i );	int x  = var.geti( string(st) );
			snprintf( (char*)st, sizeof(st), (char*)"GRPH_Lum_%03d-%03d_Y", ID, i );	int y  = var.geti( string(st) );
			snprintf( (char*)st, sizeof(st), (char*)"GRPH_Lum_%03d-%03d_DX", ID, i );	int dx = var.geti( string(st) );
			snprintf( (char*)st, sizeof(st), (char*)"GRPH_Lum_%03d-%03d_DY", ID, i );	int dy = var.geti( string(st) );
			
			logf( (char*)"GRPH_Lum_%d-%d (%d, %d, %d, %d)  ligne=%d", ID, i, x, y, dx, dy, l );
			
			pFindStar->create_graph_lum( this, l, x, y, dx, dy );
		}
		else
			break;

		i++;
		if ( i >= 10 )	break;
	}

	var.setSauve( bSauveVar );

    log_tab(false);
    logf((char*)"Capture::charge_findstar() -----END------" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::charge_vars_graph()
{
	VarManager& var = VarManager::getInstance();
	char st[255];
	int ID = getInfo();

	snprintf( (char*)st, sizeof(st), (char*)"GRPH_Str_%03d_X", ID );
	if ( var.existe( string(st) ) )
	{
		logf( (char*)"%s existe", st );
		
		snprintf( (char*)st, sizeof(st), (char*)"GRPH_Str_%03d_X", ID );  sPosGraph.X  = var.geti(string(st));
		snprintf( (char*)st, sizeof(st), (char*)"GRPH_Str_%03d_Y", ID );  sPosGraph.Y  = var.geti(string(st));
		snprintf( (char*)st, sizeof(st), (char*)"GRPH_Str_%03d_DX", ID ); sPosGraph.DX = var.geti(string(st));
		snprintf( (char*)st, sizeof(st), (char*)"GRPH_Str_%03d_DY", ID ); sPosGraph.DY = var.geti(string(st));
		

		logf_thread( (char*)"charge_grpah %d %d %d %d", sPosGraph.X, sPosGraph.Y,sPosGraph.DX, sPosGraph.DY );

		if ( pGraph )	pGraph->setPosition( sPosGraph );
		
	}
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
    //logf( (char*)"Capture::updatePos() ... %s", basename.c_str() );
	if ( pInfoGraph )	pInfoGraph->setPos( 50, pGraph->getPosDY() - 110 );

    Panel::updatePos();
    updatePosIcones();
    
    if ( bIconized )	
    {
    	pNbStars->setVisible( false);
    	pNbVizier->setVisible( false);
    }
    else
    {
    	if ( panelCapture->getStars() && panelCapture->getStars()->size() != 0 )    pNbStars->setVisible( true );
    	else																    	pNbStars->setVisible( false );
    	if ( panelCapture->getCatalog() && panelCapture->getCatalog()->size() != 0 )    pNbVizier->setVisible( true );
    	else																    		pNbVizier->setVisible( false );

    }
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
    on_top();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::releaseLeft(int xm, int ym)
{
	bTraiteReleaseLeft = true;
	logf( (char*)"Capture::releaseLeft( %d, %d)", xm, ym );
	log_tab(true);

	if ( pFermer == pFermer->isMouseOver(xm, ym) )
	{
		logf( (char*)"Capture::releaseLeft() Fermeture" );
		Captures::getInstance().setCurrent(this);
		Captures::getInstance().supprime();
		log_tab(false);
		return;		// evite le on_top => CRASH fenetre disoparu
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

		int X, Y;
		Captures& cap = Captures::getInstance();

		cap.compute_pos_icone( X, Y, cap.get_n_capture(this) );
		iconize( X, Y, cap.getDXIcon(), cap.getDYIcon());
		//cap.resize_icone( this, X, Y, cap.getDXIcon(), cap.getDYIcon() );
	}
	else
	if ( bIconized )
	{
		logf( (char*)"Iconized => traitement impossible" );
	    bTraiteReleaseLeft = false;
	}
	else
	//--------------------------------------------
	// ctrl + shift + alt
	// recherche all
    if ( iGlutModifier == (GLUT_ACTIVE_CTRL+GLUT_ACTIVE_ALT+GLUT_ACTIVE_SHIFT))    {
		logf( (char*)"CTRL+SHIFT+ALT " );
		if ( pFindStar == NULL )	create_find_star();
		
		pFindStar->setName( getBasename() );
		pFindStar->click_all();
    }
    else
	//--------------------------------------------
	// shift + alt
	// recherche
    if ( iGlutModifier == (GLUT_ACTIVE_ALT+GLUT_ACTIVE_SHIFT))    {
		logf( (char*)"SHIFT+ALT " );
		if ( pFindStar == NULL )	create_find_star();
		
		vec2 vTex = vec2( xm, ym );
		panelCapture->screen_2_tex( vTex );

		pFindStar->setName( getBasename() );
		pFindStar->click_find_star(vTex);
    }
    else
	//--------------------------------------------
	// ctrl + shift
	// distribution ligne
    if ( iGlutModifier == (GLUT_ACTIVE_CTRL+GLUT_ACTIVE_SHIFT))    {
		if ( pFindStar == NULL )	create_find_star();
		
		vec2 vTex = vec2( xm, ym );
		vec2 vJ2000;
		panelCapture->screen_2_tex( vTex );
		logf( (char*)"CTRL+SHIFT " VEC2_PRINTFN(0), VEC2_AFF(vTex) );

		pFindStar->setName( getBasename() );
		pFindStar->click_graph_distri(vTex);
    }
    else
	//--------------------------------------------
	// ctrl + alt
	// recherche ligne
    if ( iGlutModifier == (GLUT_ACTIVE_CTRL+GLUT_ACTIVE_ALT))    {
		vec2 vTex = vec2( xm, ym );
		panelCapture->screen_2_tex( vTex );
		
		logf( (char*)"CTRL+ALT " VEC2_PRINTFN(0), VEC2_AFF(vTex) );
		if ( pFindStar == NULL )	create_find_star();
		
		pFindStar->setName( getBasename() );
		pFindStar->click_graph_lum( vTex );
    }
    else
	    bTraiteReleaseLeft = false;

	on_top();
	log_tab(false);
	logf( (char*)"Capture::releaseLeft(...)   ------END---------" );
}
//--------------------------------------------------------------------------------------------------------------------
//
// Notification de pGraph lorsque la souris pointe sur une etoile
// void *p  :  p pointe sur l'indice de l'etoile
// 
//--------------------------------------------------------------------------------------------------------------------
void Capture::callback( void* p )
{
	Stars*			pStars	= panelCapture->getStars();
	StarCompare&	sc		= panelCapture->getStarCompare();
	vector<ivec2>&	cmp		= sc.getCmpViziStar();
	
	if ( pStars == NULL || pStars->size() == 0 )		return;

	for( int i=0; i<pStars->size(); i++ )	pStars->get(i)->setGraph(false);
	
	int idx = *(int*)p;
	int iii = cmp[idx].y;

	pStars->get(iii)->setGraph(true);
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


	//loadSkinPath( "images/astro" );
    //loadSkin( PanelWindow::RED );
    loadSkin( PanelWindow::BLACK );
	setBorderSize(2);

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

	int s = filename.find( ".fits" );
	if ( s == filename.size()-5 )		        bFits = true;

	logf( (char*)"Ouverture %d/%d", s, filename.size() );

	s = filename.find( ".fit" );
	if ( s == filename.size()-4 )		        bFits = true;

	panelCapture = new PanelCapture(NULL, this);

	if ( bFits )
	{
		//loadSkinPath( "images/astro" );
		loadSkin( "rouge" );
	    loadSkin( PanelWindow::BLACK );
		setBorderSize(4);

		logf((char*)"Fichier fits %s", (char*)filename.c_str() );
		fits = new Fits(filename, panelCapture );
		fits->getPanelFits()->setExtraString( "PanelFit : "+ basename );

		log((char*)"Chargement fichier" );

		log_tab(true);
		fits->chargeFits();
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
	// Titre de la fenetre (nom de fichier)
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
    logf((char*)"Capture::resize( %d,%d )   (%d)", w, h, __LINE__ );
    
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
    logf((char*)"Capture::resize( %d, %d, %d, %d )   (%d)", x, y, w, h, __LINE__ );
    log_tab(true);

    int dx, dy;

    dx = w;
    dy = h;

    if ( filename.length() !=  0 )
    {
        double r_image = (double)readBgr.w / (double)readBgr.h;
        double r_place = (double)w / (double)h;
        logf( (char*)"ratio   img = %0.4lf ecr = %0.4lf", r_image, r_place );

        if  ( r_image > r_place ) 
        {
            dx = w;
            dy = dx / r_image;

            x += (w-dx)/2;
        }
        else
        {
            dy = h;
            dx = dy * r_image;

            y += (h-dy)/2;
        }
    }
    else
    {
        dx = 800;
        dy = 30;
        filename = "pas de fichier dans " + getCurrentDirectory();
    }

	logf( (char*)" (%d, %d)  %dx%d", x, y, dx, dy );
    setPosAndSize( x, y, dx, dy );
    //panelCapture->setPosAndSize( 0, 0, dx, dy );
    panelCapture->setSize( dx, dy );
    //sPosSvg.X = -1;
    log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::fullscreen()
{
	logf( (char*)"Capture::fullscreen() %s", basename.c_str() );
    int dx = WindowsManager::getInstance().getWidth();
    int dy = WindowsManager::getInstance().getHeight();

	if ( !bIconized )		getPosition( sPosSvg );
	else					sPosSvg.X  = -1;

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
void Capture::iconize( int xIcon, int yIcon, int dxIcon, int dyIcon)
{
	if ( bIconized )	return;
	
	logf( (char*)"Capture::iconize(%d, %d, %d, %d) %s", xIcon, yIcon, dxIcon, dyIcon, basename.c_str() );
	log_tab(true);

	//----------------------------------------------
	// Sauvegarde la position
	if ( !bFullScreen )		getPosition( sPosSvg );
	else					sPosSvg.X  = -1;

	//----------------------------------------------
    VarManager& 	var	= VarManager::getInstance();
	if ( var.getb("bShowIcones")	)			setVisible( true );
	else										setVisible( false );

	bIconized = true;
	bFullScreen = false;

	int w = panelCapture->getDX();
	int	h = panelCapture->getDY();
	double r0 = (double)w/(double)h;
	double r1 = (double)dxIcon/(double)dyIcon;

	if ( r0 > r1 )	
	{
		yIcon += (dyIcon -(dxIcon / r0)) / 2;
		dyIcon = dxIcon / r0;
	}
	else
	{
		xIcon += (dxIcon -(dyIcon * r0)) / 2;
		dxIcon = dyIcon * r0;
	}

	setPosAndSize( xIcon, yIcon, dxIcon, dyIcon );
	
	if ( bFits )	{
		fits->afficheInfoFits(false);
	}
	panelCapture->iconize();

	if ( pGraph && bAffGraph )	{ pGraph->setVisible(false); pInfoGraph->setVisible(false); }

	if ( pFindStar )
	{
		pFindStar->on_top(true);
		pFindStar->setParentIconized(true);
	}

	log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::iconize()
{
	//if ( bIconized )	return;
	
	logf( (char*)"Capture::iconize()" );
	log_tab(true);

	//----------------------------------------------
	panelCapture->iconize();

	if ( pGraph && bAffGraph )	{ pGraph->setVisible(false); pInfoGraph->setVisible(false); }

	if ( pFindStar )
	{
		pFindStar->on_top(true);
		//pFindStar->setParentIconized(true);
	}

	log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::restaure()
{
	logf( (char*)"Capture::restaure() %s", basename.c_str() );
	log_tab(true);
	bIconized 				= false;

	if ( sPosSvg.X != -1 )	
	{
		setPosAndSize( sPosSvg.X, sPosSvg.Y, sPosSvg.DX, sPosSvg.DY );
		logf( (char*)"(%d, %d)  %dx%d", getPosX(), getPosY(), getPosDX(), getPosDY() );
	}
	
	sPosSvg.X  = -1;

	if ( bFits )	{
		if ( bAfficheInfoFits )		fits->afficheInfoFits(true);
		on_top();
		
	}
	graph_on_top();


	log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::on_top()
{
	//logf( (char*)"Capture::on_top() %s", basename.c_str() );
    WindowsManager& wm	= WindowsManager::getInstance();
    VarManager& 	var	= VarManager::getInstance();

    wm.onTop( this );
    if ( isFits() )	{
		wm.onTop( fits->getPanelFits() );
		if ( pGraph )
		{
			if ( pGraph->getVisible() )
			{
				//logf( (char*)"|  Active pGraph" );
				WindowsManager& wm = WindowsManager::getInstance();
				wm.onTop( pGraph );
			}
		}
		
		if ( pFindStar )		pFindStar->on_top();
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
	if ( pGraph && bAffGraph && !bIconized )	{ pGraph->setVisible(true); pInfoGraph->setVisible(true); }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::hide()
{
    setVisible( false );
	if ( pGraph && bAffGraph && !bIconized )	{ pGraph->setVisible(false); pInfoGraph->setVisible(false); }
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
    if ( pInfoGraph )	pInfoGraph->setColor( c);
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
void Capture::createGraph()
{
	logf( (char*)"Capture::createGraph()" );
	if ( pGraph != NULL )		return;
	//---------------------------
	log_tab(true);
	WindowsManager& wm	= WindowsManager::getInstance();

	pGraph = new PanelGraph();
	pGraph->setPosition( sPosGraph );
	logf_thread( (char*)"create_graph %d %d %d %d", sPosGraph.X, sPosGraph.Y,sPosGraph.DX, sPosGraph.DY );
	
	pGraph->setButtonCallback( this );
	pGraph->setPanelCallback( this );
	pGraph->setNotification( this );
	pGraph->setID( CB_BUTTON_STAR  + pGraph->get_new_ID() );
	wm.add( pGraph );

	//---------------------------
	create_info_graph();
	log_tab(false);
	//---------------------------
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::setAffGraph( bool b )
{
	logf( (char*)"Capture::setAffGraph(%s)", BOOL2STR(b) );
	
	bAfficheGraph = b;
	WindowsManager& wm = WindowsManager::getInstance();
	
	if ( b )
	{
		if ( pGraph == NULL )					createGraph();
		
		pGraph->setVisible( true ) ;
		bAffGraph = true;
		pGraph->debug_log();
		compareStar();
		if (pInfoGraph)		pInfoGraph->setVisible( true ) ;
		wm.onTop( pGraph );

		//panelCapture->eraseGaiaDR3();
		//panelCapture->deleteAllStars();
	} 
	else
	{
		Stars* pStars = panelCapture->getStars();
		for( int i=0; i<pStars->size(); i++ )	pStars->get(i)->setGraph(false);

		if ( pGraph )		pGraph->setVisible( false ) ;
		if (pInfoGraph)		pInfoGraph->setVisible( false ) ;
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::deleteGraph()
{
	logf( (char*)"Capture::deleteGraph()" );

	if ( pGraph == NULL )	return;
	//int             iID	= pGraph->getID();
	WindowsManager& wm			= WindowsManager::getInstance();
	
	if ( pInfoGraph )	{
		pGraph->sup( pInfoGraph );
		delete pInfoGraph;
	}	pInfoGraph = NULL;
	
	wm.sup( pGraph );
	delete pGraph;
	pGraph = NULL;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::compareStar()
{
	if ( panelCapture->getStars() == NULL ) 			return;
	if ( panelCapture->getStars()->size() == 0 )		return;
	if ( panelCapture->getCatalog() == NULL )			return;
	if ( panelCapture->getCatalog()->size() == 0 )		return;

	logf( (char*)"Capture::compareStar()" );
	log_tab(true);

	if ( pGraph == NULL )	setAffGraph(true);
	
	
	StarCompare& sc = panelCapture->getStarCompare();
	panelCapture->compareStar();
	//sc.compareStar();

	log( (char*)"Init min et max ..." );
	pGraph->setXmin( sc.getLumMin() );
	pGraph->setXmax( sc.getLumMax() );
	pGraph->setYmin( sc.getMagMin() );
	pGraph->setYmax( sc.getMagMax() );
	
	log( (char*)"Reset courbe ..." );
	pGraph->resetCourbeStar();
	pGraph->resetCourbeVizi();
	
	log( (char*)"Rempli les tableaux ..." );
	vector<Star*>&			star = sc.getStar();
	vector<StarCatalog*>&	vizi = sc.getVizi();
	vector<ivec2>&			cmp	 = sc.getCmpViziStar();
	
	int nb = cmp.size();
	
	for( int i=0; i<nb; i++ )
	{
		int idx, ii;
		idx = cmp[i].x;
		ii  = cmp[i].y;
		
		vec2 v = vec2( star[i]->getPonderation(), vizi[i]->getMag() );
		vec2 w = vec2( star[i]->getPonderation(), star[i]->getMagnitude() );

		pGraph->addViziStar( v, w );
	}
	
	log( (char*)"Tri les tableaux ..." );
	pGraph->sort_all();	
	
	pGraph->setName( basename );
	pGraph->setLinearY();
	
	update_info_graph();
	log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#define A_FACTOR 1.1
void Capture::affine_compareStar(bool b)
{
	return;
	logf( (char*)"Capture::affine_compareStar()" );
	log_tab(true);
	
	Stars* pStars = panelCapture->getStars();
	double cA = pStars->getA();
	
	if ( b )		cA /= A_FACTOR;
	else			cA *= A_FACTOR;
	
	pStars->setA(cA);
	//compareStar();
	StarCompare& sc = panelCapture->getStarCompare();
	for( int i=0; i<10; i++ )
	{
		bDesactiveLog = true;
		compareStar();
		bDesactiveLog = false;
		printf( (char*)"affine %0.6lf\n", sc.getMoyen() );
	}
	
	log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::update_info_graph()
{
	if ( pGraph == NULL )		return;
	if ( pInfoGraph == NULL )		{ log( (char*)"[ Erreur ] pInfoGraph inexistant"); create_info_graph();  }
	log( (char*)"Capture::update_info_graph()" );

	pInfoGraph->setVisible( false );
	if ( panelCapture->getStars() == NULL ) 			return;
	if ( panelCapture->getStars()->size() == 0 )		return;
	if ( panelCapture->getCatalog() == NULL )			return;
	if ( panelCapture->getCatalog()->size() == 0 )		return;

	int nbGaia = panelCapture->getCatalog()->size();
	int nbStar = panelCapture->getStars()->size();
	int nbCorr = panelCapture->getStarCompare().getCmpViziStar().size();
	
	//panelCapture->getStarCompare().compute_moyenne();
	//panelCapture->getStarCompare().compute_ecart_type();
	double dMoyen = panelCapture->getStarCompare().getMoyen();
	double dEcart = panelCapture->getStarCompare().getEcart();

	pInfoGraph->reset_list();
	if ( pGraph )	pInfoGraph->setVisible( pGraph->getVisible() );
	
	pInfoGraph->add_textf( (char*)"%d etoiles GAIA dr3", nbGaia );
	pInfoGraph->add_textf( (char*)"%d etoiles trouvées", nbStar );
	pInfoGraph->add_textf( (char*)"%d en correspondances", nbCorr );
	pInfoGraph->add_textf( (char*)"%0.2lf mag diff moyen", dMoyen );
	pInfoGraph->add_textf( (char*)"%0.2lf mag ecart type", dEcart );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::create_info_graph()
{
	if ( pInfoGraph != NULL )		{ log( (char*)"[ Erreur ] pInfoGraph existant"); return; }

	pInfoGraph = new PanelDebug();
	pInfoGraph->setExtraString("PanelDebug statistique etoiles");
	pInfoGraph->setBorderSize(0);
	if (pGraph)	pInfoGraph->setVisible(pGraph->getVisible());
	pInfoGraph->setPos( 50, pGraph->getPosDY() - 110 );
	pInfoGraph->setBackground( (_Texture2D*)NULL );
	//pInfoGraph->setID( CB_BUTTON_STAR  + pInfoGraph->get_new_ID() );
	pInfoGraph->setTabSize( 60 );

	pGraph->add( pInfoGraph );
	pInfoGraph->setSize( 180, 50 );
	
	update_info_graph();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::create_graph()
{
	logf( (char*)"Capture::create_graph()" );
	
	int nbGaia = 0;
	int nbStar = panelCapture->getStars()->size();	
	
	if ( panelCapture->getCatalog() != NULL )
	{
		nbGaia = panelCapture->getCatalog()->size();
	}

	logf( (char*)"Nb Gaia = %d, nb Star = %d", nbGaia, nbStar );

	
	if ( 		( (panelCapture->getStars() == NULL ) || ( panelCapture->getStars()->size() == 0 ) )
		&&		( (panelCapture->getCatalog() == NULL ) || ( panelCapture->getCatalog()->size() == 0 ) )
		)
	{
		log_tab(true);
		VarManager&	var				= VarManager::getInstance();
		bool 		bAffCatalogSvg	= bAffCatalog;
		bool		bAffStarSvg		= bAffStar;	

		bAffStar	= true;
		bAffCatalog = true;

		panelCapture->findGaiaDR3();
		panelCapture->findAllStars();
		
		compareStar();
		
		bAffStar	= bAffStarSvg;
		bAffCatalog = bAffCatalogSvg;

		if ( pInfoGraph == NULL )		create_info_graph();	
		
		if ( nbGaia == 0 )		panelCapture->eraseGaiaDR3();
		if ( nbStar == 0 )		panelCapture->deleteAllStars();

		log_tab(false);
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::graph_on_top()
{
	//log( (char*)"Capture::graph_on_top()" );
	if ( bIconized )
	{
		if ( pGraph )		pGraph->setVisible(false);
		if ( pFindStar )	pFindStar->setVisible(false);
	}
	else
	{	
		WindowsManager& wm	= WindowsManager::getInstance();
		if ( pGraph && bAfficheGraph )
		{
			pGraph->setVisible(true);
			pInfoGraph->setVisible(true);
			wm.sup( pGraph );
			wm.add( pGraph );
		}

		if ( pFindStar )	
		{
			if ( !bIconized )	{	pFindStar->on_top( false );	}
			else
			{
				if ( !bAffIconeCapture )			pFindStar->on_top( true );
			}
		}

		if ( bFits )
		{
			PanelFits*	pPanelFits = fits->getPanelFits();
			wm.sup( pPanelFits );
			wm.add( pPanelFits );
			
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//  Creation de l'objet unique FindStar
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::create_find_star()                         
{
	log( (char*)"Capture::create_find_star()" );
	if ( pFindStar )		return;
	
	log_tab(true);

	pFindStar = new FindStar();						// Nouveau FindStar
	
	pFindStar->setRB( panelCapture->getRB() );		// Transmission adresse buffer image
	pFindStar->setView( this );
	pFindStar->setConvert( panelCapture );			// Objet convertissant des coordonnees
	pFindStar->setNotificationCapture( this );		// Pour les notificaitons de FindStar

	log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
// Notificaiton de l'appui sur le bouton fermer d'un graphique FindStar
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::cb_button_mouse_up(PanelButton* panelButton)
{
	int             iID	= panelButton->getParent()->getID();
	logf( (char*)"Callback Button UP   ID = %04X", iID );

	if ( iID >= CB_BUTTON_STAR )
	{
		deleteGraph();
	}	
}
//--------------------------------------------------------------------------------------------------------------------
//
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::setGraphPosAndSize( int x, int y, int dx, int dy )
{
	logf( (char*)"Capture::setGraphPosAndSize()" );

	if ( pGraph )	pGraph->setPosAndSize( x, y, dx, dy );
}
//--------------------------------------------------------------------------------------------------------------------
//
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::save_all_vars_graph( int ID )
{
	logf( (char*)"Capture::save_all_vars_graph( %d )", ID );
	VarManager& var = VarManager::getInstance();

	var.stopSauve();

	if ( pGraph )		save_vars_graph_starCompare( ID );
	if ( pFindStar )	pFindStar->save_vars( ID );

	var.startSauve();
	var.sauve();
}
//--------------------------------------------------------------------------------------------------------------------
//
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::save_vars_graph_starCompare( int ID)
{
	logf( (char*)"Capture::save_vars_graph_starCompare( %d )", ID );
	VarManager& var = VarManager::getInstance();

	var.stopSauve();

	char st[255];

	pGraph->getPosition( sPosGraph );

	snprintf( (char*)st, sizeof(st), (char*)"GRPH_Str_%03d_X", ID );	var.set( string(st), sPosGraph.X );
	snprintf( (char*)st, sizeof(st), (char*)"GRPH_Str_%03d_Y", ID );	var.set( string(st), sPosGraph.Y );
	snprintf( (char*)st, sizeof(st), (char*)"GRPH_Str_%03d_DX", ID );	var.set( string(st), sPosGraph.DX );
	snprintf( (char*)st, sizeof(st), (char*)"GRPH_Str_%03d_DY", ID );	var.set( string(st), sPosGraph.DY );
	
	var.startSauve();
	var.sauve();
}
//--------------------------------------------------------------------------------------------------------------------
//
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::save_vars_graph_starCompare()
{
	save_vars_graph_starCompare( getInfo() );

}
//--------------------------------------------------------------------------------------------------------------------
//
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::delete_vars()
{
	logf( (char*)"Capture::delete_vars()" );
	if ( pGraph == NULL )		return;

	VarManager& var = VarManager::getInstance();
	var.stopSauve();

	char st[255];
	int ID = getInfo();

	//snprintf( (char*)st, sizeof(st), (char*)"GRPH_Str_%03d_li", ID );
	//var.set( string(st), iGraphDistriLigne );
	pGraph->getPosition( sPosGraph );

	snprintf( (char*)st, sizeof(st), (char*)"GRPH_Str_%03d_X", ID );	var.erase( string(st) );
	snprintf( (char*)st, sizeof(st), (char*)"GRPH_Str_%03d_Y", ID );	var.erase( string(st) );
	snprintf( (char*)st, sizeof(st), (char*)"GRPH_Str_%03d_DX", ID );	var.erase( string(st) );
	snprintf( (char*)st, sizeof(st), (char*)"GRPH_Str_%03d_DY", ID );	var.erase( string(st) );
	
	var.startSauve();
	var.sauve();
}
//--------------------------------------------------------------------------------------------------------------------
//
// notification de pGraph (starCompare )
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::notifie( unsigned key, void* p )
{
	//logf( (char*)"ID=%d Notification SAVE_VARS", key );
	switch(key)
	{
	case CHANGE_X:
	case CHANGE_Y:
	case CHANGE_DX:
	case CHANGE_DY:
		logf( (char*)"ID=%d Notification SAVE_VARS", key );
		save_vars_graph_starCompare();
		break;
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
// Affiche des informations de l'objet 
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::printObjet()
{
	if( pFindStar )		pFindStar->printObjet();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

