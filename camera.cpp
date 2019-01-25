#include "camera.h"

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Camera::Camera()
{
    Device_cam();
    bChargingCamera = true;
    CreatePreview();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Camera::Camera(int w, int h)
{
    logf((char*)"----------- Constructeur Camera(%d, %d) -------------", w, h);
    Device_cam();
    bChargingCamera = true;
    setWidth(w);
    setHeight(h);
    CreatePreview();
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
	
	pStr = getName();
	PanelTextOmbre* pTO = new PanelTextOmbre( pStr,	PanelText::LARGE_FONT, 0, 10 );
	pTO->setAlign( PanelText::CENTER );
	panelPreview->add( pTO );
	

	//panelPreview->setCanMove(false);
 	wm.add( panelPreview );

    logf((char*)"Camera::CreatePreview  name %s ", pStr.c_str());
    logf((char*)"Camera::CreatePreview  %d,%d %dx%d", 0, 0, getWidth(), getHeight());
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera::resizePreview(int width, int height)	{
    logf((char*) "Camera::resizePreview(%d, %d", width, height);
	WindowsManager& wm = WindowsManager::getInstance();

    vCameraSize.x = getWidth();
    vCameraSize.y = getHeight();

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

    logf((char*) "Screen  : %dx%d\n", width, height);
    logf((char*) "Preview : %d,%d %dx%d\n", xCam, yCam, dxCam, dyCam);

	
	panelPreview->setPosAndSize( xCam, yCam, dxCam, dyCam);
	panelPreview->setDisplayGL(displayGL_cb);
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
void Camera::change_background_camera(void)
{
    static float previousTime = -1;
    
    //bFreePtr = false;
    if ( bChargingCamera )
    {
            panelPreview->deleteBackground();
            //panelPreview->setBackground( (char*)background);
            try
            {
                panelPreview->setBackground( getBuffer(), vCameraSize.x, vCameraSize.y, 3);
                //ptr = getBuffer();
                //if (bSuivi)    suivi();
            }
            catch(std::exception const& e)
            {
                //panelPreview->setBackground( (char*)background);
                cerr << "ERREUR : " << e.what() << endl;
            }

            pCamFilename->changeText( (char*)"frame-0.raw" );

            pCharging = &bChargingCamera;
            pthread_chargement_camera = new thread(charge_camera);
            
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






