#include "synscan.h"
#include "camera_mgr.h"
#include "captures.h"
#include "var_mgr.h"
#include "serveur_mgr.h"
#include <sys/time.h>
#include <time.h>
//--------------------------------------------------------------------------------------------------------------------
//
//  Protocole SYNSCAN (skywatcher)
//  ------------------------------
//  https://inter-static.skywatcher.com/downloads/synscanserialcommunicationprotocol_version33.pdf
//
//--------------------------------------------------------------------------------------------------------------------
#define VAR_GLOBAL
//#define BAVARD
#ifdef BAVARD
	#define B_BAVARD true
#else
	#define B_BAVARD false
#endif
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
void SYNSCAN::decode_time(char* s, int len)
{
	double dc;
	
	for(int i=0; i<len; i++ )
	{
		if ( s[i]>='A' )	s[i]+='0'-'A'+10;
	}
	
	HH = s[1];
	MN = s[2];
	SS = s[3];
	MM = s[4];
	JJ = s[5];
	AA = s[6];
	GMT = (int)s[7];
	HETE = s[8];
	
	logf_thread( (char*)"      %04d/%02d/%02d  %02d:%02d:%02d GMT %d Heure Ete %d", 2000+AA,MM,JJ, HH,MN,SS, GMT, HETE );
}
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void SYNSCAN::encode_time(char* s, int len)
{
//#define DEBUG
	struct timeval	tv;
	struct tm*		tm;

	gettimeofday( &tv, NULL );
	tm=localtime(&tv.tv_sec);
	
#ifdef DEBUG
	logf_thread( (char*)"%d %d %d %d %d %d %d %d #", \
		tm->tm_hour, tm->tm_min, tm->tm_sec, tm->tm_mon+1, tm->tm_mday, tm->tm_year+1900-2000, 1, 0 );
#endif
	snprintf( (char*)s, len,"%c%c%c%c%c%c%c%c#", \
		tm->tm_hour, tm->tm_min, tm->tm_sec, tm->tm_mon+1, tm->tm_mday, tm->tm_year+1900-2000, 1, 0 );

//#undef DEBUG
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void SYNSCAN::decode_precise(char *s, int len)
{
	double ad;
	double dc;
	
	for(int i=0; i<len; i++ )
	{
		if ( s[i]>='A' )	s[i]+='0'-'A'+10;
	}
	
	ad = (s[1]-'0');
	ad = (s[2]-'0') +  ad * 16;
	ad = (s[3]-'0') +  ad * 16;
	ad = (s[4]-'0') +  ad * 16;
	ad = (s[5]-'0') +  ad * 16;
	ad = (s[6]-'0') +  ad * 16;
	ad = ad / 16777216.0 * 360.0;
	
	dc = (s[10]-'0');
	dc = (s[11]-'0') +  dc * 16;
	dc = (s[12]-'0') +  dc * 16;
	dc = (s[13]-'0') +  dc * 16;
	dc = (s[14]-'0') +  dc * 16;
	dc = (s[15]-'0') +  dc * 16;
	dc = dc / 16777216.0 * 360.0;
	
	//change_ad_status( ad );
	//change_dc_status( dc );
	AD_deg = ad;
	DC_deg = dc;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void SYNSCAN::encode_precise(char *s, int len)
{
	double ad = d_deg_ad;
	double dc = d_deg_dc;
	
#ifdef DEBUG
	logf_thread( (char*)" %lf, %lf", ad, dc );
#endif

	if ( ad < 0.0)	ad = 360.0 + ad;
	if ( dc < 0.0)	dc = 360.0 + dc;
#ifdef DEBUG
	logf_thread( (char*)" %lf, %lf", ad, dc );
#endif
	
	ad = ad / 360.0 * 16777216.0;
	dc = dc / 360.0 * 16777216.0;
#ifdef DEBUG
	logf_thread( (char*)" %lf, %lf", ad, dc );
#endif
	//unsigned uad = (unsigned)ad 
	//unsigned udc
	snprintf( (char*)s, len, "%06X00,%06X00#", (unsigned)ad, (unsigned)dc );
}
//--------------------------------------------------------------------------------------------------------------------
//
//			Traite le communication synscan
//
//--------------------------------------------------------------------------------------------------------------------
void SYNSCAN::traite_command( char* buffer, int n)
{
	///------------------------------------------------------
	// Get Version
	///------------------------------------------------------
	if ( strncmp( (char*)buffer, "V", 1 ) == 0 )
	{
		to_hex( buffer, n );
		logf_thread( (char*)"V : Get Version", sHex );
		write_synscan( sock_synscan, (char*)"042507#", 7, true );	
	}
	else
	///------------------------------------------------------
	// Get Device Version
	// Devices include:
	// 16 = AZM/RA Motor
	// 17 = ALT/DEC Motor
	///------------------------------------------------------
	if ( strncmp( (char*)buffer, "P", 1 ) == 0 )
	{
		to_hex( buffer, n );
		if ( buffer[1] == 1 )
		{
			logf_thread( (char*)"P : Get Device Version P... %s", sHex );
			if ( buffer[2] == 16 )
			{
				write_synscan( sock_synscan, (char*)"\x4\x4#", 3, true );	
			}
			else
			if ( buffer[2] == 17 )
			{
				write_synscan( sock_synscan, (char*)"\x4\x4#", 3, true );	
			}
			else
			if ( (char)buffer[2] == (char)0xB0 )
			{
				write_synscan( sock_synscan, (char*)"44#", 3, true );	
			}
			else
				logf_thread( (char*)"--[WARNING] Commande P Inconnue : %s", sHex );
		}
		else
			logf_thread( (char*)"P : inconnu  %s", sHex );
	}
	else
	///------------------------------------------------------
	// Get Model
	// 0 = EQ6 GOTO Series
	// 1 = HEQ5 GOTO Series
	// 2 = EQ5 GOTO Series
	// 3 = EQ3 GOTO Series
	// 4 = EQ8 GOTO Series
	// 5 = AZ-EQ6 GOTO Series
	// 6 = AZ-EQ5 GOTO Series
	// 128 ~ 143 = AZ GOTO
	// Series
	// 144 ~ 159 = DOB GOTO
	// Series
	// 160 = Al
	///------------------------------------------------------
	if ( strncmp( (char*)buffer, "m", 1 ) == 0 )
	{
		to_hex( buffer, n );
		logf_thread( (char*)"m : Get model %s %s", buffer, sHex );
		write_synscan( sock_synscan, (char*)"\x02#", 2, true );	
	}
	else
	///------------------------------------------------------
	// Get Location
	///------------------------------------------------------
	if ( strncmp( (char*)buffer, "w", 1 ) == 0 )
	{
		to_hex( buffer, n );
		logf_thread( (char*)"w : Get Location %s %s", buffer, sHex );

		char resp[80];
		snprintf( (char*)resp, sizeof(resp), "%c%c%c%c%c%c%c%c#", 0x2D,0x10,0x18,0,  0,2,0x13,0 );
		write_synscan( sock_synscan, (char*)resp, 9, true );	
	}
	else
	///------------------------------------------------------
	// Get Time
	///------------------------------------------------------
	if ( strncmp( (char*)buffer, "h", 1 ) == 0 )
	{
		to_hex( buffer, n );
		logf_thread( (char*)"h : Get Time %s %s", buffer, sHex );

		char resp[80];
		encode_time(resp, sizeof(resp));
		write_synscan( sock_synscan, (char*)resp, 9, true );	
	}
	else
	///------------------------------------------------------
	// Get precise RA/DEC
	///------------------------------------------------------
	if ( strncmp( (char*)buffer, "e", 1 ) == 0 )
	{
		to_hex( buffer, n );
	#ifdef BAVARD
		logf_thread( (char*)"e : Get precise RA/DEC %s %s", buffer, sHex );
	#endif

		char resp[80];
		encode_precise(resp, sizeof(resp));
		//logf_thread( (char*)"e : Envoi %s", resp );
		write_synscan( sock_synscan, (char*)resp, 18, B_BAVARD );	
	}
	else
	///------------------------------------------------------
	// Get Tracking Mode
	// 0 = Tracking off
	// 1 = Alt/Az tracking
	// 2 = Equatorial tracking
	// 3 = PEC mode (Sidereal + PEC)
	///------------------------------------------------------
	if ( strncmp( (char*)buffer, "t", 1 ) == 0 )
	{
		to_hex( buffer, n );
	#ifdef BAVARD
		logf_thread( (char*)"t : Get Tracking Mode %s %s", buffer, sHex );
	#endif
		write_synscan( sock_synscan, (char*)"\x02#", 2, B_BAVARD );	
	}
	else
	///------------------------------------------------------
	// Set Location
	///------------------------------------------------------
	if ( strncmp( (char*)buffer, "W", 1 ) == 0 )
	{
		to_hex( buffer, n );
		logf_thread( (char*)"W : Set Location %s %s", buffer, sHex );
		write_synscan( sock_synscan, (char*)"#", 1, true );	
	}
	else
	///------------------------------------------------------
	// Set Time
	///------------------------------------------------------
	if ( strncmp( (char*)buffer, "H", 1 ) == 0 )
	{
		to_hex( buffer, n );
		logf_thread( (char*)"H : Set Time  %s", sHex );
		decode_time(buffer, n);
		write_synscan( sock_synscan, (char*)"#", 1, true );	
	}
	else
	///------------------------------------------------------
	// Is GOTO in Progress?
	//    - Response is ASCII “0”  or “1”
	///------------------------------------------------------
	if ( strncmp( (char*)buffer, "L", 1 ) == 0 )
	{
		to_hex( buffer, n );
	#ifdef BAVARD
		logf_thread( (char*)"L : Is GOTO in Progress? %s %s", buffer, sHex );
	#endif
		write_synscan( sock_synscan, (char*)"0#", 2, B_BAVARD );	
	}
	else
	///------------------------------------------------------
	// Is Alignment Complete?
	//   - align=1 if aligned    and 0 if not
	///------------------------------------------------------
	if ( strncmp( (char*)buffer, "J", 1 ) == 0 )
	{
		to_hex( buffer, n );
	#ifdef BAVARD
		logf_thread( (char*)"J : Is Alignment Complete? %s %s", buffer, sHex );
	#endif
		write_synscan( sock_synscan, (char*)"\x1#", 2, B_BAVARD );	
	}
	else
	///------------------------------------------------------
	// Cancel GOTO
	///------------------------------------------------------
	if ( strncmp( (char*)buffer, "M", 1 ) == 0 )
	{
		to_hex( buffer, n );
		logf_thread( (char*)"M : Cancel GOTO %s", buffer );
		//decode_precise(buffer, n);
	
		write_synscan( sock_synscan, (char*)"#", 1, true );	
	}
	else
	///------------------------------------------------------
	// Sync precise RA/DEC
	///------------------------------------------------------
	if ( strncmp( (char*)buffer, "s", 1 ) == 0 )
	{
		to_hex( buffer, n );
		logf_thread( (char*)"s : Sync precise %s", buffer );
		decode_precise(buffer, n);
		Serveur_mgr::getInstance()._sync( AD_deg, DC_deg );
	
		write_synscan( sock_synscan, (char*)"#", 1, true );	
	}
	else
	///------------------------------------------------------
	// GOTO precise RA/DEC
	///------------------------------------------------------
	if ( strncmp( (char*)buffer, "r", 1 ) == 0 )
	{
		to_hex( buffer, n );
		logf_thread( (char*)"r : GOTO precise RA/DEC %s", buffer );
		decode_precise(buffer, n);
		Serveur_mgr::getInstance()._goto( AD_deg, DC_deg );

		write_synscan( sock_synscan, (char*)"#", 1, true );	
	}
	else
	///------------------------------------------------------
	// Commande inconnue
	///------------------------------------------------------
	{
		to_hex( buffer, n );
		logf_thread( (char*)"--[WARNING] \"%s\" Commande inconnue %s", buffer, sHex );
	}
		
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


		if ( strncmp( (char*)buffer, "Ka", 2 ) == 0 )
		{ 
			logf_thread( (char*)"K : Reception de %s", buffer );
			write_synscan( sock_synscan, (char*)"a#", 2, true );
		}
		else
			traite_command( (char*)buffer, n );

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
	logf_thread( (char*)"[Warning] %s", BOOL2STR(listen_1) );
	while (listen_1) {
		logf_thread( (char*)"[Warning] Synscan tentative de connection " );
		longueur = sizeof(struct sockaddr_in);
		int sock = accept(sock_listen_synscan, (struct sockaddr *) & adresse, & longueur);

		if (sock < 0)			continue;
		if ( sock_synscan != -1 )
		{
			close( sock_synscan );
			char *some_addr;
		    some_addr = inet_ntoa( adresse.sin_addr); // return the IP
		    sIP_synscan = string(some_addr);
		    
			logf_thread( (char*)"[Warning] Synscan tentative de connection %s", some_addr );
			continue;
		}
		
		sock_synscan = sock;
		
		char *some_addr;
        some_addr = inet_ntoa( adresse.sin_addr); // return the IP
	    sIP_synscan = string(some_addr);

		//sIP_init = string( some_addr );
	
		logf_thread( (char*)"SYNSCAN::thread_listen_SYNSCAN() connexion SOCKET 2" );
		logf_thread( (char*)"  sock = %d  sock_listen_synscan = %d  IP = %s:%d sur %s", sock_listen_synscan, sock_synscan, some_addr, (int)adresse.sin_port, sIP_listen_synscan.c_str() );

		traite_connexion_synscan();
		system( (char*)"aplay /home/rene/.astropilot/sounds/cembalo-1.wav" );
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
void SYNSCAN::to_hex( char* s, int len )
{

    if ( len > 30 )
    {
    	log_thread( (char*)"[Erreur] message trop long" );
    	return;
    }
  	char* ptr = sHex;
    
    for (int i=0; i<len; i++ )	{
    	sprintf( (char*)ptr, " %02X", (unsigned)s[i] );
    	ptr += 3;
    
    }
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
        if ( bLog )
        {
			to_hex(s, len);
	        logf_thread( (char*)"      Envoi a SYNSCAN  %s", sHex );
	    }
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

    if ( sock_synscan!= -1 )                shutdown(sock_synscan, 2);

    
    if ( sock_listen_synscan!= -1 )                  shutdown(sock_listen_synscan, 2);

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

