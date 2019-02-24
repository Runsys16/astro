#include "serveur_mgr.h"

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Serveur_mgr::Serveur_mgr()
{
    logf((char*)"----------- Constructeur Serveur_mgr() -------------" );
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
void Serveur_mgr::traite_connexion2()
{
    unsigned char buffer[255];
    int n;

    while( 1)
    {
        n = read(sock_stellarium,buffer,255);

        if (n <= 0)
        {
            logf( (char*)"ERROR reading from socket");
            break;
        }

        logf( (char*)"nb octet lu : %d", n);

        struct stellarium ss;
        decode( ss, buffer );

        float ra = com2rad(ss.ra);
        float dc = com2rad(ss.dc);

/*
        CODE PYTHON
            a = ad_rad
            d = dc_rad

            a = a - aHoraire
            if (a <0.0 ):      a = 2.0*math.pi + a
            
            #d = math.pi/2.0 - d
            
            if ( a > math.pi ):
                a =  a - math.pi
                d = math.pi - d
*/

        if (ra <0.0 )           ra = 2.0*M_PI + ra;
        if ( ra > M_PI )
        {
            ra =  ra - M_PI;
            dc = M_PI - dc;
        }

        struct hms HMS;
        rad2hms( HMS, ra );
        logf( (char*)"  AsDrte : %02d:%02d:%0.2f (%0.4f)", (int)HMS.h, (int)HMS.m, HMS.s, ra);

        
        struct dms DMS;
        rad2dms( DMS, dc );
        logf( (char*)"  Declin : %02d:%02d:%0.2f (%0.4f)", (int)DMS.d, (int)DMS.m, DMS.s, dc);
        
        char cmd[255];
        sprintf( cmd, "ia%f;id%f", RAD2DEG(ra), RAD2DEG(dc) );
        Serial::getInstance().write_string(cmd);

    }
    
    logf( (char*)"Fermeture du sock_stellarium %d", sock_stellarium );
    close(sock_stellarium);

    sock_stellarium = -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serveur_mgr::thread_listen_2()
{
	struct sockaddr_in adresse;
	socklen_t          longueur;


	if ((sock_2 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}
	
	memset(& adresse, 0, sizeof(struct sockaddr));
	adresse.sin_family = AF_INET;
	//adresse.sin_addr.s_addr = htonl(INADDR_ANY);
	adresse.sin_port = htons(10003);
	
	inet_aton("127.0.0.1", &adresse.sin_addr ); 
	
	if (bind(sock_2, (struct sockaddr *) & adresse, sizeof(adresse)) < 0) {
		perror("bind");
		exit(EXIT_FAILURE);
	}
	
	longueur = sizeof(struct sockaddr_in);
	if (getsockname(sock_2, (struct sockaddr *) & adresse, & longueur) < 0) {
		perror("getsockname");
		exit(EXIT_FAILURE);
	}
    logf( (char*)"---------------------------------------------------------------");
	logf( (char*)"Mon adresse : IP = %s, Port = %u", inet_ntoa(adresse.sin_addr), ntohs(adresse.sin_port));
    logf( (char*)"---------------------------------------------------------------");

	listen(sock_2, 5);
	while (1) {
		longueur = sizeof(struct sockaddr_in);
		sock_stellarium = accept(sock_2, (struct sockaddr *) & adresse, & longueur);

		if (sock_stellarium < 0)			continue;
		
		logf( (char*)"********** SOCKET 2*************" );
		logf( (char*)"       sock = %d  sock_2 = %d", sock_2, sock_stellarium );
		logf( (char*)"********** SUCCES 2*************" );
		//exit(EXIT_SUCCESS);
		traite_connexion2();
	}
    logf( (char*)"** Fermeture de sock_2" );
	close(sock_2);
	sock_2 = -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serveur_mgr::start_2()
{
    th_2 = std::thread(&Serveur_mgr::thread_listen_2, this);
    th_2.detach();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serveur_mgr::traite_connexion1()
{
    unsigned char buffer[255];
    int n;

    while( 1)
    {
        n = read(sock_ref,buffer,255);

        if (n <= 0)
        {
            logf( (char*)"ERROR reading from socket");
            break;
        }

        logf( (char*)"nb octet lu : %d", n);

        struct stellarium ss;
        decode( ss, buffer );

        float ra = com2rad(ss.ra);
        float dc = com2rad(ss.dc);

/*
        CODE PYTHON
            a = ad_rad
            d = dc_rad

            a = a - aHoraire
            if (a <0.0 ):      a = 2.0*math.pi + a
            
            #d = math.pi/2.0 - d
            
            if ( a > math.pi ):
                a =  a - math.pi
                d = math.pi - d
*/

        if (ra <0.0 )           ra = 2.0*M_PI + ra;
        if ( ra > M_PI )
        {
            ra =  ra - M_PI;
            dc = M_PI - dc;
        }

        struct hms HMS;
        rad2hms( HMS, ra );
        logf( (char*)"  AsDrte : %02d:%02d:%0.2f (%0.4f)", (int)HMS.h, (int)HMS.m, HMS.s, ra);

        
        struct dms DMS;
        rad2dms( DMS, dc );
        logf( (char*)"  Declin : %02d:%02d:%0.2f (%0.4f)", (int)DMS.d, (int)DMS.m, DMS.s, dc);
        
        char cmd[255];
        sprintf( cmd, "Ma;a%f;d%f", RAD2DEG(ra), RAD2DEG(dc) );
        Serial::getInstance().write_string(cmd);

    }
    
    logf( (char*)"Fermeture du sock_ref %d", sock_ref);
    close(sock_ref);

    sock_ref = -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serveur_mgr::thread_listen_1()
{
	struct sockaddr_in adresse;
	socklen_t          longueur;


	if ((sock_1 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}
	
	memset(& adresse, 0, sizeof(struct sockaddr));
	adresse.sin_family = AF_INET;
	//adresse.sin_addr.s_addr = htonl(INADDR_ANY);
	adresse.sin_port = htons(10001);
	
	inet_aton("127.0.0.1", &adresse.sin_addr ); 
	
	if (bind(sock_1, (struct sockaddr *) & adresse, sizeof(adresse)) < 0) {
		perror("bind");
		exit(EXIT_FAILURE);
	}
	
	longueur = sizeof(struct sockaddr_in);
	if (getsockname(sock_1, (struct sockaddr *) & adresse, & longueur) < 0) {
		perror("getsockname");
		exit(EXIT_FAILURE);
	}
    logf( (char*)"---------------------------------------------------------------");
	logf( (char*)"Mon adresse : IP = %s, Port = %u", inet_ntoa(adresse.sin_addr), ntohs(adresse.sin_port));
    logf( (char*)"---------------------------------------------------------------");

	listen(sock_1, 5);
	while (1) {
		longueur = sizeof(struct sockaddr_in);
		sock_ref = accept(sock_1, (struct sockaddr *) & adresse, & longueur);

		if (sock_ref < 0)			continue;
		
		logf( (char*)"********** SOCKET 1*************" );
		logf( (char*)"       sock = %d  sock_2 = %d", sock_1, sock_ref );
		logf( (char*)"********** SUCCES 1*************" );
		//exit(EXIT_SUCCESS);
		traite_connexion1();
		
		
	}
    logf( (char*)"** Fermeture de sock_1" );
	close(sock_1);
	sock_1 = -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serveur_mgr::start_1()
{
    th_1 = std::thread(&Serveur_mgr::thread_listen_1, this);
    th_1.detach();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serveur_mgr::write_stellarium(char* s)
{
    write( sock_stellarium, s, 24 );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serveur_mgr::close_all()
{
    logf( (char*)"** Fermeture de tous les sockets ..." );

    if ( sock_stellarium!= -1 )         close(sock_stellarium);
    if ( sock_ref!= -1 )                close(sock_ref);

    if ( sock_1!= -1 )                  close(sock_1);
    if ( sock_2!= -1 )                  close(sock_2);
    
    sock_1          = -1;
    sock_2          = -1;
    sock_stellarium = -1;
    sock_ref        = -1;

    sleep(1);
}





