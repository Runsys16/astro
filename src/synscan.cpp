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

    sock_ref		= -1;

	#ifdef VAR_GLOBAL
	VarManager& var = VarManager::getInstance();

    if ( !var.existe("IP_SYNSCAN"))		var.set("IP_SYNSCAN", "127.0.0.1" );
    sIP_synscan = *var.gets("IP_SYNSCAN" );

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
        n = read(sock_ref, buffer, 255);
        //logf_thread( (char*)"SYNSCAN::traite_connexion_init()" );
        log_tab(true);

        if (n <= 0)
        {
            logf_thread( (char*)"ERROR reading from socket (init)");
            break;
        }

		buffer[n] = 0;

		if ( buffer[0] == '#' )			
		{ 
		}

       log_tab(false);
    }
    
    logf_thread( (char*)"Deconnexion du sock_init %d", sock_ref );
    close(sock_ref);

    sock_ref = -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void SYNSCAN::thread_listen_synscan()
{
    logf_thread( (char*)"SYNSCAN::thread_listen_SYNSCAN()");

	struct sockaddr_in adresse;
	socklen_t          longueur;


	if ((sock_1 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

    int enable = 1;
    if (setsockopt(sock_1, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        perror("setsockopt(SO_REUSEADDR) failed");
		exit(EXIT_FAILURE);
    }
	
	memset(& adresse, 0, sizeof(struct sockaddr));
	adresse.sin_family = AF_INET;
	//adresse.sin_addr.s_addr = htonl(INADDR_ANY);
	adresse.sin_port = htons(10003);
	
#ifdef VAR_GLOBAL
	//sIP_init = VarManager::getInstance().gets("IP_INIT");
	inet_aton( sIP_synscan.c_str(), &adresse.sin_addr ); 
#else
#ifdef LOCALHOST
	inet_aton("127.0.0.1", &adresse.sin_addr ); 
#endif
#endif


	if (bind(sock_1, (struct sockaddr *) & adresse, sizeof(adresse)) < 0) {
		logf_thread( (char*) "[ERREUR] bind (sock_1)");
		sock_1 = -1;
		//exit(EXIT_FAILURE);
		return;
	}
	
	longueur = sizeof(struct sockaddr_in);
	if (getsockname(sock_1, (struct sockaddr *) & adresse, & longueur) < 0) {
		logf_thread( (char*) "[ERREUR] getsockname (sock_1)");
		return;
	}


    int option = 1;
    if(setsockopt(sock_1,SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),(char*)&option,sizeof(option)) < 0)
    {
		logf_thread( (char*) "[ERREUR] setsockopt (sock_1)");
        close(sock_1);
		return;

    }


    logf_thread( (char*)"---------------------------------------------------------------");
	logf_thread( (char*)"Init  Ecoute de IP = %s, Port = %u", inet_ntoa(adresse.sin_addr), ntohs(adresse.sin_port));
    logf_thread( (char*)"---------------------------------------------------------------");

	listen(sock_1, 5);
	while (listen_1) {
		longueur = sizeof(struct sockaddr_in);
		sock_ref = accept(sock_1, (struct sockaddr *) & adresse, & longueur);

		if (sock_ref < 0)			continue;
		
		char *some_addr;
        some_addr = inet_ntoa( adresse.sin_addr); // return the IP

		//sIP_init = string( some_addr );
	
		logf_thread( (char*)"SYNSCAN::thread_listen_SYNSCAN() connexion SOCKET 2" );
		logf_thread( (char*)"  sock = %d  sock_1 = %d  IP = %s:%d sur %s", sock_1, sock_ref, some_addr, (int)adresse.sin_port, sIP_synscan.c_str() );

		traite_connexion_synscan();
	}
    logf_thread( (char*)"Fermeture de sock_1 (%s:%u)", inet_ntoa(adresse.sin_addr), ntohs(adresse.sin_port) );
	close(sock_1);
	sock_1 = -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void SYNSCAN::start_synscan()
{
    th_1 = std::thread(&SYNSCAN::thread_listen_synscan, this);
    th_1.detach();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void SYNSCAN::write_stellarium(char* s)
{
#define LOG
    if ( sock_ref != -1 )
    {
        write( sock_ref, s, 24 );

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
void SYNSCAN::write_stellarium(int socket, char* s, int len)
{
	write_stellarium( socket, s, len, true );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void SYNSCAN::write_stellarium(int socket, char* s, int len, bool bLog)
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
//
// Envoi a stellarium la position du telescope
//
// Octet 	0 		= 24
//			1 		= 0
//			2 		= 0
//			3		= 0
//			4..11	= Heure (time(0) )
//			12..15	= AD(rad) / M_PI * 2147483648
//			16..19	= DC(rad) / M_PI * 2147483648
//--------------------------------------------------------------------------------------------------------------------
void SYNSCAN::write_stellarium(double ad, double dc)
{
    unsigned char buff[24];
    memset( buff, 0, 24);
    //--------------------------------------------------------
    buff[0] = 24;
    //--------------------------------------------------------
    time_t 			t 		= time(0);
    long long 		tl 		= (unsigned long long)t;
    unsigned char* 	conv 	= (unsigned char*)&tl;
    
    buff[4] = conv[0];
    buff[5] = conv[1];
    buff[6] = conv[2];
    buff[7] = conv[3];
    buff[8] = conv[4];
    buff[9] = conv[5];
    buff[10] = conv[6];
    buff[11] = conv[7];
    //--------------------------------------------------------
    float fa, fd;
    
    fa = ad;// / M_PI * 2147483648;
    fd = dc;//) / M_PI * 2147483648;

    if ( fa <0.0 )    {      
        fa = 180.0 + fa;
        fd = 180.0 - fd;
    }
    if ( fa >180.0 )    {      
        fa = fa - 360.0;
    }
    
    if ( fd >180.0)    {      
        fd = -fd + 180.0;
        if ( fa < 180.0 )       fa = -180.0 + fa;
    }
    
    float rfa = DEG2RAD(fa) / M_PI * 2147483648;
    float rfd = DEG2RAD(fd) / M_PI * 2147483648;

    int a = rfa;
    int d = rfd;
    //--------------------------------------------------------
    conv = (unsigned char*)&a;
    buff[12] = conv[0];
    buff[13] = conv[1];
    buff[14] = conv[2];
    buff[15] = conv[3];
    //--------------------------------------------------------
    conv = (unsigned char*)&d;
    buff[16] = conv[0];
    buff[17] = conv[1];
    buff[18] = conv[2];
    buff[19] = conv[3];
    //--------------------------------------------------------
    if ( ad!=0.0 && dc!=0.0 )
    {
		write_stellarium( (char*)buff );

        #ifdef LOG
    	struct hms HMS;
    	struct dms DMS;
		
        logf_thread( (char*)"Em Stellarium\tAd=%0.8f  \tDc=%0.8f", fa, fd );
        deg2hms( fa, HMS );
        deg2dms( fd, DMS );
        logf_thread( (char*)"				\tAd=%0.0fh%0.0f\'%0.2f\"  \tDc=%0.0fd\%2.0f'%2.2f\"", HMS.h, HMS.m, HMS.s, DMS.d, DMS.m, DMS.s );
    	#endif
    }
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

	//close(sock_1);
	//close(sock_1);
    //sleep(1);

    if ( sock_ref!= -1 )                shutdown(sock_ref, 2);

    
    if ( sock_1!= -1 )                  shutdown(sock_1, 2);
    //sleep(1);
    
    /*
    sock_1          = -1;
    sock_1          = -1;
    sock_stellarium = -1;
    sock_ref        = -1;
    */

    //sleep(1);
    /*
    logf_thread( (char*)"sock_1=%d sock_1=%d", sock_1, sock_1 );
    while( sock_1 != -1)	;
    while( sock_1!= -1)	;
    while( sock_stellarium != -1)	;
    while( sock_ref!= -1)	;
    */
    logf_thread( (char*)"sock_1=%d sock_1=%d", sock_1, sock_1 );
    log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void SYNSCAN::print_list()
{
    logf( (char*)"---- SYNSCAN::print_list()" );

	if ( sock_ref != -1 )				logf( (char*)"  SYNSCAN : %s", sIP_synscan.c_str() );
	else								logf( (char*)"  SYNSCAN listen" );

}    
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

