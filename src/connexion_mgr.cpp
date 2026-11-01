#include "connexion_mgr.h"
#include <ASICamera2.h>

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Connexion_mgr::Connexion_mgr()
{
    bStartDEV = false;
    bExitThreadDEV = false;
    bStartZWO = false;
    bExitThreadZWO = false;
    
    nCamZWO  = 0;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Connexion_mgr::startDEV()
{
    if ( !bStartDEV )
    {
        th_poll_connexionDEV = std::thread(&Connexion_mgr::threadPoolingDEV, this); 
        th_poll_connexionDEV.detach();
        log_thread( (char*)"[ THREAD ] Connexion_mgr::start() !! " );
        bStartDEV = true;
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Connexion_mgr::startZWO()
{
	//return;
    if ( !bStartZWO )
    {
        th_poll_connexionZWO = std::thread(&Connexion_mgr::threadPoolingZWO, this); 
        th_poll_connexionZWO.detach();
        log_thread( (char*)"[ THREAD ] Connexion_mgr::startZWO() !! " );
        bStartZWO = true;
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool Connexion_mgr::isExclude(string st)
{
    vector<string>& exclude = getExclude();
    int nb = exclude.size();
    for( int i=0; i<nb; i++ )
    {
        if ( st.find(exclude[i]) != string::npos ) {
        	//logf( (char*)" %s exclue !!!", st.c_str() );
	        return true;
        }
    }
   	//logf( (char*)" %s OK non-exclue !!!", st.c_str() );
    return false;
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
            if ( isExclude(t_port_polling[i])  )    continue;
            logf_thread( (char*)"Connexion_mgr::add_port()  %s", t_port_polling[i].c_str() );
            log_tab(true);

			son( "/home/rene/.astropilot/sounds/login.wav" );

            sleep( 1 );
            if ( t_port_polling[i].find("video") != string::npos )
            {
                //if ( !isExclude(t_port_polling[i])  )
                log_tab(true);
                Camera_mgr::getInstance().add( t_port_polling[i] );
                t_port_current.push_back(  t_port_polling[i] );
                log_tab(false);
                logf_thread( (char*)"Connexion_mgr::add_port()  %s  END", t_port_polling[i].c_str() );
            }
            else if ( t_port_polling[i].find("ttyACM") != string::npos )
            {
                //logf_thread( (char*)"Connexion_mgr::add_port()  %s", t_port_polling[i].c_str() );
                Serial::getInstance().init( t_port_polling[i] );
                t_port_current.push_back(  t_port_polling[i] );
                arduino_online(true);
            }
			log_tab(false);
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
			logf_thread( (char*)"Connexion_mgr::sup_port()  %s", t_port_current[i].c_str() );
			log_tab( true);
			son( "/home/rene/.astropilot/sounds/logout.wav" );
        
            if ( t_port_current[i].find("video") != string::npos )
            {
                Camera_mgr::getInstance().sup( t_port_current[i] );
                t_port_current.erase(  t_port_current.begin() + i );
            }
            else if ( t_port_current[i].find("ttyACM") != string::npos )
            {
                Serial::getInstance().sclose();
                t_port_current.erase(  t_port_current.begin() + i );
                arduino_online(false);
            }
			log_tab( false);
            return;
        }
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
/*
void Connexion_mgr::sup_port(string)
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
            if ( t_port_current[i].find("video") != string::npos )
            {
                logf( (char*)"Connexion_mgr::sup_port()  %s", t_port_current[i].c_str() );
                Camera_mgr::getInstance().sup( t_port_current[i] );
                t_port_current.erase(  t_port_current.begin() + i );
            }
            else if ( t_port_current[i].find("ttyACM") != string::npos )
            {
                logf( (char*)"Connexion_mgr::sup_port()  %s", t_port_current[i].c_str() );
                Serial::getInstance().sclose();
                t_port_current.erase(  t_port_current.begin() + i );
                arduino_online(false);
            }
            return;
        }
    }
}
*/
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Connexion_mgr::threadPoolingDEV()
{
    logf_thread( (char*)"[ THREAD ] Connexion_mgr::threadPoolingDEV() START" );
    sleep(1);
    //logf( (char*)"Connexion_mgr::pooling()" );
    
    struct dirent *lecture;
    DIR *rep;
    
    while ( 1 )
    {
		rep = opendir("/dev/" );
	    t_port_polling.clear();
	    
    	while( lecture = readdir(rep) )
    	{

		    string s;
		    int found;
		    s = lecture->d_name;

			//logf_thread( (char*)"[ THREAD ] Connexion_mgr::threadPoolingDEV() lecture %s ", s.c_str() );
		    
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
		        t_port_polling.push_back( string((char*)dev));
		        

		    }
		    if ( bExitThreadDEV.load() )		break;

		}
		        
	    closedir(rep);    
	
		add_port();
		sup_port();


	    if ( bExitThreadDEV.load() )		break;
        sleep(1);
	    if ( bExitThreadDEV.load() )		break;
    }

    logf_thread( (char*)"[ THREAD ] Connexion_mgr::threadPoolingDEV() END" );
	bStartDEV = false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Connexion_mgr::print_list()
{
    logf( (char*)"---- Connexion sur /dev" );

    int nb0 = t_port_polling.size();
    int nb1 = t_port_current.size();
    
    bool bFound = false;

    logf( (char*)"  exclude : " );
    vector<string>& exclude = getExclude();
    int nb = exclude.size();
    for( int i=0; i<nb; i++ )
    {
        logf( (char*)"    %s", exclude[i].c_str() );
    }

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

	logf( (char*)"  %d camera(s) ZWO connected", nCamZWO);
}    
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Connexion_mgr::threadPoolingZWO()
{
	logf_thread( (char*)"[ THREAD ] Connexion_mgr::threadPoolingZWO() START" );
    sleep(1);
    //logf( (char*)"Connexion_mgr::pooling()" );
    while (1) {
    
		int n = ASIGetNumOfConnectedCameras();
		//n = nCamZWO;
		
		if( n != nCamZWO )	{
			logf_thread( (char*)"Connexion_mgr::poolingZWO %d camera(s) ZWO connected", n);
			log_tab(true);
			if ( n > nCamZWO )	{
				Camera_mgr::getInstance().add_zwo( n-1 );
			}
			else {
				//logf_thread( (char*)"suppression ZWO no %d", n);
				Camera_mgr::getInstance().sup_zwo( n );
			}
			nCamZWO = n;
			log_tab(false);
		}
    
        if ( bExitThreadZWO.load() )		break;
	
	    sleep(1);
    }
	logf_thread( (char*)"[ THREAD ] Connexion_mgr::threadPoolingZWO() FIN" );
    bStartZWO = false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------




