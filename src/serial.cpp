#include "serial.h"    /* Standard input/output definitions */
#include "serveur_mgr.h"    /* Standard input/output definitions */
#include "panel_console_serial.h"    
#include "var_mgr.h"
#include <unistd.h>

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Serial::Serial()
{
    logf_thread((char*)"Constructeur Serial::Serial() -------------" );
    fTimeOut = 0.0;
    fTimer10s = 11.0;
    fd = -1;

    idx = 0;
    nbZero = 0;
    
    bFree = true;
    bConnect = false;
    bOldJoy = false;// getJoy();
    
    VarManager& var = VarManager::getInstance();
    
    sVersionArduinoValable = "PAS_VERSION";
    
    if ( var.existe("VER_ARDUINO") )			sVersionArduinoValable = *var.gets("VER_ARDUINO");
    else
	    logf_thread( (char*)"[ Erreur ] VER_ARDUINO inconnu" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serial::init( string dev)
{
    logf_thread((char*)"Serial::init(%s) -------------", dev.c_str() );
    fd = -1;
    dev_name = dev;
    idx = 0;
    nbZero = 0;

    sopen();
    start_thread();
    
    bPrintInfo = true;
    sVersionArduino = "";
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serial::idle( double elapsedTime )
{
    fTimer10s += elapsedTime;
    if ( fTimer10s >= 10.0 )
    {
        fTimer10s -= 10.0;
        if ( fd != -1 )			write_g();
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int Serial::write_byte( char b)
{
    if ( fd ==-1 )      return -1;

    int n = write(fd,&b,1);

    if( n!=1)           return -1;
    return 0;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int Serial::write_string( const char* str, bool bAff)
{
    //return -1;
    
    if ( fd ==-1 )
    {
    	log_thread( (char*)"[Error] Serial::write_string() Fichier -1" );
    	return -1;
    }

	
    if ( (fTimeMili - fTimeOut) < 0.2 )     
    {
    	log_thread( (char*)"[Error] Serial::write_string() TimeOut" );
    	return -1;
    }
    
    

    if ( str[0] == 'a' || str[0] == 'd' )
    {
        if ( bFree )
        {
            bFree = false;
        }
        else
        {
            string s = string(str);
            tCommandes.push_back(s);
            logf_thread( (char*)"Cmd en attent: %s", (char*)s.c_str() );
            return 0;
        }
    }
    else    
    if ( str[0] == 'n' )                bFree = true;


    
    if ( bAff )
    {
        logf_thread( (char*)"Arduino : %s", str );
        //PanelConsoleSerial::getInstance().writeln( (char*)"console" );
        PanelConsoleSerial::getInstance().getConsole()->affiche( (char*)"" );
        PanelConsoleSerial::getInstance().getConsole()->affiche( (char*)str );
    }
        
    fTimeOut = fTimeMili;
    

    int len = strlen(str);

    int n = write(fd, str, len);
    if( n!=len ) 
    {
        logf_thread( (char*)"Envoi %d/%d", n, len );
        return -1;
    }

    n = write_byte('\n');
    if( n!=0 ) 
    {
        logf_thread( (char*)"envoid de %d/1", n );
        return -1;
    }
    return 0;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int Serial::write_string( const char* str)
{
    return write_string( str, false );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serial::write_g_thread()
{
	if ( fd == -1 )				return;
	
    char cmd[255];
    sprintf( cmd, "g" );

	int ret =  write_string(cmd, false);
	
	sleep( 1 );
	if ( bConnect == false )		nbConnect++;
	else	{
		change_arduino( true );
		nbConnect = 0;
	}
	if ( nbConnect >= 2 )			
	{
		log_thread( (char*)"Connexion pas de reponse arduino" );
		change_arduino( false );
		sclose();
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serial::write_g()
{
	logf_thread( (char*)"[thread] Serial::write_g() !! %d", __LINE__ );
    th_write_g = std::thread(&Serial::write_g_thread, this);
    th_write_g.detach();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serial::push_cmd( char* cmd )
{
    tCommandes.push_back( string(cmd) );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serial::push_cmd( string& cmd )
{
    tCommandes.push_back( string(cmd) );
}
/*
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serial::sound()
{
    //aplay /home/rene/.astropilot/sounds/send.wav
    char *arguments[] = { (char*)"aplay", (char*)"/home/rene/.astropilot/sounds/send.wav", (char*)NULL };
    execv( "/usr/bin/aplay", arguments );

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serial::sound_thread()
{
    th_sound = std::thread(&Serial::sound, this);
    th_sound.detach();
}
*/
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serial::emet_commande()
{
    //logf_thread((char*)"Serial::emet_commande()" );
    //log_tab(true);
    if ( fd == -1 )			return;
    
    if (tCommandes.size()!=0 )
    {
        string s = tCommandes[0];
        
        logf_thread( (char*)"Lance la comande en attente : %s", (char*)s.c_str() );        
        char* str = (char*)s.c_str();
        int len = strlen(str);
        int n = write(fd, str, len);

        PanelConsoleSerial::getInstance().getConsole()->affiche( (char*)"" );
        PanelConsoleSerial::getInstance().getConsole()->affiche( (char*)str );

        if( n!=len ){
            logf_thread((char*)"[Error] Il manque des caracteres !!" );
            //log_tab(false);
            return;
        }

        n = write_byte('\n');

        if( n!=0 ){
            logf_thread((char*)"[Error] Sur le retour chariot!!" );
            //log_tab(false);
            return;
        }
        tCommandes.erase( tCommandes.begin()+0 );
        bFree = false;
    }
    else
    logf_thread( (char*)"Plus de commande en attente" );        
    
    //log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serial::to_bin( unsigned val, char* buffer, int len)
{
	int bit  = 0;
	int mask = 1;
	int i;

	for ( i = 0 ; i < (len-1) ; i++) 
	{
		bit = (val & mask) >> i ;
		buffer[ 15 - i ] = (char)('0' + bit);
		mask <<= 1 ;
	}					
	buffer[i] = 0;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serial::read_thread()
{
    unsigned char b[1];
    int i=0;
    int bPremiereConnexion = true;
    
    logf_thread( (char*)"START Serial::read_thread" );
    VarManager& var= VarManager::getInstance();
    bConnect = false;
    
    get_info_arduino();
    
    do {
        if ( fd == -1 ) break;
         
        int n = read(fd, b, 1);  // read a char at a time
        if( n==-1) 
        {
            continue;
        }    // couldn't read
        if( n==0 ) {
            usleep( 10 * 1000 ); // wait 10 msec try again
            nbZero++;
            if ( nbZero > 10)        sclose();
            //printf("wait ...");
            continue;
        }
        //buf[i] = b[0]; i++;
        if ( n>1 )          logf_thread( (char*)"[ERREUR]Serial::read_thread() PERTE DE CARRACTERE nb=%d",  n );
        char c = b[0];
        //if ( c==10)
        //printf( "%c", c);
        bool bAffiche = true;

        if (   ( n==1 && buffer[0]!='W' && (c=='\n' || c=='\r') )     ||
               ( n==1 && idx==3 && buffer[0]=='W' ) )
        {
            if ( idx != 0 )
            {
                buffer[idx++] = 0;
                string test = buffer;
                
            	if ( buffer[0] != 'W' && var.getb("bVerboseArduino") ) 
		            PanelConsoleSerial::getInstance().getConsole()->affiche( (char*)buffer );
                
				//-------------------------------------------------------------------
				//
                if ( test.find("= START") != string::npos )
                {
                    bPrintInfo = false;
                }
                else
				//-------------------------------------------------------------------
				//
                if ( test.find("= STOP") != string::npos )
                {
					if ( !bVersionArduino ) {		// pas de reception de version
						logf_thread((char*)"[ Erreur ] Mauvaise version Arduino sur STOP" );
						//logf_thread((char*)"[ Erreur ]   pas de version" );
						//bVersionArduino = false;
					}
						
                    bPrintInfo = true;
                    bAffiche = false;
					
					// Initialise les coordonnées
					if ( bPremiereConnexion )
						Serveur_mgr::getInstance()._sync( d_deg_ad, d_deg_dc );
					bPremiereConnexion = false;
                }
                else
				//-------------------------------------------------------------------
				//
				//-------------------------------------------------------------------
                if ( test.find("dbl click") != string::npos )
                    system( (char*)"aplay /home/rene/.astropilot/sounds/logout.wav" );
                else
				//-------------------------------------------------------------------
				//
                if ( test.find("Pas sideral") != string::npos )
                {
                    int n = test.find("Pas sideral");
                    string sPas = test.substr(n+11+3);
                    
                    pas_sideral = strtod(sPas.c_str(),NULL);
	
	 	            if ( var.getb("bVerboseArduino") )
	                    logf_thread( (char*)"Pas sideral : %0.8f", pas_sideral );
                }
                else
				//-------------------------------------------------------------------
				//
                if ( test.find("GOTO OK") != string::npos )
                {
                    if (bSound)     system( (char*)"aplay /home/rene/.astropilot/sounds/login.wav" );
                    bFree = true;
                    emet_commande();
                    //PanelConsoleSerial::getInstance().getConsole()->affiche( (char*)buffer );
                }
                else
				//-------------------------------------------------------------------
				//
                if ( test.find("V :") != string::npos )
                {
		        	if ( test.find( sVersionArduinoValable ) == string::npos )	{
		        		logf_thread((char*)"[ Erreur ] Mauvaise version Arduino %s", test.c_str() );
		        		logf_thread((char*)"[ Erreur ]   recherchée \"Version %s\"  | trouvée \"%s\"", VER_ARDUINO, buffer );
		        		bVersionArduino = false;
		        		change_arduino( false );
		        	}
		        	else {
		        		//logf_thread((char*)"  Version OK" );
 		        		bConnect = true;
						bVersionArduino = true;
		        		change_arduino( true );
		        	}
                }
                else
				//-------------------------------------------------------------------
				// Lecture des booleens
				//  voir champ de bits
				//-------------------------------------------------------------------
                if ( buffer[0] == 'W' )
                {
					f_bool B;
					B.u = (unsigned)buffer[2]<<8 + (unsigned)buffer[1];
					B.u = 0 + (unsigned)buffer[1];

					char buf_bool[17];
					char str[80];

					to_bin( B.u, buf_bool, sizeof(buf_bool) );
					snprintf( str, sizeof(str), (char*)"W %s - %02X",  buf_bool, B.u );
					
	 	            if ( var.getb("bVerboseArduino") )	{
			            PanelConsoleSerial::getInstance().getConsole()->affiche( (char*)str );
						logf_thread( (char*)str );
					}

					changeAsc( B.b.bRotAD );
					changeDec( B.b.bRotDC );
					changeSui( B.b.bRotSiderale );
					changeRetourPos( B.b.bRetStellar );

		            if ( B.b.bJoy != bOldJoy )
		            {
		                changeJoy(B.b.bJoy);
		                
		                if ( bSound)
		                {
				            if ( B.b.bJoy )			system( (char*)"aplay /home/rene/.astropilot/sounds/receive.wav" );
							else	                system( (char*)"aplay /home/rene/.astropilot/sounds/send.wav" );
		                }
		                bOldJoy = B.b.bJoy;
		            }
                }
                else
				//-------------------------------------------------------------------
				//
                if ( buffer[0] != '-'  )
                {
                    if (bPrintInfo) PanelConsoleSerial::getInstance().getConsole()->affiche( (char*)buffer );
                }
				//-------------------------------------------------------------------
				//
                else
                    PanelConsoleSerial::getInstance().writeln( (char*)buffer );
            }



            idx = 0;
            buffer[0] = 0;
            ///*
            //*/
        }
        else
        {
            buffer[idx++] = c;
        }
        b[0] = 0;
        //if ( c < 20 )   printf( "%d-%c,", (int)c, c );
        
    } while( true );

    bConnect = false;
	
	changeJoy( false );
    changeDec( true );
    changeAsc( true );
	changeSui( false );
	changeRetourPos( false );
	system( (char*)"aplay /home/rene/.astropilot/sounds/cembalo-1.wav" );
    logf_thread( (char*)"FIN   Serial::read_thread" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serial::start_thread()
{
    if ( fd != -1 )             
    {
		system( (char*)"aplay /home/rene/.astropilot/sounds/cembalo-1.wav" );
		logf_thread( (char*)"[thread] Serial::start_thread() !! %d", __LINE__);
        th_serial = std::thread(&Serial::read_thread, this);
        th_serial.detach();
        nbConnect = 0;
		
		
		sleep(1);
        logf_thread( (char*)"Envoi de la commande g" );
	   	write_g();
	   	

    }
    else                        logf_thread( (char*)"Port pas ouvert !! " );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
// takes the string name of the serial port (e.g. "/dev/tty.usbserial","COM1")
// and a baud rate (bps) and connects to that port at that speed and 8N1.
// opens the port in fully raw mode so you can send binary data.
// returns valid fd, or -1 on error
void Serial::sopen()
{
    logf_thread( (char*)"Serial::sopen()");
    
    if (fd != -1)
    {        
        logf_thread( (char*)"sopen fd != -1 !! " );
        return;
    }
    
    
    int baud = 115200;
    struct termios toptions;
    
    //fprintf(stderr,"init_serialport: opening port %s @ %d bps\n",
    //        serialport,baud);

    fd = open(dev_name.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)  {
        logf_thread( (char*)"init_serialport: Unable to open port ");
        fd = -1;
        return;
    }
    
    if (tcgetattr(fd, &toptions) < 0) {
        logf_thread( (char*)"init_serialport: Couldn't get term attributes");
        fd = -1;
        return;
    }
    
    speed_t brate = baud; // let you override switch below if needed
    switch(baud) {
    case 4800:   brate=B4800;   break;
    case 9600:   brate=B9600;   break;
#ifdef B14400
    case 14400:  brate=B14400;  break;
#endif
    case 19200:  brate=B19200;  break;
#ifdef B28800
    case 28800:  brate=B28800;  break;
#endif
    case 38400:  brate=B38400;  break;
    case 57600:  brate=B57600;  break;
    case 115200: brate=B115200; break;
    }
    cfsetispeed(&toptions, brate);
    cfsetospeed(&toptions, brate);

    // 8N1
    toptions.c_cflag &= ~PARENB;
    toptions.c_cflag &= ~CSTOPB;
    toptions.c_cflag &= ~CSIZE;
    toptions.c_cflag |= CS8;
    // no flow control
    toptions.c_cflag &= ~CRTSCTS;

    toptions.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
    toptions.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

    toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
    toptions.c_oflag &= ~OPOST; // make raw

    // see: http://unixwiz.net/techtips/termios-vmin-vtime.html
    toptions.c_cc[VMIN]  = 0;
    toptions.c_cc[VTIME] = 20;
    
    if( tcsetattr(fd, TCSANOW, &toptions) < 0) {
        logf_thread( (char*)"init_serialport: Couldn't set term attributes");
        fd = -1;
        return;
    }

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
// takes the string name of the serial port (e.g. "/dev/tty.usbserial","COM1")
// and a baud rate (bps) and connects to that port at that speed and 8N1.
// opens the port in fully raw mode so you can send binary data.
// returns valid fd, or -1 on error
void Serial::sclose()
{
    if ( fd ==-1 )      return;

    close( fd );
    fd = -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serial::reset()
{
    tCommandes.clear();
    bFree = true;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serial::testVersionArduino()
{
	bVersionArduino = true;
	sVersionArduinoValable = VER_ARDUINO;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serial::print_list()
{
    logf( (char*)"---- Arduino --------" );
    if ( isConnect() )
    {
    	logf( (char*)"  EQ5_DRIVE  : v%s", VER_ARDUINO );
    }
    else
    {
    	logf( (char*)" Aucune Arduino connectée" );
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------




