#include "camera_zwo.h"
#include "camera_mgr.h"
#include "file_browser.h"
#include "timer.h"
//--------------------------------------------------------------------------------------------------------------------
#include <png.h>
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
char sUnite_us[] = "us";
char sUnite_ms[] = "ms";
char sUnite_sc[] = "s";
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
CameraZWO::~CameraZWO()
{
    logf_thread( (char*)"Destructeur ~CameraZWO() -----------" );
    log_tab(true);

    WindowsManager&	 vm = WindowsManager::getInstance();

    iCharging			= -1;
    bCharging			= false;

    logf_thread( (char*)"fermeture thread (lecture image) join()" );
    bExitThread = true;
    bStopThreadVideo = true;
    while( bThreadCaptureImage );
    while( bThreadCaptureVideo );

    logf_thread( (char*)"join() OK" );

    close_zwo();
    close_device();

	if ( panelCamera != NULL )	{
	    logf_thread( (char*)"fermeture panelCamera" );
	    log_tab(true);
		vm.sup( panelCamera );
	    delete panelCamera;
	    panelCamera = NULL;
	    log_tab(false);
    }
    
	if ( panelControl != NULL )	{
	    logf_thread( (char*)"fermeture panelControl" );
	    log_tab(true);
		vm.sup( panelControl );
		delete panelControl;
	    panelControl = NULL;
	    log_tab(false);
	}


    
    if ( pImgBuf0 )		free(pImgBuf0);
    if ( pImgBuf1 )		free(pImgBuf1);
    
	pImgBuf0 = NULL;
	pImgBuf1 = NULL;

    //panelCamera->deleteBackground();
    log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
CameraZWO::CameraZWO(int n)
{
    logf_thread((char*)"Constructeur CameraZWO( %d ) -------------", n );
    log_tab(true);
    //Device_cam();

    init();
    init_zwo( n  );
    
    CreatePreview();
    CreateControl();
    
    video();

    log_tab(false);
    //logf_thread((char*)"Constructeur Camera() ---- END ----" );
}
//---------------------------------------.----------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::init()
{
    logf_thread((char*)"CameraZWO::init() -------------");

	hz					= 0.0;

	width				= -1;
	height				= -1;
    bCharging			= false;
    iCharging			= -1;
    bExitThread			= false;
	bStopThreadVideo	= false;
	bStopThreadImage	= false;
    
    bEnregistre			= false;
    
	bThreadCaptureImage	= false;
    bThreadCaptureVideo	= false;

    vCameraSize.x		= -1;
    vCameraSize.y		= -1;

    expo				= -1;
    gain				= -1;
    wb_r				= -1;
    wb_b				= -1;
    bin					= -1;
    flip				= -1;
    bright				= -1;

	bExpo				= false;
	bGain				= false;
	bWB_R				= false;
	bWB_B				= false;
	bBandwith			= false;
    
    previousTime		= -1;
    bStartThread		= false;
    bPanelControl		= false;

    nb_images			= 0;
    sSauveDir			= "/home/rene/.astropilot/";
    
    iCamID				= -1;
    
	panelCamera			= NULL;
	panelControl		= NULL;
	pImgBuf0			= NULL;
	pImgBuf1			= NULL;
	
	dCurrentTime		= Timer::getInstance().getCurrentTime();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::changeValueDouble( double val, void* p )	{
	val = round(val);
	//return;
	//logf( (char*)"changeValueDouble %lf", val );
	
	double t 			= (double)Timer::getInstance().getCurrentTime();
	double elapsedTime 	= t - dCurrentTime;

	// initialise les variables	
	Control* 			pControl	= (Control*)p;
	int 				type		= pControl->getExtraValue();
	PanelSpinEditText* 	pSet		= (PanelSpinEditText*) pControl->getSpinEditText();
	
	// Recupere la valeur
	switch( type )
	{
	case ASI_EXPOSURE :		changeValueDoubleExpo( val, pSet );	break;
	case ASI_GAIN:			gain = (long) val;					break;
	case ASI_WB_R:			wb_r = (long) val;					break;
	case ASI_WB_B:			wb_b = (long) val;					break;
	case ASI_HARDWARE_BIN:	bin = (long) val;					break;
	case ASI_FLIP:			flip = (long) val;					break;
	case ASI_BRIGHTNESS:	bright = (long) val;				break;
	}
	
	// Temps mini 100ms
	if (  elapsedTime > 0.1 )	{ 
		dCurrentTime = t; 
		changeValueDoubleClick( val, p ); 
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::changeValueDoubleClick( double val, void* p )	{
	val = round(val);
//return;
	if( p == &bExpo )
	{
		bExpo = (bool)val;
		logf( (char*)"Click Expo %s", BOOL2STR((bool)val)  );
		return;
	}
	else if( p == &bGain )
	{
		bGain = (bool)val;
		logf( (char*)"Click Gain %s", BOOL2STR((bool)val)  );
		return;
	}
	else if( p == &bWB_R )
	{
		bWB_R = (bool)val;
		logf( (char*)"Click WB_R %s", BOOL2STR((bool)val)  );
		return;
	}
	else if( p == &bWB_B )
	{
		bWB_B = (bool)val;
		logf( (char*)"Click WB_B %s", BOOL2STR((bool)val)  );
		return;
	}
	else if( p == &bBandwith )
	{
		bBandwith = (bool)val;
		logf( (char*)"Click BandWidth %s", BOOL2STR((bool)val)  );
		return;
	}
	/*
	*/
	//if ( typeid(*p) != typeid(
	
	Control* 			pControl	= (Control*)p;
	int 				type		= pControl->getExtraValue();
	PanelSpinEditText* 	pSet		= (PanelSpinEditText*) pControl->getSpinEditText();
	
	switch( type )
	{
	case ASI_EXPOSURE:
		{
		ASISetControlValue(iCamID, ASI_EXPOSURE,  expo, ASI_FALSE);
		save_var_control( ASI_EXPOSURE, expo, false );
		break;
		}
	case ASI_GAIN:
		{
		gain = (long) val;
		ASISetControlValue(iCamID, ASI_GAIN, 			gain, ASI_FALSE);
		save_var_control( ASI_GAIN, gain, false );
		break;
		}
	case ASI_WB_R:
		{
		wb_r = (long) val;
		ASISetControlValue(iCamID, ASI_WB_R, 			wb_r, ASI_FALSE);
		save_var_control( ASI_WB_R, wb_r, false );
		break;
		}
	case ASI_WB_B:
		{
		wb_b = (long) val;
		ASISetControlValue(iCamID, ASI_WB_B, 			wb_b, ASI_FALSE);
		save_var_control( ASI_WB_B, wb_b, false );
		break;
		}
	case ASI_HARDWARE_BIN:
		{
		bin = (long) val;
		ASISetControlValue(iCamID, ASI_HARDWARE_BIN,  	 bin, ASI_FALSE);
		save_var_control( ASI_HARDWARE_BIN, bin, false );
		break;
		}
	case ASI_FLIP:
		{
		flip = (long) val;
		ASISetControlValue(iCamID, ASI_FLIP, 			flip, ASI_FALSE);
		save_var_control( ASI_FLIP, flip, false );
		logf_thread( (char*)"Flip : %d - %s", (int)flip, str_flip( (ASI_FLIP_STATUS)flip ) );
		}
	case ASI_BRIGHTNESS:
		{
		bright = (long) val;
		ASISetControlValue(iCamID, ASI_BRIGHTNESS, 			flip, ASI_FALSE);
		save_var_control( ASI_BRIGHTNESS, bright, false );
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::save_var_control( ASI_CONTROL_TYPE type, long lValue, bool bCreate )
{
	VarManager& var = VarManager::getInstance();

	string 	s = name + "_" + string( str_caps((ASI_CONTROL_TYPE) type) );
	
	if ( bCreate )
	{
		if ( !var.existe( s ) )
		{
			var.set( s, (int) lValue );
			//logf_thread( (char*)"Creation de la variable  : %s = %d", s.c_str(), (int)lValue );
		}
		return;
	}
	
 	var.set( s, (int) lValue );

}
//--------------------------------------------------------------------------------------------------------------------
//
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::compute_expo()
{
	if ( expo >= 1000000 )
	{
		expo_val = expo / 1000000;
		expo_unite = SEC;
	}
	else if ( expo >= 1000 )
	{
		expo_val = expo / 1000;
		expo_unite = MILI;
	}
	else
	{
		expo_val = expo;
		expo_unite = MICRO;
	}
	expo_unites[MICRO]	= sUnite_us;
	expo_unites[MILI]	= sUnite_ms;
	expo_unites[SEC]	= sUnite_sc;
}
//--------------------------------------------------------------------------------------------------------------------
//
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::load_var_control()
{
	VarManager& var = VarManager::getInstance();
	string s;

	int i;
	for (i=0; i<100; i++ )
	{
		string sCAPS = string( str_caps( (ASI_CONTROL_TYPE) i) );
		//logf_thread( (char*)"CAPS  %s", sCAPS.c_str() );
		if (  sCAPS.find( "inconnu" ) != string::npos )	
		{
			break;
		}
		s = name + "_" + string( str_caps((ASI_CONTROL_TYPE) i) );
		
		if ( var.existe(s) )
		{
			switch( i )
			{
			case ASI_GAIN:				{ gain = var.geti(s);  break; }
			case ASI_EXPOSURE:			{ expo = var.geti(s);  compute_expo(); break; }
			case ASI_WB_R:				{ wb_r = var.geti(s);  break; }
			case ASI_WB_B:				{ wb_b = var.geti(s);  break; }
			case ASI_FLIP:				{ flip = var.geti(s);  break; }
			case ASI_HARDWARE_BIN:		{ bin  = var.geti(s);  break; }
			case ASI_BRIGHTNESS:		{ bright  = var.geti(s);  break; }
			default: continue;
			}
			//logf_thread( (char*)"  %s", s.c_str() );
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//
//--------------------------------------------------------------------------------------------------------------------
long CameraZWO::get_var_control(ASI_CONTROL_TYPE type)
{
	long val = -1L;
	
	switch( type )
	{
	case ASI_GAIN:				{ val = gain;  break; }
	case ASI_EXPOSURE:			{ val = expo;  break; }
	case ASI_WB_R:				{ val = wb_r;  break; }
	case ASI_WB_B:				{ val = wb_b;  break; }
	case ASI_FLIP:				{ val = flip;  break; }
	case ASI_HARDWARE_BIN:		{ val = bin;   break; }
	case ASI_BRIGHTNESS:		{ val = bright;   break; }
	default: break;;
	}
	
	return val;
}
//--------------------------------------------------------------------------------------------------------------------
//
// VOIR DOC ; 
// ----------
//  /home/rene/Documents/astronomie/logiciel/ASI_Camera_SDK/ASI_linux_mac_SDK_V1.41/doc/ASICamera2 Software Development Kit.pdf
//
//--------------------------------------------------------------------------------------------------------------------
int CameraZWO::addControl()	{
    logf_thread((char*)"CameraZWO::addControl() ---------------");
    log_tab(true);
 
    int xt  = 10;
    int yt  = 10;
    int dyt = 16;
    int n = 0;

	// Get controle
	int 				iNumOfCtrl = 0;

	// Variable de la boucle	
	long 				lValue;
	long				max;
	long				min;
	ASI_BOOL			bRet;
	ASI_CONTROL_CAPS	ControlCaps;
	
	// Recupere le nombre de controle
	ASIGetNumOfControls(iCamID, &iNumOfCtrl);
	logf_thread( (char*)"Liste des controles (ASI_CONTROL_CAPS)" );


	load_var_control();

	// Affichage control
	for( int i=0; i < iNumOfCtrl; i++)
	{
		ASIGetControlCaps( iCamID, i, &ControlCaps);
		
		#ifdef DEBUG_CONTROLS
		logf_thread( (char*)"%02ld - CONTROL_TYPE :\t%s ", ControlCaps.ControlType, (char*)str_caps(ControlCaps.ControlType) );
		logf_thread( (char*)"\t  name :\t%s ", 	(char*)  ControlCaps.Name );
		logf_thread( (char*)"\t descr :\t%s ",	(char*)  ControlCaps.Description );
		logf_thread( (char*)"\t   max :\t%ld ", 		 ControlCaps.MaxValue );
		logf_thread( (char*)"\t   min :\t%ld ", 		 ControlCaps.MinValue );
		logf_thread( (char*)"\tdefaut :\t%ld ", 		 ControlCaps.DefaultValue );
		logf_thread( (char*)"\tisAuto :\t%s ", 	BOOL2STR(ControlCaps.IsAutoSupported) );
		logf_thread( (char*)"\tisWrit :\t%s ", 	BOOL2STR(ControlCaps.IsWritable) );
		#endif

		ASI_ERROR_CODE e = ASIGetControlValue( iCamID, ControlCaps.ControlType, &lValue,  &bRet );
		
		if ( e != ASI_SUCCESS )	
			logf_thread( (char*)"[ ERREUR ]  ASIGetControlValue" );

		if( ControlCaps.ControlType == ASI_EXPOSURE )		lValue /= 10000;
		
		#ifdef DEBUG_CONTROLS
		logf_thread( (char*)"\t value :\t%ld ", 	lValue );
		logf_thread( (char*)"\tisAuto :\t%s ", 	BOOL2STR(bRet) );
		#endif

		max = ControlCaps.MaxValue;
		min = ControlCaps.MinValue;

		if ( ControlCaps.ControlType == ASI_EXPOSURE )		{min = 0; max=1000;}
		
		if ( ControlCaps.IsWritable == ASI_TRUE)
		{
			createControlID( xt, yt + n*dyt, &ControlCaps );
			save_var_control( ControlCaps.ControlType, lValue, true );
			n++;
		}
	}

	// Change la taille de la fenetre controle 
	int dy = ++n*dyt;
	panelControl->setSize( 200+170, dy);

	// sorite
    logf_thread( (char*)"dy=%d", dy);
    log_tab(false);
    
    return dy;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelCheckBox* CameraZWO::createCheckBox( int x, int y, ASI_CONTROL_CAPS* p, Control* pControl )	
{
    //logf_thread( (char*)"CameraZWO::createCheckBox()" );
    PanelCheckBox* pCheck = new PanelCheckBox();
    
    pCheck->setPosAndSize( x+280, y, 14, 14);
    pCheck->setTrue(  (char*) "images/coche1.tga" );
    pCheck->setFalse( (char*) "images/coche0.tga" );
    
    bool*	pBool = NULL;
    switch(p->ControlType)
    {
    case ASI_EXPOSURE:				pBool = &bExpo; break;
    case ASI_GAIN:					pBool = &bGain; break;
    case ASI_WB_B:					pBool = &bWB_B; break;
    case ASI_WB_R:					pBool = &bWB_R; break;
    case ASI_BANDWIDTHOVERLOAD:		pBool = &bBandwith; break;
    }

    pCheck->setListener( pBool );
    pCheck->setChangeValue( this );
    pCheck->setID( (void*)pBool );

	panelControl->add( pCheck );
 	
 	return pCheck;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelSpinEditText* CameraZWO::createSpinEdit( int x, int y, ASI_CONTROL_CAPS* p, Control* pControl )	
{
    PanelSpinEditText* pSET = new PanelSpinEditText();
    
    double		dmin = p->MinValue;
    double		dmax = p->MaxValue;
	double		delta_tour = dmax-dmin;
    double		step =  1.0;
    ASI_BOOL	bRet;
    long		lValue = get_var_control( p->ControlType );

    if ( p->ControlType == ASI_EXPOSURE )	{ dmin = 1.0; dmax = 1000.0; delta_tour = 300.0; }
    if ( lValue == -1L )	ASIGetControlValue( iCamID, p->ControlType, &lValue,  &bRet );

    
    pSET->set( dmin, dmax, delta_tour, 1.0 );
    pSET->set_ndecimal(0);
    pSET->set_delta( 20, 8 );
    pSET->setPosAndSize( x+150+150, y, 90, 14 );
    pSET->setExtraString( p->Name );
    pSET->setChangeValue( this );
    pSET->setID( (void*)pControl );
    pSET->set_val( (double)lValue );
    
    if ( p->ControlType == ASI_EXPOSURE )
    {
        pSET->set_unite( expo_unites[expo_unite] );
        pSET->set_val( (double)expo_val );
        pSET->set( 0.0, 1000.0, 300.0, 1.0 );
    }
    

	pControl->getStep();
	pControl->setExtraValue( p->ControlType);
	pControl->setSpinEditText(pSET);

	panelControl->add( pSET );    

	return pSET;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::createControlID( int x, int y, ASI_CONTROL_CAPS* p )	
{
    //logf_thread( (char*)"Ajout de '%s'", str );

	panelControl->add( new PanelTextOmbre( (char*)p->Name,  	   PanelText::NORMAL_FONT, x, y ) );

    Control* pControl = new Control(NULL);
    
    
    if (pControl)       {
        PanelSpinEditText*	pSET	= createSpinEdit(x, y, p, pControl );
        PanelCheckBox*		pCheck	= NULL;

        if ( p->IsAutoSupported == ASI_TRUE )	pCheck = createCheckBox(x, y, p, pControl );

	    logf_thread( (char*)"Ajout de '%s'  %.2f>%.2f %s  %.2f", 
	    		p->Name, (float)p->MinValue, (float)p->MaxValue, 
	    		( p->IsWritable ? (char*)"R/W" : (char*)"R" ), 
	    		(float)pControl->getValue() );

		char str1[256];
		snprintf( (char*)str1, sizeof(str1), "%.0ld~%.0ld", 
		          (long)p->MinValue, (long)p->MaxValue );

		PanelText* pp = new PanelText( (char*)"  ", PanelText::NORMAL_FONT, x + 170, y );
		pp->razTabSize();
		pp->setTabSize(85);
		pp->setColor(0x808080ff);
		pp->changeText( (char*)str1 );
		panelControl->add( pp );
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::CreateControl()	{
    logf_thread((char*)"CameraZWO::CreateControl() --------------- %s", name.c_str() );
    log_tab(true);
    
    if ( getFd() == -1 )
    {
        logf_thread((char*)"Le materiel n'est pas encore ouvert");
    }

    panelControl = new PanelWindow();
    panelControl->setDisplayGL(displayGLnuit_cb);
    panelControl->setExtraString( getDevName() );
    //panelControl->loadSkinPath( "images/astro" );
	//panelControl->setBorderSize(8);
    panelControl->loadSkin( PanelWindow::YELLOW );
	panelControl->setBorderSize(9);

	WindowsManager& wm = WindowsManager::getInstance();
	int wsc = wm.getWidth();
	int hsc = wm.getHeight();
    
    
    int dx = 200 + 150;
    int dy = 150;
    int x = 20;
    int y = 20;

    panelControl->setPosAndSize( x, y, dx, dy);
    
    ///home/rene/programmes/opengl/video/essai
    wm.add(panelControl);
    panelControl->setBackground((char*)"images/background.tga");
    
    dy = addControl();
    resizeControlFirst(wsc, hsc, dx, dy);

    log_tab(false);
    logf_thread((char*)"CameraZWO::CreateControl() --------------- %s", name.c_str() );
    panelControl->setVisible(false);
    //logf((char*)"panelControl  %d,%d %dx%d\n", x, y, dx, dy);   
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::CreatePreview()	{
    logf_thread((char*)"CameraZWO::CreatePreview ------------- %s", name.c_str() );
    log_tab(true);

	WindowsManager& wm = WindowsManager::getInstance();

	//panelCamera = new PanelSimple();
	panelCamera = new PanelCamera(this);
    panelCamera->setExtraString( "PanelCamera View");
    panelCamera->setExtraString( name.c_str() );

	//int wsc = wm.getWidth();
	//int hsc = wm.getHeight();
	int wsc = width;
	int hsc = height;
    logf_thread((char*)"wsc=%d hsc=%d", wsc, hsc);
	iconSizePreview(wsc, hsc);
	//panelCamera->setBackground( (char*)"frame-0.raw");
	
	//pCamFilename = new PanelText( (char*)"frame-0.raw",		PanelText::LARGE_FONT, 20, 10 );
	//pCamFilename = new PanelText( (char*)getDevName(),		PanelText::LARGE_FONT, 20, 10 );
	//panelCamera->add( pCamFilename );
	
	pNbStars = new PanelText( (char*)"0",		PanelText::LARGE_FONT, wsc-50, 10 );
	panelCamera->add( pNbStars );
	panelCamera->getStars()->setPanelNbStars( pNbStars );
	
	//string pStr = getName().c_str();
	pPanelName = new PanelText( (char*)name.c_str(),	PanelText::LARGE_FONT, 0, 10 );
	pPanelName->setAlign( PanelText::CENTER );
	panelCamera->add( pPanelName );

	//panelCamera->setCanMove(false);
 	wm.add( panelCamera );

    logf_thread((char*)"name    %s ", name.c_str() );
    //logf_thread((char*)"    devname %s ", getDevName() );
    logf_thread((char*)"%d,%d %dx%d", 0, 0, getWidth(), getHeight());

    log_tab(false);
    logf_thread((char*)"CameraZWO::CreatePreview ------------- %s", name.c_str() );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::iconSizePreview(int width, int height)	{
    logf_thread( (char*) "CameraZWO::iconSizePreview(%d, %d) %s", width, height, (char*)getDevName() );
    log_tab(true);
	WindowsManager& wm = WindowsManager::getInstance();

	int wsc = wm.getWidth();
	int hsc = wm.getHeight();

	bFullScreen	= false;
	bIconized	= true;

    if ( vCameraSize.x == -1 )
    {
        //vCameraSize.x = getWidth();
        //vCameraSize.y = getHeight();
        vCameraSize.x = width;
        vCameraSize.y = height;
        logf_thread( (char*)"vCameraSize.x=%d vCameraSize.y=%d", vCameraSize.x, vCameraSize.y);
    }


	
	float rsc = (float)wsc/(float)hsc;
	float rpv = (float)vCameraSize.x/(float)vCameraSize.y;

    int modX, modY;
    float zoom;
    
	if ( rsc > rpv )    {
	    zoom = (float)hsc/(float)vCameraSize.y;

	    dxCam = zoom * (float)vCameraSize.x;
	    dyCam = zoom * (float)vCameraSize.y;

	    xCam = modX = (wsc - dxCam) / 2;
	    yCam = modY = 0;
	} 
	else                {
	    zoom = (float)wsc/(float)vCameraSize.x;

	    dxCam = zoom * (float)vCameraSize.x;
	    dyCam = zoom * (float)vCameraSize.y;

	    xCam = modX = 0;
	    yCam = modY = (hsc - dyCam) / 2;
	} 

    
    dxCam /= 8;
    dyCam /= 8;
    
    /*
    dxCam = width;
    dyCam = height;
    
    xCam = (wm.getWidth() - width ) / 2;
    yCam = (wm.getHeight() - height ) / 2;
	*/
    logf_thread( (char*) "Screen  : %dx%d", wsc, hsc);
    logf_thread( (char*) "Preview : %d,%d %dx%d", xCam, yCam, dxCam, dyCam);

	
	panelCamera->setPosAndSize( xCam, yCam, dxCam, dyCam);
	panelCamera->raz_ech_usr();
	panelCamera->compute_echelle();
	
	if ( panelControl != NULL )			panelControl->setVisible(false);
	//panelCamera->setDisplayGL(displayGLCamera_cb);
    log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::fullSizePreview(int width, int height)	{
    logf((char*) "CameraZWO::fullSizePreview(%d, %d) %s ", width, height, (char*)getName().c_str() );
	WindowsManager& wm = WindowsManager::getInstance();
	
	bFullScreen	= true;
	bIconized	= false;

	int wsc = width;
	int hsc = height;
    //logf((char*) "Screen (%d, %d)", wsc, hsc);


    if ( vCameraSize.x == -1 )
    {
        vCameraSize.x = getWidth();
        vCameraSize.y = getHeight();
        logf((char*)" vCameraSize.x=%d vCameraSize.y=%d", vCameraSize.x, vCameraSize.y);
    }


	
	float rsc = (float)wsc/(float)hsc;
	float rpv = (float)vCameraSize.x/(float)vCameraSize.y;

    int modX, modY;
    float zoom;
    

	if ( rsc > rpv )    {
	    zoom = (float)hsc/(float)vCameraSize.y;

	    dxCam = zoom * (float)vCameraSize.x;
	    dyCam = zoom * (float)vCameraSize.y;

	    xCam = modX = (wsc - dxCam) / 2;
	    yCam = modY = 0;
	} 
	else                {
	    zoom = (float)wsc/(float)vCameraSize.x;

	    dxCam = zoom * (float)vCameraSize.x;
	    dyCam = zoom * (float)vCameraSize.y;

	    xCam = modX = 0;
	    yCam = modY = (hsc - dyCam) / 2;
	} 


    logf((char*) "   Screen  : %dx%d", wsc, hsc);
    logf((char*) "   Preview : %d,%d %dx%d", xCam, yCam, dxCam, dyCam);

	panelCamera->setPosAndSize( xCam, yCam, dxCam, dyCam);
	panelCamera->raz_ech_usr();
	panelCamera->compute_echelle();
	
	if ( bPanelControl )		panelControl->setVisible( true );
	//panelCamera->setDisplayGL(displayGLCamera_cb);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
//bool*     pCharging;
//
void charge_camera()
{
    write_image();

    //*pCharging = true;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::change_background(void)
{
	if ( iCharging == -1 )			return;

    //log((char*)"START CameraZWO::change_background()");
    if ( vCameraSize.x == -1 )
    {
        vCameraSize.x = getWidth();
        vCameraSize.y = getHeight();
        vCameraSize = get_vCameraSize();

        //logf((char*)" vCameraSize.x=%d vCameraSize.y=%d", vCameraSize.x, vCameraSize.y);
    }
    //bFreePtr = false;
    if ( bCharging && iCharging != -1 )
    {
        //log((char*)"START CameraZWO::change_background()");
        panelCamera->deleteBackground();
        try
        {
            //panelCamera->setBackground( getBuffer(), vCameraSize.x, vCameraSize.y, 3);
            
            readBgr.ptr = NULL;

            int i = iCharging;
            switch( i )
            {
            case 0 : { readBgr.ptr = pImgBuf0; break; }
            case 1 : { readBgr.ptr = pImgBuf1; break; }
           	}

            readBgr.w   = width;
            readBgr.h   = height;
            readBgr.d   = 3;
             
            panelCamera->setBackground( readBgr.ptr, readBgr.w, readBgr.h, readBgr.d);
            panelCamera->setRB( &readBgr);
            
            //logf((char*)" %s : %ld", getName().c_str(), getBuffer() );
            //if (bSuivi)    suivi();
            
        }
        catch(std::exception const& e)
        {
            log((char*)"[ ERROR ] --- CameraZWO::change_background()");
            cerr << "ERREUR : " << e.what() << endl;
        }

        //pCamFilename->changeText( (char*)getDevName() );

        //pCharging = &bCharging;
        //startThread = true;
        
        float t = Timer::getInstance().getCurrentTime();
        if ( previousTime != -1 )
        {
            if ( ++nb_images >= 10 )
            {
                hz = 10.0 / (t - previousTime);
                previousTime = t;
                nb_images = 0;
            }
            //logf((char*)"CameraZWO::change_background()   Hz=%.0f %s", hz, getName().c_str() );
        }
        else
            previousTime = t;
        
        bCharging = false;
     }
    //log((char*)"END   CameraZWO::change_background()");
}
//--------------------------------------------------------------------------------------------------------------------
//
// Retour TRUE si le caractee est traité (utilise par main )
//
//--------------------------------------------------------------------------------------------------------------------
bool CameraZWO::keyboard(char key)
{
//return false;
    //std::cout << (int)key << std::endl;
    logf( (char*)"CameraZWO::keyboard( %d )", (int)key );
	Control* pControl;
	
	//if ( !panelControl->getVisible() )       return false;

	switch(key){ 

	case 'i':
		logf( (char*)"Touche 'i'" );
		break;


	case 'h':
		logf( (char*)"key = 'h'" );
		if ( iGlutModifier && GLUT_ACTIVE_SHIFT )
		{
			return false;
		}
		else
		{
			enregistre();
		}
		break;
	    
    case 'H':
	    logf( (char*)"key = 'H'" );
	    video();
	    break;
	    
    case 'j':
        logf( (char*)"File browser" );
		    FileBrowser::getInstance().setCallBack(this); 
		    FileBrowser::getInstance().setFiltre(".cam");
		    FileBrowser::getInstance().setFilename("");
		    FileBrowser::getInstance().setExtra(FB_CALLBACK_KEY_s);
		    FileBrowser::getInstance().setNewline(true);
		    FileBrowser::getInstance().change_dir(sSauveDir);
		    FileBrowser::getInstance().affiche();
        break;

    case 'J':  // '-'
        logf( (char*)"File browser" );
        FileBrowser::getInstance().setCallBack(this); 
        FileBrowser::getInstance().setFiltre(".cam");
        FileBrowser::getInstance().setFilename("");
        FileBrowser::getInstance().setExtra(FB_CALLBACK_KEY_S);
        FileBrowser::getInstance().setNewline(true);
        FileBrowser::getInstance().change_dir(sSauveDir);
        FileBrowser::getInstance().affiche();
        return false;
        break;

    default:
        return false;        
	}
	return true;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::setVisible(bool b)
{
    //log((char*)"CameraZWO::setVisible()");
    panelCamera->setVisible( b );
    if ( bPanelControl )	panelControl->setVisible( b );
    //log((char*)"CameraZWO::setVisible()");
    //panelControl->setVisible( b );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::setControlVisible(bool b)
{
    panelControl->setVisible( b );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::togglePanel()
{
	log((char*)"CameraZWO::togglePanel()");
	bPanelControl = ! bPanelControl;
	if (panelControl )		panelControl->setVisible( bPanelControl );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
GLubyte* CameraZWO::getPtr()
{
    return (GLubyte*)getBuffer();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::recentreSuivi()
{
    if ( panelCamera!=NULL )		panelCamera->recentreSuivi();
    else
    	logf( (char*)"Pas de panel preview !!");
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::setColor( long color )
{
    if ( panelCamera )		panelCamera->setColor( color );
    if ( panelControl )		panelControl->setColor( color );
	if ( pCamFilename )		pCamFilename->setColor(color);
	if ( pNbStars )			pNbStars->setColor(color);
}
//--------------------------------------------------------------------------------------------------------------------
//
// Compare les etoiles trouvées par le programme
// aux etoiles du catalogue vizier
// Affiche le resultat dans la console
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::compareStar()
{
   	log( (char*)"CameraZWO::compareStar()");
   	log_tab(true);

	starCompare.setStars(  panelCamera->getStars() );
	starCompare.setVizier( &vizier);

	starCompare.compareStar();

	double d = starCompare.compute_delta();
	panelCamera->getStars()->set_delta( d );
   	log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
// Compare les etoiles trouvées par le programme
// aux etoiles du catalogue vizier
// Affiche le resultat dans la console
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::saveCompareStar()
{
   	log( (char*)"PanelCapture::saveCompareStar()");

	starCompare.setStars(  panelCamera->getStars() );
	starCompare.setVizier( &vizier);

	starCompare.saveCompareStar();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::print_roi_format()
{
	ASI_CAMERA_MODE mode;
	int width, height, bin;
	ASI_IMG_TYPE img;
	
	ASIGetROIFormat( iCamID, &width, &height, &bin, &img  );
	
	logf_thread( (char*) "   --------------------------------------------------" );
	logf_thread( (char*) "   width = %d, height = %d, bin = %d, IMG_TYPE = %s", width, height, bin, str_image(img) );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::print_mode()
{
	ASI_CAMERA_MODE mode;
	ASIGetCameraMode( iCamID, &mode  );
	
	logf_thread( (char*) "   --------------------------------------------------" );
	logf_thread( (char*) "       mode :\t %d-%s", (int)mode, str_mode(mode)  );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::print_info( ASI_CAMERA_INFO* pASI_camera_info )
{
	logf_thread( (char*) "       Name :\t %s", 			pASI_camera_info->Name );
	logf_thread( (char*) "         ID :\t %d", 			pASI_camera_info->CameraID );
	logf_thread( (char*) "      Width :\t %ld", 		pASI_camera_info->MaxWidth );
	logf_thread( (char*) "     Height :\t %ld", 		pASI_camera_info->MaxHeight );
	logf_thread( (char*) "    isCOlor :\t %d", (int)	pASI_camera_info->IsColorCam );
	logf_thread( (char*) "      bayer :\t %d", (int)	pASI_camera_info->BayerPattern );
	//logf_thread( (char*) "     SupportedBins :\t %ld", ASI_CAMERA_INFO->SupportedBins[16]; //1 means bin1 which is supported by every camera, 2 means bin 2 etc.. 0 is the end of supported binning method
	//logf_thread( (char*) "     SupportedVideoFormat :\t %ld", ASI_CAMERA_INFO->SupportedVideoFormat[8]; //this array will content with the support output format type.IMG_END is the end of supported video format
	logf_thread( (char*) " pixel size :\t %.2lf um",	pASI_camera_info-> PixelSize );	//the pixel size of the camera, unit is um. such like 5.6um
	logf_thread( (char*) "    shutter :\t %d", (int)	pASI_camera_info->MechanicalShutter );
	logf_thread( (char*) "        ST4 :\t %d", (int)	pASI_camera_info->ST4Port );
	logf_thread( (char*) "  CoolerCam :\t %d", (int)	pASI_camera_info->IsCoolerCam );
	logf_thread( (char*) "    USBHost :\t %d", (int)	pASI_camera_info->IsUSB3Host );
	logf_thread( (char*) "    USB3Cam :\t %d", (int)	pASI_camera_info->IsUSB3Camera );
	logf_thread( (char*) " ElecPerADU :\t %f", (float)	pASI_camera_info->ElecPerADU );
	logf_thread( (char*) "    bitdeph :\t %d", (int)	pASI_camera_info->BitDepth );
	logf_thread( (char*) " TriggerCam :\t %d", (int)	pASI_camera_info->IsTriggerCam );
	logf_thread( (char*) "   bitDepth :\t %d", (int)	pASI_camera_info->BitDepth );
	/* 
	*/

	name = string( pASI_camera_info->Name );
	while( name.find(" ") != string::npos )			name.replace( name.find(" "), 1, "_");


	setName( name );
	setDevName( pASI_camera_info->Name );
	
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
char * CameraZWO::str_erro( ASI_ERROR_CODE error )
{
	switch( (int)error )
	{
		case ASI_SUCCESS:								return (char*)"ASI_SUCCESS";
		case ASI_ERROR_INVALID_INDEX:					return (char*)"ASI_ERROR_INVALID_INDEX";
		case ASI_ERROR_INVALID_ID:						return (char*)"ASI_ERROR_INVALID_ID";
		case ASI_ERROR_INVALID_CONTROL_TYPE:			return (char*)"ASI_ERROR_INVALID_CONTROL_TYPE";
		case ASI_ERROR_CAMERA_CLOSED:					return (char*)"ASI_ERROR_CAMERA_CLOSED";
		case ASI_ERROR_CAMERA_REMOVED:					return (char*)"ASI_ERROR_CAMERA_REMOVED";
		case ASI_ERROR_INVALID_PATH:					return (char*)"ASI_ERROR_INVALID_PATH";
		case ASI_ERROR_INVALID_FILEFORMAT:				return (char*)"ASI_ERROR_INVALID_FILEFORMAT";
		case ASI_ERROR_INVALID_SIZE:					return (char*)"ASI_ERROR_INVALID_SIZE";
		case ASI_ERROR_INVALID_IMGTYPE:					return (char*)"ASI_ERROR_INVALID_IMGTYPE";
		case ASI_ERROR_OUTOF_BOUNDARY:					return (char*)"ASI_ERROR_OUTOF_BOUNDARY";
		case ASI_ERROR_TIMEOUT:							return (char*)"ASI_ERROR_TIMEOUT";
		case ASI_ERROR_INVALID_SEQUENCE:				return (char*)"ASI_ERROR_INVALID_SEQUENCE";
		case ASI_ERROR_BUFFER_TOO_SMALL:				return (char*)"ASI_ERROR_BUFFER_TOO_SMALL";
		case ASI_ERROR_VIDEO_MODE_ACTIVE:				return (char*)"ASI_ERROR_VIDEO_MODE_ACTIVE";
		case ASI_ERROR_EXPOSURE_IN_PROGRESS:			return (char*)"ASI_ERROR_EXPOSURE_IN_PROGRESS";
		case ASI_ERROR_GENERAL_ERROR:					return (char*)"ASI_ERROR_GENERAL_ERROR";
		case ASI_ERROR_INVALID_MODE:					return (char*)"ASI_ERROR_INVALID_MODE";
		case ASI_ERROR_GPS_NOT_SUPPORTED:				return (char*)"ASI_ERROR_GPS_NOT_SUPPORTED";
		case ASI_ERROR_GPS_VER_ERR:						return (char*)"ASI_ERROR_GPS_VER_ERR";
		case ASI_ERROR_GPS_FPGA_ERR:					return (char*)"ASI_ERROR_GPS_FPGA_ERR";
		case ASI_ERROR_GPS_PARAM_OUT_OF_RANGE:			return (char*)"ASI_ERROR_GPS_PARAM_OUT_OF_RANGE";
		case ASI_ERROR_GPS_DATA_INVALID:				return (char*)"ASI_ERROR_GPS_DATA_INVALID";
		case ASI_ERROR_END:								return (char*)"ASI_ERROR_END";
	}
	return (char*)"ASI_ERROR_CODE inconnu";
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
char * CameraZWO::str_bayer( ASI_BAYER_PATTERN bayer )
{
	switch( (int)bayer )
	{
		case ASI_BAYER_RG:								return (char*)"ASI_BAYER_RG";
		case ASI_BAYER_BG:								return (char*)"ASI_BAYER_BG";
		case ASI_BAYER_GR:								return (char*)"ASI_BAYER_GR";
		case ASI_BAYER_GB:								return (char*)"ASI_BAYER_GB";
	}
	return (char*)"ASI_BAYER_PATTERN inconnu";
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
char * CameraZWO::str_image( ASI_IMG_TYPE img )
{
	switch( (int)img )
	{
		case ASI_IMG_RAW8:								return (char*)"ASI_IMG_RAW8";
		case ASI_IMG_RGB24:								return (char*)"ASI_IMG_RGB24";
		case ASI_IMG_RAW16:								return (char*)"ASI_IMG_RAW16";
		case ASI_IMG_Y8:								return (char*)"ASI_IMG_Y8";
		case -1:										return (char*)"ASI_IMG_END";
	}
	return (char*)"ASI_IMG_TYPE inconnu";
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
char * CameraZWO::str_mode( ASI_CAMERA_MODE mode )
{
	switch( (int)mode )
	{
		case ASI_MODE_NORMAL:							return (char*)"ASI_MODE_NORMAL";
		case ASI_MODE_TRIG_SOFT_EDGE:					return (char*)"ASI_MODE_TRIG_SOFT_EDGE";
		case ASI_MODE_TRIG_RISE_EDGE:					return (char*)"ASI_MODE_TRIG_RISE_EDGE";
		case ASI_MODE_TRIG_FALL_EDGE:					return (char*)"ASI_MODE_TRIG_FALL_EDGE";
		case ASI_MODE_TRIG_SOFT_LEVEL:					return (char*)"ASI_MODE_TRIG_SOFT_LEVEL";
		case ASI_MODE_TRIG_HIGH_LEVEL:					return (char*)"ASI_MODE_TRIG_HIGH_LEVEL";
		case ASI_MODE_TRIG_LOW_LEVEL:					return (char*)"ASI_MODE_TRIG_LOW_LEVEL";
		case ASI_MODE_END:								return (char*)"ASI_MODE_END";
	}
	return (char*)"ASI_CAMERA_MODE inconnu";
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
char * CameraZWO::str_caps( ASI_CONTROL_TYPE type )
{
	switch( (int)type )
	{
		case ASI_GAIN:									return (char*)"ASI_GAIN";
		case ASI_EXPOSURE:								return (char*)"ASI_EXPOSURE";
		case ASI_GAMMA:									return (char*)"ASI_GAMMA";
		case ASI_WB_R:									return (char*)"ASI_WB_R";
		case ASI_WB_B:									return (char*)"ASI_WB_B";
		case ASI_BRIGHTNESS:							return (char*)"ASI_BRIGHTNESS";
		case ASI_BANDWIDTHOVERLOAD:						return (char*)"ASI_BANDWIDTHOVERLOAD";
		case ASI_OVERCLOCK:								return (char*)"ASI_OVERCLOCK";
		case ASI_TEMPERATURE:							return (char*)"ASI_TEMPERATURE";
		case ASI_FLIP:									return (char*)"ASI_FLIP";
		case ASI_AUTO_MAX_GAIN:							return (char*)"ASI_AUTO_MAX_GAIN";
		case ASI_AUTO_MAX_EXP:							return (char*)"ASI_AUTO_MAX_EXP";
		case ASI_AUTO_MAX_BRIGHTNESS:					return (char*)"ASI_AUTO_MAX_BRIGHTNESS";
		case ASI_HARDWARE_BIN:							return (char*)"ASI_HARDWARE_BIN";
		case ASI_HIGH_SPEED_MODE:						return (char*)"ASI_HIGH_SPEED_MODE";
		case ASI_COOLER_POWER_PERC:						return (char*)"ASI_COOLER_POWER_PERC";
		case ASI_TARGET_TEMP:							return (char*)"ASI_TARGET_TEMP";
		case ASI_COOLER_ON:								return (char*)"ASI_COOLER_ON";
		case ASI_MONO_BIN:								return (char*)"ASI_MONO_BIN";
		case ASI_FAN_ON:								return (char*)"ASI_FAN_ON";
		case ASI_PATTERN_ADJUST:						return (char*)"ASI_PATTERN_ADJUST";
		case ASI_ANTI_DEW_HEATER:						return (char*)"ASI_ANTI_DEW_HEATER";
	}
	return (char*)"ASI_CONTROL_TYPE inconnu";
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
char * CameraZWO::str_flip( ASI_FLIP_STATUS type )
{
	switch( (int)type )
	{
	case ASI_FLIP_NONE: 		return (char*)"ASI_FLIP_NONE";
	case ASI_FLIP_HORIZ:	  	return (char*)"ASI_FLIP_HORIZ";//: horizontal flip
	case ASI_FLIP_VERT: 		return (char*)"ASI_FLIP_VERT";// vertical flip
	case ASI_FLIP_BOTH: 		return (char*)"ASI_FLIP_BOTH";//:both horizontal and vertical flip
	}
	return (char*)"ASI_FLIP_STATUS inconnu";
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::print_mode( ASI_SUPPORTED_MODE* p )
{
	logf_thread( (char*)"Mode(s) camera supporté" );
	
	for( int i=0; i<16; i++ )
	{
		ASI_CAMERA_MODE cammode = p->SupportedCameraMode[i];
		logf_thread( (char*)" %d - %d:%s", i, (int)cammode, str_mode(cammode) ) ;//
		if ( (int)cammode == -1 )	break;
	}
	//logf_thread( (char*) " TriggerCam :\t %d", (int)pASI_camera_info->IsTriggerCam );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::print_images_spported( ASI_IMG_TYPE* p )
{
	logf_thread( (char*)"Mode(s) image supportees" );
	
	for( int i=0; i<8; i++ )
	{
		ASI_IMG_TYPE img = p[i];
		logf_thread( (char*)" %d - %d:%s", i, (int)img, str_image(img) ) ;//
		if ( (int)img == -1 )	break;
	}
	//logf_thread( (char*) " TriggerCam :\t %d", (int)pASI_camera_info->IsTriggerCam );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::init_zwo( int n )
{
	if ( n >= 0 )
	{
		iCamID = n;
		// GET CAMERA_INFO
		ASIGetCameraProperty( &ASI_camera_info, iCamID );
		iCamID = ASI_camera_info.CameraID;
		print_info( &ASI_camera_info );
		
		// taille mas de la capture
		width  = ASI_camera_info.MaxWidth;
		height = ASI_camera_info.MaxHeight;
		
		// Ouverture de la camera
		if(ASIOpenCamera(iCamID) != ASI_SUCCESS)
		{
			logf_thread( (char*)"[ ERREUR ] ASIOpenCamera error");
			iCamID = -1;
		    log_tab(false);
			return;
		}
		logf_thread( (char*)"OpenCamera OK");

		// GET CAMERA_MODE
		ASIGetCameraSupportMode( iCamID, &ASI_Supported_Mode );
		print_mode( &ASI_Supported_Mode );
		print_mode();
		print_images_spported( ASI_camera_info.SupportedVideoFormat );
		print_roi_format();

		// Init camera
		ASIInitCamera( iCamID );


		// Configuration ROI
		while(ASI_SUCCESS != ASISetROIFormat(iCamID, width, height, 1, (ASI_IMG_TYPE)ASI_IMG_RGB24));//IMG_RAW8

		
	}
	else
		logf_thread( (char*)"[ ERREUR ] No camera invalide !" );

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::close_zwo()
{
	if ( iCamID == -1 )	return;
	
	// Fermeture Camera
	ASICloseCamera(iCamID);

	iCamID = -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::coloriage( unsigned char* p )
{
	//return;
	
	if ( width == -1 || height == -1 )	return;
	
	long nb = iImgSize / 3;
	unsigned char R;
	for( long i=0; i<nb; i++ )
	{
		R = p[i*3 + 0];
		p[i*3 + 0] = p[i*3 + 2];
		p[i*3 + 2] = R;
		/*
		p[i*3 + 0] = 0;  // rouge
		p[i*3 + 1] = 0;  //vert
		p[i*3 + 2] = 0;  // bleu2
		*/
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::thread_capture_image()
{
    logf_thread( (char*)"[ THREAD ] \"%s\" start thread_capture_image ", name.c_str() );

	ASI_EXPOSURE_STATUS status;
	int iDropped = 0;

		
	ASISetControlValue(iCamID, ASI_GAIN, 			gain, ASI_FALSE);
	ASISetControlValue(iCamID, ASI_WB_R, 			wb_r, ASI_FALSE);
	ASISetControlValue(iCamID, ASI_WB_B, 			wb_b, ASI_FALSE);
	ASISetControlValue(iCamID, ASI_EXPOSURE,   		expo, ASI_FALSE);
	ASISetControlValue(iCamID, ASI_HARDWARE_BIN,  	 bin, ASI_FALSE);
	ASISetControlValue(iCamID, ASI_FLIP, 			flip, ASI_FALSE);

	//ASISetControlValue(iCamID, ASI_BANDWIDTHOVERLOAD, 40, ASI_FALSE);

	ASIStartExposure(iCamID, ASI_FALSE);
	usleep(10000);//10ms
	
	status = ASI_EXP_WORKING;
	
	logf_thread( (char*)"Capture sur %s ...", name.c_str() );
	while(status == ASI_EXP_WORKING)
	{
		ASIGetExpStatus(iCamID, &status);		
	}
	logf_thread( (char*)"Fin de Capture sur %s ...", name.c_str() );
	
	
	if(status == ASI_EXP_SUCCESS)
	{
		ASIGetDataAfterExp(iCamID, pImgBuf0, iImgSize);
		coloriage(pImgBuf0);
		logf_thread( (char*)"Capture OK" );
		bCharging = true;
		
		new_filename(".png");
		string s = dirname + filename;
		writeImage( (char*)s.c_str(), width, height, pImgBuf0, (char*)"TEST" );

	}

	// liberation des buffers
	//if ( pImgBuf0 )	free( pImgBuf0 );
	//pImgBuf0 = NULL;
		

    logf_thread( (char*)"[ THREAD ] \"%s\" stop thread_capture_image ", name.c_str() );
	bThreadCaptureImage = false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::thread_capture_video()
{
    logf_thread( (char*)"[ THREAD ] \"%s\" start thread_capture_video ", name.c_str() );
	ASI_EXPOSURE_STATUS status;
	int iDropped = 0;

	ASIStartVideoCapture(iCamID);
	usleep(10000);//10ms
	
	status = ASI_EXP_WORKING;

	//----------------------------------------------------------------------------	
	
	while( !bStopThreadVideo )
	{
		if ( bPause )	{ usleep(250); continue; }
		while( bCharging  );
		
		iCharging = (++iCharging) % 2;
		
		int i = iCharging;
		// ASICamera.h    line 657
	    int iTimeOut = expo*2+500;

		switch(i)	{
		case 0 :	
			{
			ASIGetVideoData(iCamID, pImgBuf0, iImgSize, iTimeOut );
			coloriage(pImgBuf0);
			break;
			}
		case 1 :
			{
			ASIGetVideoData(iCamID, pImgBuf1, iImgSize, iTimeOut );
			coloriage(pImgBuf1);
			break;
			}
		}

		bCharging = true;

		if ( bEnregistre )
		{
			new_filename(".png");
			string s = dirname + filename;
			
			unsigned char * p;
			
			p = ( i == 0 ? pImgBuf0 : pImgBuf1);
			
			writeImage( (char*)s.c_str(), width, height, p, (char*)"TEST" );
			bEnregistre = false;
		}
	}

	//----------------------------------------------------------------------------	


	ASIStopVideoCapture(iCamID);

	// liberation des buffers
	logf( (char*)"Liberation memoire .. %lX %lX", (long)pImgBuf0, (long)pImgBuf1 );
	if ( pImgBuf0 )	free( pImgBuf0 );
	if ( pImgBuf1 )	free( pImgBuf1 );
	pImgBuf0 = pImgBuf1 = NULL;
		
	

    logf_thread( (char*)"[ THREAD ] \"%s\" stop thread_capture_video ", name.c_str() );
	bThreadCaptureVideo = false;
}
/*
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::enregistre_name()
{
	enregistre();
}
*/
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::changeValueDoubleExpo( double val, PanelSpinEditText *pSpin )
{
    //logf( (char*)"CameraZWO::changeValueDouble(%lf, %s)", val, pSpin->getExtraString().c_str() );

	expo_val = val;
	
	if       ( expo_val < 1.0   && expo_unite != MICRO )		{ expo_val = 999.0; 	change_unite(false, pSpin); }
	else if  ( expo_val > 999.0 && expo_unite != SEC )			{ expo_val = 1.0;		change_unite(true,  pSpin); }
	
	if ( expo_val < 1.0 )			expo_val = 1.0;
	if ( expo_val > 999.0 )			expo_val = 999.0;
	
	pSpin->set_val( expo_val );
	
	expo = (long)expo_val;
	switch( expo_unite )
	{
	case MICRO:		expo *= 1;				break;
	case MILI:		expo *= 1000;			break;
	case SEC:		expo *= 1000000;		break;
	}
	
    //logf( (char*)"|  exposure = %ld", expo );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::changeValueDoubleClickExpo( double val, PanelSpinEditText *p )
{
	changeValueDoubleExpo(val, p);

	expo = (long)expo_val;
	switch( expo_unite )
	{
	case MICRO:		expo *= 1;				break;
	case MILI:		expo *= 1000;			break;
	case SEC:		expo *= 1000000;		break;
	}
    logf( (char*)"|  exposure = %ld", expo );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::change_unite( bool bPlus, PanelSpinEditText* pSpin )
{
    logf( (char*)"CameraZWO::change_unite(%s, %s)", BOOL2STR(bPlus), pSpin->getExtraString().c_str() );

	int i = expo_unite;
	if( bPlus )			i++;
	else				i--;
	
	if (i<0)		i = 0;
	if (i>2)		i = 2;
	
	expo_unite = (UNITE)i;
	switch( expo_unite )
	{
	case MICRO:
		pSpin->set_unite( sUnite_us );
		break;
	case MILI:
		pSpin->set_unite( sUnite_ms );
		break;
	case SEC:
		pSpin->set_unite( sUnite_sc );
		break;
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::enregistre()
{
	logf( (char*)"CameraZWO::enregistre()" );

	// Si capture video => on stope la capture
	if ( bThreadCaptureVideo )	{
		bEnregistre = true;
		return;
	}

	// Si capture image est déja lancé
	// pas de capture + message log
	if ( bThreadCaptureImage )
	{
		logf( (char*)"[ WARNING ] Photo en cours de traitement ..." );

		return;
	}

	iImgSize = width*height*(3);
	//if ( pImgBuf0 != NULL )		free( pImgBuf0 );
	if ( pImgBuf0 == NULL )	pImgBuf0 =  (unsigned char*)malloc( iImgSize );
	iCharging = 0;

	bStopThreadImage = false;
	bThreadCaptureImage = true;
    thread tCap = std::thread(&CameraZWO::thread_capture_image, this); 
    tCap.detach();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::video()
{
	logf( (char*)"CameraZWO::video()" );

	if ( bThreadCaptureVideo )
	{
		logf( (char*)"[ WARNING ] Arret de la Video ..." );
		
		bStopThreadVideo = true;
		iCharging = -1;
		return;
	}

	iImgSize = width*height*(3);
	logf( (char*)"Allocation memoire .. %lX %lX", (long)pImgBuf0, (long)pImgBuf1 );
	if ( pImgBuf0 == NULL )		pImgBuf0 =  (unsigned char*)malloc( iImgSize );
	if ( pImgBuf1 == NULL )		pImgBuf1 =  (unsigned char*)malloc( iImgSize );
	iCharging = 0;

	// ASI_FALSE ASI_TRUE
	ASISetControlValue(iCamID, ASI_GAIN, 			gain, ASI_FALSE);
	ASISetControlValue(iCamID, ASI_EXPOSURE,   		expo, ASI_FALSE);
	ASISetControlValue(iCamID, ASI_WB_R, 			wb_r, ASI_FALSE);
	ASISetControlValue(iCamID, ASI_WB_B, 			wb_b, ASI_FALSE);
	ASISetControlValue(iCamID, ASI_HARDWARE_BIN,  	 bin, ASI_FALSE);
	ASISetControlValue(iCamID, ASI_FLIP, 			flip, ASI_FALSE);

	bStopThreadVideo = false;
	bThreadCaptureVideo = true;
    thread tCap = std::thread(&CameraZWO::thread_capture_video, this); 
    tCap.detach();
}
//--------------------------------------------------------------------------------------------------------------------
//
//
// void CameraZWO::callback(bool b, int n, char* pStr )
//------------------------------------------------
//    fonction callback de filebrowser
//
//------------------------
// 
// n = -1     =>  bouton QUIT
//
// Voir fonction keyboard
//------------------------
// 		n = 10     =>  bouton OK   sur touche 'A'
// 		n = 11     =>  bouton OK   sur touche 'a'
//
//--------------------------------------------------------------------------------------------------------------------
void CameraZWO::callback(bool b, int n, char* pStr )
{
	logf( (char*)"CameraZWO::callback( %s, %d, %s )", BOOL2STR(b), n, (pStr==NULL ? (char*)"NULL": pStr)  );
	switch( n )
	{
	case FB_CALLBACK_KEY_s:
		logf( (char*)"Touche s" );
		break;
	case FB_CALLBACK_KEY_S:
		logf( (char*)"Touche S" );
		break;
	case FB_CALLBACK_KEY_QUIT:
		logf( (char*)"QUIT" );
		break;
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int CameraZWO::writeImage(char* filename, int width, int height, unsigned char *buffer, char* title)
{
 	logf( (char*)"CameraZWO::writeImage( %s )", filename );

	int code = 0;
   FILE *fp = NULL;
   png_structp png_ptr = NULL;
   png_infop info_ptr = NULL;
   png_bytep row = NULL;
   
      // Open file for writing (binary mode)
   fp = fopen(filename, "wb");
   if (fp == NULL) {
      fprintf(stderr, "Could not open file %s for writing\n", filename);
      code = 1;
      goto finalise;
   }

   // Initialize write structure
   png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if (png_ptr == NULL) {
      fprintf(stderr, "Could not allocate write struct\n");
      code = 1;
      goto finalise;
   }

   // Initialize info structure
   info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL) {
      fprintf(stderr, "Could not allocate info struct\n");
      code = 1;
      goto finalise;
   }


   // Setup Exception handling
   if (setjmp(png_jmpbuf(png_ptr))) {
      fprintf(stderr, "Error during png creation\n");
      code = 1;
      goto finalise;
   }

   png_init_io(png_ptr, fp);

   // Write header (8 bit colour depth)
   png_set_IHDR(png_ptr, info_ptr, width, height,
         8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
         PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

   // Set title
   if (title != NULL) {
      png_text title_text;
      title_text.compression = PNG_TEXT_COMPRESSION_NONE;
      title_text.key = (char*)"Title";
      title_text.text = title;
      png_set_text(png_ptr, info_ptr, &title_text, 1);
   }

   png_write_info(png_ptr, info_ptr);

   // Allocate memory for one row (3 bytes per pixel - RGB)
   //row = (png_bytep) malloc(3 * width * sizeof(png_byte));

   // Write image data
   	int x, y;
	for (y=0 ; y<height ; y++) {
		long offset = y * width * 3;
		png_write_row(png_ptr, buffer + offset);
	}

   // End write
   png_write_end(png_ptr, NULL);


   finalise:
   if (fp != NULL) fclose(fp);
   if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
   if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
   //if (row != NULL) free(row);

   return code;
}



