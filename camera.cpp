#include "camera.h"

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Camera::Camera()
{
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
    
    CreatePreview();
    CreateControl();
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
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::CreatePreview()	{
    log((char*)"----------- CreatePreview -------------");

	WindowsManager& wm = WindowsManager::getInstance();
	//wm.setScreenSize( getWidth(), height );
    //setWidth(950);
    //setHeight(534);

	panelPreview = new PanelSimple();
	resizePreview(getWidth(), getHeight());
	panelPreview->setBackground( (char*)"frame-0.raw");
	
	pCamFilename = new PanelText( (char*)"frame-0.raw",		PanelText::LARGE_FONT, 20, 10 );
	panelPreview->add( pCamFilename );

    string  pStr;
	
	pStr = "";//
	//pStr = getName();
	PanelTextOmbre* pTO = new PanelTextOmbre( pStr,	PanelText::LARGE_FONT, 0, 10 );
	pTO->setAlign( PanelText::CENTER );
	panelPreview->add( pTO );
	

	//panelPreview->setCanMove(false);
 	wm.add( panelPreview );

    logf((char*)"    name %s ", pStr.c_str());
    logf((char*)"    %d,%d %dx%d", 0, 0, getWidth(), getHeight());
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::resizePreview(int width, int height)	{
    logf((char*) "Camera::resizePreview(%d, %d)", width, height);
	WindowsManager& wm = WindowsManager::getInstance();

    //vCameraSize.x = getWidth();
    //vCameraSize.y = getHeight();

	int wsc = wm.getWidth();
	int hsc = wm.getHeight();
	wsc = getWidth();
	hsc = getHeight();
	
	float rsc = (float)wsc/(float)hsc;
	float rpv = (float)vCameraSize.x/(float)vCameraSize.y;

    int modX, modY;
    float zoom;
    
    /*
    printf( "rsc=%f   rpv=%f\n", rsc, rpv);
    printf( "wsc=%d   hsc=%d\n", wsc, hsc);
    printf( "vCamera.x=%.2f   vCamera.y=%.2f\n", vCameraSize.x, vCameraSize.y);
	*/
	if ( rsc > rpv )    {
	    zoom = (float)height/(float)vCameraSize.y;

	    dxCam = zoom * (float)vCameraSize.x;
	    dyCam = zoom * (float)vCameraSize.y;

	    xCam = modX = (wsc - dxCam) / 2;
	    yCam = modY = 0;
	} 
	else                {
	    zoom = (float)width/(float)vCameraSize.x;
        printf( "Zoom=%f\n", zoom );

	    dxCam = zoom * (float)vCameraSize.x;
	    dyCam = zoom * (float)vCameraSize.y;

	    xCam = modX = 0;
	    yCam = modY = (hsc - dyCam) / 2;
	} 

    logf((char*) "   Screen  : %dx%d", width, height);
    logf((char*) "   Preview : %d,%d %dx%d", xCam, yCam, dxCam, dyCam);

	
	panelPreview->setPosAndSize( xCam, yCam, dxCam, dyCam);
	panelPreview->setDisplayGL(displayGL_cb);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::createControlID(PanelSimple * p, int x, int y, char* str)	{
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
    logf((char*)"------------- Camera::CreateControl() ---------------");
	WindowsManager& wm = WindowsManager::getInstance();

    panelControl = new PanelWindow();
    int dx = 200;
    int dy = 150;
    int x = getWidth() - dx - 20;
    int y = 20;

    panelControl->setPosAndSize( x, y, dx, dy);
    ///home/rene/programmes/opengl/video/essai
    wm.add(panelControl);
    panelControl->setBackground((char*)"background.tga");
    
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

    dy = ++n*dyt;
    panelControl->setSize( dx, dy);
    //resizeControl(getWidth(), getHeight());

    //panelControl->setVisible(bPanelControl);

    logf((char*)"panelControl  %d,%d %dx%d\n", x, y, dx, dy);   
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool*     pCharging;

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
    log((char*)"Camera::threadExtractImg()");
    mainloop();

    bChargingCamera = true;
    log((char*)"Camera::threadExtractImg()");
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::change_background_camera(void)
{
    static float previousTime = -1;

    log((char*)"START Camera::change_background_camera()");
    logf((char*)" vCameraSize.x=%d vCameraSize.y=%d", vCameraSize.x, vCameraSize.y);
    if ( vCameraSize.x = -1 )
    {
        vCameraSize.x = getWidth();
        vCameraSize.y = getHeight();
    }
    //bFreePtr = false;
    if ( bChargingCamera )
    {
        panelPreview->deleteBackground();
        //panelPreview->setBackground( (char*)background);
        //log((char*)" bChargingCamera=true");
        try
        {
            panelPreview->setBackground( getBuffer(), vCameraSize.x, vCameraSize.y, 3);
            //logf((char*)" vCameraSize.x=%d vCameraSize.y=%d", vCameraSize.x, vCameraSize.y);
            //vCameraSize.x = getWidth();
            //vCameraSize.y = getHeight();
            //ptr = getBuffer();
            //if (bSuivi)    suivi();
        }
        catch(std::exception const& e)
        {
            //panelPreview->setBackground( (char*)background);
            log((char*)"ERROR --- Camera::change_background_camera()");
            cerr << "ERREUR : " << e.what() << endl;
        }

        pCamFilename->changeText( (char*)getDevName() );

        pCharging = &bChargingCamera;

        //pthread_chargement_camera = new thread(charge_camera);
        log((char*)"  Start Thread");
        thread_chargement_camera = memberThread();
        log((char*)"  Fin   Thread");
        
        float t = Timer::getInstance().getCurrentTime();
        if ( previousTime != -1 )
        {
            float ecoule = t - previousTime;
            char hz[255];
            sprintf(hz, "%.1fHz", 1.0/ecoule);
            //pHertz->changeText( hz );
            //logf( (char*)"Time : %0.4f", ecoule );
        }
        previousTime = t;
        
        bChargingCamera = false;
     }
    log((char*)"END   Camera::change_background_camera()");
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






