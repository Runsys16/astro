#include "camera_mgr.h"


//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Camera_mgr::Camera_mgr()
{
    pCurrent = NULL;
    nActive = 0;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::add( string dev_name )
{
    logf((char*)"----------- Camera_mgr::add( \"%s\" ) -------------", dev_name.c_str());

    Camera *        pCamera = new Camera(1600,900);
    
    pCameras.push_back( pCamera );
    pCamera->setDevName( (char*)dev_name.c_str() );
    pCamera->open_device();
    pCamera->init_device();
    pCamera->capability_list();
    pCamera->start_capturing();
    //pCamera->addControl();
    
    pCamera->CreatePreview();
    pCamera->CreateControl();

    active();
    onBottom();
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
    if ( pCurrent )         change_hertz( pCurrent->getHertz() );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::keyboard( char key )
{
    //logf((char*)"----------- Camera_mgr::change_background_camera() -------------");
    if ( pCurrent )                 pCurrent->keyboard( key );

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::resize(int width, int height)
{
    if ( pCurrent )            
        WindowsManager::getInstance().onBottom( pCurrent->getPanelPreview() );
        //WindowsManager::getInstance().onBottom( pCurrent->getPanelPreview() );


    for( int i=0; i<pCameras.size(); i++ )
    {
        pCameras[i]->resizePreview( width, height );
        pCameras[i]->resizeControl( width, height );
    
        if ( pCameras[i] != pCurrent )
            WindowsManager::getInstance().onBottom( pCameras[i]->getPanelPreview() );
        else
            pCameras[i]->fullSizePreview( width, height );
    }
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::onBottom()
{
    if ( pCurrent )            
        WindowsManager::getInstance().onBottom( pCurrent->getPanelPreview() );


    for( int i=0; i<pCameras.size(); i++ )
    {
        if ( pCameras[i] != pCurrent )
            WindowsManager::getInstance().onBottom( pCameras[i]->getPanelPreview() );
    }
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::active()
{
    int w = WindowsManager::getInstance().getWidth();
    int h = WindowsManager::getInstance().getHeight();

    if ( pCurrent )         pCurrent->resizePreview( w, h );
    //pCurrent->resizeControl( width, height );

    onBottom();

    nActive++;
    int n = pCameras.size();
    nActive = nActive % n;
    
    pCurrent = pCameras[nActive];
    
    
    if ( pCurrent )         pCurrent->fullSizePreview( w, h );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------





