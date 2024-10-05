#include "captures.h"


//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Captures::Captures()
{
    logf((char*)"Constructeur Captures() -----------" );
    log_tab( true );

	init();

    log_tab( false );
    logf((char*)"Constructeur Captures() -----FIN---- %d", mode );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::init()
{
    logf((char*)"Captures::init() -----------" );
    log_tab( true );
    current_capture = -1;


    VarManager&         var = VarManager::getInstance();
    mode =0;
    if( var.existe("bShowIcones") )         bShowIcones = var.getb( "bShowIcones" );
    if( var.existe("mode") )				mode = var.geti( "mode" );
    else									var.set( "mode", 0 );
    mode %= 4;

    charge2();
    
    bFullPreview 		= false;
    bShowPreview 		= false;
    bIcones  			= true;
	bAfficheInfoSouris 	= false;
	bAfficheInfoFits 	= false;
	bAfficheGrille 		= false;

    dxIcon				= width  / 8;
    dyIcon				= height / 6;


    sCurrentDir = "";
    afficheInfoFits( mode );
    resize_all();

//    rotate_capture_plus(true);
    log_tab( false );
    logf((char*)" Captures::init() -----FIN---- %d", mode );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool Captures::isMouseOverCapture(int xm, int ym)
{
    int nb = captures.size();
    for( int n=0; n<nb; n++ )
    {
        if ( captures[n]->isMouseOver(xm, ym) )            return true;
    }
    return false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::charge_image( string dirname, string filename )
{
    logf( (char*)"Captures::charge_image() ......" );
    logf( (char*)"    %s",(char*)dirname.c_str() );
    logf( (char*)"    %s",(char*)filename.c_str() );
    log_tab( true );

    //if ( current_capture != -1 )    captures[current_capture]->iconize();   
    
    sCurrentDir = string( dirname ); 

    captures.push_back( new Capture(dirname, filename) );
    sauve();
    current_capture = captures.size() - 1;
    resize_all();
    //onTop( captures[current_capture] );

    log_tab( false );
    logf( (char*)"Captures::charge_image() ...END..." );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::update()
{
    //logf( (char*)"Captures::update() ..." );
    return;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::rotate_capture_moins(bool b)
{
    logf( (char*)"Captures::rotate_capture_moins(%s)  %d/%d", b? (char*)"true":(char*)"false", current_capture, captures.size() );
    if ( current_capture == -1 )		current_capture = captures.size() - 1;
    if ( current_capture < 0 )	 		{ current_capture = -1; return; }

    //---------------- determine si la capture courante est full
    int  previous = current_capture;
    bool bFull = false;
    
    if ( previous!=-1 )
    {
        bFull = captures[previous]->getFullScreen() ;
    }
    //------------------------------------------------------------
    int n = captures.size();
    
    if ( n == 0 )           return;

    if ( b != bIcones )
    {
        bIcones = b;
        if ( b )		captures[current_capture]->iconize( dxIcon, dyIcon );
    }
    else
    {
        current_capture--;
        if ( current_capture == -1 )            current_capture = n-1;
    }

	Capture* pCurr = captures[current_capture];
	if ( previous != -1 )		Capture* pPrev = captures[previous];
	
	if ( pCurr->isFits() && pCurr->getAfficheInfoFits() )		{
		pCurr->getFits()->afficheInfoFits(true);
      	WindowsManager::getInstance().onTop( pCurr->getFits()->getPanelFits() );
		pCurr->getFits()->getPanelCorrectionFits()->onTop();      	
	}
    
    if ( bIcones )      { bShowIcones = true; bFullPreview = false; bShowPreview = false; }
    else                bShowPreview = true;
    resize_all();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::rotate_capture_plus(bool b)
{
    logf( (char*)"Captures::rotate_capture_plus(%s)  %d/%d", b? (char*)"true":(char*)"false", current_capture, captures.size() );

    //---------------- determine si la capture courante est full
    if ( current_capture == -1 )		current_capture = captures.size() - 1;
    if ( current_capture < 0 )	 		{ current_capture = -1; return; }
    
    
    int  previous = current_capture;
    bool bFull = false;
    
    if ( previous!=-1 )
    {
        bFull = captures[previous]->getFullScreen() ;
    }
    //------------------------------------------------------------

    int n = captures.size();
    if ( n == 0 )           return;

    if ( b != bIcones )
    {
        bIcones = b;
        if ( b )		captures[current_capture]->iconize( dxIcon, dyIcon );
    }
    else
    {
        current_capture = ++current_capture % n;
    }
    
	Capture* pCurr = captures[current_capture];
	if ( previous != -1 )		Capture* pPrev = captures[previous];
	
	if ( pCurr && pCurr->isFits() ) {
		logf( (char*)"Active le panelCapture %d", current_capture );
		Fits* pFits = pCurr->getFits();
		
		pFits->getPanelCorrectionFits()->setVisible(true);
		pFits->getPanelCorrectionFits()->setPos(50,50);
		WindowsManager::getInstance().onTop( pFits->getPanelCorrectionFits() );      	

		if ( pCurr->getAfficheInfoFits() )		{
			pFits->afficheInfoFits(true);
		  	WindowsManager::getInstance().onTop( pFits->getPanelFits() );
		}
	}

    if ( bIcones )      { bShowIcones = true; bFullPreview = false; bShowPreview = false; }
    else                bShowPreview = true;

    resize_all();
    
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::resize_icone( Capture* p, int dx, int y, int dxi, int dyi )
{
    p->resize(dx+10,y+10,dxi-20,dyi-20);
    //p->iconize();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::resize_normal(Capture* p, int dx, int dy)
{
    logf( (char*)"Captures::resize_normal()" );
    p->resize(10,10,dx-20,dy-20);
    p->restaure( bAfficheInfoFits, bAfficheGrille, bAfficheInfoSouris );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::resize_all()
{
    logf( (char*)"Captures::resize_all()    ------------ curr=%d", current_capture );
    log_tab(true);    
    //if ( captures.size() == 0 )             return;
    

    bool bFull = false;
    if ( current_capture != -1 )        bFull = captures[current_capture]->getFullScreen();
    
    //logf( (char*)"  %d   full = %s", current_capture, bFull ? (char*)"true":(char*)"false" );
    logf( (char*)"Current=%d   bFull:%s bIcones:%s bShowPreview:%s bShowIcones:%s"   
                                                                , current_capture
                                                                , BOOL2STR(bFull)
                                                                , BOOL2STR(bIcones)
                                                                , BOOL2STR(bShowPreview)
                                                                , BOOL2STR(bShowIcones)
                                                                 );
    
    
    //--------------------------------------------------
    // Calcul les dimensions pour l'icone
    //--------------------------------------------------
    int dx, dy;// dxi, dyi;
    float ratio = (float)width/(float)height;
    
    dx = width - dxIcon;
    dy = height; 

    int DY;
    int y=10;

    int n = captures.size();
    int m = n;
    
    if ( bShowPreview )      m--;

    //if ( m<=0 )      { log_tab(false); return; }
    if ( m<=0 )      { goto fin_return; }

    if ( m>1 )      DY = (height-20-dyIcon) / (m-1);    
    else            DY = (height-20-dyIcon) / (m);    

    //--------------------------------------------------
    // Affiche l'image choisie
    //--------------------------------------------------
    if ( current_capture != -1  )
    {
        Capture* p = captures[current_capture];
        
        p->show();
	    p->restaure( bAfficheInfoFits, bAfficheGrille, bAfficheInfoSouris );
        p->onTop();
        
        if 		( bFullPreview )            p->fullscreen();
        else if ( !bIcones )				resize_normal( p, dx, dy);
		else								current_capture = -1;
		
        //if ( p->isFits() && !bIcones)	{	
        if ( p->isFits() )	{	
            logf( (char*)"Current fits onTop icone:%s", BOOL2STR(p->isIconized()) );
        	p->onTop();
        }

        logf( (char*)"Current icone:%s", BOOL2STR(p->isIconized()) );
    } 
    //if ( m <= 0 )            return;
    //--------------------------------------------------
    // Affiche les autres en icones
    //--------------------------------------------------
    for (int i=0; i<n; i++)
    {
        Capture* p = captures[i];
        logf( (char*)"Traitement n=%d:%s", i, BOOL2STR(p->isIconized()) );
        
        if ( i!=current_capture || bIcones )
        {
        	p->iconize( dxIcon, dyIcon );
            if ( !bShowIcones )		p->hide();

            resize_icone( p, dx, y, dxIcon, dyIcon );

            y += DY;
        }        
    } 

fin_return:
    log_tab(false);    
    logf( (char*)"Captures::resize_all() ---------END--------" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::glutSpecialFunc(int key, int x, int y)	{

    int n = current_capture;
    if ( n == -1)                   return;
    
    
    switch( key)
    {
	// right
	case 102:	
	    {
        if ( n == -1 )          return;
        Capture&  c = *captures[n];

	    logf( (char*)"Touche right !!" );
        float m = c.getCentX() + 10.0;
        c.setCentX( m );

        logf( (char*)"Echelle=%0.2f  dx=%0.2f dy=%0.2f", c.getEchelle(), c.getCentX(), c.getCentY() );
        
        }
		break;
	// left
	case 100:	
	    {
        if ( n == -1 )          return;
        Capture&  c = *captures[n];

        float m = c.getCentX() - 10.0;
        c.setCentX( m );

	    logf( (char*)"Touche left !!" );
        logf( (char*)"Echelle=%0.2f  dx=%0.2f dy=%0.2f", c.getEchelle(), c.getCentX(), c.getCentY() );

	    }
		break;
	// up
	case 101:	
	    {
        if ( n == -1 )          return;
        Capture&  c = *captures[n];
        
        float m = c.getCentY() - 10.0;
        c.setCentY( m );

	    logf( (char*)"Touche up !!" );
        logf( (char*)"Echelle=%0.2f  dx=%0.2f dy=%0.2f", c.getEchelle(), c.getCentX(), c.getCentY() );

		}
		break;
	// down
	case 103:	
	    {
        if ( n == -1 )          return;
        Capture&  c = *captures[n];

        float m = c.getCentY() + 10.0;
        c.setCentY( m );

        float e = c.getEchelle();

	    logf( (char*)"Touche down !!" );
        logf( (char*)"Echelle=%0.2f  dx=%0.2f dy=%0.2f", c.getEchelle(), c.getCentX(), c.getCentY() );

		}	
		break;
	// pgup
	case 104:	
	    {
        if ( n == -1 )          return;
	    logf( (char*)"Key (PGUP) : Zoom" );
	    log_tab(true);
        Capture&  c = *captures[n];

        c.getPreview()->wheelUp(x,y);
        /*
        float e = c.getEchelle() / 0.9;
        c.setEchelle(e);
		*/
        
        logf( (char*)"Echelle=%0.2f  dx=%0.2f dy=%0.2f", c.getEchelle(), c.getCentX(), c.getCentY() );
	    log_tab(false);
		}
		break;
	// pgdown
	case 105:	
	    {
        if ( n == -1 )          return;

	    logf( (char*)"Key (PGDN) : Zoom" );
	    log_tab(true);
        Capture&  c = *captures[n];

        c.getPreview()->wheelDown(x,y);
        
        /*
        float e = c.getEchelle() * 0.9;
        c.setEchelle(e);
        */
        logf( (char*)"Echelle=%0.2f  dx=%0.2f dy=%0.2f", c.getEchelle(), c.getCentX(), c.getCentY() );
	    log_tab(false);

        }
		break;
	// home
	case 106:	
	    {
        if ( n == -1 )          return;
        Capture&  c = *captures[n];

        c.setEchelle( 1.0 );
        c.setCentX( 0.0 );
        c.setCentY( 0.0 );

	    logf( (char*)"Touche home !!" );
        logf( (char*)"Echelle=%0.2f  dx=%0.2f dy=%0.2f", c.getEchelle(), c.getCentX(), c.getCentY() );

		}
		break;
    default:
        return;
    }
    
    //Capture&  c = *captures[n];
    //c.update();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::ajoute()
{
    logf( (char*)"Captures::ajoute()" );
    captures.push_back( new Capture() );
    sauve();
    current_capture = captures.size() - 1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::ajoute(string filename)
{
    logf( (char*)"Captures::ajoute()" );
    captures.push_back( new Capture(filename) );
    sauve();
    current_capture = captures.size() - 1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::supprime()
{
    logf( (char*)"Captures::supprime() current_capture = %d ", current_capture );
    log_tab(true);
    int n = captures.size();
    if ( current_capture != -1 )
    {
        Capture* p = captures[current_capture];
        if ( p == NULL )		logf( (char*)"[ERROR] Capture no %d pointeur NULL ", current_capture );
        delete p;
        captures.erase(captures.begin()+current_capture);
        sauve();

        int n = captures.size();
        if ( n == 0 )           current_capture  = -1;
        else
        {
            current_capture = ++current_capture % n;
        }
    
        showIcones();
    }
    else 
        logf( (char*)" supprime()  current_capture = -1" );

    log_tab(false);
    logf( (char*)"Captures::supprime() current_capture = %d ", current_capture );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::onTop(Capture* p)
{
    logf( (char*)"Captures::onTop(%s)", (char*)p->getBasename().c_str() );



    int nb = captures.size();
    int i;
    for ( i=0; i<nb; i++ )
    {
        if ( captures[i] == p ) break;
    }
    //--------------------------------------------
    if ( i >= nb )          return;
    
    logf( (char*)"  trouve  -> %s", (char*)p->getBasename().c_str() );

    if ( p->isFits() )          p->afficheFits();
    bool bFull = false;
    if ( current_capture != -1 )            bFull = captures[current_capture]->getFullScreen();
    
    current_capture = i;
    captures[current_capture]->setFullScreen(bFull);
    bShowPreview = true;
    if ( bIcones )          bIcones = false;
    
    if ( p->isFits() )
    {
    	if ( p->getAfficheInfoFits() )
    	{
    		p->afficheInfoFits(true);
    	}
    }
    resize_all();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::findAllStar()
{
    int n = current_capture;
    if ( n != -1 )
    {
        captures[n]->getPreview()->findAllStars();
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::deleteAllStars()
{
    int n = current_capture;
    if ( n != -1 )
    {
        captures[n]->getPreview()->deleteAllStars();
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::print_list()
{
    int nb = captures.size();
    for( int i=0; i<nb; i++ )
    {
        string filename = captures[i]->getFilename();
        logf( (char*)"   %d - %s", i, (char*)filename.c_str() );
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::sauve(void)
{
    VarManager&         var = VarManager::getInstance();


    int no = 0;
    
    while( true )
    {
        string key = "FileCapture" + to_string(no++);
        if ( var.existe( key ) )
        {
            logf( (char*)"Suppression de la var[\"%s\"]", key.c_str() );
            var.erase( key );
        }
        else
        {
            break;
        }
    }

    if ( captures.size() == 0 )         var.sauve();

    for(int i=0; i<captures.size(); i++)
    {
        string key = "FileCapture" + to_string(i);
        var.set( key, captures[i]->getFilename() );
        logf( (char*)"Creation de var[\"%s\"]", key.c_str() );// captures[i]->getFilename().c_str() );
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::charge(void)
{
    VarManager&         var = VarManager::getInstance();

    string filename = "/home/rene/.astropilot/captures.txt";
    logf( (char*)"Chargement des valeurs dans '%s'", (char*)filename.c_str() );
    
    std::ifstream fichier;
    fichier.open(filename, std::ios_base::app);

    if ( !fichier ) 
    {
        logf( (char*)"[ERROR]impossble d'ouvrir : '%s'", (char*)filename.c_str() );
        return;
    }

    string line;
    int nbLigne = 0;
    int noFile = 0;
    
    
    while ( getline (fichier, line) )
    {
        //sscanf( line.c_str(), "( %f , %f ) / ( %f , %f )", &rx, &ry, &ox, &oy );
        //ajoute(line);
        string filename = line;
        captures.push_back( new Capture(filename) );
        current_capture = captures.size() - 1;
        //captures[current_capture]->iconize();
        captures[current_capture]->setFullScreen(false);
        
        //logf( (char*)"charge : %s", (char*)filename.c_str() );
        string name = "FileCapture" + to_string( noFile++ );
        logf( (char*)"charge : %s %s ", (char*)name.c_str(), (char*)filename.c_str() );
        var.set( name, filename );
        
    }    
    fichier.close();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::charge2(void)
{
    logf( (char*)"Captures::charge2')" );
	log_tab( true );            

    VarManager&         var = VarManager::getInstance();

    int no = 0;
    
    while( true )
    {
        string key = "FileCapture" + to_string(no++);
        if ( var.existe( key ) )
        {
            string filename = *var.gets( key );;

            captures.push_back( new Capture(filename) );
            current_capture = captures.size() - 1;
            //captures[current_capture]->iconize();
            captures[current_capture]->setFullScreen(false);
            //var.set( name, filename );
        }
        else
        {
            break;
        }
    }
    
	log_tab( false );            
    logf( (char*)"Captures::charge2 ----END----" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::position(double ra, double dc)
{
    if (current_capture == -1)              return;
    
    
    captures[current_capture]->getPreview()->getStars()->position(ra, dc);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::setCurrent(Capture* p)
{
    for(int i=0; i<captures.size(); i++)
    {
        if ( captures[i] == p )     current_capture = i;
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::fullscreen()
{
    bFullPreview = !bFullPreview;
    logf( (char*)"Captures::fullscreen  => %s", bFullPreview ? (char*)"true":(char*)"false" );

    if ( bFullPreview )                 bShowIcones = false;
    else                                bShowIcones = true;

    if ( current_capture != -1 )
    {
        captures[current_capture]->setFullScreen(bFullPreview);
    }

    resize_all();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::showIcones()
{
    bShowIcones = true;
    
    resize_all();
}

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::hideIcones()
{
    bShowIcones = false;
    resize_all();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::switchAffIcones()
{
    bShowIcones = !bShowIcones;

    VarManager&         var = VarManager::getInstance();
    
    var.set( "bShowIcones", bShowIcones );

    resize_all();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::setColor(long c)
{
    for(int i=0; i<captures.size(); i++)
    {
        captures[i]->setColor( c);
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::setMode()
{
    for(int i=0; i<captures.size(); i++)
    {
    	Capture*	pCapture = captures[i];    	
    	
    	pCapture->getPreview()->setInfoSouris( bAfficheInfoSouris );
    	
        if ( pCapture->isFits() )		{
        	pCapture->getPreview()->setAffGrille( bAfficheGrille );
        	pCapture->afficheInfoFits( bAfficheInfoFits );
        }
        
        if ( i == current_capture )			pCapture->onTop();
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::afficheInfoFits()
{
	mode = ++mode % 4;
	logf( (char*)"Captures::afficheInfoFits()" );
	afficheInfoFits( mode );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::afficheInfoFits(int n)
{
	logf( (char*)"Captures::afficheInfoFits(%d)", n );
    VarManager::getInstance().set( "mode", n );


	switch( n)	{
	case 0 : bAfficheGrille = false; bAfficheInfoFits = false; bAfficheInfoSouris = false; break;
	case 1 : bAfficheGrille = true;  bAfficheInfoFits = false; bAfficheInfoSouris = false;  break;
	case 2 : bAfficheGrille = true;  bAfficheInfoFits = false; bAfficheInfoSouris = true;  break;
	case 3 : bAfficheGrille = true;  bAfficheInfoFits = true;  bAfficheInfoSouris = true; break;
	}

	setMode();	
}
/*
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::afficheInfoSouris()
{
	bAfficheInfoSouris = !bAfficheInfoSouris;
	
    for(int i=0; i<captures.size(); i++)
    {
    	Capture*		pCapture = captures[i];
        PanelCapture*	p = pCapture->getPreview();
        if ( p )		p->setInfoSouris(bAfficheInfoSouris);
    }
}
*/
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::onTop()
{
	if ( current_capture != -1 )	{
    	Capture*		pCapture = captures[current_capture];
    	if ( pCapture->isFits() && bAfficheInfoFits )	{
    		WindowsManager::getInstance().onTop( pCapture->getFits()->getPanelFits() );
			WindowsManager::getInstance().onTop( pCapture->getFits()->getPanelCorrectionFits());      	
    	}
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::change_ad( double ad )
{
	if ( current_capture != -1 && captures[current_capture]->isFits() )
	{
		captures[current_capture]->getPreview()->change_ad( ad );
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Captures::change_dc( double dc )
{
	if ( current_capture != -1 && captures[current_capture]->isFits() )
	{
		captures[current_capture]->getPreview()->change_dc( dc );
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------




