#include "serveur_mgr.h"
#include "camera_mgr.h"
#include "captures.h"
#include "var_mgr.h"
#include <sys/time.h>
#include <time.h>
//#include "MathlibD.h"
//--------------------------------------------------------------------------------------------------------------------
#define VAR_GLOBAL
//#define LOCALHOST
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Serveur_mgr::Serveur_mgr()
{
    logf((char*)"----------- Constructeur Serveur_mgr() -------------" );
    listen_1 = true;
    listen_2 = true;
    bTraite_depl = true;
    bTraite_init = true;

    sock_deplacement	= -1;
    uPort_deplacement	= 10001;
    sock_init			= -1;
    uPort_init			= 10002;

	#ifdef VAR_GLOBAL
	VarManager& var = VarManager::getInstance();

    if ( !var.existe("IP_INIT"))		var.set("IP_INIT", "127.0.0.1" );
    sIP_listen_init = *(var.gets("IP_INIT"));

    if ( !var.existe("IP_DEPL"))		var.set("IP_DEPL", "127.0.0.1" );
    sIP_listen_depl = *var.gets("IP_DEPL" );
	#endif
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
double Serveur_mgr::com2rad( int ra_int )
{
    double ad_rad = ra_int;
    ad_rad = ((ad_rad-0.5) * M_PI )/  (double)(2147483648.0) ;
    return ad_rad;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serveur_mgr::decode( struct stellarium& ss, unsigned char* buffer )
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
void Serveur_mgr::traite_connexion_init()
{
    unsigned char buffer[255];
    int n;

    while( bTraite_init )
    {
        n = read(sock_init, buffer, sizeof(buffer));
        logf_thread( (char*)"Serveur_mgr::traite_connexion_init()" );
        log_tab(true);

        if (n <= 0)
        {
            logf_thread( (char*)"[ERROR] Reading from socket (init)");
	        log_tab(false);
            break;
        }

        logf_thread( (char*)"reception de %d octets", n);
		buffer[n] = 0;

		if ( n == 20 )
		{
			//-------------------------------------------------
			// recuperation des infos
		    struct stellarium ss;
		    decode( ss, buffer );
			//-------------------------------------------------

		    double AD = com2rad(ss.ra);
		    double DC = com2rad(ss.dc);

		    logf_thread( (char*)"ra=%0.10lf, dc=%0.10lf", AD, DC );
			_sync_rad( AD, DC );
		}        

        log_tab(false);

    }
    
    logf_thread( (char*)"Init Deconnexion du sock_init %d", sock_init );
    close(sock_init);

    sock_init = -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serveur_mgr::thread_listen_init()
{
    //logf_thread( (char*)"Serveur_mgr::thread_listen_init()");

	struct sockaddr_in adresse;
	socklen_t          longueur;


	if ((sock_listen_init = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

    int enable = 1;
    if (setsockopt(sock_listen_init, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        perror("setsockopt(SO_REUSEADDR) failed");
		exit(EXIT_FAILURE);
    }
	
	memset(& adresse, 0, sizeof(struct sockaddr));
	adresse.sin_family = AF_INET;
	//adresse.sin_addr.s_addr = htonl(INADDR_ANY);
	adresse.sin_port = htons(uPort_init);
	
#ifdef VAR_GLOBAL
	//sIP_listen_init = VarManager::getInstance().gets("IP_INIT");
	inet_aton( sIP_listen_init.c_str(), &adresse.sin_addr ); 
#else
#ifdef LOCALHOST
	inet_aton("127.0.0.1", &adresse.sin_addr ); 
#endif
#endif


	if (bind(sock_listen_init, (struct sockaddr *) & adresse, sizeof(adresse)) < 0) {
		logf_thread( (char*) "[ERREUR] bind (sock_listen_init)Init ");
		sock_listen_init = -1;
		//exit(EXIT_FAILURE);
		return;
	}
	
	longueur = sizeof(struct sockaddr_in);
	if (getsockname(sock_listen_init, (struct sockaddr *) & adresse, & longueur) < 0) {
		logf_thread( (char*) "[ERREUR] getsockname (sock_listen_init) Init ");
		return;
	}


    int option = 1;
    if(setsockopt(sock_listen_init,SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),(char*)&option,sizeof(option)) < 0)
    {
		logf_thread( (char*) "[ERREUR] setsockopt (sock_listen_init Init )");
        close(sock_listen_init);
		return;

    }


    logf_thread( (char*)"---------------------------------------------------------------");
	logf_thread( (char*)"Ecoute sur IP = %s, Port = %u (Init)", inet_ntoa(adresse.sin_addr), ntohs(adresse.sin_port));
    logf_thread( (char*)"---------------------------------------------------------------");

	listen(sock_listen_init, 5);
	while (listen_2) {
		longueur = sizeof(struct sockaddr_in);
		int sock = accept(sock_listen_init, (struct sockaddr *) & adresse, & longueur);

		if (sock < 0)			continue;
		if ( sock_init != -1 )
		{
			close( sock );
			char *some_addr;
		    some_addr = inet_ntoa( adresse.sin_addr); // return the IP
			logf_thread( (char*)"[Warning] lx200 tentative de connection %s", some_addr );
			continue;
		}
		
		sock_init = sock;

		char *some_addr;
        some_addr = inet_ntoa( adresse.sin_addr); // return the IP
        sIP_init = string( some_addr );

		//sIP_listen_init = string( some_addr );
	
		logf_thread( (char*)"Serveur_mgr::thread_listen_init() connexion" );
		logf_thread( (char*)"  sock = %d  sock_listen_init = %d  IP = %s:%d sur %s", sock_listen_init, sock_init, some_addr, (int)adresse.sin_port, sIP_listen_init.c_str() );

		traite_connexion_init();
	}
    logf_thread( (char*)"Fermeture de sock_listen_init (%s:%u) Init ", inet_ntoa(adresse.sin_addr), ntohs(adresse.sin_port) );
	close(sock_listen_init);
	sock_listen_init = -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serveur_mgr::start_init()
{
    th_init = std::thread(&Serveur_mgr::thread_listen_init, this);
    th_init.detach();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serveur_mgr::traite_connexion_deplacement()
{
    unsigned char buffer[255];
    int n;

    while( bTraite_depl )
    {
        n = read(sock_deplacement,buffer,255);
        logf_thread( (char*)"Serveur_mgr::traite_connexion_deplacement()" );
        log_tab( true );

        if (n <= 0)
        {
            logf_thread( (char*)"ERROR reading from socket (deplacement)");
	        log_tab( false );
            break;  
        }

        logf_thread( (char*)"reception de %d octets", n);
		buffer[n] = 0;

		if ( n == 20 )
		{
			//-------------------------------------------------
			// recuperation des infos
		    struct stellarium ss;
		    decode( ss, buffer );
			//-------------------------------------------------

		    double AD = com2rad(ss.ra);
		    double DC = com2rad(ss.dc);

		    logf_thread( (char*)"ra=%0.10lf, dc=%0.10lf", AD, DC );
			_goto_rad( AD, DC );
		}        

        log_tab( false );
    }
    
    logf_thread( (char*)"Depl Deconnexion du sock_deplacement %d", sock_deplacement);
    close(sock_deplacement);

	system( (char*)"aplay /home/rene/.astropilot/sounds/cembalo-1.wav" );
    sock_deplacement = -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serveur_mgr::thread_listen_deplacement()
{
    //logf_thread( (char*)"Serveur_mgr::thread_listen_deplacement()");
    
	struct sockaddr_in 	adresse;		
	socklen_t          	longueur;
    int					option = 1;
    int					enable = 1;

	memset(& adresse, 0, sizeof(struct sockaddr));
	adresse.sin_family = AF_INET;

	if ((sock_listen_deplacement = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
        //close(sock_listen_deplacement);
        goto sortie_deplacement;
		exit(EXIT_FAILURE);
	}
	
    if (setsockopt(sock_listen_deplacement, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        perror("setsockopt(SO_REUSEADDR) failed");
        //close(sock_listen_deplacement);
        goto sortie_deplacement;
		exit(EXIT_FAILURE);
    }

	adresse.sin_port = htons(uPort_deplacement);
	
#ifdef VAR_GLOBAL
	//sIP_listen_depl = VarManager::getInstance().gets("IP_DEPL");
	inet_aton( sIP_listen_depl.c_str(), &adresse.sin_addr ); 
#else
#ifdef LOCALHOST
	inet_aton("127.0.0.1", &adresse.sin_addr ); 
#endif
#endif

	
	if (bind(sock_listen_deplacement, (struct sockaddr *) & adresse, sizeof(adresse)) < 0) {
		logf_thread( (char*) "[ERREUR] bind (sock_listen_deplacement) Depl ");
        //close(sock_listen_deplacement);
        goto sortie_deplacement;
		return;
	}
	
	longueur = sizeof(struct sockaddr_in);
	if (getsockname(sock_listen_deplacement, (struct sockaddr *) & adresse, & longueur) < 0) {
		logf_thread( (char*) "[ERREUR] getsockname (sock_listen_deplacement) Depl ");
        //close(sock_listen_deplacement);
        goto sortie_deplacement;
		return;
	}

    if(setsockopt(sock_listen_deplacement,SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),(char*)&option,sizeof(option)) < 0)
    {
		logf_thread( (char*) "[ERREUR] setsockopt (sock_listen_deplacement) Depl ");
        //close(sock_listen_deplacement);
        goto sortie_deplacement;
		return;
    }


    logf_thread( (char*)"---------------------------------------------------------------");
	logf_thread( (char*)"Ecoute sur IP = %s, Port = %u (Deplacement)", inet_ntoa(adresse.sin_addr), ntohs(adresse.sin_port));
    logf_thread( (char*)"---------------------------------------------------------------");

	listen(sock_listen_deplacement, 5);
	while ( listen_1 ) {
		longueur = sizeof(struct sockaddr_in);
		int sock = accept(sock_listen_deplacement, (struct sockaddr *) & adresse, & longueur);

		if (sock < 0)			continue;
		if ( sock_deplacement != -1 )
		{
			close( sock );
			char *some_addr;
		    some_addr = inet_ntoa( adresse.sin_addr); // return the IP
			logf_thread( (char*)"[Warning] lx200 tentative de connection %s", some_addr );
			continue;
		}
		
		sock_deplacement = sock;
		
		char *some_addr;
        some_addr = inet_ntoa( adresse.sin_addr);
        sIP_depl = string( some_addr );

		logf_thread( (char*)"Serveur_mgr::thread_listen_deplacement() connexion" );
		logf_thread( (char*)"  sock = %d  sock_listen_deplacement = %d  IP = %s:%d sur %s", sock_listen_deplacement, sock_deplacement, some_addr, (int)adresse.sin_port, sIP_listen_depl.c_str() );

		system( (char*)"aplay /home/rene/.astropilot/sounds/cembalo-1.wav" );
		traite_connexion_deplacement();
	}

sortie_deplacement:
    logf_thread( (char*)"Fermeture de Depl  (%s:%u)", inet_ntoa(adresse.sin_addr), ntohs(adresse.sin_port) );
	close(sock_listen_deplacement);
	sock_listen_deplacement = -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serveur_mgr::start_deplacement()
{
    th_depl = std::thread(&Serveur_mgr::thread_listen_deplacement, this);
    th_depl.detach();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serveur_mgr::write_stellarium(char* s)
{
//#define LOG
    if ( sock_deplacement != -1 )
    {
        write( sock_deplacement, s, 24 );

        #ifdef LOG
        char trame[24*4+80]  = "";
        char t[24*4+80] = "";
        
        trame[0] = 0;
        
        for (int i=0; i<24; i++ )	{
        	strcpy( t, trame );
        	unsigned char c = s[i];
        	snprintf( (char*)trame, sizeof(trame), "%s %02X", t, c );
        
        }
        logf_thread( (char*)"Envoid a stellarium  %s", trame );
        #endif
    }
//#undef LOG
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serveur_mgr::write_stellarium(int socket, char* s, int len)
{
	write_stellarium( socket, s, len, true );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serveur_mgr::write_stellarium(int socket, char* s, int len, bool bLog)
{
#define __LOG
    if ( socket != -1 )
    {
        write( socket, s, len );

        #ifdef __LOG
        char trame[24*4+80]  = "";
        char t[23*4+80] = "";
        
        trame[0] = 0;
        
        for (int i=0; i<len; i++ )	{
        	strcpy( t, trame );
        	unsigned char c = s[i];
        	snprintf( (char*)trame, sizeof(trame), "%s %02X", t, c );
        
        }
        if ( bLog )
	        logf_thread( (char*)"  | Envoi a stellarium  \"%s\" - %s",s,  trame );
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
void Serveur_mgr::write_stellarium(double ad, double dc)
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
    double fa, fd;
    
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
    
    double rfa = DEG2RAD(fa) / M_PI * 2147483648;
    double rfd = DEG2RAD(fd) / M_PI * 2147483648;

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
		
        logf_thread( (char*)"Em Stellarium\tAd=%0.8lf  \tDc=%0.8lf", fa, fd );
        deg2hms( fa, HMS );
        deg2dms( fd, DMS );
        logf_thread( (char*)"				\tAd=%0.0lfh%0.0lf\'%0.2lf\"  \tDc=%0.0lfd\%2.0lf'%2.2lf\"", HMS.h, HMS.m, HMS.s, DMS.d, DMS.m, DMS.s );
    	#endif
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serveur_mgr::_goto( double ra, double dc )
{
    struct hms HMS;
    rad2hms( ra, HMS );
    logf_thread( (char*)"AD : %02dh%02dm%0.2lfs (%0.8lf)", (int)HMS.h, (int)HMS.m, HMS.s, ra );
    
    struct dms DMS;
    rad2dms( dc, DMS );
    logf_thread( (char*)"DC : %02dd%02d\'%0.2lf\" (%0.8lf)", (int)DMS.d, (int)DMS.m, DMS.s, dc );
    
    char cmd[255];
    sprintf( cmd, "A%lf;D%lf", ra, dc );
    logf_thread( (char*)"Serveur_mgr::_goto() envoi arduino : %s", cmd );
 
    if ( Serial::getInstance().write_string(cmd) != 0 )
    {
	    logf_thread( (char*)"[ERROR] Serial" );
    }

	
	if (bSound)     system( (char*)"aplay /home/rene/.astropilot/sounds/xylofon.wav" );
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serveur_mgr::_sync( double ra, double dc )
{
    logf_thread( (char*)"Serveur_mgr::_sync(( %lf, %lf )", ra, dc );


	Serial& serial = Serial::getInstance();

    if ( !serial.isConnect() )
    {
    	write_stellarium( ra, dc );
		change_ad_status( ra );
		change_dc_status( dc );
    }
	//return;
    struct hms HMS;			deg2hms( ra, HMS );
	struct dms DMS;			deg2dms( dc, DMS );
	
    logf_thread( (char*)"  AD : %02dh%02dm%0.2lfs (%0.8lf)", (int)HMS.h, (int)HMS.m, HMS.s, ra );
    logf_thread( (char*)"  DC : %02dd%02d\'%0.2lf\" (%0.8lf)", (int)DMS.d, (int)DMS.m, DMS.s, dc );
    
	//-------------------------------------------------
    if ( serial.isConnect() )
    {
		char cmd[255];

		snprintf( (char*)cmd, sizeof(cmd), "ia%lf;id%lf", ra, dc );
    	logf_thread( (char*)"Serveur_mgr::_sync() envoi arduino : %s", cmd );

		if (  serial.write_string( (char*)cmd ) != 0 )
		{
	    	logf_thread( (char*)"[ERROR] Serial" );
		}
		//else
		if (bSound)     system( (char*)"aplay /home/rene/.astropilot/sounds/prompt.wav" );
    }
	//-------------------------------------------------
    //logf_thread( (char*)"Envoi Camera_mgr et Captures position (ra,dec) = (%0.6lf, %0.6lf)", ra, dc );
	//Camera_mgr::getInstance().position(ra, dc);
	// Captures::getInstance().position(ra, dc);
	//-------------------------------------------------

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serveur_mgr::_goto_rad( double ra, double dc )
{
	_goto( RAD2DEG(ra), RAD2DEG(dc) );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serveur_mgr::_sync_rad( double ra, double dc )
{
	double dra = RAD2DEG(ra);
	double ddc = RAD2DEG(dc);
	_sync( dra, ddc );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serveur_mgr::close_all()
{
    logf_thread( (char*)"Fermeture de tous les sockets ..." );
    log_tab(true);

    listen_1 = false;
    listen_2 = false;
    bTraite_depl = false;
    bTraite_init = false;

    if ( sock_deplacement!= -1 )         shutdown(sock_deplacement, 2);
    if ( sock_init!= -1 )                shutdown(sock_init, 2);

    
    if ( sock_listen_deplacement!= -1 )           shutdown(sock_listen_deplacement, 2);
    if ( sock_listen_init!= -1 )                  shutdown(sock_listen_init, 2);

    logf_thread( (char*)"sock_listen_deplacement=%d sock_listen_init=%d", sock_listen_deplacement, sock_listen_init );
    log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serveur_mgr::print_list()
{
    //logf( (char*)"---- Serveur_mgr::print_list()" );

	if ( sock_listen_init == -1 )		logf( (char*)"  STEL Init\timpossible d'ouvrir : %s", sIP_listen_init.c_str() );
	else if ( sock_init != -1 )			logf( (char*)"  STEL Init\tconnexion de \t: %s", sIP_init.c_str() );
	else								logf( (char*)"  STEL Init\tlisten sur \t\t: %s : %d", sIP_listen_init.c_str(), uPort_init );

	if ( sock_listen_deplacement == -1)	logf( (char*)"  STEL depl\timpossible d'ouvrir : %s", sIP_listen_depl.c_str() );
	else if ( sock_deplacement != -1)	logf( (char*)"  STEL depl\tconnexion de \t: %s", sIP_depl.c_str() );
	else								logf( (char*)"  STEL depl\tlisten sur \t\t: %s : %d", sIP_listen_depl.c_str(), uPort_deplacement );
}    
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

