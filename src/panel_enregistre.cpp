#include "camera_mgr.h"
#include "panel_enregistre.h"
#include "file_browser.h"
#include "police.h"
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
/*
char		sUS[] = "us";
char		sMS[] = "ms";
char		sSC[] = "s";
*/
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelEnregistre::PanelEnregistre()
{
	init();
	init_panel();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelEnregistre::init()
{
    VarManager&         var = VarManager::getInstance();

    INIT_VARI( RECORD_Frames, 1 )
    INIT_VARI( RECORD_Tempo, 1 )
    INIT_VARI( RECORD_Num, 1 )
    INIT_VARB( RECORD_Suivi, true )
    
    sDirname	= "/home/rene/.astropilot/capture";
    sFilename	= "capture_astro_%d_%t";
    
    INIT_VARS( RECORD_Dirname,  sDirname )
    INIT_VARS( RECORD_Filename, sFilename )

    iFrames     = var.geti( "RECORD_Frames" );
    iTempo		= var.geti( "RECORD_Tempo" );
    
    sDirname	= * var.gets( "RECORD_Dirname" );
    sFilename	= * var.gets( "RECORD_Filename" );
    
    bCheckSuivi	= var.getb( "RECORD_Suivi" );

    logf( (char*)"Dirname  : %s", sDirname.c_str() );
    logf( (char*)"Filename : %s", sFilename.c_str() );
	
	dTime			= 1.0;
	bShooting		= false;
	bExitShooting	=false;
	
	pFrames			= NULL;
	pTempo			= NULL;
	pOK				= NULL;
	pQUIT			= NULL;
	pCamera			= NULL;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelEnregistre::init_panel()
{
	WindowsManager&     wm  = WindowsManager::getInstance();
    VarManager&         var = VarManager::getInstance();

    wm.add(this);

    int p   =   0;
    int dy  =  14;
    int x   =  5;
    int y   =   0;
    int dx  =  90;
    int DX	= 380;
    float gris = 0.7;
    vcf4 c  = vcf4(gris, gris, gris, 1.0);

    setExtraString( string("PanelEnregistre") );
    setSize( DX, 200 );
    setPos( 1000, 200 );
    loadSkin( PanelWindow::YELLOW );
	setBorderSize(6);
	setResizable(false);
 	setBackground((char*)"images/background.tga");

	int size = 10;
	y += (2*dy-size)/2;
	
    //-----------------------------------------------------
    //                       TITRE
    //-----------------------------------------------------
	PanelText* pText = new PanelText( (char*)"PRISE DE VUE", (char*)_ARIAL_BLACK ,  0, y, size, 0xFFffFFff, true );
	pText->setAlign( PanelText::CENTER );
	add( pText );
	y += (2*dy-size)/2 + size;
	
    //-----------------------------------------------------
    //-----------------------------------------------------
	PanelSimple* pH0 = new PanelSimple();
	pH0->setBackground( getTextureTop() );
	pH0->setPosAndSize( 0, y, DX, 6 );
	add( pH0 );
    y += 6;

    y += 5;
    
    //-----------------------------------------------------
    //                       Frames
    //-----------------------------------------------------
	add(      new PanelTextOmbre( (char*)"Frames\t:",		    PanelText::NORMAL_FONT, x, y, get_color(c) ) );
    pFrames = new PanelSpinEditText();
    pFrames->setExtraString( "APN::pFrames" );
    pFrames->set( 1, 5000, 300, 2 );
    pFrames->set_delta( 20, 8 );
    pFrames->set_val( iFrames );
    pFrames->setPosAndSize( x+dx, y, 160-dx, dy );
	pFrames->setChangeValue( this, (void*)pFrames );
    add(pFrames);
    y += dy;

    //-----------------------------------------------------
    //                       Tempo
    //-----------------------------------------------------
	add(       new PanelTextOmbre( (char*)"Tempo (ms)\t:",		PanelText::NORMAL_FONT, x, y, get_color(c) ) );
    pTempo = new PanelSpinEditText();
    pTempo->setExtraString( "APN::pTempo" );
    pTempo->set( 1, 1000, 300, 1);
    pTempo->set_delta( 20, 8 );
    pTempo->set_val( iTempo );
    pTempo->setPosAndSize( x+dx, y, 160-dx, dy );
	pTempo->setChangeValue( this, (void*)pTempo );
    add(pTempo);
    y += dy;
    //-----------------------------------------------------
    y += dy;

    //-----------------------------------------------------
    //                       Repertoire
    //-----------------------------------------------------
	add(   new PanelTextOmbre( (char*)"Repertoire\t:",		    PanelText::NORMAL_FONT, x, y, get_color(c) ) );
    pDirname = new PanelEditText();
    pDirname->setExtraString( "APN::pDirname" );
    pDirname->changeText( sDirname );
    pDirname->setPosAndSize( x+dx, y, DX-dx, dy );
    pDirname->resetCursor();
    pDirname->setCallBackMouse( this, pDirname );
	add(pDirname);
    y += dy;

    //-----------------------------------------------------
    //                       Nom de fichier
    //-----------------------------------------------------
	add(   new PanelTextOmbre( (char*)"Fichier(s)\t:",		    PanelText::NORMAL_FONT, x, y, get_color(c) ) );
    pFilename = new PanelEditText();
    pFilename->setExtraString( "APN::pFilename" );
    pFilename->changeText( sFilename );
    pFilename->setPosAndSize( x+dx, y, DX-dx, dy );
    pFilename->resetCursor();
    pFilename->setCallBackMouse( this, pFilename );
	add(pFilename);
    y += dy;
    //-----------------------------------------------------
    y += dy;

    //-----------------------------------------------------
    //                       Suivi
    //-----------------------------------------------------
	add(       new PanelTextOmbre( (char*)"Suivi\t\t:",			PanelText::NORMAL_FONT, x, y, get_color(c) ) );
    pCheckSuivi = new PanelCheckBox();
    pCheckSuivi->setPosAndSize( x+92, y, 14, 14);
    pCheckSuivi->setTrue(  (char*) "images/coche1.tga" );
    pCheckSuivi->setFalse( (char*) "images/coche0.tga" );
    pCheckSuivi->setListener( &bCheckSuivi );
    pCheckSuivi->setChangeValue( this );
    pCheckSuivi->setID( (void*)pCheckSuivi );

	add( pCheckSuivi );
    y += dy;
    //-----------------------------------------------------
    //                       Camera NAME
    //-----------------------------------------------------
	add(       new PanelTextOmbre( (char*)"Camera\t:",			PanelText::NORMAL_FONT, x, y, get_color(c) ) );
    pCameraName = new PanelTextOmbre( (char*)"---",			PanelText::NORMAL_FONT, 0, 0 );
    pCameraName->setPosAndSize( x+92, y, 14, 14);

	add( pCameraName );
    y += dy;
    //-----------------------------------------------------
    //                       Camera Suvi NAME
    //-----------------------------------------------------
	add(       new PanelTextOmbre( (char*)"Cam Suivi\t:",			PanelText::NORMAL_FONT, x, y, get_color(c) ) );
    pSuiviName = new PanelTextOmbre( (char*)"---",			PanelText::NORMAL_FONT, 0, 0 );
    pSuiviName->setPosAndSize( x+92, y, 14, 14);

	add( pSuiviName );
    y += dy;
    y += 5;

    //-----------------------------------------------------
    //-----------------------------------------------------

	pH0 = new PanelSimple();
	pH0->setBackground( getTextureBottom() );
	pH0->setPosAndSize( 0, y, DX, 6 );
	add( pH0 );
    y += dy;
    
    
    int buttonDX = 80;
    int buttonDY = 20;
    
    //-----------------------------------------------------
    //                       OK
    //-----------------------------------------------------
    pOK = new PanelButton();
    pOK->setChangeValue( this, (void*)pOK );
    pOK->setExtraString( string("APN::button_ok") );
	pOK->setPosAndSize(  (DX/2 - buttonDX) / 2, y, buttonDX, buttonDY );
	add(pOK);
    
	pOK->setUp(   (char*)"images/ok_over.tga" );
	pOK->setDown( (char*)"images/ok_down.tga" );
	pOK->setOver( (char*)"images/ok_down.tga" );
    
    //-----------------------------------------------------
    //                       QUIT
    //-----------------------------------------------------
    pQUIT = new PanelButton();
    pQUIT->setChangeValue( this, (void*)pQUIT );
    pQUIT->setExtraString( string("APN::button_quit") );
	pQUIT->setPosAndSize( DX/2 + (DX/2 - buttonDX) / 2, y, buttonDX, buttonDY );
	add(pQUIT);
    
	pQUIT->setUp(   (char*)"images/quit_over.tga" );
	pQUIT->setDown( (char*)"images/quit_down.tga" );
	pQUIT->setOver( (char*)"images/quit_down.tga" );
    
    
    y += 2*dy;
    
    setSize( DX, y );
    

    
    
    wm.onTop(this);
    wm.changeCapture(this);
    wm.changeFocus(pFrames);

	/*
	x	= getPosX();
	y	= getPosY();
	dx	= getPosDX();
	dy	= getPosDY();
	*/

	INIT_VARI( PanelEnregistre_X,   getPosX() );
	INIT_VARI( PanelEnregistre_Y,   getPosY() );
	//INIT_VARI( PanelEnregistre_DX,  getPosDX() );
	//INIT_VARI( PanelEnregistre_DY,  getPosDY() );
	//INIT_VARI( PanelEnregistre_DY,  getPosDY() );

	x	= var.geti( "PanelEnregistre_X" );
	y	= var.geti( "PanelEnregistre_Y" );
	//dx	= var.geti( "PanelEnregistre_DX" );
	//dy	= var.geti( "PanelEnregistre_DY" );

	setPos( x, y );
	setVisible( var.getb( "bPanelEnregistre" ) );

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelEnregistre::~PanelEnregistre()
{
	if ( pOK )			{ sup(pOK);			delete pOK; }
	if ( pQUIT )		{ sup(pQUIT);		delete pQUIT; }
	if ( pFrames )		{ sup(pFrames);		delete pFrames; }
	if ( pTempo )		{ sup(pTempo);		delete pTempo; }
	if ( pDirname )		{ sup(pDirname);	delete pDirname; }
	if ( pFilename )	{ sup(pFilename);	delete pFilename; }
	if ( pCheckSuivi )	{ sup(pCheckSuivi);	delete pCheckSuivi; }

	pFrames		= NULL;
	pTempo		= NULL;
	pOK			= NULL;
	pQUIT		= NULL;
	pDirname	= NULL;
	pFilename	= NULL;
	pCheckSuivi = NULL;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelEnregistre::setVisible(bool b)
{
    logf( (char*)"PanelEnregistre::setVisible(%s)", BOOL2STR(b) );

    PanelWindow::setVisible(b);
    WindowsManager& wm = WindowsManager::getInstance();
    if (b)
    {
        wm.onTop(this);
        wm.changeCapture(this);
        wm.changeFocus(pFrames);
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelEnregistre::supCallBacks()
{
    WindowsManager& wm = WindowsManager::getInstance();

    while( wm.is_call_back_keyboard( pTempo ) )   		wm.sup_call_back_keyboard( pTempo );
    while( wm.is_call_back_keyboard( pFrames ) )    	wm.sup_call_back_keyboard( pFrames );
    while( wm.is_call_back_keyboard( pFilename ) )      wm.sup_call_back_keyboard( pFilename );
    while( wm.is_call_back_keyboard( pDirname ) )       wm.sup_call_back_keyboard( pDirname );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool PanelEnregistre::keyboard(char key, int x, int y)
{
    WindowsManager& wm = WindowsManager::getInstance();
   
    Panel* p = wm.getFocus();
    
    //if ( p == pTime || p == pIso || p == pFrames || p == pTempo || p == pNum )
    if ( p == pFrames || p == pTempo || p == pDirname || p == pFilename )
    {
        supCallBacks();
        wm.call_back_keyboard( p );
        logf( (char*)"PanelEnregistre::keyboard" );
    }
    else
    {
        wm.stopKeyboard();
        return false;
    }
        

    wm.startKeyboard();

    /*
    if (        key ==  '.'        )                wm.keyboardFunc( key, x, y);
    if ( '0' <= key &&  key <= '9' )                wm.keyboardFunc( key, x, y);
    if ( ' ' <= key &&  key <= 126 )                return true;
    */
    if (   0 <= key &&  key <  'z' )                wm.keyboardFunc( key, x, y);


    switch(key){
    case 27:
        {
        logf( (char*)"Echappe" );
        wm.stopKeyboard();
        setVisible( false );
        wm.changeFocus(NULL);
        supCallBacks();
        saveValues();
        }
        return true;
    case '\r':
        {
        logf( (char*)"Touche Entree" );
        wm.stopKeyboard();
        setVisible( false );
        wm.changeFocus(NULL);
        saveValues();
        supCallBacks();
        commande_photo();
        }
        return true;
    case 17:
        return true;
    }
    return true;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelEnregistre::saveValues()
{
    VarManager&         var = VarManager::getInstance();

    iFrames = stoi( pFrames->getText(), NULL, 10 );
    var.set( "RECORD_Frames", iFrames );
    logf( (char*)"Frames = %d", iFrames );    
    
    iTempo = stoi( pTempo->getText(), NULL, 10 );
    var.set( "RECORD_Tempo", iTempo );
    logf( (char*)"Tempo = %d", iTempo );
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Camera* PanelEnregistre::find_camera()
{
	//logf_thread( (char*)"PanelEnregistre::find_camera()" );

	return Camera_mgr::getInstance().getCurrent();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Camera* PanelEnregistre::find_camera_suivi()
{
	return Camera_mgr::getInstance().getAsserv();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
volatile bool PanelEnregistre::check_suivi()
{
	Camera_mgr& cams = Camera_mgr::getInstance();
	Camera*		pCam = cams.getAsserv();
	
	//if ( bExitShooting )		return true;
	
    if ( pCam == NULL )			return false;
    
    volatile double d = pCam->getEcart();
    
    //logf_thread( (char*)"PanelEnregistre::check_suivi = %lf/%lf", d, dErr );
    if ( d > dErr )				return false;
    
     
	return true;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelEnregistre::shooting()
{
    logf_thread( (char*)"PanelEnregistre::shooting()" );
    log_tab(true);

	Camera*		pCam = find_camera();
	char		sFramesCapture[20];
	
	if ( pCam == NULL ) 
	{
		bExitShooting = true;
	}
	else
	{
		int n = 0;
	    logf_thread( (char*)"Enregistrement de %d images", iFrames );
		
		while( bExitShooting != true )
		{

		    snprintf( sFramesCapture, sizeof(sFramesCapture), "%d/%d", n+1, iFrames );
		    logf_thread( (char*)"Prise une image %s", sFramesCapture );
		    pFramesCapture->changeText( sFramesCapture );
		    log_tab(true);
			
			pCam->setDirname(  sDirname  );
			pCam->setFilename( sFilename );
			pCam->enregistre();
			
			// tempo 10 ms entre chaque test
			while( pCam->getSaving() == false ) usleep( 10 *1000 );
			
			if ( bCheckSuivi )
			{
				// tempo 10 ms entre chaque test
				while( check_suivi() == false ) 		usleep( 10 *1000 );
			}

		    logf_thread( (char*)"Image OK" );
			n++;
			

			// tempo de iTempo ms
			usleep( iTempo * 1000 );
			
		    log_tab(false);

			if ( n >= iFrames )	break;
		}
	}    
	
    log_tab(false);
    if ( bExitShooting )		logf_thread( (char*)"[ WARNING ] Arret de la prise d'images" );
    bShooting = false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelEnregistre::commande_photo()
{
	
    logf( (char*)"PanelEnregistre::commande_photo()" );
	if ( bShooting )			{ logf( (char*)"[ WARNING ] Photos en cours" ); return; }
	
	Camera*		pCam = find_camera();
	if ( pCam == NULL )			{ logf( (char*)"[ WARNING ] Pas de camera" ); return; }
	

	if ( !pCam->isCamera() )	{ logf( (char*)"[ WARNING ] Ce n'est pas une camera" ); return; }

	Camera_mgr&	cams	= Camera_mgr::getInstance();
	pCam				= cams.getAsserv();
	
	if ( pCam == NULL && bCheckSuivi )
	{
		logf( (char*)"[ WARNING ] Suivi impossible !!!"  );
		return;
	}
	
	bExitShooting = false;
    th_shooting = thread( &PanelEnregistre::shooting, this );
    bShooting = true;
    th_shooting.detach();


	/*
	string filename = " -o /home/rene/Documents/astronomie/k200d/test-"
                        + to_string(iNum++);
    //string iso = " -i " + pIso->getText();
    //string stime = " -t " + pTime->getText();
    string frames = " -d=4 -F " + pFrames->getText();
    string timeout = " --timeout=" + pTempo->getText();
    string focus = " -f";
    string mode = " -m M";
    string format = " --file_format=JPG";
    string command = "/home/rene/Documents/astronomie/logiciel/k200d/pktriggercord-0.84.04/pktriggercord-cli";

    //command = command + filename + iso + stime + frames +  timeout + mode + format;
    command = command + filename + frames +  timeout + mode + format;


    logf( (char*) command.c_str() );
	*/
}
//--------------------------------------------------------------------------------------------------------------------
//
//													FONTCTIONS VIRTUELLES
//
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelEnregistre::changeValueDouble( double val, void *pv )
{
	Panel* p = (Panel*)pv;
	
    //logf( (char*)"PanelEnregistre::changeValueDouble(%lf, %s)", val, p->getExtraString().c_str() );

    VarManager&	    var = VarManager::getInstance();
	double t 			= (double)Timer::getInstance().getCurrentTime();
	double elapsedTime 	= t - dTime;

	// Temps mini 100ms
	if (  elapsedTime > 0.1 )	
	{
		dTime = t;
		changeValueDoubleClick(val, p);
		return;
	}

	
	if ( p == pFrames )
	{
	    var.set( "RECORD_Frames", (int)val );
	    iFrames = (int)val;
	}
	else if ( p == pTempo )
	{
	    var.set( "RECORD_Tempo", (int)val );
	    iTempo = (int)val;
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelEnregistre::changeValueDoubleClick( double val, void *pv )
{
	Panel* p = (Panel*)pv;

    logf( (char*)"PanelEnregistre::changeValueDoubleClick(%lf, %s)", val, p->getExtraString().c_str() );
    VarManager&         var = VarManager::getInstance();

	if ( p == pFrames )
	{
	    var.set( "RECORD_Frames", (int)round(val) );
	    iFrames = (int)val;
	}
	else if ( p == pTempo )
	{
	    var.set( "RECORD_Tempo", (int)val );
	    iTempo = (int)val;
	}
	else if ( p == pOK )
	{
		commande_photo();
		//setVisible( false );
	}
	else if ( p == pQUIT )
	{
		setVisible( false );
	}
	else if ( p == pCheckSuivi )
	{
	    //logf( (char*)"|  bCheckSuivi %s", BOOL2STR( bCheckSuivi ) );
	    var.set( "RECORD_Suivi", bCheckSuivi );
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelEnregistre::updatePos()
{
	Panel::updatePos();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelEnregistre::displayGL()
{
    VarManager&         var = VarManager::getInstance();
    unsigned int color; 
    if ( var.getb("bNuit") )        color = 0xffff0000;
    else                            color = 0xffffffff;
    
    setColor( color );
    //pTime->setColor( color );
    //pIso->setColor( color );
    if (pFrames)	pFrames->setColor( color );
    if (pTempo)		pTempo->setColor( color );
    
    PanelWindow::displayGL();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelEnregistre::idle( float fElapsedtime )
{
	//logf( (char*)"PanelEnregistre::idle( %0.2f )", fElapsedtime );
	Camera*	p = find_camera();
	if ( p != pCamera )
	{
		pCamera = p;
		
		if ( p )			pCameraName->changeText( pCamera->getName() );
		else				pCameraName->changeText( (char*)"---" );
	}



	p = find_camera_suivi();
	if ( p != pCameraSuivi )
	{
		pCameraSuivi = p;
		
		if ( p )			pSuiviName->changeText( pCamera->getName() );
		else				pSuiviName->changeText( (char*)"---" );
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelEnregistre::releaseLeftCB( int x, int y, void* pv)
{
	logf( (char*)"PanelEnregistre::releaseLeftCB" );
	
	if ( pv == pDirname || pv == pFilename )
	{
		start_filebrowser();
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelEnregistre::start_filebrowser()
{
   	logf((char*)"PanelEnregistre::start_filebrowser()" );

	VarManager& var = VarManager::getInstance();
    FileBrowser& fb = FileBrowser::getInstance();
    
    fb.getVisible();
    fb.setFiltre( "" );
    
    bool bFileBrowser = true;

    fb.change_dir( sDirname );
    fb.setFilename( sFilename );
    fb.setCallBackOK( this );
    fb.setTitle( "Fichier Image" );
	fb.loadSkin( PanelWindow::YELLOW );
    string s = "Choisissez un nom de fichier, et un repertoire pour la camera ";
	Camera*		pCam = find_camera();
    if ( pCam )		s += ": " + pCam->getName();
    fb.setSTitle( s.c_str() );
    
    if ( bFileBrowser )         fb.affiche();
    else                        fb.cache();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelEnregistre::callback( bool bb, int ii, char* pp )
{
   	logf((char*)"PanelEnregistre::callback()" );
	// si bouton quit
	if ( !bb )			return;
	
	VarManager& var = VarManager::getInstance();
    FileBrowser& fb = FileBrowser::getInstance();
    
    sFilename = fb.getFilename();
    pFilename->changeText( sFilename );
	
	sDirname = fb.getWorkingDir();
    pDirname->changeText( sDirname );

    var.set( "RECORD_Dirname",  sDirname  );
    var.set( "RECORD_Filename", sFilename );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

