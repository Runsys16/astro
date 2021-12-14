#include "camera_mgr.h"


//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Camera_mgr::Camera_mgr()
{
    logf((char*)"Constructeur Camera_mgr() -------------" );
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
    logf((char*)"Camera_mgr::add( \"%s\" ) -------------", sdev_name.c_str());

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
    logf((char*)"Camera_mgr::add() -------------" );
    log_tab(true);


    pCameras.push_back( p );

    p->CreatePreview();
    p->CreateControl();

    if ( p->getFd() == -1 ) 
    {
        p->setControlVisible(false);
        logf( (char*)"Ce n'est pas une camera" );
    }

    active();
    onBottom();
    
    vizier.charge();
    log_tab(false);
    logf((char*)"Camera_mgr::add() -------------" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::addImages( Camera* p )
{
    logf((char*)"Camera_mgr::addImages() -------------" );


    pCameras.push_back( p );

    p->CreatePreview();
    p->CreateControl();
    p->change_background();

    if ( p->getFd() == -1 ) 
    {
        p->setControlVisible(false);
        logf( (char*)"Ce n'est pas une camera" );
    }

    active();
    onBottom();
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::sup( string name )
{
    logf((char*)"Camera_mgr::sup() -------------" );

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

    active();
    onBottom();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::sup( Camera * p )
{
    logf((char*)"Camera_mgr::sup() -------------" );


    for( int i=0; i<pCameras.size(); i++ )
    {
        if ( pCameras[i] == p )
        {
            int i = getNum( p );
            pCameras.erase(pCameras.begin()+i);
            logf((char*)"  Camera_mgr::sup() %s OK", p->getDevName() );
            delete p;
        }
    }

    pCurrent = NULL;

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::change_background_camera( void )
{
    //logf((char*)"Camera_mgr::change_background_camera() -------------");
    
    for( int i=0; i<pCameras.size(); i++ )
    {
        pCameras[i]->change_background();
    }
    if ( pCurrent )         change_hertz( pCurrent->getHertz() );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool Camera_mgr::keyboard( char key )
{
    //logf((char*)"Camera_mgr::change_background_camera() -------------");
    if ( pCurrent )                 return pCurrent->keyboard( key );
    return false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::resize(int width, int height)
{
    logf((char*)"Camera_mgr::resize(%d, %d) -------------", width, height);
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
    if ( pCurrent && pCurrent->getPanelPreview() )            
        WindowsManager::getInstance().onBottom( pCurrent->getPanelPreview() );

    /*
    for( int i=0; i<pCameras.size(); i++ )
    {
        if ( pCameras[i] != pCurrent )
            WindowsManager::getInstance().onBottom( pCameras[i]->getPanelPreview() );
    }
    */
    
}
/*
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::idleGL()
{
    if ( pCurrent )            
        WindowsManager::getInstance().onBottom( pCurrent->getPanelPreview() );
    

    int nb = pCameras.size();

    for( int i=0; i<nb; i++ )
    {
        pCameras[i]->idleGL();
    }
}
*/
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::update()
{
    if ( pCurrent )            
        WindowsManager::getInstance().onBottom( pCurrent->getPanelPreview() );
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::active()
{
    logf((char*)"Camera_mgr::active() -------------" );

    int w = WindowsManager::getInstance().getWidth();
    int h = WindowsManager::getInstance().getHeight();

    if ( pCurrent != NULL )         pCurrent->resizePreview( w, h );

    nActive++;
    int n = pCameras.size();
    if ( n== 0 )    return;

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
    logf((char*)"Camera_mgr::reOrder() -------------" );
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


    if ( pCurrent )            
        WindowsManager::getInstance().onBottom( pCurrent->getPanelPreview() );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void  Camera_mgr::togglePanel()
{
    if ( pCurrent )         pCurrent->togglePanel();
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
vec2* Camera_mgr::getSuivi()
{
    if ( pCurrent!=NULL )       return pCurrent->getSuivi();
    return NULL;
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
    if ( pCurrent )             return pCurrent->get_vCameraSize();
    else                        return ivec2( 0, 0);
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
bool Camera_mgr::haveNewFrame()
{
    if (pCurrent)       return pCurrent->haveNewFrame();
    return false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::haveUseFrame(bool b)
{
    if (pCurrent)       pCurrent->haveUseFrame(b);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
struct readBackground*      Camera_mgr::getRB()
{
    if (pCurrent)       return pCurrent->getRB();

    return NULL;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::deleteAllStars()
{
    if (pCurrent == NULL)                   return;
    if (pCurrent->getRB() == NULL)          return;
    
    logf( (char*)"Camera_mgr::findAllStars()" );
    
    pCurrent->getPanelPreview()->getStars()->deleteAllStars();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::findAllStars()
{
    if (pCurrent == NULL)                   return;
    if (pCurrent->getRB() == NULL)          return;
    
    logf( (char*)"Camera_mgr::findAllStars()" );
    
    pCurrent->getPanelPreview()->getStars()->setView( pCurrent->getPanelPreview() );
    pCurrent->getPanelPreview()->getStars()->setRB( pCurrent->getRB() );
    pCurrent->getPanelPreview()->getStars()->findAllStars();

    char t[] = "00000000000";  
    PanelText* p = pCurrent->getPanelNbStars();
    sprintf( t, "%d", pCurrent->getPanelPreview()->getStars()->size() );
    p->changeText( t );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::suivi()
{
    if (pCurrent == NULL)                   return;
    if (pCurrent->getRB() == NULL)          return;
    
    
    if ( getRB() != NULL )
    {
        //logf( (char*)"Camera_mgr::suivi()" );
        Panel * pView= pCurrent->getPanelPreview();
        pCurrent->getPanelPreview()->getStars()->setView( (PanelSimple*)pView );
        pCurrent->getPanelPreview()->getStars()->suivi( getRB() );
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::position(double ra, double dc)
{
    if (pCurrent == NULL)                   return;
    if (pCurrent->getRB() == NULL)          return;
    
    
    if ( getRB() != NULL )
    {
        pCurrent->getPanelPreview()->getStars()->position(ra, dc);
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::setColor(long color)
{
    int nb = pCameras.size();
    for (int i=0; i<nb; i++)
    {
        pCameras[i]->setColor(color);
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void  Camera_mgr::tex2screen(int& x, int& y)
{
    if (pCurrent == NULL)                   return;
    pCurrent->getPanelPreview()->tex2screen(x, y);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void  Camera_mgr::tex2screen(float& x,float& y)
{
    if (pCurrent == NULL)                   return;
    pCurrent->getPanelPreview()->tex2screen(x, y);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void  Camera_mgr::screen2tex(int& x,int& y)
{
    if (pCurrent == NULL)                   return;
    pCurrent->getPanelPreview()->screen2tex(x, y);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void  Camera_mgr::screen2tex(float& x,float& y)
{
    if (pCurrent == NULL)                   return;
    pCurrent->getPanelPreview()->screen2tex(x, y);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void  Camera_mgr::start_threads()
{   
    int nb = pCameras.size();
    for (int i=0; i<nb; i++)
    {
        pCameras[i]->start_thread();
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void  Camera_mgr::add_catalogue(StarCatalog* p)
{   
    if (pCurrent == NULL)                   return;
    pCurrent->add_catalogue(p);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void  Camera_mgr::setRefCatalog(double _0, double _1)
{   
    if (pCurrent == NULL)                   { logf((char*)"pCurrent NULL"); return; }
    pCurrent->setRefCatalog(_0, _1);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------





