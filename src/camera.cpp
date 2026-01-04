#include "camera.h"


#include "camera_mgr.h"
#include "file_browser.h"
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Camera::~Camera()
{
    logf_thread( (char*)"Destructeur ~Camera() -----------" );
    log_tab(true);
    bCharging = false;


    logf_thread( (char*)"fermeture thread (lecture image) join()" );
    bExitThread = true;
    while( bStartThread );
    logf_thread( (char*)"join() OK" );

    close_device();
    
    WindowsManager& vm = WindowsManager::getInstance();

    //panelCamera->deleteBackground();

	if ( panelCamera != NULL )	{
		vm.sup( panelCamera );
	    delete panelCamera;
    }
    
	if ( panelControl != NULL )	{
		vm.sup( panelControl );
		delete panelControl;
	}
    log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Camera::Camera()
{
    logf_thread((char*)"Constructeur Camera() -------------" );
    log_tab(true);
    //Device_cam();

    init();

    //CreatePreview();
    log_tab(false);
    //logf_thread((char*)"Constructeur Camera() ---- END ----" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Camera::Camera(int w, int h)
{
    logf_thread((char*)"Constructeur Camera(%d, %d) -------------", w, h);
    log_tab(true);
    //Device_cam();
    
    init();
    //setWidth(w);
    //setHeight(h);
    log_tab(false);
    //logf_thread((char*)"Constructeur Camera(%d, %d) ---- END ----", w, h);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::init()
{
    //logf_thread((char*)"Camera::init() -------------");

	hz					= 0.0;

    bCharging			= true;
    bExitThread			= false;
    vCameraSize.x		= -1;
    vCameraSize.y		= -1;
    
    previousTime		= -1;
    bStartThread		= false;
    bPanelControl		= false;

    nb_images			= 0;
    sSauveDir			= "/home/rene/.astropilot/";
    
	panelCamera			= NULL;
	panelControl		= NULL;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int Camera::addControlName( const string& sName, int xt, int yt )	{

    if ( isControl(sName ) )
    {
		createControlIDminmax(     panelControl, xt, yt, (char*)sName.c_str() );
		//createControlID(     panelControl, xt, yt, (char*)sName.c_str() );
		return 1;
	}
	return 0;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int Camera::addControl()	{
    logf_thread((char*)"Camera::addControl() ---------------");
    log_tab(true);
 
    int xt  = 10;
    int yt  = 10;
    int dyt = 16;
    int n = 0;

    n += addControlName( string("Brightness"), 			xt, yt + n*dyt );
    n += addControlName( string("Contrast"), 			xt, yt + n*dyt );
    n += addControlName( string("Saturation"), 			xt, yt + n*dyt );
    n += addControlName( string("Hue"),					xt, yt + n*dyt );
    n += addControlName( string("Gamma"),				xt, yt + n*dyt );
    n += addControlName( string("Sharpness"),			xt, yt + n*dyt );
    n += addControlName( string("Exposure, Auto"),		xt, yt + n*dyt );
    n += addControlName( string("Exposure (Absolute)"),	xt, yt + n*dyt );
    n += addControlName( string("Power Line"),			xt, yt + n*dyt );
    n += addControlName( string("Pan"),					xt, yt + n*dyt );
    n += addControlName( string("Backlight"),			xt, yt + n*dyt );
    n += addControlName( string("Auto Exposure"),		xt, yt + n*dyt );
    n += addControlName( string("Exposure Time"),		xt, yt + n*dyt );
    n += addControlName( string("Tilt"),				xt, yt + n*dyt );
    n += addControlName( string("Zoom"),				xt, yt + n*dyt );
	n += addControlName( string("White"),				xt, yt + n*dyt );
    n += addControlName( string("Temperature"),			xt, yt + n*dyt );

    int dy = ++n*dyt;
    panelControl->setSize( 200+150, dy);
    
    logf_thread( (char*)"dy=%d", dy);
    log_tab(false);
    
    return dy;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::changeValueDouble( double val, void *p )	{
	//logf( (char*)"changeValueDouble %lf", val );
	Control* pControl = (Control*)p;
	pControl->setValue( (int)val );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::createControlID(PanelSimple * p, int x, int y, char* str)	{
    //logf( (char*)"Ajout de '%s'", str );

	p->add( new PanelTextOmbre( (char*)str,  	   PanelText::NORMAL_FONT, x, y ) );

    Control* pControl = getControl(str);
    
    if (pControl)       {

        PanelSpinEditText* pSET = new PanelSpinEditText();
        
        double min = pControl->getMin();
        double max = pControl->getMax();
        double step = pControl->getStep();
        
        pSET->set( min, max, max-min, 1 );
        pSET->set_ndecimal(0);
        pSET->set_delta( 20, 8 );
        pSET->setPos( x+150, y );
        pSET->setSize( 90, 14 );
        pSET->setExtraString( "Control" );
        pSET->setChangeValue( (ChangeValue*)this );
        pSET->setID( (void*)pControl );
        pSET->set_val( pControl->getValue() );
        p->add(pSET);

	    logf_thread( (char*)"Ajout de '%s'  %.2f>%.2f/%.2f = %.2f", str, (float)min, (float)max, (float)step, (float)pControl->getValue() );
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::createControlIDminmax(PanelSimple * p, int x, int y, char* str0)	{   
	p->add( new PanelTextOmbre( (char*)str0,  	   PanelText::NORMAL_FONT, x, y ) );

    Control* pControl = getControl(str0);

    if (pControl)       {

        PanelSpinEditText* pSET = new PanelSpinEditText();
        
        double min = pControl->getMin();
        double max = pControl->getMax();
        double step = pControl->getStep();
        
        pSET->set( min, max, max-min, 1 );
        pSET->set_ndecimal(0);
        pSET->set_delta( 20, 8 );
        pSET->setPos( x+150+150, y );
        pSET->setSize( 90, 14 );
        pSET->setExtraString( "Control" );
        pSET->setChangeValue( (ChangeValue*)this );
        pSET->setID( (void*)pControl );
        pSET->set_val( pControl->getValue() );
        p->add(pSET);

	    logf_thread( (char*)"Ajout de '%s'  %.2f>%.2f/%.2f = %.2f", str0, (float)min, (float)max, (float)step, (float)pControl->getValue() );

		char str1[256];
		//snprintf( (char*)str1, sizeof(str1), "%.0lf~%.0lf\t\ %.0lf", min, max, step );
		snprintf( (char*)str1, sizeof(str1), "%.0lf~%.0lf\t %.0lf", min, max, step );
		PanelText* pp = new PanelText( (char*)"  ", PanelText::NORMAL_FONT, x + 150, y );
		pp->razTabSize();
		pp->setTabSize(85);
		pp->setColor(0x808080ff);
		pp->changeText( (char*)str1 );
		p->add( pp );
	
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::CreateControl()	{
    logf_thread((char*)"Camera::CreateControl() --------------- %s", getDevName() );
    log_tab(true);
    
    if ( getFd() == -1 )
    {
        logf_thread((char*)"Le materiel n'est pas encore ouvert");
    }

    panelControl = new PanelWindow();
    panelControl->setDisplayGL(displayGLnuit_cb);
    panelControl->setExtraString( getDevName() );
    panelControl->loadSkinPath( "images/astro" );
	panelControl->setBorderSize(8);

	WindowsManager& wm = WindowsManager::getInstance();
	int wsc = wm.getWidth();
	int hsc = wm.getHeight();
    
    
    int dx = 200 +150;
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
    logf_thread((char*)"Camera::CreateControl() --------------- %s", getDevName() );
    panelControl->setVisible(false);
    //logf((char*)"panelControl  %d,%d %dx%d\n", x, y, dx, dy);   
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::start_thread()
{
    if ( !bStartThread )
    {
	    logf_thread((char*)"[THREAD] Camera::start_thread() start %s", (char*)getName().c_str());
        thread_chargement = thread(&Camera::threadExtractImg, this);
        thread_chargement.detach();

    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::CreatePreview()	{
    logf_thread((char*)"Camera::CreatePreview ------------- %s", getName().c_str() );
    log_tab(true);

	WindowsManager& wm = WindowsManager::getInstance();

	//panelCamera = new PanelSimple();
	panelCamera = new PanelCamera(this);
    panelCamera->setExtraString( "PanelCamera View");
    panelCamera->setExtraString( getName().c_str() );

	int wsc = wm.getWidth();
	int hsc = wm.getHeight();
    logf((char*)"wsc=%d hsc=%d", wsc, hsc);
	iconSizePreview(wsc, hsc);
	//panelCamera->setBackground( (char*)"frame-0.raw");
	
	//pCamFilename = new PanelText( (char*)"frame-0.raw",		PanelText::LARGE_FONT, 20, 10 );
	pCamFilename = new PanelText( (char*)getDevName(),		PanelText::LARGE_FONT, 20, 10 );
	panelCamera->add( pCamFilename );
	
	pNbStars = new PanelText( (char*)"0",		PanelText::LARGE_FONT, wsc-50, 10 );
	panelCamera->add( pNbStars );
	panelCamera->getStars()->setPanelNbStars( pNbStars );
	
	//string pStr = getName().c_str();
	pPanelName = new PanelText( (char*)getName().c_str(),	PanelText::LARGE_FONT, 0, 10 );
	pPanelName->setAlign( PanelText::CENTER );
	panelCamera->add( pPanelName );

	//panelCamera->setCanMove(false);
 	wm.add( panelCamera );

    logf_thread((char*)"    name    %s ", getName().c_str() );
    logf_thread((char*)"    devname %s ", getDevName() );
    logf_thread((char*)"    %d,%d %dx%d", 0, 0, getWidth(), getHeight());

    log_tab(false);
    logf_thread((char*)"Camera::CreatePreview ------------- %s", getName().c_str() );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::resizeControl(int width, int height)	{
    logf((char*) "Camera::resizeControl(%d, %d) %s", width, height, getName().c_str() );
    int dx = panelControl->getDX();
    int dy = panelControl->getDY();
    
    //if ( dx==0)     return;
    dx = 200+150;
    dy = 192;
    
    //int x = width - dx - 20;
    int x = 20;

    int nb = Camera_mgr::getInstance().getNum(this);
    int y = 10+nb * (20+dy);

    logf((char*) "                          x=%d y=%d nb=%d", x, y, nb );
    panelControl->setPos( x, y );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::resizeControlFirst(int width, int height, int dx, int dy)	{
    logf((char*) "Camera::resizeControlFirst(%d, %d, %d, %d) %s", width, height, dx, dy, getDevName() );
    
    //if ( dx==0)     return;

    dx = 200+150;
    dy = 192;

    int x = width - dx - 20;

    int nb = Camera_mgr::getInstance().getSize();
    int y = 10+nb * (20+dy);

    logf((char*) "x=%d y=%d nb=%d %s", x, y, nb, getDevName() );
    panelControl->setPos( x, y );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::iconSizePreview(int width, int height)	{
    logf((char*) "Camera::iconSizePreview(%d, %d) %s", width, height, (char*)getDevName() );
	WindowsManager& wm = WindowsManager::getInstance();

	int wsc = width;
	int hsc = height;

	bFullScreen	= false;
	bIconized	= true;

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

    
    dxCam /= 8;
    dyCam /= 8;

    logf((char*) "   Screen  : %dx%d", wsc, hsc);
    logf((char*) "   Preview : %d,%d %dx%d", xCam, yCam, dxCam, dyCam);

	
	panelCamera->setPosAndSize( xCam, yCam, dxCam, dyCam);
	panelCamera->raz_ech_usr();
	panelCamera->compute_echelle();
	
	if ( panelControl != NULL )			panelControl->setVisible(false);
	//panelCamera->setDisplayGL(displayGLCamera_cb);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::fullSizePreview(int width, int height)	{
    logf((char*) "Camera::fullSizePreview(%d, %d) %s ", width, height, (char*)getName().c_str() );
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
bool*     pCharging;
//
void charge_camera()
{
    write_image();

    *pCharging = true;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::threadExtractImg()
{

    bStartThread = true;
    while( true )
    {
        
        if ( ( !bPause || bOneFrame ) && bCharging == false ) 
        {
            mainloop();
            if ( getFd() != -1 )            bCharging = true;
            bOneFrame = false;
        }
        
        if ( bExitThread )  break;
    }
    logf_thread((char*)"[THREAD] Camera::threadExtractImg()  stop %s", (char*)getName().c_str());
    bStartThread = false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::change_background(void)
{

    //log((char*)"START Camera::change_background()");
    if ( vCameraSize.x == -1 )
    {
        vCameraSize.x = getWidth();
        vCameraSize.y = getHeight();
        vCameraSize = get_vCameraSize();

        logf((char*)" vCameraSize.x=%d vCameraSize.y=%d", vCameraSize.x, vCameraSize.y);
    }
    //bFreePtr = false;
    if ( bCharging )
    {
        //log((char*)"START Camera::change_background()");
        panelCamera->deleteBackground();
        try
        {
            //panelCamera->setBackground( getBuffer(), vCameraSize.x, vCameraSize.y, 3);
            readBgr.ptr = getBuffer();
            readBgr.w = vCameraSize.x;
            readBgr.h = vCameraSize.y;
            readBgr.d = 3;
             
            panelCamera->setBackground( readBgr.ptr, readBgr.w, readBgr.h, readBgr.d);
            panelCamera->setRB( &readBgr);
            //logf((char*)" %s : %ld", getName().c_str(), getBuffer() );
            //if (bSuivi)    suivi();
        }
        catch(std::exception const& e)
        {
            log((char*)"ERROR --- Camera::change_background()");
            cerr << "ERREUR : " << e.what() << endl;
        }

        //pCamFilename->changeText( (char*)getDevName() );

        //pCharging = &bCharging;
        //startThread = true;
        
        float t = Timer::getInstance().getCurrentTime();
        if ( previousTime != -1 )
        {
            if ( nb_images++ >= 10 )
            {
                hz = 10.0 / (t - previousTime);
                previousTime = t;
                nb_images = 0;
            }
            //logf((char*)"Camera::change_background()   Hz=%.0f %s", hz, getName().c_str() );
        }
        else
            previousTime = t;
        
        bCharging = false;
     }
    //log((char*)"END   Camera::change_background()");
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool Camera::keyboard(char key)
{
return false;
    //std::cout << (int)key << std::endl;
	Control* pControl;
	if ( !panelControl->getVisible() )       return false;
	
	switch(key){ 
	
    case 'E':
        pControl = getControl("Exposure (Absolute)");
        if (pControl)       pControl->plus();
        break;
    case 'e':    
        pControl = getControl("Exposure (Absolute)");
        if (pControl)       pControl->moins();
        break;
    case 'D':
        pControl = getControl("Exposure, Auto");
        if (pControl)       pControl->plus();
        else                log( (char*)"Control NULL" );
        break;
    case 'd':    
        pControl = getControl("Exposure, Auto");
        if (pControl)       pControl->moins();
        break;
    case 'B':
        pControl = getControl("Bright");
        if (pControl)       pControl->plus();
        break;
    case 'b':    
        pControl = getControl("Bright");
        if (pControl)       pControl->moins();
        break;
    case 'C':
        pControl = getControl("Contras");
        if (pControl)       pControl->plus();
        break;
    case 'c':
        pControl = getControl("Contras");
        if (pControl)       pControl->moins();
        break;
    case 'H':
        pControl = getControl("Hue");
        if (pControl)       pControl->plus();
        break;
    case 'h':
        pControl = getControl("Hue");
        if (pControl)       pControl->moins();
        break;
    case 'G':
        pControl = getControl("Gamma");
        if (pControl)       pControl->plus();
        break;
    case 'g':
        pControl = getControl("Gamma");
        if (pControl)       pControl->moins();
        break;
    case 'Z':
        pControl = getControl("Sharp");
        if (pControl)       pControl->plus();
        break;
    case 'z':
        pControl = getControl("Sharp");
        if (pControl)       pControl->moins();
        break;
    case 'S':
        pControl = getControl("Satu");
        if (pControl)       pControl->plus();
        break;
    case 's':
        pControl = getControl("Satu");
        if (pControl)       pControl->moins();
        break;
    case 'W':
        pControl = getControl(0x0098091A);
        if (pControl)       pControl->plus();
        break;
    case 'w':
        pControl = getControl(0x0098091A);
        if (pControl)       pControl->moins();
        break;
    case 'X':
        pControl = getControl(0x0098090C);
        if (pControl)       pControl->plus();
        break;
    case 'x':
        pControl = getControl(0x0098090C);
        if (pControl)       pControl->moins();
        break;
    case 'l':  // '-'
        capability_list();
        break;

    case 'A':  // '-'
        //capability_save();
        logf( (char*)"capability_save()" );
        FileBrowser::getInstance().setFiltre(".cam");
        FileBrowser::getInstance().setFilename("");
        setExtra(10);
        FileBrowser::getInstance().setNewline(true);
        FileBrowser::getInstance().change_dir(sSauveDir);
        FileBrowser::getInstance().setCallBack(this);
        FileBrowser::getInstance().affiche();
        break;

    case 'a':  // '-'
        //capability_load();
        logf( (char*)"capability_load()" );
        FileBrowser::getInstance().setFiltre(".cam");
        setExtra(11);
        FileBrowser::getInstance().setNewline(true);
        FileBrowser::getInstance().change_dir(sSauveDir);
        FileBrowser::getInstance().setCallBack(this);
        FileBrowser::getInstance().affiche();
        break;

    default:
        return false;        
	}
	return true;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::setVisible(bool b)
{
    //log((char*)"Camera::setVisible()");
    panelCamera->setVisible( b );
    if ( bPanelControl )	panelControl->setVisible( b );
    //log((char*)"Camera::setVisible()");
    //panelControl->setVisible( b );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::setControlVisible(bool b)
{
    panelControl->setVisible( b );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::togglePanel()
{
	bPanelControl = ! bPanelControl;
    panelControl->setVisible( bPanelControl );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool Camera::haveNewFrame()
{
    return getHaveNew();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::haveUseFrame(bool)
{
    return setHaveNew(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
GLubyte* Camera::getPtr()
{
    return (GLubyte*)getBuffer();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int Camera::get_xCam()
{
    xCam = panelCamera->getPosX();
    return xCam;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int Camera::get_yCam()
{
    yCam = panelCamera->getPosY();
    return yCam;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int Camera::get_dxCam()
{
    return dxCam;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int Camera::get_dyCam()
{
    return dyCam;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
ivec2 Camera::get_vCameraSize()
{
    return vCameraSize;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::recentreSuivi()
{
    if ( panelCamera!=NULL )		panelCamera->recentreSuivi();
    else
    	logf( (char*)"Pas de panel preview !!");
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
vec2* Camera::getSuivi()
{
    if ( panelCamera!=NULL )           return panelCamera->getSuivi();
    return NULL;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::setColor( long color )
{
    panelCamera->setColor( color );
    panelControl->setColor( color );
}
//--------------------------------------------------------------------------------------------------------------------
//
// Compare les etoiles trouvées par le programme
// aux etoiles du catalogue vizier
// Affiche le resultat dans la console
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::compareStar()
{
   	log( (char*)"Camera::compareStar()");
   	log_tab(true);

	starCompare.setStars(  panelCamera->getStars() );
	starCompare.setVizier( &vizier);

	starCompare.compareStar();

	double d = starCompare.computeDelta();
	panelCamera->getStars()->setDelta( d );
   	log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
// Compare les etoiles trouvées par le programme
// aux etoiles du catalogue vizier
// Affiche le resultat dans la console
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::saveCompareStar()
{
   	log( (char*)"PanelCapture::saveCompareStar()");

	starCompare.setStars(  panelCamera->getStars() );
	starCompare.setVizier( &vizier);

	starCompare.saveCompareStar();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

