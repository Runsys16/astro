#include "camera.h"


#include "camera_mgr.h"
#include "file_browser.h"
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Camera::~Camera()
{
    logf( (char*)"Destructeur ~Camera() -----------" );
    bCharging = false;


    logf( (char*)"  join()" );
    bExitThread = true;
    while( bStartThread );
    logf( (char*)"  join() OK" );

    close_device();
    
    WindowsManager& vm = WindowsManager::getInstance();

    //panelPreview->deleteBackground();

    vm.sup( panelPreview );
    vm.sup( panelControl );
    vm.sup( panelControl );
    
    delete panelPreview;
    delete panelControl;
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Camera::Camera()
{
    logf((char*)"Constructeur Camera() -------------" );
    log_tab(true);
    //Device_cam();

    init();

    //CreatePreview();
    log_tab(false);
    logf((char*)"Constructeur Camera() -------------" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Camera::Camera(int w, int h)
{
    logf((char*)"Constructeur Camera(%d, %d) -------------", w, h);
    log_tab(true);
    //Device_cam();
    
    init();
    //setWidth(w);
    //setHeight(h);
    log_tab(false);
    logf((char*)"Constructeur Camera(%d, %d) -------------", w, h);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::init()
{
    logf((char*)"Camera::init() -------------");

    bCharging = true;
    bExitThread = false;
    vCameraSize.x = -1;
    vCameraSize.y = -1;
    
    previousTime = -1;
    bStartThread = false;

    nb_images   = 0;
    sSauveDir = "/home/rene/.astropilot/";
    
    start_thread();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int Camera::addControl()	{
    logf((char*)"Camera::addControl() ---------------");
    log_tab(true);
 
    int xt  = 10;
    int yt  = 10;
    int dyt = 16;
    int n = 0;
    string name;
    name = "Brightness";
    if ( isControl(name ) )
        createControlID(     panelControl, xt, yt + n++*dyt, (char*)name.c_str() );
    name = "Contrast";
    if ( isControl(name ) )
        createControlID(     panelControl, xt, yt + n++*dyt, (char*)name.c_str() );
    name = "Saturation";
    if ( isControl(name ) )
        createControlID(     panelControl, xt, yt + n++*dyt, (char*)name.c_str() );
    name = "Hue";
    if ( isControl(name ) )
        createControlID(     panelControl, xt, yt + n++*dyt, (char*)name.c_str() );
    name = "Gamma";
    if ( isControl(name ) )
        createControlID(     panelControl, xt, yt + n++*dyt, (char*)name.c_str() );
    name = "Sharpness";
    if ( isControl(name ) )
        createControlID(     panelControl, xt, yt + n++*dyt, (char*)name.c_str() );
    name = "Exposure, Auto";
    if ( isControl(name ) )
        createControlID(     panelControl, xt, yt + n++*dyt, (char*)name.c_str() );
    name = "Exposure (Absolute)";
    if ( isControl(name ) )
        createControlID(     panelControl, xt, yt + n++*dyt, (char*)name.c_str() );
    name = "Power Line";
    if ( isControl(name ) )
        createControlID(     panelControl, xt, yt + n++*dyt, (char*)name.c_str() );
    //createControlID(     panelControl, xt, yt + n++*dyt, (char*)"" );
    createControlIDbyID( panelControl, xt, yt + n++*dyt, (char*)"White Balance (auto)", 0x0098090C );
    createControlIDbyID( panelControl, xt, yt + n++*dyt, (char*)"White Balance", 0x0098091A );

    int dy = ++n*dyt;
    panelControl->setSize( 200, dy);
    
    logf( (char*)"dy=%d", dy);
    log_tab(false);
    
    return dy;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::createControlID(PanelSimple * p, int x, int y, char* str)	{
    logf( (char*)"Ajout de '%s'", str );

	p->add( new PanelTextOmbre( (char*)str,  	   PanelText::NORMAL_FONT, x, y ) );

    PanelText* pt = new PanelText( (char*)"     ", PanelText::NORMAL_FONT, x + 150, y );
    p->add( pt );

    Control* pControl = getControl(str);
    if (pControl)       {
        pControl->setPanelText(pt);
        char text[] = "0000000000000000";
        sprintf( text, "%d", pControl->getValue()); 
        pt->changeText(text);
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::createControlIDbyID(PanelSimple * p, int x, int y, char* str, int id)	{   
    logf( (char*)"Ajout de '%s'", str );
	p->add( new PanelTextOmbre( (char*)str,  	   PanelText::NORMAL_FONT, x, y ) );
    PanelText* pt = new PanelText( (char*)"     ", PanelText::NORMAL_FONT, x + 150, y );
    p->add( pt );

    Control* pControl = getControl(id);
    if (pControl)       {
        pControl->setPanelText(pt);
        char text[] = "0000000000000000";
        sprintf( text, "%d", pControl->getValue()); 
        pt->changeText(text);
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::CreateControl()	{
    logf((char*)"Camera::CreateControl() --------------- %s", getDevName() );
    log_tab(true);
    
    if ( getFd() == -1 )
    {
        logf((char*)"Le materiel n'est pas encore ouvert");
    }

    panelControl = new PanelWindow();
    panelControl->setDisplayGL(displayGLnuit_cb);
    
	WindowsManager& wm = WindowsManager::getInstance();
	int wsc = wm.getWidth();
	int hsc = wm.getHeight();
    
    
    int dx = 200;
    int dy = 150;
    int x = wsc - dx - 20;
    int y = 20;

    panelControl->setPosAndSize( x, y, dx, dy);
    
    ///home/rene/programmes/opengl/video/essai
    wm.add(panelControl);
    panelControl->setBackground((char*)"images/background.tga");
    
    dy = addControl();
    resizeControlFirst(wsc, hsc, dx, dy);

    log_tab(false);
    logf((char*)"Camera::CreateControl() --------------- %s", getDevName() );
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
        thread_chargement = thread(&Camera::threadExtractImg, this);
        thread_chargement.detach();

        bStartThread = true;
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::CreatePreview()	{
    logf((char*)"CreatePreview ------------- %s", getName() );
    log_tab(true);

	WindowsManager& wm = WindowsManager::getInstance();

	//panelPreview = new PanelSimple();
	panelPreview = new PanelCamera();
    //panelPreview->setDisplayGL(displayGLnuit_cb);

	int wsc = wm.getWidth();
	int hsc = wm.getHeight();
    logf((char*)"wsc=%d hsc=%d", wsc, hsc);
	resizePreview(wsc, hsc);
	//panelPreview->setBackground( (char*)"frame-0.raw");
	
	//pCamFilename = new PanelText( (char*)"frame-0.raw",		PanelText::LARGE_FONT, 20, 10 );
	pCamFilename = new PanelText( (char*)getDevName(),		PanelText::LARGE_FONT, 20, 10 );
	panelPreview->add( pCamFilename );
	
	pNbStars = new PanelText( (char*)"0",		PanelText::LARGE_FONT, wsc-50, 10 );
	panelPreview->add( pNbStars );
	panelPreview->getStars()->setPanelNbStars( pNbStars );
	
	
	//string pStr = getName();
	PanelTextOmbre* pTO = new PanelTextOmbre( (char*)getName(),	PanelText::LARGE_FONT, 0, 10 );
	pTO->setAlign( PanelText::CENTER );
	panelPreview->add( pTO );
	

	//panelPreview->setCanMove(false);
 	wm.add( panelPreview );

    logf((char*)"    name    %s ", getName() );
    logf((char*)"    devname %s ", getDevName() );
    logf((char*)"    %d,%d %dx%d", 0, 0, getWidth(), getHeight());

    log_tab(false);
    logf((char*)"CreatePreview ------------- %s", getName() );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::resizeControl(int width, int height)	{
    logf((char*) "Camera::resizeControl(%d, %d) %s", width, height, getName() );
    int dx = panelControl->getDX();
    int dy = panelControl->getDY();
    
    //if ( dx==0)     return;
    dx = 200;
    dy = 192;
    
    int x = width - dx - 20;

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

    dx = 200;
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
void Camera::resizePreview(int width, int height)	{
    logf((char*) "Camera::resizePreview(%d, %d) %s", width, height, getName() );
	WindowsManager& wm = WindowsManager::getInstance();

	int wsc = width;
	int hsc = height;


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

	
	panelPreview->setPosAndSize( xCam, yCam, dxCam, dyCam);
	//panelPreview->setDisplayGL(displayGLCamera_cb);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::fullSizePreview(int width, int height)	{
    logf((char*) "Camera::fullSizePreview(%d, %d) %s ", width, height, getName() );
	WindowsManager& wm = WindowsManager::getInstance();

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

	panelPreview->setPosAndSize( xCam, yCam, dxCam, dyCam);
	//panelPreview->setDisplayGL(displayGLCamera_cb);
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
    logf((char*)"[THREAD] Camera::threadExtractImg() start %s", (char*)getName());

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
    logf((char*)"[THREAD] Camera::threadExtractImg()  stop %s", (char*)getName());
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
        panelPreview->deleteBackground();
        try
        {
            //panelPreview->setBackground( getBuffer(), vCameraSize.x, vCameraSize.y, 3);
            readBgr.ptr = getBuffer();
            readBgr.w = vCameraSize.x;
            readBgr.h = vCameraSize.y;
            readBgr.d = 3;
             
            panelPreview->setBackground( readBgr.ptr, readBgr.w, readBgr.h, readBgr.d);
            panelPreview->setRB( &readBgr);
            //logf((char*)" %s : %ld", getName(), getBuffer() );
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
            //logf((char*)"Camera::change_background()   Hz=%.0f %s", hz, getName() );
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
    panelPreview->setVisible( b );
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
    panelControl->setVisible( !panelControl->getVisible() );
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
    xCam = panelPreview->getPosX();
    return xCam;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int Camera::get_yCam()
{
    yCam = panelPreview->getPosY();
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
vec2* Camera::getSuivi()
{
    if ( panelPreview!=NULL )           return panelPreview->getSuivi();
    return NULL;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::setColor( long color )
{
    //panelPreview->setColor( color );
    panelControl->setColor( color );
}


//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

