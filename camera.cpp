#include "camera.h"


#include "camera_mgr.h"

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Camera::~Camera()
{
    logf( (char*)"----------- Destructeur ~Camera() -----------" );
    bChargingCamera = false;

    close_device();

    logf( (char*)"  join()" );
    while( startThread );

    logf( (char*)"  join() OK" );
    
    WindowsManager& vm = WindowsManager::getInstance();

    panelPreview->deleteBackground();

    vm.sup( panelPreview );
    vm.sup( panelControl );
    
    delete panelPreview;
    delete panelControl;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Camera::Camera()
{
    logf((char*)"----------- Constructeur Camera() -------------" );
    Device_cam();

    init();

    //CreatePreview();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Camera::Camera(int w, int h)
{
    logf((char*)"----------- Constructeur Camera(%d, %d) -------------", w, h);
    Device_cam();
    
    init();
    setWidth(w);
    setHeight(h);
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::init()
{
    logf((char*)"Camera::init() -------------");

    bChargingCamera = true;
    vCameraSize.x = -1;
    vCameraSize.y = -1;
    
    previousTime = -1;
    startThread = false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::CreatePreview()	{
    logf((char*)"----------- CreatePreview ------------- %s", getName() );

	WindowsManager& wm = WindowsManager::getInstance();

	//panelPreview = new PanelSimple();
	panelPreview = new PanelWindow();

	int wsc = wm.getWidth();
	int hsc = wm.getHeight();
    logf((char*)"    wsc=%d hsc=%d", wsc, hsc);
	resizePreview(wsc, hsc);
	//panelPreview->setBackground( (char*)"frame-0.raw");
	
	//pCamFilename = new PanelText( (char*)"frame-0.raw",		PanelText::LARGE_FONT, 20, 10 );
	pCamFilename = new PanelText( (char*)getDevName(),		PanelText::LARGE_FONT, 20, 10 );
	panelPreview->add( pCamFilename );
	
	
	//string pStr = getName();
	PanelTextOmbre* pTO = new PanelTextOmbre( getName(),	PanelText::LARGE_FONT, 0, 10 );
	pTO->setAlign( PanelText::CENTER );
	panelPreview->add( pTO );
	

	//panelPreview->setCanMove(false);
 	wm.add( panelPreview );

    logf((char*)"    name %s ", getName() );
    logf((char*)"    %d,%d %dx%d", 0, 0, getWidth(), getHeight());
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

    int nb = Camera_mgr::getInstance().getNum(this);
    int y = 10+nb * (20+dy);

    logf((char*) "Camera::resizeControlFirst x=%d y=%d nb=%d %s", x, y, nb, getDevName() );
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

    
    dxCam /= 4;
    dyCam /= 4;

    logf((char*) "   Screen  : %dx%d", wsc, hsc);
    logf((char*) "   Preview : %d,%d %dx%d", xCam, yCam, dxCam, dyCam);

	
	panelPreview->setPosAndSize( xCam, yCam, dxCam, dyCam);
	panelPreview->setDisplayGL(displayGL_cb);
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
	panelPreview->setDisplayGL(displayGL_cb);
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
int Camera::addControl()	{
    logf((char*)"------------- Camera::addControl() ---------------");
 
    int xt  = 10;
    int yt  = 10;
    int dyt = 16;
    int n = 0;

    createControlID(     panelControl, xt, yt + n++*dyt, (char*)"Brightness" );
    createControlID(     panelControl, xt, yt + n++*dyt, (char*)"Contrast" );
    createControlID(     panelControl, xt, yt + n++*dyt, (char*)"Saturation" );
    createControlID(     panelControl, xt, yt + n++*dyt, (char*)"Hue" );
    createControlID(     panelControl, xt, yt + n++*dyt, (char*)"Gamma" );
    createControlID(     panelControl, xt, yt + n++*dyt, (char*)"Sharpness" );
    createControlID(     panelControl, xt, yt + n++*dyt, (char*)"Exposure, Auto" );
    createControlID(     panelControl, xt, yt + n++*dyt, (char*)"Exposure (Absolute)" );
    createControlID(     panelControl, xt, yt + n++*dyt, (char*)"Power Line" );
    createControlIDbyID( panelControl, xt, yt + n++*dyt, (char*)"White Balance (auto)", 0x0098090C );
    createControlIDbyID( panelControl, xt, yt + n++*dyt, (char*)"White Balance", 0x0098091A );

    int dy = ++n*dyt;
    panelControl->setSize( 200, dy);
    
    logf( (char*)"dy=%d", dy);
    return dy;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::CreateControl()	{
    logf((char*)"------------- Camera::CreateControl() --------------- %s", getDevName() );
    if ( getFd() == -1 )
    {
        logf((char*)"Le materiel n'est pas encore ouvert");
    }

    panelControl = new PanelWindow();
    
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
    panelControl->setBackground((char*)"background.tga");
    
    dy = addControl();
    resizeControlFirst(wsc, hsc, dx, dy);

    //logf((char*)"panelControl  %d,%d %dx%d\n", x, y, dx, dy);   
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
    //logf((char*)"Camera::threadExtractImg() start %s", (char*)getName());
    mainloop();

    if ( getFd() != -1 )            bChargingCamera = true;
    
    //logf((char*)"Camera::threadExtractImg()  stop %s", (char*)getName());
    startThread = false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
std::thread Camera::memberThread()
{
    //logf((char*)"Camera::memberThread() %s", (char*) getName() );
    return std::thread(&Camera::threadExtractImg, this); 
}    
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::change_background_camera(void)
{

    //log((char*)"START Camera::change_background_camera()");
    if ( vCameraSize.x == -1 )
    {
        vCameraSize.x = getWidth();
        vCameraSize.y = getHeight();
        logf((char*)" vCameraSize.x=%d vCameraSize.y=%d", vCameraSize.x, vCameraSize.y);
    }
    //bFreePtr = false;
    if ( bChargingCamera )
    {
        panelPreview->deleteBackground();
        try
        {
            panelPreview->setBackground( getBuffer(), vCameraSize.x, vCameraSize.y, 3);
            //logf((char*)" %s : %ld", getName(), getBuffer() );
            //if (bSuivi)    suivi();
        }
        catch(std::exception const& e)
        {
            log((char*)"ERROR --- Camera::change_background_camera()");
            cerr << "ERREUR : " << e.what() << endl;
        }

        //pCamFilename->changeText( (char*)getDevName() );

        pCharging = &bChargingCamera;
        startThread = true;
        thread_chargement_camera = memberThread();
        thread_chargement_camera.detach();
        
        float t = Timer::getInstance().getCurrentTime();
        if ( previousTime != -1 )
        {
            hz = 1.0 / (t - previousTime);
            //logf((char*)"Camera::change_background_camera()   Hz=%.0f %s", hz, getName() );
        }
        previousTime = t;
        
        bChargingCamera = false;
     }
    //log((char*)"END   Camera::change_background_camera()");
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
void Camera::keyboard(char key)
{
    //std::cout << (int)key << std::endl;
	Control* pControl;
	
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
        
	}
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
    return xCam;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int Camera::get_yCam()
{
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





