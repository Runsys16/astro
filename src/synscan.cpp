#include "synscan.h"
#include "camera_mgr.h"
#include "captures.h"
#include "var_mgr.h"
#include "serveur_mgr.h"
#include <sys/time.h>
#include <time.h>
//#include "MathlibD.h"
//--------------------------------------------------------------------------------------------------------------------
#define VAR_GLOBAL
//#define LOCALHOST
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
SYNSCAN::SYNSCAN()
{
    logf((char*)"----------- Constructeur SYNSCAN() -------------" );
    listen_1 = true;
    traite_1 = true;
    
    uPort = 10004;

    sock_synscan		= -1;

	#ifdef VAR_GLOBAL
	VarManager& var = VarManager::getInstance();

    if ( !var.existe("IP_SYNSCAN"))		var.set("IP_SYNSCAN", "127.0.0.1" );
    sIP_listen_synscan = *var.gets("IP_SYNSCAN" );

	#endif
	
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
float SYNSCAN::com2rad( int ra_int )
{
    float ad_rad = ra_int;
    ad_rad = ((ad_rad-0.5) * M_PI )/  (float)(2147483648.0) ;
    return ad_rad;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void SYNSCAN::decode( struct stellarium& ss, unsigned char* buffer )
{
    long long tz = 0;
    tz |= (unsigned long)buffer[4];
    tz |= (unsigned long)buffer[5]<<8;
    tz |= (unsigned long)buffer[6]<<16;
    tz |= (unsigned long)buffer[7]<<24;
    tz |= (unsigned long)buffer[8]<<32;
    tz |= (unsigned long)buffer[9]<<40;
    tz |= (unsigned long)buffer[10]<<48;
    tz |= (unsigned long)buffer[11]<<56;
    ss.tz = tz;

    int l;
    l = 0;
    l |= buffer[12];
    l |= buffer[13]<<8;
    l |= buffer[14]<<16;
    l |= buffer[15]<<24;
    ss.ra = l;

    l = 0;
    l |= buffer[16];
    l |= buffer[17]<<8;
    l |= buffer[18]<<16;
    l |= buffer[19]<<24;
    ss.dc = l;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void SYNSCAN::traite_connexion_synscan()
{
    unsigned char buffer[255];
    int n;

    while( traite_1 )
    {
        n = read(sock_synscan, buffer, 255);
        //logf_thread( (char*)"SYNSCAN::traite_connexion_synscan()" );
        log_tab(true);

        if (n <= 0)
        {
            logf_thread( (char*)"ERROR reading from socket (synscan)");
            break;
        }

		buffer[n] = 0;

		logf_thread( (char*)"Reception de %s", buffer );

		if ( strncmp( (char*)buffer, "Ka", 2 ) == 0 )
		{ 
			write_synscan( sock_synscan, (char*)"a#\r", 1, true );
		}

       log_tab(false);
    }
    
    logf_thread( (char*)"Deconnexion du sock_synscan %d", sock_synscan );
    close(sock_synscan);

    sock_synscan = -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void SYNSCAN::thread_listen_synscan()
{
    logf_thread( (char*)"SYNSCAN::thread_listen_SYNSCAN()");

	struct sockaddr_in adresse;
	socklen_t          longueur;


	if ((sock_listen_synscan = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

    int enable = 1;
    if (setsockopt(sock_listen_synscan, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        perror("setsockopt(SO_REUSEADDR) failed");
		exit(EXIT_FAILURE);
    }
	
	memset(& adresse, 0, sizeof(struct sockaddr));
	adresse.sin_family = AF_INET;
	//adresse.sin_addr.s_addr = htonl(INADDR_ANY);
	adresse.sin_port = htons(uPort);
	
#ifdef VAR_GLOBAL
	//sIP_init = VarManager::getInstance().gets("IP_INIT");
	inet_aton( sIP_listen_synscan.c_str(), &adresse.sin_addr ); 
#else
#ifdef LOCALHOST
	inet_aton("127.0.0.1", &adresse.sin_addr ); 
#endif
#endif


	if (bind(sock_listen_synscan, (struct sockaddr *) & adresse, sizeof(adresse)) < 0) {
		logf_thread( (char*) "[ERREUR] bind (sock_listen_synscan)");
		sock_listen_synscan = -1;
		//exit(EXIT_FAILURE);
		return;
	}
	
	longueur = sizeof(struct sockaddr_in);
	if (getsockname(sock_listen_synscan, (struct sockaddr *) & adresse, & longueur) < 0) {
		logf_thread( (char*) "[ERREUR] getsockname (sock_listen_synscan)");
		sock_listen_synscan = -1;
		return;
	}


    int option = 1;
    if(setsockopt(sock_listen_synscan,SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),(char*)&option,sizeof(option)) < 0)
    {
		logf_thread( (char*) "[ERREUR] setsockopt (sock_listen_synscan)");
        close(sock_listen_synscan);
		sock_listen_synscan = -1;
		return;

    }


    logf_thread( (char*)"---------------------------------------------------------------");
	logf_thread( (char*)"Ecoute sur IP = %s, Port = %u (Synscan)", inet_ntoa(adresse.sin_addr), ntohs(adresse.sin_port));
    logf_thread( (char*)"---------------------------------------------------------------");

	listen(sock_listen_synscan, 5);
	while (listen_1) {
		longueur = sizeof(struct sockaddr_in);
		int sock = accept(sock_listen_synscan, (struct sockaddr *) & adresse, & longueur);

		if (sock < 0)			continue;
		if ( sock_synscan != -1 )
		{
			close( sock );
			char *some_addr;
		    some_addr = inet_ntoa( adresse.sin_addr); // return the IP
		    sIP_synscan = string(some_addr);
		    
			logf_thread( (char*)"[Warning] lx200 tentative de connection %s", some_addr );
			continue;
		}
		
		sock_synscan = sock;
		
		char *some_addr;
        some_addr = inet_ntoa( adresse.sin_addr); // return the IP

		//sIP_init = string( some_addr );
	
		logf_thread( (char*)"SYNSCAN::thread_listen_SYNSCAN() connexion SOCKET 2" );
		logf_thread( (char*)"  sock = %d  sock_listen_synscan = %d  IP = %s:%d sur %s", sock_listen_synscan, sock_synscan, some_addr, (int)adresse.sin_port, sIP_listen_synscan.c_str() );

		traite_connexion_synscan();
	}
    logf_thread( (char*)"Fermeture de sock_listen_synscan (%s:%u)", inet_ntoa(adresse.sin_addr), ntohs(adresse.sin_port) );
	close(sock_listen_synscan);
	sock_listen_synscan = -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void SYNSCAN::start_synscan()
{
    th_synscan_listen = std::thread(&SYNSCAN::thread_listen_synscan, this);
    th_synscan_listen.detach();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void SYNSCAN::write_synscan(char* s)
{
#define LOG
    if ( sock_synscan != -1 )
    {
        write( sock_synscan, s, 24 );

        #ifdef LOG
        char t[25*4+80] = "";
        
      	strcpy( t, s );
      	char* ptr = t + strlen(s);
        
        for (int i=0; i<strlen(s); i++ )	{
        	sprintf( (char*)ptr, " %02X", (unsigned char)s[i] );
        	ptr += 3;
        
        }

        logf_thread( (char*)"Envoid a SYNSCAN  %s", t );
        #endif
    }
#undef LOG
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void SYNSCAN::write_synscan(int socket, char* s, int len)
{
	write_synscan( socket, s, len, true );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void SYNSCAN::write_synscan(int socket, char* s, int len, bool bLog)
{
#define __LOG
    if ( socket != -1 )
    {
        write( socket, s, len );

        #ifdef __LOG
        char t[25*4+80] = "";
        
      	strcpy( t, s );
      	char* ptr = t + strlen(s);
        
        for (int i=0; i<strlen(s); i++ )	{
        	sprintf( (char*)ptr, " %02X", (unsigned char)s[i] );
        	ptr += 3;
        
        }
        if ( bLog )
	        logf_thread( (char*)"  | Envoi a SYNSCAN  %s", t );
        #endif
    }
#undef __LOG
}
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void SYNSCAN::close_all()
{
    logf_thread( (char*)"Fermeture de tous les sockets ..." );
    log_tab(true);

    listen_1 = false;
    traite_1 = false;

	//close(sock_listen_synscan);
	//close(sock_listen_synscan);
    //sleep(1);

    if ( sock_synscan!= -1 )                shutdown(sock_synscan, 2);

    
    if ( sock_listen_synscan!= -1 )                  shutdown(sock_listen_synscan, 2);
    //sleep(1);
    
    /*
    sock_listen_synscan          = -1;
    sock_listen_synscan          = -1;
    sock_stellarium = -1;
    sock_synscan        = -1;
    */

    //sleep(1);
    /*
    logf_thread( (char*)"sock_listen_synscan=%d sock_listen_synscan=%d", sock_listen_synscan, sock_listen_synscan );
    while( sock_listen_synscan != -1)	;
    while( sock_listen_synscan!= -1)	;
    while( sock_stellarium != -1)	;
    while( sock_synscan!= -1)	;
    */
    logf_thread( (char*)"sock_listen_synscan=%d sock_listen_synscan=%d", sock_listen_synscan, sock_listen_synscan );
    log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void SYNSCAN::print_list()
{
    //logf( (char*)"---- SYNSCAN::print_list()" );

	if ( sock_listen_synscan == -1 )		logf( (char*)"  SYNSCAN\timpossible d'ouvrir : %s", sIP_listen_synscan.c_str() );
	else if ( sock_synscan != -1 )			logf( (char*)"  SYNSCAN\t connexion de \t: %s", sIP_synscan.c_str() );
	else									logf( (char*)"  SYNSCAN\tlisten sur \t\t: %s : %d", sIP_listen_synscan.c_str(), uPort );

}    
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

