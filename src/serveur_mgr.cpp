#include "serveur_mgr.h"
#include "camera_mgr.h"
#include "captures.h"
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Serveur_mgr::Serveur_mgr()
{
    logf((char*)"----------- Constructeur Serveur_mgr() -------------" );
    listen_1 = true;
    listen_2 = true;
    traite_1 = true;
    traite_2 = true;

    sock_stellarium = -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
float Serveur_mgr::com2rad( int ra_int )
{
    float ad_rad = ra_int;
    ad_rad = ((ad_rad-0.5) * M_PI )/  (float)(2147483648.0) ;
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

    while( traite_2 )
    {
        n = read(sock_ref,buffer,255);
        logf( (char*)"Serveur_mgr::traite_connexion_init()" );

        if (n <= 0)
        {
            logf( (char*)" ERROR reading from socket");
            break;
        }

        logf( (char*)"  nb octet lu : %d", n);

        struct stellarium ss;
        decode( ss, buffer );

        double ra = com2rad(ss.ra);
        double dc = com2rad(ss.dc);

        logf( (char*)"  reception de ra=%0.10f, dc=%0.10f", ra, dc);


        struct hms HMS;
        rad2hms( HMS, ra );
        logf( (char*)"  AD : %02dh%02dm%0.2fs (%0.8f)", (int)HMS.h, (int)HMS.m, HMS.s, RAD2DEG(ra) );
        
        struct dms DMS;
        rad2dms( DMS, dc );
        logf( (char*)"  Dc : %02dd%02d\'%0.2f\" (%0.8f)", (int)DMS.d, (int)DMS.m, DMS.s, RAD2DEG(dc) );
        
        char cmd[255];
        sprintf( cmd, "ia%f;id%f", RAD2DEG(ra), RAD2DEG(dc) );
        Serial::getInstance().write_string(cmd);
        logf( (char*)"  Envoi arduino : %s", cmd );
        
        Camera_mgr::getInstance().position(ra, dc);
        Captures::getInstance().position(ra, dc);
        Xref = 0.0;
        Yref = 0.0;
        //Zref = 0.0;

    }
    
    logf( (char*)"Fermeture du sock_ref %d", sock_ref );
    close(sock_ref);

    sock_ref = -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serveur_mgr::thread_listen_init()
{
    logf( (char*)"Serveur_mgr::thread_listen_init()");

	struct sockaddr_in adresse;
	socklen_t          longueur;


	if ((sock_2 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

    int enable = 1;
    if (setsockopt(sock_2, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        perror("setsockopt(SO_REUSEADDR) failed");
		exit(EXIT_FAILURE);
    }
	
	memset(& adresse, 0, sizeof(struct sockaddr));
	adresse.sin_family = AF_INET;
	//adresse.sin_addr.s_addr = htonl(INADDR_ANY);
	adresse.sin_port = htons(10002);
	
	inet_aton("127.0.0.1", &adresse.sin_addr ); 
	
	if (bind(sock_2, (struct sockaddr *) & adresse, sizeof(adresse)) < 0) {
		logf( (char*) "[ERREUR] bind (sock_2)");
		sock_2 = -1;
		//exit(EXIT_FAILURE);
		return;
	}
	
	longueur = sizeof(struct sockaddr_in);
	if (getsockname(sock_2, (struct sockaddr *) & adresse, & longueur) < 0) {
		logf( (char*) "[ERREUR] getsockname (sock_2)");
		return;
	}


    int option = 1;
    if(setsockopt(sock_2,SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),(char*)&option,sizeof(option)) < 0)
    {
		logf( (char*) "[ERREUR] setsockopt (sock_2)");
        close(sock_2);
		return;

    }


    //logf( (char*)"---------------------------------------------------------------");
	logf( (char*)"  Ecoute de IP = %s, Port = %u", inet_ntoa(adresse.sin_addr), ntohs(adresse.sin_port));
    //logf( (char*)"---------------------------------------------------------------");

	listen(sock_2, 5);
	while (listen_2) {
		longueur = sizeof(struct sockaddr_in);
		sock_ref = accept(sock_2, (struct sockaddr *) & adresse, & longueur);

		if (sock_ref < 0)			continue;
		
		char *some_addr;
        some_addr = inet_ntoa( adresse.sin_addr); // return the IP

		logf( (char*)"Serveur_mgr::thread_listen_init() connexion SOCKET 2" );
		logf( (char*)"  sock = %d  sock_1 = %d  IP = %s:%d", sock_1, sock_stellarium, some_addr, (int)adresse.sin_port );

		traite_connexion_init();
	}
    logf( (char*)"** Fermeture de sock_2" );
	close(sock_2);
	sock_2 = -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serveur_mgr::start_init()
{
    th_2 = std::thread(&Serveur_mgr::thread_listen_init, this);
    th_2.detach();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serveur_mgr::traite_connexion_deplacement()
{
    unsigned char buffer[255];
    int n;

    while( traite_1 )
    {
        n = read(sock_stellarium,buffer,255);

        if (n <= 0)
        {
            logf( (char*)"ERROR reading from socket");
            break;  
        }

        logf( (char*)"Serveur_mgr::traite_connexion_deplacement()" );
        logf( (char*)"nb octet lu : %d", n);

        struct stellarium ss;
        decode( ss, buffer );

        float ra = com2rad(ss.ra);
        float dc = com2rad(ss.dc);


        struct hms HMS;
        rad2hms( HMS, ra );
        logf( (char*)"  AD : %02dh%02dm%0.2fs (%0.8f)", (int)HMS.h, (int)HMS.m, HMS.s, RAD2DEG(ra) );
        
        struct dms DMS;
        rad2dms( DMS, dc );
        logf( (char*)"  DC : %02dd%02d\'%0.2f\" (%0.8f)", (int)DMS.d, (int)DMS.m, DMS.s, RAD2DEG(dc) );
        
        char cmd[255];
        sprintf( cmd, "A%f;D%f", RAD2DEG(ra), RAD2DEG(dc) );
        logf( (char*)"  Envoi arduino : %s", cmd );
        Serial::getInstance().write_string(cmd);

    }
    
    logf( (char*)"Fermeture du sock_stellarium %d", sock_stellarium);
    close(sock_stellarium);

    sock_stellarium = -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serveur_mgr::thread_listen_deplacement()
{
    logf( (char*)"Serveur_mgr::thread_listen_deplacement()");
    
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
	adresse.sin_port = htons(10001);
	
	inet_aton("127.0.0.1", &adresse.sin_addr ); 
	
	if (bind(sock_1, (struct sockaddr *) & adresse, sizeof(adresse)) < 0) {
		logf( (char*) "[ERREUR] bind (sock_1)");
		sock_1 = -1;
		return;
	}
	
	longueur = sizeof(struct sockaddr_in);
	if (getsockname(sock_1, (struct sockaddr *) & adresse, & longueur) < 0) {
		logf( (char*) "[ERREUR] getsockname (sock_1)");
		return;
	}

    int option = 1;
    if(setsockopt(sock_1,SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),(char*)&option,sizeof(option)) < 0)
    {
		logf( (char*) "[ERREUR] setsockopt (sock_1)");
        close(sock_1);
		return;
    }


    //logf( (char*)"---------------------------------------------------------------");
	logf( (char*)" Ecoute de IP = %s, Port = %u", inet_ntoa(adresse.sin_addr), ntohs(adresse.sin_port));
    //logf( (char*)"---------------------------------------------------------------");

	listen(sock_1, 5);
	while ( listen_1 ) {
		longueur = sizeof(struct sockaddr_in);
		sock_stellarium = accept(sock_1, (struct sockaddr *) & adresse, & longueur);

		if (sock_stellarium < 0)			continue;
		
		char *some_addr;
        some_addr = inet_ntoa( adresse.sin_addr); // return the IP

		logf( (char*)"Serveur_mgr::thread_listen_deplacement() connexion SOCKET 1" );
		logf( (char*)"  sock = %d  sock_1 = %d  IP = %s:%d", sock_1, sock_stellarium, some_addr, (int)adresse.sin_port );

		traite_connexion_deplacement();
		
		
	}
    logf( (char*)"** Fermeture de sock_1" );
	close(sock_1);
	sock_1 = -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serveur_mgr::start_deplacement()
{
    th_1 = std::thread(&Serveur_mgr::thread_listen_deplacement, this);
    th_1.detach();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serveur_mgr::write_stellarium(char* s)
{
    if ( sock_stellarium != -1 )
    {
        write( sock_stellarium, s, 24 );
        //logf( (char*)"Envoid a stellarium  %s", s );
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serveur_mgr::close_all()
{
    logf( (char*)"** Fermeture de tous les sockets ..." );

    listen_1 = false;
    listen_2 = false;
    traite_1 = false;
    traite_2 = false;

	//close(sock_1);
	//close(sock_2);
    sleep(1);

    if ( sock_stellarium!= -1 )         shutdown(sock_stellarium, 2);
    if ( sock_ref!= -1 )                shutdown(sock_ref, 2);

    
    if ( sock_1!= -1 )                  shutdown(sock_1, 2);
    if ( sock_2!= -1 )                  shutdown(sock_2, 2);
    //sleep(1);
    
    sock_1          = -1;
    sock_2          = -1;
    sock_stellarium = -1;
    sock_ref        = -1;

    sleep(1);
}





