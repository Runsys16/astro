#include "capture.h"


//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Capture::Capture()
{
    logf((char*)"----------- Constructeur Capture() -----------" );
    

    bFirst = true;
    //Camera();
    setName( (char*) "Capture" );
    setDevName( (char*) "Capture" );
    vCameraSize.x = 1920;
    vCameraSize.y = 1080;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::change_background_camera(void)
{
    
   //if ( pthread_chargement )   delete pthread_chargement;

    if ( bFirst ) 
    {
        log((char*)"!!!!!Capture::change_background_camera()");
        
        string filename = "/home/rene/Documents/astronomie/capture/2019-02-25/m1/m1-004.png";
        panelPreview->deleteBackground();
        
        struct readBackground       readB;
        readB.ptr = WindowsManager::OpenImage( (const std::string)filename, readB.w, readB.h, readB.d );

        if ( readB.ptr != NULL )
        {
            panelPreview->setBackground( readB.ptr, readB.w, readB.h, readB.d);
            bFirst = false;
        }
    }
    
    if (panelPreview==NULL)     bFirst = true;
    //log((char*)"change_background_pleiade()");
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
GLubyte* Capture::getPtr()
{
    return NULL;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool  Capture::haveNewFrame()
{
    return false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Capture::haveUseFrame(bool b)
{
    bNewBackground = false;
}




