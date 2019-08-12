#include "bluetooth.h"


//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
BluetoothManager::BluetoothManager()
{
    bStart = false;
    bScan = false;
    bStopScan = false;
    bCancel = false;
    bConnect = false;
    bPrintErreurDevice = false;
    bPrintErreurSocket = false;
    sock = -1;
    start();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void BluetoothManager::start()
{
    if ( !bStart )
    {
        th_scan = thread(&BluetoothManager::scan, this);
        th_scan.detach();
        bStart = true;
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void BluetoothManager::scan()
{
    if ( bScan )            return;
    
    logf( (char*)"BluetoothManager::scan() ..." );
    
    inquiry_info *ii = NULL;
    int max_rsp, num_rsp;
    int dev_id, len, flags;
    int i;
    char addr[19] = { 0 };
    char name[248] = { 0 };
    bStopScan = false;
    bScan = true;
    int sock_scan;
    
    while( !bStopScan )
    {

        dev_id = hci_get_route(NULL);
        if (dev_id < 0) {
            //perror("opening socket");
            if (!bPrintErreurDevice)
                logf( (char*)"[Erreur] BluetoothManager::scan()  opening device ..." );
                
            sleep(10);
            bPrintErreurDevice = true;
            continue;
        }
        bPrintErreurDevice = false;
        
        
        sock_scan = hci_open_dev( dev_id );
        if (sock_scan < 0) {
            //perror("opening socket");
            if (!bPrintErreurSocket)
                logf( (char*)"[Erreur] BluetoothManager::scan()  opening socket() ..." );
            bPrintErreurSocket = true;
            sleep(10);
            continue;
        }
        bPrintErreurSocket = false;

        len  = 8;
        max_rsp = 255;
        flags = IREQ_CACHE_FLUSH;
        ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));
        
        num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
        if( num_rsp < 0 ) perror("hci_inquiry");

        //logf( (char*)"BluetoothManager::scan() ..." );
        for (i = 0; i < num_rsp; i++) {
            ba2str(&(ii+i)->bdaddr, addr);
            memset(name, 0, sizeof(name));
            if (hci_read_remote_name(sock_scan, &(ii+i)->bdaddr, sizeof(name), name, 0) < 0)       strcpy(name, "[unknown]");
            
            //logf( (char*)"   BluetoothManager::scan  %s  %s\n", addr, name);
            string sName = string(name);
            
            if ( sName.find("DSD TECH HC-05") != string::npos )
            {   
                //*
                if ( bConnect == false )
                {
                    bConnect = true;
                    //bConnect = true;
                    logf( (char*)"BluetoothManager::scan() lancement de la connexion BT..." );
                    connect_hc05();

                    th_read = thread(&BluetoothManager::th_read_hc05, this);
                    th_read.detach();
                    th_write = thread(&BluetoothManager::th_write_hc05, this);
                    th_write.detach();

                }
                //*/
            }
            
        }


        free( ii );
        close( sock_scan );

        sleep(2);
        
        //logf( (char*)"Fin du scan ..." );
        bStart = false;
    }
    bScan = false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void BluetoothManager::connect_hc05()
{
    logf( (char*)"BluetoothManager::connect_hc05()" );
    
    struct sockaddr_rc addr;
    int status;
    
    char dest[18] = "00:14:03:06:63:02";

    // allocate a socket
    sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    //fcntl(sock, F_SETFL, O_NONBLOCK);
    // set the connection parameters (who to connect to)
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t) 1;
    str2ba( dest, &addr.rc_bdaddr );

    // connect to server
    logf( (char*)"  BT: connexion ..." );
    status = connect( sock, (struct sockaddr *)&addr, sizeof(addr) );

    if( status < 0 ) 
    {
        bCancel = true;
        logf( (char*)"[Erreur] connexion Bluetooth" );
    }

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void BluetoothManager::th_read_hc05()
{
    logf( (char*)"BluetoothManager::th_read_hc05()" );

    bCancel = false;
    int n = 0;
    char mess[255];
    int bytes_read;
    
    
    while( !bCancel ) {

        memset(mess,0,sizeof(mess));

        //bytes_read = recv(sock, mess, sizeof(mess), 0) ;
        bytes_read = read(sock, mess, sizeof(mess)) ;

        if(bytes_read <= 0)
        {
            logf( (char*)"[Erreur] Lecture Bluetooth" );
            break;
        }
        
        //logf( (char*)"reception : %s", (char*)mess ); 
    }
    logf( (char*)"BluetoothManager::th_read_hc05() fin" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void BluetoothManager::th_write_hc05()
{
    logf( (char*)"BluetoothManager::th_write_hc05()" );

    bCancel = false;
    int n = 0, status;
    char mess[255];
    while( !bCancel ) {
        sleep( 2 );
        sprintf( mess, "astropilot  %d.... !\n", n++ );
        string str = string(mess);

        write_hc05( str );
    }
    logf( (char*)"BluetoothManager::th_write_hc05() fin" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void BluetoothManager::write_hc05(string str)
{
    int status;
    
    status = write( sock, str.c_str(), str.size() );
    //logf( (char*)"  BT: Envoi de %s", (char*)str.c_str() );

    if( status < 0 ) 
    {
        bCancel = true;
        logf( (char*)"[Erreur] ecriture Bluetooth" );
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void BluetoothManager::disconnect_hc05()
{
    if ( sock == -1 )       return;
    
    logf( (char*)"BluetoothManager::disconnect_hc05()" );
    bCancel = true;
    sleep(1);
    close(sock);
    sock = -1;
    

    //th_read.join();
    //th_write.join();


    //logf( (char*)"  BT: deconnexion" );

    bConnect = false;
    sleep(1);
    bCancel = false;

}


//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

