#include "camera_mgr.h"
#include "pleiade.h"
#include "main.h"


//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Camera_mgr::Camera_mgr()
{
    logf_thread((char*)"Constructeur Camera_mgr() -------------" );
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

        //logf_thread( (char*)"    [%d/%d] %s compare %s", i, nb, pCameras[i]->getDevName(), sdev.c_str() );
        
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
//#define INTERN_CAM

    logf_thread((char*)"Camera_mgr::add( \"%s\" ) -------------", sdev_name.c_str());
    if ( camera_exist( sdev_name ) )            return;

	log_tab(true);


    //logf_thread( (char*)"    Not Exist" );

    Camera *        pCamera = new Camera(1600,900);
    //pCameras.push_back( pCamera );
    string newString = sdev_name;
    
    pCamera->setDevName( newString );

    pCamera->open_device();
    if ( !pCamera->getIOCapability() ) {
    	delete pCamera;
	    logf_thread((char*)"[Warning] Delete( \"%s\" ) device commande", sdev_name.c_str());
	    log_tab(false);
	    logf_thread((char*)"Camera_mgr::add( \"%s\" ) END", sdev_name.c_str());
    	return;
    }
#ifndef INTERN_CAM
    if ( pCamera->getName().find( "USB Camera" ) == 0 )
    {
    	delete pCamera;
	    logf_thread((char*)"[Warning] Delete( \"%s\" )  camera interne", sdev_name.c_str());
	    log_tab(false);
	    logf_thread((char*)"Camera_mgr::add( \"%s\" ) END", sdev_name.c_str());
	    return;
    }
#endif

    
    pCamera->init_device();
    pCamera->capability_list();
    pCamera->start_capturing();


    pCamera->CreatePreview();
    pCamera->CreateControl();

    pCameras.push_back( pCamera );
	
    onBottom();
    active( pCamera );

	//bDesactiveLog = true;
    pCamera->start_thread();
	//bDesactiveLog = false;

	log_tab(false);
    logf_thread((char*)"Camera_mgr::add( \"%s\" ) END", sdev_name.c_str());
}
//--------------------------------------------------------------------------------------------------------------------
//
// Utiliser par la camera speciale pleiade
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::add( Camera* p )
{
    logf_thread((char*)"Camera_mgr::add(Camera *)"	 );
    log_tab(true);


    pCameras.push_back( p );

    p->CreatePreview();
    p->CreateControl();

    if ( p->getFd() == -1 ) 
    {
        p->setControlVisible(false);
        logf_thread( (char*)"Ce n'est pas une camera" );
    }

    onBottom();
	active( p );
    
    vizier.set_camera_reference();
    
    log_tab(false);
    logf_thread((char*)"Camera_mgr::add(Camera*) END" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::addImages( Camera* p )
{
    logf_thread((char*)"Camera_mgr::addImages() -------------" );


    pCameras.push_back( p );

    p->CreatePreview();
    p->CreateControl();
    p->change_background();

    if ( p->getFd() == -1 ) 
    {
        p->setControlVisible(false);
        logf_thread( (char*)"Ce n'est pas une camera" );
    }

    active();
    onBottom();
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::sup( string name )
{
    logf_thread((char*)"Camera_mgr::sup( \"%s\" ) curent = \"%s\"", name.c_str(), pCurrent->getDevName() );

    Camera* pCamera = getCamera( name );

	if( pCurrent == pCamera )		pCurrent = NULL;

    if ( pCamera )
    {   
        logf_thread((char*)"  Camera_mgr::sup() %s", pCamera->getDevName() );
        int i = getNum( pCamera );
        pCameras.erase(pCameras.begin()+i);
        logf_thread((char*)"  Camera_mgr::sup() %s OK", pCamera->getDevName() );
        delete pCamera;
    }


    //active();
    //onBottom();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::sup( Camera * p )
{
	if ( p == NULL )			return;
    logf_thread((char*)"Camera_mgr::sup(%s)", p->getDevName() );
	if ( pCameras.size() == 0 )	return;


    for( int i=0; i<pCameras.size(); i++ )
    {
        if ( pCameras[i] == p )
        {
            int i = getNum( p );
            pCameras.erase(pCameras.begin()+i);
            logf_thread((char*)"  Camera_mgr::sup() %s OK", p->getDevName() );
            delete p;
        }
    }

	if( pCurrent == p )		pCurrent = NULL;

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::change_background_camera( void )
{
    //logf_thread((char*)"Camera_mgr::change_background_camera() -------------");
    
    for( int i=0; i<pCameras.size(); i++ )
    {
        pCameras[i]->change_background();
    }
    if ( pCurrent )         change_hertz( pCurrent->getHertz() );
    else					change_hertz( 0.0 );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool Camera_mgr::keyboard( char key )
{
    //logf_thread((char*)"Camera_mgr::change_background_camera() -------------");
    if ( pCurrent )                 return pCurrent->keyboard( key );
    return false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::resize(int width, int height)
{
    logf_thread((char*)"Camera_mgr::resize(%d, %d) -------------", width, height);
    for( int i=0; i<pCameras.size(); i++ )
    {
        pCameras[i]->resizeControl( width, height );
    
        if ( pCameras[i] == pCurrent )
            pCameras[i]->fullSizePreview( width, height );
        else
            pCameras[i]->iconSizePreview( width, height );
    }
    
    reOrder();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::onBottom()
{
    if ( pCurrent && pCurrent->getPanelCamera() )            
        WindowsManager::getInstance().onBottom( pCurrent->getPanelCamera() );

    /*
    for( int i=0; i<pCameras.size(); i++ )
    {
        if ( pCameras[i] != pCurrent )
            WindowsManager::getInstance().onBottom( pCameras[i]->getPanelCamera() );
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
        WindowsManager::getInstance().onBottom( pCurrent->getPanelCamera() );
    

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
        WindowsManager::getInstance().onBottom( pCurrent->getPanelCamera() );
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::active()
{
	// Si la camera est connecte au demarrage => Plantage
    //logf_thread((char*)"Camera_mgr::active() -------------" );

    int w = WindowsManager::getInstance().getWidth();
    int h = WindowsManager::getInstance().getHeight();

    if ( pCurrent != NULL )         pCurrent->iconSizePreview( w, h );

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
void Camera_mgr::active( Camera* pCamera)
{
	// Si la camera est connecte au demarrage => Plantage
    //logf_thread((char*)"Camera_mgr::active() -------------" );
    if ( pCamera == NULL )			return;

    int w = WindowsManager::getInstance().getWidth();
    int h = WindowsManager::getInstance().getHeight();

    if ( pCurrent != NULL )         pCurrent->iconSizePreview( w, h );

    int n = pCameras.size();
    nActive = -1;
	for( int i=0; i<n; i++ )
	{
		if ( pCameras[i] == pCamera )		nActive = i;
	}
	if ( nActive == -1 )			logf_thread( (char*)"[Erreur] Camera_mgr::active(Camera*) nAtive = -1" );

    pCurrent = pCamera;
    
    reOrder();


    pCamera->fullSizePreview( w, h );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void  Camera_mgr::reOrder()
{
    logf_thread((char*)"Camera_mgr::reOrder() -------------" );
    int nb = pCameras.size();

    for( int i=0, a=0; i<nb; i++, a++ )
    {
        if ( pCameras[i] != pCurrent )
        {
            WindowsManager::getInstance().onBottom( pCameras[i]->getPanelCamera() );
            pCameras[i]->getPanelCamera()->setPos( a*00+10, a*200+10 );
        }
        else
            a--;
    }


    if ( pCurrent )            
        WindowsManager::getInstance().onBottom( pCurrent->getPanelCamera() );
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
    logf_thread( (char*)"---- Camera_mgr::print_list()" );

    int nb0 = pCameras.size();
    
 
    logf_thread( (char*)"  pCameras : " );
    for( int i=0; i<nb0; i++ )
    {
        logf_thread( (char*)"    %s", pCameras[i]->getName().c_str() );
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
    
    logf_thread( (char*)"Camera_mgr::findAllStars()" );
    
    pCurrent->getPanelCamera()->getStars()->deleteAllStars();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Camera_mgr::findAllStars()
{
    if (pCurrent == NULL)                   return;
    if (pCurrent->getRB() == NULL)          return;
    
    logf_thread( (char*)"Camera_mgr::findAllStars()" );
    
    pCurrent->getPanelCamera()->getStars()->setView( pCurrent->getPanelCamera() );
    pCurrent->getPanelCamera()->getStars()->setRB( pCurrent->getRB() );
    pCurrent->getPanelCamera()->getStars()->findAllStars();
    pCurrent->getPanelCamera()->getStars()->setVisible(true);

    /*
    char t[] = "00000000000";  
    PanelText* p = pCurrent->getPanelNbStars();
    sprintf( t, "%d", pCurrent->getPanelCamera()->getStars()->size() );
    p->changeText( t );
    */
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
        //logf_thread( (char*)"Camera_mgr::suivi()" );
        Panel * pView= pCurrent->getPanelCamera();
        pCurrent->getPanelCamera()->getStars()->setView( (PanelSimple*)pView );
        pCurrent->getPanelCamera()->getStars()->suivi( getRB() );
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
    	if ( typeid(Pleiade) == typeid(*pCurrent) )	{
    		logf_thread( (char*)"Camera_mgr::position() pCurrent = objet<pleiade>" );
    	}
    	else {
	        pCurrent->getPanelCamera()->getStars()->position(ra, dc);
	   	}
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
    pCurrent->getPanelCamera()->tex2screen(x, y);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void  Camera_mgr::tex2screen(double& x,double& y)
{
    if (pCurrent == NULL)                   return;
    pCurrent->getPanelCamera()->tex2screen(x, y);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void  Camera_mgr::screen2tex(int& x,int& y)
{
    if (pCurrent == NULL)                   return;
    pCurrent->getPanelCamera()->screen2tex(x, y);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void  Camera_mgr::screen2tex(double& x,double& y)
{
    if (pCurrent == NULL)                   return;
    pCurrent->getPanelCamera()->screen2tex(x, y);
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
    if (pCurrent == NULL)                   { logf_thread((char*)"pCurrent NULL"); return; }
    pCurrent->setRefCatalog(_0, _1);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void  Camera_mgr::stopAllCameras()
{   
    int nb = pCameras.size();
    for (int i=0; i<nb; i++)
    {
        if ( pCameras[i] )				pCameras[i]->stop_thread();
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void  Camera_mgr::cam_full_screen()
{   
    if (pCurrent == NULL)                   return; 
    
    // taille main.cpp
    pCurrent->fullSizePreview( width, height );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void  Camera_mgr::compareStar()
{   
    if (pCurrent == NULL)                   { logf_thread((char*)"pCurrent NULL"); return; }
    
    pCurrent->compareStar();    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------





