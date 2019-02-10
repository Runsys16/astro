#include "connexion_mgr.h"

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Connexion_mgr::Connexion_mgr()
{
    bStart = false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Connexion_mgr::start()
{
    if ( !bStart )
    {
        th_poll_connexion = startThread();
        th_poll_connexion.detach();
        bStart = true;
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Connexion_mgr::add_port()
{
    int nb0 = t_port_polling.size();
    int nb1 = t_port_current.size();
    
    bool bFound = false;

    for( int i=0; i<nb0; i++ )
    {
        bFound = false;
        for( int j=0; j<nb1; j++ )
        {
            if ( t_port_polling[i].find( t_port_current[j] ) != std::string::npos )
            {
                bFound = true;
            }
        }
        if ( !bFound ){
            sleep( 1 );
            Camera_mgr::getInstance().add( t_port_polling[i] );
            t_port_current.push_back(  t_port_polling[i] );
            return;
        }
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Connexion_mgr::sup_port()
{
    int nb0 = t_port_polling.size();
    int nb1 = t_port_current.size();
    
    bool bFound = false;
    
    for( int i=0; i<nb1; i++ )
    {
        bFound = false;
        for( int j=0; j<nb0; j++ )
        {
            if ( t_port_current[i].find( t_port_polling[j] ) != std::string::npos )
            {
                bFound = true;
            }
        }
        if ( !bFound )
        {
            logf( (char*)"Connexion_mgr::sup_port()  %s", t_port_current[i].c_str() );
            Camera_mgr::getInstance().sup( t_port_current[i] );
            t_port_current.erase(  t_port_current.begin() + i );
            return;
        }
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Connexion_mgr::pooling()
{
    sleep(1);
    //logf( (char*)"Connexion_mgr::pooling()" );
    
    struct dirent *lecture;
    DIR *rep;
    rep = opendir("/dev/" );
    t_port_polling.clear();
    
    while ((lecture = readdir(rep))) {

        string s;
        int found;
        s = lecture->d_name;
        
        // Recherche CAMERA
        found = s.find( "video" );
        if ( found!=std::string::npos )
        {
            char dev[255];
            sprintf( dev, "/dev/%s", s.c_str() );
            //logf( (char*)"  Found device VIDEO   '%s'", dev );

            t_port_polling.push_back( string((char*)dev));
        }

        // Recherche ARDUINO
        found = s.find( "ttyACM" );
        if ( found!=std::string::npos )
        {
            char dev[255];
            sprintf( dev, "/dev/%s", s.c_str() );
            //logf( (char*)"  Found device ARDUINO '%s'", dev );
            
        }
    }

    closedir(rep);    

    add_port();
    sup_port();    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Connexion_mgr::threadPooling()
{
    while( 1 )
    {   
        pooling();
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
std::thread Connexion_mgr::startThread()
{
    return std::thread(&Connexion_mgr::threadPooling, this); 
}    
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Connexion_mgr::print_list()
{
    logf( (char*)"---- Connexion_mgr::print_list()" );

    int nb0 = t_port_polling.size();
    int nb1 = t_port_current.size();
    
    bool bFound = false;

    logf( (char*)"  t_port_polling : " );
    for( int i=0; i<nb0; i++ )
    {
        logf( (char*)"    %s", t_port_polling[i].c_str() );
    }

    logf( (char*)"  t_port_current : " );
    for( int i=0; i<nb1; i++ )
    {
        logf( (char*)"    %s", t_port_current[i].c_str() );
    }
}    
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------




