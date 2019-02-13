#include "camera_mgr.h"


//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Camera_mgr::Camera_mgr()
{
    logf((char*)"----------- Constructeur Camera_mgr() -------------" );
    pCurrent = NULL;
    nActive = 0;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool Camera_mgr::camera_exist( string sdev )
{
    int nb = pCameras.size();
    for( int i=0; i<nb; i++ )
    {

        //logf( (char*)"    [%d/%d] %s compare %s", i, nb, pCameras[i]->getDevName(), sdev.c_str() );
        
        if ( sdev.find( (char*)pCameras[i]->getDevName() ) != std::string::npos )
        {
        
            return true;
        }
    }
    return false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::add( string sdev_name )
{

    if ( camera_exist( sdev_name ) )            return;

    //logf( (char*)"    Not Exist" );
    logf((char*)"----------- Camera_mgr::add( \"%s\" ) -------------", sdev_name.c_str());

    Camera *        pCamera = new Camera(1600,900);
    //pCameras.push_back( pCamera );
    string newString = sdev_name;
    pCamera->setDevName( newString );
    pCamera->open_device();
    pCamera->getIOName();
    pCamera->init_device();
    pCamera->capability_list();
    pCamera->start_capturing();
    //pCamera->addControl();
    
    pCamera->CreatePreview();
    pCamera->CreateControl();

    pCameras.push_back( pCamera );

    active();
    onBottom();

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::add( Camera* p )
{
    logf((char*)"----------- Camera_mgr::add() -------------" );


    pCameras.push_back( p );

    p->CreatePreview();
    p->CreateControl();

    active();
    onBottom();
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::sup( string name )
{
    logf((char*)"----------- Camera_mgr::sup() -------------" );

    Camera* pCamera = getCamera( name );
    if ( pCamera )
    {   
        logf((char*)"  Camera_mgr::sup() %s", pCamera->getDevName() );
        int i = getNum( pCamera );
        pCameras.erase(pCameras.begin()+i);
        logf((char*)"  Camera_mgr::sup() %s OK", pCamera->getDevName() );
        delete pCamera;
    }

    pCurrent = NULL;

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

    for( int i=0; i<pCameras.size(); i++ )
    {
        pCameras[i]->resizeControl( width, height );
    
        if ( pCameras[i] == pCurrent )
            pCameras[i]->fullSizePreview( width, height );
        else
            pCameras[i]->resizePreview( width, height );
    }
    
    reOrder();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::onBottom()
{
    if ( pCurrent )            
        WindowsManager::getInstance().onBottom( pCurrent->getPanelPreview() );

    /*
    for( int i=0; i<pCameras.size(); i++ )
    {
        if ( pCameras[i] != pCurrent )
            WindowsManager::getInstance().onBottom( pCameras[i]->getPanelPreview() );
    }
    */
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::idleGL()
{
    if ( pCurrent )            
        WindowsManager::getInstance().onBottom( pCurrent->getPanelPreview() );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::active()
{
    logf((char*)"----------- Camera_mgr::active() -------------" );

    int w = WindowsManager::getInstance().getWidth();
    int h = WindowsManager::getInstance().getHeight();

    if ( pCurrent != NULL )         pCurrent->resizePreview( w, h );

    nActive++;
    int n = pCameras.size();
    nActive = nActive % n;
    
    pCurrent = pCameras[nActive];
    
    if ( pCurrent )         pCurrent->fullSizePreview( w, h );
    
    reOrder();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void  Camera_mgr::reOrder()
{
    logf((char*)"----------- Camera_mgr::reOrder() -------------" );
    int nb = pCameras.size();

    for( int i=0, a=0; i<nb; i++, a++ )
    {
        if ( pCameras[i] != pCurrent )
        {
            WindowsManager::getInstance().onBottom( pCameras[i]->getPanelPreview() );
            pCameras[i]->getPanelPreview()->setPos( a*100, a*100 );
        }
        else
            a--;
    }

    logf((char*)"DEBUG" );

    if ( pCurrent )            
        WindowsManager::getInstance().onBottom( pCurrent->getPanelPreview() );
    logf((char*)"DEBUG" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
GLubyte* Camera_mgr::getPtr()
{
    if ( pCurrent )             return pCurrent->getPtr();
    return NULL;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int Camera_mgr::get_xCam()
{
    
    if ( pCurrent )             return pCurrent->get_xCam();
    return -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int Camera_mgr::get_yCam()
{
    
    if ( pCurrent )             return pCurrent->get_yCam();
    return -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int Camera_mgr::get_dxCam()
{
    
    if ( pCurrent )             return pCurrent->get_dxCam();
    return -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int Camera_mgr::get_dyCam()
{
    
    if ( pCurrent )             return pCurrent->get_dyCam();
    return -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int Camera_mgr::getSize()
{
    return pCameras.size();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int Camera_mgr::getNum(Camera* p)
{
    int nb = pCameras.size();
    for (int i=0; i<nb; i++)
    {
        if ( pCameras[i] == p )         return i;
    }
    return -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
ivec2 Camera_mgr::get_vCameraSize()
{
    return pCurrent->get_vCameraSize();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Camera* Camera_mgr::getCamera( string name )
{
    int nb = pCameras.size();
    for (int i=0; i<nb; i++)
    {
        string s = (char*)pCameras[i]->getDevName();
        if ( s.find( name ) != string::npos )         return pCameras[i];
    }
    return NULL;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::print_list()
{
    logf( (char*)"---- Camera_mgr::print_list()" );

    int nb0 = pCameras.size();
    
 
    logf( (char*)"  pCameras : " );
    for( int i=0; i<nb0; i++ )
    {
        logf( (char*)"    %s", pCameras[i]->getName() );
    }

}    
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------





