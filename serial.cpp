#include "serial.h"    /* Standard input/output definitions */
#include "panel_console_serial.h"    
#include <unistd.h>

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Serial::Serial()
{
    logf((char*)"Constructeur Serial::Serial() -------------" );
    fTimeOut = 0.0;
    fd = -1;

    idx = 0;
    nbZero = 0;
    
    bFree = true;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serial::init( string dev)
{
    logf((char*)"Serial::init(%s) -------------", dev.c_str() );
    fd = -1;
    dev_name = dev;
    idx = 0;
    nbZero = 0;

    sopen();
    start_thread();
    
    bPrintInfo = true;
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
int Serial::write_string( const char* str)
{
    //return -1;
    
    if ( fd ==-1 )                          return -1;
    if ( (fTimeMili - fTimeOut) < 1.0 )     return -1;
    

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
            logf( (char*)"Cmd en attent: %s", (char*)s.c_str() );
            return 0;
        }
    }
    else    
    if ( str[0] == 'n' )                bFree = true;


    
    //logf( (char*)"Arduino : %s", str );
    //PanelConsoleSerial::getInstance().writeln( (char*)"console" );
    PanelConsoleSerial::getInstance().getConsole()->affiche( (char*)"" );
    PanelConsoleSerial::getInstance().getConsole()->affiche( (char*)str );
    
    fTimeOut = fTimeMili;
    

    int len = strlen(str);
    int n = write(fd, str, len);
    if( n!=len ) 
        return -1;
    n = write_byte('\n');
    if( n!=len ) 
        return -1;
    return 0;
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
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serial::sound()
{
    //aplay /usr/share/sounds/purple/send.wav
    char *arguments[] = { (char*)"aplay", (char*)"/usr/share/sounds/purple/send.wav", (char*)NULL };
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
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serial::emet_commande()
{
    logf((char*)"Serial::emet_commande()" );
    log_tab(true);
    
    if (tCommandes.size()!=0 )
    {
        string s = tCommandes[0];
        
        logf( (char*)"Lance la comande en attente : %s", (char*)s.c_str() );        
        char* str = (char*)s.c_str();
        int len = strlen(str);
        int n = write(fd, str, len);

        PanelConsoleSerial::getInstance().getConsole()->affiche( (char*)"" );
        PanelConsoleSerial::getInstance().getConsole()->affiche( (char*)str );

        if( n!=len ){
            logf((char*)"[ERREUR]Il manque des caracteres !!" );
            log_tab(false);
            return;
        }

        n = write_byte('\n');

        if( n!=0 ){
            logf((char*)"[ERREUR]Sur le retour chariot!!" );
            log_tab(false);
            return;
        }
        tCommandes.erase( tCommandes.begin()+0 );
        bFree = false;
    }
    else
    logf( (char*)"Plus de commande en attente" );        
    
    log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serial::read_thread()
{
    unsigned char b[1];
    int i=0;
    logf( (char*)"START Serial::read_thread" );
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
        if ( n>1 )          logf( (char*)"[ERREUR]Serial::read_thread() PERTE DE CARRACTERE nb=%d",  n );
        char c = b[0];
        //if ( c==10)
        //printf( "%c", c);
        bool bAffiche = true;

        if ( n == 1 && (c == '\n' || c == '\r') )
        {
            if ( idx != 0 )
            {
                buffer[idx++] = 0;
                
                string test = buffer;
                //sound_thread();
                if ( test.find("=INFO START") != string::npos )
                {
                    bPrintInfo = false;
                }
                else
                if ( test.find("=INFO STOP") != string::npos )
                {
                    bPrintInfo = true;
                    bAffiche = false;
                }
                else
                if ( test.find("Change joy ...NOK") != string::npos )
                {
                    changeJoy(false);
                    
                    system( (char*)"aplay /usr/share/sounds/purple/send.wav" );
                }
                else 
                if ( test.find("Change joy ...OK") != string::npos )
                {
                    changeJoy(true);
                    system( (char*)"aplay /usr/share/sounds/purple/receive.wav" );
                }
                else
                if ( test.find("Retour Stellarium on") != string::npos )
                {
                    changeRetourPos( true );
                }
                else
                if ( test.find("Retour Stellarium off") != string::npos )
                {
                    changeRetourPos( false );
                }
                else
                if ( test.find("dbl click") != string::npos )
                {
                    system( (char*)"aplay /usr/share/sounds/purple/logout.wav" );
                }
                else
                if ( test.find("Rotation terre") != string::npos )
                {
                    if ( test.find("OUI") != string::npos)
                    {
                        //logf( (char*)"ARDUINO Rotation terre : OUI" );
                        changeSui( true );
                    }
                    else
                    {
                        //logf( (char*)"ARDUINO Rotation terre : NON" );
                        changeSui( false );
                    }
                }
                else
                if ( test.find("Rotation Declinaison") != string::npos )
                {
                    if ( test.find("normal") != string::npos)
                    {
                        //logf( (char*)"Declinaison : normale" );
                        changeDec( true );
                    }
                    else
                    {
                        //logf( (char*)"Declinaison : inverse" );
                        changeDec( false );
                    }
                }
                else
                if ( test.find("Rotation  Asc Droite") != string::npos )
                {
                    if ( test.find("normal") != string::npos)
                    {
                        //logf( (char*)"Asc Droite : normale" );
                        changeAsc( true );
                    }
                    else
                    {
                        //logf( (char*)"Asc Droite : inverse" );
                        changeAsc( false );
                    }
                }
                else
                if ( test.find("GOTO OK") != string::npos )
                {
                    if (bSound)     system( (char*)"aplay /usr/share/sounds/purple/login.wav" );
                    bFree = true;
                    emet_commande();
                    PanelConsoleSerial::getInstance().getConsole()->affiche( (char*)buffer );
                }

                if ( bPrintInfo == true && bAffiche )
                {
                    //PanelConsoleSerial::getInstance().writeln( (char*)"console" );
                    //logf( (char*)"Buffer : \"%s\"", (char*)buffer );
                    PanelConsoleSerial::getInstance().writeln( (char*)buffer );
                }    

            }
            idx = 0;
            buffer[0] = 0;
            /*
            if ( buffer[0] != '-' )
            {
                PanelConsoleSerial::getInstance().getConsole()->affiche( (char*)buffer );
            }
            */
        }
        else
        {
            buffer[idx++] = c;
        }
        b[0] = 0;
        //if ( c < 20 )   printf( "%d-%c,", (int)c, c );
        
    } while( true );

    logf( (char*)"FIN   Serial::read_thread" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Serial::start_thread()
{
    if ( fd != -1 )             
    {
        th_serial = std::thread(&Serial::read_thread, this);
        th_serial.detach();
    }
    else                        logf( (char*)"Port deja ouvert !! " );
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
    logf( (char*)"Serial::sopen()");
    
    if (fd != -1)
    {        
        logf( (char*)"sopen fd != -1 !! " );
        return;
    }
    
    
    int baud = 115200;
    struct termios toptions;
    
    //fprintf(stderr,"init_serialport: opening port %s @ %d bps\n",
    //        serialport,baud);

    fd = open(dev_name.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)  {
        logf( (char*)"init_serialport: Unable to open port ");
        fd = -1;
        return;
    }
    
    if (tcgetattr(fd, &toptions) < 0) {
        logf( (char*)"init_serialport: Couldn't get term attributes");
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
        logf( (char*)"init_serialport: Couldn't set term attributes");
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




