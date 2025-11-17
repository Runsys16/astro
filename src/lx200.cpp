#include "lx200.h"
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
LX200::LX200()
{
    logf((char*)"----------- Constructeur LX200() -------------" );
    listen_1 = true;
    traite_1 = true;

    sock_ref		= -1;

	#ifdef VAR_GLOBAL
	VarManager& var = VarManager::getInstance();

    if ( !var.existe("IP_LX200"))		var.set("IP_LX200", "127.0.0.1" );
    sIP_lx200 = *var.gets("IP_LX200" );

	#endif
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
float LX200::com2rad( int ra_int )
{
    float ad_rad = ra_int;
    ad_rad = ((ad_rad-0.5) * M_PI )/  (float)(2147483648.0) ;
    return ad_rad;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void LX200::decode( struct stellarium& ss, unsigned char* buffer )
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
void LX200::traite_connexion_lx200()
{
    unsigned char buffer[255];
    int n;

    while( traite_1 )
    {
        n = read(sock_ref, buffer, 255);
        //logf_thread( (char*)"LX200::traite_connexion_init()" );
        log_tab(true);

        if (n <= 0)
        {
            logf_thread( (char*)"ERROR reading from socket (init)");
	        log_tab(false);
            break;
        }

		buffer[n] = 0;
        //logf_thread( (char*)"Init nb octet lu : %d   \"%s\"", n, (char*)buffer );

		if ( buffer[0] == '#' )			
		{ 
			if ( buffer[0] == '#' && buffer[1] == '#' )
			{
				write_stellarium( sock_ref, (char*)"##", 2 );	
				log_tab(false);
				continue;
			}
			else
			if ( buffer[0] == '#' && buffer[1] == 0x6 )
			{
				write_stellarium( sock_ref, (char*)"P", 1 );	
				log_tab(false);
				continue;
			}
			else
			if ( strcmp( (char*)buffer, "#:GVP#" ) == 0 )
			{
				logf_thread( (char*)"%s Get Telescope NAME", buffer );
				write_stellarium( sock_ref, (char*)"EQ5_DRIVE#", 10, true );
				log_tab(false);
				continue;
			}
			else
			if ( strcmp( (char*)buffer, "#:GVN#" ) == 0 )
			{
				logf_thread( (char*)"%s Get Firmware Version", buffer );
				write_stellarium( sock_ref, (char*)"00.7#", 5, true );	
				log_tab(false);
				continue;
			}
			else
			if ( strcmp( (char*)buffer, "#:GVD#" ) == 0 )
			{
				logf_thread( (char*)"%s Get Firmware Date", buffer );
				write_stellarium( sock_ref, (char*)"NOV 17 2025#", 12, true );	
				log_tab(false);
				continue;
			}
			else
			if ( strcmp( (char*)buffer, "#:GVT#" ) == 0 )
			{
				logf_thread( (char*)"%s Get Firmware Heure", buffer );
				write_stellarium( sock_ref, (char*)"15:03:25#", 9, true );
				log_tab(false);
				continue;
			}
			else
			if ( strcmp( (char*)buffer, "#:Gg#" ) == 0 )
			{
				logf_thread( (char*)"%s Get Longiture", buffer );
				write_stellarium( sock_ref, (char*)"s000*00#", 8, true );
				log_tab(false);
				continue;
			}
			else
			if ( strcmp( (char*)buffer, "#:Gt#" ) == 0 )
			{
				logf_thread( (char*)"%s Get Lattitude", buffer );
				write_stellarium( sock_ref, (char*)"s45*53#", 7, true );
				log_tab(false);
				continue;
			}
			else
			if ( strcmp( (char*)buffer, "#:GC#" ) == 0 )
			{
				char 			sTime[30];
				struct timeval	tv;
				struct tm*		tm;

				gettimeofday( &tv, NULL );
				tm=localtime(&tv.tv_sec);
				
				snprintf( (char*)sTime, sizeof(sTime),"%02d/%02d/%02d#", tm->tm_mon+1, tm->tm_mday, tm->tm_year-100 );

				logf_thread( (char*)"%s Get Local Date  : %s", buffer, sTime );
				write_stellarium( sock_ref, (char*)sTime, strlen(sTime), true );
				log_tab(false);
				continue;
			}
			else
			if ( strcmp( (char*)buffer, "#:GL#" ) == 0 )
			{
				char 			sTime[20];
				struct timeval	tv;
				struct tm*		tm;

				gettimeofday( &tv, NULL );
				tm=localtime(&tv.tv_sec);
				
				snprintf( (char*)sTime, sizeof(sTime),"%02d:%02d:%02d#", tm->tm_hour, tm->tm_min, tm->tm_sec );

				logf_thread( (char*)"%s Get Local Time   %s", buffer, sTime );
				write_stellarium( sock_ref, sTime, strlen(sTime), true );	

				log_tab(false);
				continue;
			}
			///------------------------------------------------------
			else
			if ( strcmp( (char*)buffer, "#:GD#" ) == 0 )
			{
				//logf_thread( (char*)"%s Get Telescope DEC", buffer );
				struct dms	DMS;
				char		s[80];
				char		sign[2];
				
				deg2dms( d_deg_dc, DMS );
				if ( DMS.d >= 0 )		sign[0] = '+';
				else					sign[0] = '-';
				sign[1] = 0;
				
				DMS.d = abs(DMS.d);
				snprintf( s, sizeof(s), "%s%02d*%02d\'%02d#", sign, (int)DMS.d, (int)DMS.m, (int)DMS.s );


				write_stellarium( sock_ref, s, strlen(s), false );	
				log_tab(false);
				continue;
			}
			///------------------------------------------------------
			else
			if ( strcmp( (char*)buffer, "#:GR#" ) == 0 )
			{
				//logf_thread( (char*)"%s Get Telescope RA", buffer );

				struct hms	HMS;
				char		s[80];
				
				deg2hms( d_deg_ad, HMS );
				snprintf( s, sizeof(s), "%02d:%02d:%02d#", (int)HMS.h, (int)HMS.m, (int)HMS.s );

				write_stellarium( sock_ref, (char*)s, strlen(s), false );	
				log_tab(false);
				continue;
			}
			///------------------------------------------------------
			else
			if ( strcmp( (char*)buffer, "#:D#" ) == 0 )
			{
				//logf_thread( (char*)"Distance Bars" );
				write_stellarium( sock_ref, (char*)"s#", 2, false );
				log_tab(false);
				continue;
			}
			else
			if ( strcmp( (char*)buffer, "#:GW#" ) == 0 )
			{
				//logf_thread( (char*)"%s Get Scope Alignment Status", buffer );
				write_stellarium( sock_ref, (char*)"PN3#", 4, false );	
				log_tab(false);
				continue;
			}
			else
			if ( strcmp( (char*)buffer, "#:GG#" ) == 0 )
			{
				logf_thread( (char*)"%s Get UTC offset time", buffer );
				write_stellarium( sock_ref, (char*)"-01#", 4, true );
				log_tab(false);
				continue;
			}
			else
			if ( strcmp( (char*)buffer, "#:Q#" ) == 0 )
			{
				logf_thread( (char*)"%s Abort", buffer );
				log_tab(false);
				continue;
			}
			else
			if (	buffer[0] == '#' &&
					buffer[1] == ':' &&
					buffer[2] == 'S' &&
					buffer[3] == 'r' )
			{
				logf_thread( (char*)"%s Set target object RA", buffer );
				write_stellarium( sock_ref, (char*)"1", 1, true );	
				
				struct hms	 HMS;
				HMS.h = (buffer[4]-'0') * 10 + (buffer[5]-'0');
				HMS.m = (buffer[7]-'0') * 10 + (buffer[8]-'0');
				HMS.s = (buffer[10]-'0') * 10 + (buffer[11]-'0');
				
				double ra = hms2rad(HMS);
				change_ad_status( RAD2DEG(ra) );
				
				log_tab(false);
				continue;
			}
			else
			if (	buffer[0] == '#' &&
					buffer[1] == ':' &&
					buffer[2] == 'S' &&
					buffer[3] == 'd' )
			{
				logf_thread( (char*)"%s Set target object DEC", buffer );
				write_stellarium( sock_ref, (char*)"1", 1, true );	
				
				struct dms	 DMS;
				int sign;

				if ( buffer[4] == '+' )		sign = 1;
				else						sign = -1;
				DMS.d = sign * (buffer[5]-'0') * 10 + (buffer[6]-'0');
				DMS.m = (buffer[8]-'0') * 10 + (buffer[9]-'0');
				DMS.s = (buffer[11]-'0') * 10 + (buffer[12]-'0');
				
				double dc = dms2rad(DMS);
				change_dc_status( RAD2DEG(dc) );
				
				log_tab(false);
				continue;
			}
			else
			if ( strcmp( (char*)buffer, "#:MS#" ) == 0 )
			{
				logf_thread( (char*)"%s Slew to Target Object", buffer );
				write_stellarium( sock_ref, (char*)"0", 1, true );	
				log_tab(false);
				continue;
			}
			/*
			else
			if ( strcmp( (char*)buffer, "#:D#" ) == 0 )
			{
				write_stellarium( sock_ref, (char*)"s#", 2 );	
				logf_thread( (char*)"Distnace Bars" );
				log_tab(false);
				continue;
			}
			else
			if ( strcmp( (char*)buffer, "#:D#" ) == 0 )
			{
				write_stellarium( sock_ref, (char*)"s#", 2 );	
				logf_thread( (char*)"Distnace Bars" );
				log_tab(false);
				continue;
			}
			*/
			else
			{
				logf_thread( (char*)"--[WARNING] \"%s\" Commande inconnue", buffer );
			}
			
			log_tab(false);
			continue;
		}

		//-------------------------------------------------
		// recuperation des infos
        struct stellarium ss;
        decode( ss, buffer );
		//-------------------------------------------------

        double ra = com2rad(ss.ra);
        double dc = com2rad(ss.dc);

        logf_thread( (char*)"reception de ra=%0.10f, dc=%0.10f", ra, dc);
        
        if ( !Serial::getInstance().isConnect() )
        {
        	write_stellarium( RAD2DEG(ra), RAD2DEG(dc) );
			change_ad_status( RAD2DEG(ra) );
			change_dc_status( RAD2DEG(dc) );
        }



        struct hms HMS;
        rad2hms( ra, HMS );
        logf_thread( (char*)"AD : %02dh%02dm%0.2fs (%0.8f)", (int)HMS.h, (int)HMS.m, HMS.s, RAD2DEG(ra) );
        
        struct dms DMS;
        rad2dms( dc, DMS );
        logf_thread( (char*)"Dc : %02dd%02d\'%0.2f\" (%0.8f)", (int)DMS.d, (int)DMS.m, DMS.s, RAD2DEG(dc) );
        
        char cmd[255];
        sprintf( cmd, "ia%f;id%f", RAD2DEG(ra), RAD2DEG(dc) );

		//-------------------------------------------------
        logf_thread( (char*)"Envoi arduino : \"%s\"", cmd );
        Serial::getInstance().write_string(cmd);
		//-------------------------------------------------
        logf_thread( (char*)"Envoi Camera_mgr et Captures position (ra,dec) = (%0.6lf, %0.6lf)", ra, dc );
		Camera_mgr::getInstance().position(ra, dc);
        Captures::getInstance().position(ra, dc);
		//-------------------------------------------------

        Xref = 0.0;
        Yref = 0.0;
        //Zref = 0.0;
        log_tab(false);

    }
    
    logf_thread( (char*)"Deconnexion du sock_init %d", sock_ref );
    close(sock_ref);

    sock_ref = -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void LX200::thread_listen_lx200()
{
    logf_thread( (char*)"LX200::thread_listen_lx200()");

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
	inet_aton( sIP_lx200.c_str(), &adresse.sin_addr ); 
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
	
		logf_thread( (char*)"LX200::thread_listen_lx200() connexion SOCKET 2" );
		logf_thread( (char*)"  sock = %d  sock_1 = %d  IP = %s:%d sur %s", sock_1, sock_ref, some_addr, (int)adresse.sin_port, sIP_lx200.c_str() );

		traite_connexion_lx200();
	}
    logf_thread( (char*)"Fermeture de sock_1 (%s:%u)", inet_ntoa(adresse.sin_addr), ntohs(adresse.sin_port) );
	close(sock_1);
	sock_1 = -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void LX200::start_lx200()
{
    th_1 = std::thread(&LX200::thread_listen_lx200, this);
    th_1.detach();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void LX200::write_stellarium(char* s)
{
//#define LOG
    if ( sock_ref != -1 )
    {
        write( sock_ref, s, 24 );

        #ifdef LOG
        char trame[24*4+80]  = "";
        char t[24*4+80] = "";
        
        trame[0] = 0;
        
        for (int i=0; i<24; i++ )	{
        	strcpy( t, trame );
        	unsigned char c = s[i];
        	snprintf( (char*)trame, sizeof(trame), "%s %02X", t, c );
        
        }
        logf_thread( (char*)"Envoid a lx200  %s", trame );
        #endif
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void LX200::write_stellarium(int socket, char* s, int len)
{
	write_stellarium( socket, s, len, true );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void LX200::write_stellarium(int socket, char* s, int len, bool bLog)
{
#define __LOG
    if ( socket != -1 )
    {
        write( socket, s, len );

        #ifdef __LOG
        char trame[24*4+80]  = "";
        char t[24*4+80] = "";
        
        trame[0] = 0;
        
        for (int i=0; i<len; i++ )	{
        	strcpy( t, trame );
        	unsigned char c = s[i];
        	snprintf( (char*)trame, sizeof(trame), "%s %02X", t, c );
        
        }
        if ( bLog )
	        logf_thread( (char*)"  | Envoi a lx200  \"%s\" - %s",s,  trame );
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
void LX200::write_stellarium(double ad, double dc)
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
void LX200::close_all()
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
void LX200::print_list()
{
    logf( (char*)"---- LX200::print_list()" );

	if ( sock_ref != -1 )				logf( (char*)"  Init : %s", sIP_lx200.c_str() );
	else								logf( (char*)"  Init listen" );

}    
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

