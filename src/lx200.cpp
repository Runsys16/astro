#include "lx200.h"
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
LX200::LX200()
{
    logf((char*)"----------- Constructeur LX200() -------------" );
    bListen = true;
    bLx200 = true;

    sock_lx200		= -1;
    uPort			= 10003;

#ifdef VAR_GLOBAL
	VarManager& var = VarManager::getInstance();

    if ( !var.existe("IP_LX200") )		printf( "IPLX200" );
    if ( !var.existe("IP_LX200") )		var.set("IP_LX200", (char*)"127.0.0.1" );
    sIP_listen_lx200 = *var.gets("IP_LX200" );

#endif

#ifdef PANEL_LX200_DEBUG
	panel_debug.setPosAndSize( 5, 5, 130, 20);
	panel_debug.setBackground((char*)"images/background.tga");
	panel_debug.setColor( 0xffFFffFF );
	WindowsManager::getInstance().add( &panel_debug );
	WindowsManager::getInstance().onTop( &panel_debug );
#endif
}
//--------------------------------------------------------------------------------------------------------------------
//
//				G – Get Telescope Information
//
//--------------------------------------------------------------------------------------------------------------------
void LX200::traite_command_G( char* buffer, int n)
{
	///------------------------------------------------------
	// Get Telescope NAME
	///------------------------------------------------------
	if ( strcmp( (char*)buffer, "#:GVP#" ) == 0 )
	{
		logf_thread( (char*)"%s Get Telescope NAME", buffer );
		write_lx200( sock_lx200, (char*)"EQ5_DRIVE#", 10, true );
	}
	else
	///------------------------------------------------------
	// Get Firmware Version
	///------------------------------------------------------
	if ( strcmp( (char*)buffer, "#:GVN#" ) == 0 )
	{
		logf_thread( (char*)"%s Get Firmware Version", buffer );
		write_lx200( sock_lx200, (char*)"00.7#", 5, true );	
	}
	else
	///------------------------------------------------------
	// Get Firmware Date
	///------------------------------------------------------
	if ( strcmp( (char*)buffer, "#:GVD#" ) == 0 )
	{
		logf_thread( (char*)"%s Get Firmware Date", buffer );
		write_lx200( sock_lx200, (char*)"NOV 17 2025#", 12, true );	
	}
	else
	///------------------------------------------------------
	// Get Firmware Heure
	///------------------------------------------------------
	if ( strcmp( (char*)buffer, "#:GVT#" ) == 0 )
	{
		logf_thread( (char*)"%s Get Firmware Heure", buffer );
		write_lx200( sock_lx200, (char*)"15:03:25#", 9, true );
	}
	else
	///------------------------------------------------------
	// Get Longiture
	///------------------------------------------------------
	if ( strcmp( (char*)buffer, "#:Gg#" ) == 0 )
	{
		logf_thread( (char*)"%s Get Longiture", buffer );
		write_lx200( sock_lx200, (char*)"s000*00#", 8, true );
	}
	else
	///------------------------------------------------------
	// Get Lattitude
	///------------------------------------------------------
	if ( strcmp( (char*)buffer, "#:Gt#" ) == 0 )
	{
		logf_thread( (char*)"%s Get Lattitude", buffer );
		write_lx200( sock_lx200, (char*)"s45*53#", 7, true );
	}
	else
	///------------------------------------------------------
	// Get Local Date
	///------------------------------------------------------
	if ( strcmp( (char*)buffer, "#:GC#" ) == 0 )
	{
		char 			sTime[30];
		struct timeval	tv;
		struct tm*		tm;

		gettimeofday( &tv, NULL );
		tm=localtime(&tv.tv_sec);
		
		snprintf( (char*)sTime, sizeof(sTime),"%02d/%02d/%02d#", tm->tm_mon+1, tm->tm_mday, tm->tm_year-100 );

		logf_thread( (char*)"%s Get Local Date  : %s", buffer, sTime );
		write_lx200( sock_lx200, (char*)sTime, strlen(sTime), true );
	}
	else
	///------------------------------------------------------
	// Get Local Time
	///------------------------------------------------------
	if ( strcmp( (char*)buffer, "#:GL#" ) == 0 )
	{
		char 			sTime[20];
		struct timeval	tv;
		struct tm*		tm;

		gettimeofday( &tv, NULL );
		tm=localtime(&tv.tv_sec);
		
		snprintf( (char*)sTime, sizeof(sTime),"%02d:%02d:%02d#", tm->tm_hour, tm->tm_min, tm->tm_sec );

		logf_thread( (char*)"%s Get Local Time   %s", buffer, sTime );
		write_lx200( sock_lx200, sTime, strlen(sTime), true );	
	}
	else
	///------------------------------------------------------
	// Get Telescope DEC
	///------------------------------------------------------
	if ( strcmp( (char*)buffer, "#:GD#" ) == 0 )
	{
		//logf_thread( (char*)"%s Get Telescope DEC", buffer );
		struct dms	DMS;
		char		s[80];
		char		sign[2];
		double  	d = d_deg_dc;
		
		deg2dms( d_deg_dc, DMS );
		if ( DMS.d >= 0 )		sign[0] = '+';
		else					sign[0] = '-';
		sign[1] = 0;
		
		DMS.d = abs(DMS.d);
		snprintf( s, sizeof(s), "%s%02d*%02d\'%02d#", sign, (int)DMS.d, (int)DMS.m, (int)DMS.s );

		#ifdef PANEL_LX200_DEBUG
			//panel_debug.add_textf( (char*)"GD :\t%s %lf", s,  d_deg_dc );
			panel_debug.add_textf( (char*)"GD :\t%s", s );
		#endif

		write_lx200( sock_lx200, s, strlen(s), false );	
	}
	else
	///------------------------------------------------------
	// Get Telescope RA
	///------------------------------------------------------
	if ( strcmp( (char*)buffer, "#:GR#" ) == 0 )
	{
		//logf_thread( (char*)"%s Get Telescope RA", buffer );

		struct hms	HMS;
		char		s[80];
		
		deg2hms( d_deg_ad, HMS );
		snprintf( s, sizeof(s), "%02d:%02d:%02d#", (int)HMS.h, (int)HMS.m, (int)HMS.s );

		#ifdef PANEL_LX200_DEBUG
			//panel_debug.add_textf( (char*)"GR :\t%s %lf", s, d_deg_ad );
			panel_debug.add_textf( (char*)"GR :\t%s", s );
		#endif

		write_lx200( sock_lx200, (char*)s, strlen(s), false );	
	}
	else
	///------------------------------------------------------
	// Get Scope Alignment Status
	///------------------------------------------------------
	if ( strcmp( (char*)buffer, "#:GW#" ) == 0 )
	{
		//logf_thread( (char*)"%s Get Scope Alignment Status", buffer );
		write_lx200( sock_lx200, (char*)"GN3#", 4, false );	
	}
	else
	///------------------------------------------------------
	// Get UTC offset time
	///------------------------------------------------------
	if ( strcmp( (char*)buffer, "#:GG#" ) == 0 )
	{
		logf_thread( (char*)"%s Get UTC offset time", buffer );
		write_lx200( sock_lx200, (char*)"-01#", 4, true );
	}
	///------------------------------------------------------
	// Commande inconnue
	///------------------------------------------------------
	else
	{
		logf_thread( (char*)"--[WARNING] \"%s\" Commande G inconnue", buffer );
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//				M – Telescope Movement Commands
//
//--------------------------------------------------------------------------------------------------------------------
void LX200::traite_command_M( char* buffer, int n)
{
	Serial& arduino = Serial::getInstance();

	///------------------------------------------------------
	// Move Telescope West at current slew rate
	///------------------------------------------------------
	if ( strncmp( (char*)buffer, "#:Mw", 4 ) == 0 )
	{
		logf_thread( (char*)"%s Move Telescope West at current slew rate", buffer );
		char cmd[] = "j1;x100";
		logf_thread( (char*)"  |Envoi arduino : %s", cmd );
		if ( arduino.isConnect() )		arduino.write_string(cmd);
	}
	else
	///------------------------------------------------------
	// Move Telescope Est at current slew rate
	///------------------------------------------------------
	if ( strncmp( (char*)buffer, "#:Me", 4 ) == 0 )
	{
		logf_thread( (char*)"%s Move Telescope Est at current slew rate", buffer );
		char cmd[] = "j1;x-100";
		logf_thread( (char*)"  |Envoi arduino : %s", cmd );
		if ( arduino.isConnect() )		arduino.write_string(cmd);
	}
	else
	///------------------------------------------------------
	// Move Telescope North at current slew rate
	///------------------------------------------------------
	if ( strncmp( (char*)buffer, "#:Mn", 4 ) == 0 )
	{
		logf_thread( (char*)"%s Move Telescope North at current slew rate", buffer );
		char cmd[] = "j1;y100";
		logf_thread( (char*)"  |Envoi arduino : %s", cmd );
		if ( arduino.isConnect() )		arduino.write_string(cmd);
	}
	else
	///------------------------------------------------------
	// Move Telescope South at current slew rate
	///------------------------------------------------------
	if ( strncmp( (char*)buffer, "#:Ms", 4 ) == 0 )
	{
		logf_thread( (char*)"%s Move Telescope South at current slew rate", buffer );
		char cmd[] = "j1;y-100";
		logf_thread( (char*)"  |Envoi arduino : %s", cmd );
		if ( arduino.isConnect() )		arduino.write_string(cmd);
	}
	else
	///------------------------------------------------------
	// Slew to Target Object
	///------------------------------------------------------
	if ( strcmp( (char*)buffer, "#:MS#" ) == 0 )
	{
		logf_thread( (char*)"%s Slew to Target Object", buffer );
		write_lx200( sock_lx200, (char*)"0", 1, true );	
		
		Serveur_mgr::getInstance()._goto_rad( dRA, dDC );

		if ( !Serial::getInstance().isConnect() )
		{
			change_ad_status( RAD2DEG(dRA) );
			change_dc_status( RAD2DEG(dDC) );
		}
		/*
		*/
	}
	///------------------------------------------------------
	// Commande inconnue
	///------------------------------------------------------
	else
	{
		logf_thread( (char*)"--[WARNING] \"%s\" Commande M inconnue", buffer );
	}
		
}
//--------------------------------------------------------------------------------------------------------------------
//
//				Q – Movement Commands
//
//--------------------------------------------------------------------------------------------------------------------
void LX200::traite_command_Q( char* buffer, int n)
{
	Serial& arduino = Serial::getInstance();

	///------------------------------------------------------
	// Halt southward Slews
	///------------------------------------------------------
	if ( strncmp( (char*)buffer, "#:Qw", 4 ) == 0 )
	{
		logf_thread( (char*)"%s Halt westward Slews", buffer );
		char cmd[] = "x0;y=0";
		logf_thread( (char*)"  |Envoi arduino : %s", cmd );
		if ( arduino.isConnect() )		arduino.write_string(cmd);
	}
	else
	///------------------------------------------------------
	// Halt estward Slews
	///------------------------------------------------------
	if ( strncmp( (char*)buffer, "#:Qe", 4 ) == 0 )
	{
		logf_thread( (char*)"%s Halt estward Slews", buffer );
		char cmd[] = "x0;y=0";
		logf_thread( (char*)"  |Envoi arduino : %s", cmd );
		if ( arduino.isConnect() )		arduino.write_string(cmd);
	}
	else
	///------------------------------------------------------
	// Halt northward Slews
	///------------------------------------------------------
	if ( strncmp( (char*)buffer, "#:Qn", 4 ) == 0 )
	{
		logf_thread( (char*)"%s Halt northward Slews", buffer );
		char cmd[] = "x0;y=0";
		logf_thread( (char*)"  |Envoi arduino : %s", cmd );
		if ( arduino.isConnect() )		arduino.write_string(cmd);
	}
	else
	///------------------------------------------------------
	// Halt southward Slews
	///------------------------------------------------------
	if ( strncmp( (char*)buffer, "#:Qs", 4 ) == 0 )
	{
		logf_thread( (char*)"%s Halt southward Slews", buffer );
		char cmd[] = "x0;y=0";
		logf_thread( (char*)"  |Envoi arduino : %s", cmd );
		if ( arduino.isConnect() )		arduino.write_string(cmd);
	}
	else
	///------------------------------------------------------
	// Halt all current slewing
	///------------------------------------------------------
	if ( strcmp( (char*)buffer, "#:Q#" ) == 0 )
	{
		logf_thread( (char*)"%s Abort", buffer );
		/*
		Serial& arduino = Serial::getInstance();
		char cmd[] = "n";
		logf_thread( (char*)"  |Envoi arduino : %s", cmd );
		arduino.write_string(cmd);
		*/
	}
	///------------------------------------------------------
	// Command inconnue
	///------------------------------------------------------
	else
	{
		logf_thread( (char*)"--[WARNING] \"%s\" Commande Q inconnue", buffer );
	}
		
}
//--------------------------------------------------------------------------------------------------------------------
//
//				R – Slew Rate Commands
//
//--------------------------------------------------------------------------------------------------------------------
void LX200::traite_command_R( char* buffer, int n)
{
	Serial& arduino = Serial::getInstance();

	///------------------------------------------------------
	// Set Slew rate to Centering rate more Slowest
	///------------------------------------------------------
	if ( strncmp( (char*)buffer, "#:RG", 4 ) == 0 )
	{
		logf_thread( (char*)"%s Set Slew rate to Centering rate more Slowest", buffer );
		char cmd[] = "z4";
		logf_thread( (char*)"  |Envoi arduino : %s", cmd );
		if ( arduino.isConnect() )		arduino.write_string(cmd);
	}
	else
	///------------------------------------------------------
	// Set Slew rate to Centering rate Slowest
	///------------------------------------------------------
	if ( strncmp( (char*)buffer, "#:RC", 4 ) == 0 )
	{
		logf_thread( (char*)"%s Set Slew rate to Centering rate Slowest", buffer );
		char cmd[] = "z3";
		logf_thread( (char*)"  |Envoi arduino : %s", cmd );
		if ( arduino.isConnect() )		arduino.write_string(cmd);
	}
	else
	///------------------------------------------------------
	// Set Slew rate to Centering rate Fastest
	///------------------------------------------------------
	if ( strncmp( (char*)buffer, "#:RM", 4 ) == 0 )
	{
		logf_thread( (char*)"%s Set Slew rate to Centering rate Fastest", buffer );
		char cmd[] = "z2";
		logf_thread( (char*)"  |Envoi arduino : %s", cmd );
		if ( arduino.isConnect() )		arduino.write_string(cmd);
	}
	else
	///------------------------------------------------------
	// Set Slew rate to Centering rate more Fastest
	///------------------------------------------------------
	if ( strncmp( (char*)buffer, "#:RS", 4 ) == 0 )
	{
		logf_thread( (char*)"%s Set Slew rate to Centering rate more Fastest", buffer );
		char cmd[] = "z1";
		logf_thread( (char*)"  |Envoi arduino : %s", cmd );
		if ( arduino.isConnect() )		arduino.write_string(cmd);
	}
	///------------------------------------------------------
	// Command inconnue
	///------------------------------------------------------
	else
	{
		logf_thread( (char*)"--[WARNING] \"%s\" Commande Q inconnue", buffer );
	}
		
}
//--------------------------------------------------------------------------------------------------------------------
//
//			S – Telescope Set Commands
//
//--------------------------------------------------------------------------------------------------------------------
void LX200::traite_command_S( char* buffer, int n)
{
	///------------------------------------------------------
	// Set Offset UTC
	///------------------------------------------------------
	if ( strncmp( (char*)buffer, "#:SG", 4 ) == 0 )
	{
		logf_thread( (char*)"%s Set Offset UTC", buffer );
		write_lx200( sock_lx200, (char*)"1", 1, true );	
	}
	else
	///------------------------------------------------------
	// Set Lattitude
	///------------------------------------------------------
	if ( strncmp( (char*)buffer, "#:St", 4 ) == 0 )
	{
		logf_thread( (char*)"%s Set Lattitude", buffer );
		write_lx200( sock_lx200, (char*)"1", 1, true );	
	}
	else
	///------------------------------------------------------
	// Set Longitude
	///------------------------------------------------------
	if ( strncmp( (char*)buffer, "#:Sg", 4 ) == 0 )
	{
		logf_thread( (char*)"%s Set Longitude", buffer );
		write_lx200( sock_lx200, (char*)"1", 1, true );	
	}
	else
	///------------------------------------------------------
	// Set local Time
	///------------------------------------------------------
	if ( strncmp( (char*)buffer, "#:SL", 4 ) == 0 )
	{
		logf_thread( (char*)"%s Set Local time", buffer );
		write_lx200( sock_lx200, (char*)"1", 1, true );	
	}
	else
	///------------------------------------------------------
	// Set local date
	///------------------------------------------------------
	if ( strncmp( (char*)buffer, "#:SC", 4 ) == 0 )
	{
		logf_thread( (char*)"%s Set Local date", buffer );
		write_lx200( sock_lx200, (char*)"1", 1, true );	
	}
	else
	///------------------------------------------------------
	// Set target object DEC
	///------------------------------------------------------
	if ( strncmp( (char*)buffer, "#:Sd", 4 ) == 0 )
	{
		logf_thread( (char*)"%s Set target object DEC", buffer );
		write_lx200( sock_lx200, (char*)"1", 1, true );	
		
		struct dms	 DMS;
		int sign;

		if ( buffer[4] == '+' )		sign = 1;
		else						sign = -1;
		DMS.d = sign * ((buffer[5]-'0') * 10 + (buffer[6]-'0'));
		DMS.m = (buffer[8]-'0') * 10 + (buffer[9]-'0');
		DMS.s = (buffer[11]-'0') * 10 + (buffer[12]-'0');
		
		dDC = dms2rad(DMS);
	}
	else
	///------------------------------------------------------
	// Set target object RA
	///------------------------------------------------------
	if ( strncmp( (char*)buffer, "#:Sr", 4 ) == 0 )
	{
		logf_thread( (char*)"%s Set target object RA", buffer );
		write_lx200( sock_lx200, (char*)"1", 1, true );	
		
		struct hms	 HMS;
		HMS.h = (buffer[4]-'0') * 10 + (buffer[5]-'0');
		HMS.m = (buffer[7]-'0') * 10 + (buffer[8]-'0');
		HMS.s = (buffer[10]-'0') * 10 + (buffer[11]-'0');
		
		dRA = hms2rad(HMS);
		if ( dRA > M_PI )		dRA = -2.0*M_PI + dRA;
	}
	///------------------------------------------------------
	// Commande inconnue
	///------------------------------------------------------
	else
	{
		logf_thread( (char*)"--[WARNING] \"%s\" Commande S inconnue", buffer );
	}
		
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void LX200::traite_connexion_lx200()
{
    unsigned char buffer[255];
    int n;

    while( bLx200 )
    {
		#ifdef PANEL_LX200_DEBUG
			panel_debug.setVisible(bPanelStdOut);
		#endif
    
        n = read(sock_lx200, buffer, 255);
        //logf_thread( (char*)"LX200::traite_connexion_init()" );
        log_tab(true);

        if (n <= 0)
        {
            logf_thread( (char*)"[Error] reading from socket (lx200)");
	        log_tab(false);
            break;
        }

		buffer[n] = 0;
        //logf_thread( (char*)"lx200 :  \"%s\"", (char*)buffer );

		if ( buffer[0] == '#' )			
		{ 
			if ( buffer[0] == '#' && buffer[1] == '#' )
			{
				logf_thread( (char*)"%s START", buffer );
				write_lx200( sock_lx200, (char*)"##", 2 );	
			}
			else
			if ( buffer[0] == '#' && buffer[1] == 0x6 )
			{
				logf_thread( (char*)"%s ACK", buffer );
				write_lx200( sock_lx200, (char*)"P", 1 );		// Polar mode
			}
			else
			//------------------------------------------------------
			if ( strncmp( (char*)buffer, "#:G", 3 ) == 0 )
			{
				traite_command_G( (char*)buffer, n );
			}
			else
			if ( strncmp( (char*)buffer, "#:M", 3 ) == 0 )
			{
				traite_command_M( (char*)buffer, n );
			}
			else
			if ( strncmp( (char*)buffer, "#:Q", 3 ) == 0 )
			{
				traite_command_Q( (char*)buffer, n );
			}
			else
			if ( strncmp( (char*)buffer, "#:R", 3 ) == 0 )
			{
				traite_command_R( (char*)buffer, n );
			}
			else
			if ( strncmp( (char*)buffer, "#:S", 3 ) == 0 )
			{
				traite_command_S( (char*)buffer, n );
			}
			//------------------------------------------------------
			else
			if ( strcmp( (char*)buffer, "#:D#" ) == 0 )
			{
				//logf_thread( (char*)"Distance Bars" );
				write_lx200( sock_lx200, (char*)"s#", 2, false );
				#ifdef PANEL_LX200_DEBUG
					panel_debug.reset_list();
				#endif
			}
			else
			if ( strcmp( (char*)buffer, "#:CM#" ) == 0 )
			{
				logf_thread( (char*)"%s Sync", buffer );
				write_lx200( sock_lx200, (char*)"#", 1, false );

				Serveur_mgr::getInstance()._sync_rad( dRA, dDC );
				if ( Serial::getInstance().isConnect() )
				{
					change_ad_status( RAD2DEG(dRA) );
					change_dc_status( RAD2DEG(dDC) );
				}
				/*
				*/
			}
			else
			{
				logf_thread( (char*)"--[WARNING] \"%s\" Commande inconnue", buffer );
			}
			
			log_tab(false);
		}


    }
    
    logf_thread( (char*)"Deconnexion du sock_lx200 %d", sock_lx200 );
    close(sock_lx200);

#ifdef PANEL_LX200_DEBUG
	panel_debug.setVisible(false);
#endif
	bLx200 = false;
    sock_lx200 = -1;
	system( (char*)"aplay /home/rene/.astropilot/sounds/cembalo-1.wav" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void LX200::thread_listen_lx200()
{
    //logf_thread( (char*)"LX200::thread_listen_lx200()");

	struct sockaddr_in adresse;
	socklen_t          longueur;


	if ((sock_listen_lx200 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

    int enable = 1;
    if (setsockopt(sock_listen_lx200, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
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
	inet_aton( sIP_listen_lx200.c_str(), &adresse.sin_addr ); 
#else
#ifdef LOCALHOST
	inet_aton("127.0.0.1", &adresse.sin_addr ); 
#endif
#endif


	if (bind(sock_listen_lx200, (struct sockaddr *) & adresse, sizeof(adresse)) < 0) {
		logf_thread( (char*) "[Error] bind (sock_listen_lx200)");
		sock_listen_lx200 = -1;
		//exit(EXIT_FAILURE);
		return;
	}
	
	longueur = sizeof(struct sockaddr_in);
	if (getsockname(sock_listen_lx200, (struct sockaddr *) & adresse, & longueur) < 0) {
		logf_thread( (char*) "[Error] getsockname (sock_listen_lx200)");
		return;
	}


    int option = 1;
    if(setsockopt(sock_listen_lx200,SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),(char*)&option,sizeof(option)) < 0)
    {
		logf_thread( (char*) "[Error] setsockopt (sock_listen_lx200)");
        close(sock_listen_lx200);
		return;

    }


    logf_thread( (char*)"---------------------------------------------------------------");
	logf_thread( (char*)"Ecoute sur IP = %s, Port = %u (Lx200)", inet_ntoa(adresse.sin_addr), ntohs(adresse.sin_port));
    logf_thread( (char*)"---------------------------------------------------------------");

	listen(sock_listen_lx200, 5);
	while (bListen) {
		longueur = sizeof(struct sockaddr_in);
		int sock = accept(sock_listen_lx200, (struct sockaddr *) & adresse, & longueur);

		if (sock < 0)			continue;
		if ( sock_lx200 != -1 )
		{
			close( sock );
			char *some_addr;
		    some_addr = inet_ntoa( adresse.sin_addr); // return the IP
			logf_thread( (char*)"[Warning] lx200 tentative de connection %s", some_addr );
			continue;
		}
		
		sock_lx200 = sock;
		char *some_addr;
        some_addr = inet_ntoa( adresse.sin_addr); // return the IP
		sIP_lx200 = string(some_addr);


		//sIP_init = string( some_addr );
	
		logf_thread( (char*)"LX200::thread_listen_lx200() connexion SOCKET 2" );
		logf_thread( (char*)"  sock = %d  sock_listen_lx200 = %d  IP = %s:%d sur %s", sock_listen_lx200, sock_lx200, some_addr, (int)adresse.sin_port, sIP_listen_lx200.c_str() );

		//panel_win.setVisible(bPanelStdOut);
		//PANEL_LX200_DEBUG.setVisible(bPanelStdOut);
		bLx200 = true;
		system( (char*)"aplay /home/rene/.astropilot/sounds/cembalo-1.wav" );
		traite_connexion_lx200();
	}
    logf_thread( (char*)"Fermeture de sock_listen_lx200 (%s:%u)", inet_ntoa(adresse.sin_addr), ntohs(adresse.sin_port) );
	close(sock_listen_lx200);
	sock_listen_lx200 = -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void LX200::start_lx200()
{
    th_lx200_listen = std::thread(&LX200::thread_listen_lx200, this);
    th_lx200_listen.detach();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void LX200::write_lx200(char* s)
{
#define LOG
    if ( sock_lx200 != -1 )
    {
        write( sock_lx200, s, strlen(s) );

        #ifdef LOG
        char t[25*4+80] = "";
        
      	strcpy( t, s );
      	char* ptr = t + strlen(s);
        
        for (int i=0; i<strlen(s); i++ )	{
        	sprintf( (char*)ptr, " %02X", (unsigned char)s[i] );
        	ptr += 3;
        
        }

        logf_thread( (char*)"Envoi a lx200  %s", t );
        #endif
    }
#undef LOG
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void LX200::write_lx200(int socket, char* s, int len)
{
	write_lx200( socket, s, len, true );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void LX200::write_lx200(int socket, char* s, int len, bool bLog)
{
#define __LOG
    if ( socket != -1 )
    {
        write( socket, s, len );

        #ifdef __LOG
        char t[255]		= "";
        
      	strcpy( t, s );
      	char* ptr = t + strlen(s);
        
        for (int i=0; i<strlen(s); i++ )	{
        	sprintf( (char*)ptr, " %02X", (unsigned char)s[i] );
        	ptr += 3;
        }
        
        if ( bLog )
	        logf_thread( (char*)"  | Envoi a lx200  \"%s\" - %s", s, t );
        #endif
    }
#undef __LOG
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void LX200::close_all()
{
    logf_thread( (char*)"Fermeture de tous les sockets ..." );
    log_tab(true);

    bListen = false;
    bLx200 = false;

	//close(sock_listen_lx200);
	//close(sock_listen_lx200);
    //sleep(1);

    if ( sock_lx200!= -1 )                shutdown(sock_lx200, 2);

    
    if ( sock_listen_lx200!= -1 )         shutdown(sock_listen_lx200, 2);
    //sleep(1);
    
    /*
    sock_listen_lx200          = -1;
    sock_listen_lx200          = -1;
    sock_stellarium = -1;
    sock_lx200        = -1;
    */

    //sleep(1);
    /*
    logf_thread( (char*)"sock_listen_lx200=%d sock_listen_lx200=%d", sock_listen_lx200, sock_listen_lx200 );
    while( sock_listen_lx200 != -1)	;
    while( sock_listen_lx200!= -1)	;
    while( sock_stellarium != -1)	;
    while( sock_lx200!= -1)	;
    */
    sock_lx200        = -1;

    logf_thread( (char*)"sock_listen_lx200=%d sock_listen_lx200=%d", sock_listen_lx200, sock_listen_lx200 );
    log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void LX200::print_list()
{
    //logf( (char*)"---- LX200::print_list()" );

	if ( sock_listen_lx200 == -1 )		logf( (char*)"  LX200\timpossible d'ouvrir : %s", sIP_listen_lx200.c_str() );
	else if ( sock_lx200 != -1 )		logf( (char*)"  LX200\tconnexion de \t: %s", sIP_lx200.c_str() );
	else								logf( (char*)"  LX200\tlisten sur \t\t: %s : %d", sIP_listen_lx200.c_str(), uPort );

}    
#ifdef PANEL_LX200_DEBUG
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void LX200::setColor( uint32_t  c)
{
	panel_debug.setColor( c );
}
#endif

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

