#include "camera_mgr.h"


//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Camera_mgr::Camera_mgr()
{
    pCurrent = NULL;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::add( string dev_name )
{
    logf((char*)"----------- Camera_mgr::add( \"%s\" ) -------------", dev_name.c_str());

    Camera *        pCamera = new Camera(1600,900);
    
    pCameras.push_back( pCamera );
    pCamera->setDevName( dev_name.c_str() );
    pCamera->open_device();
    pCamera->init_device();
    pCamera->capability_list();
    pCamera->start_capturing();
    //pCamera->addControl();
    
    pCamera->CreatePreview();
    pCamera->CreateControl();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::change_background_camera( void )
{
    //logf((char*)"----------- Camera_mgr::change_background_camera() -------------");
    for( int i=0; i<pCameras.size(); i++ )
    {
        pCameras[i]->change_background_camera();
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

