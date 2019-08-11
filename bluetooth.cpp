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
    int dev_id, sock, len, flags;
    int i;
    char addr[19] = { 0 };
    char name[248] = { 0 };
    bStopScan = false;
    bScan = true;
    
    while( !bStopScan )
    {

        dev_id = hci_get_route(NULL);
        sock = hci_open_dev( dev_id );
        if (dev_id < 0 || sock < 0) {
            perror("opening socket");
            exit(1);
        }

        len  = 8;
        max_rsp = 255;
        flags = IREQ_CACHE_FLUSH;
        ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));
        
        num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
        if( num_rsp < 0 ) perror("hci_inquiry");

        for (i = 0; i < num_rsp; i++) {
            ba2str(&(ii+i)->bdaddr, addr);
            memset(name, 0, sizeof(name));
            if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), 
                name, 0) < 0)
            strcpy(name, "[unknown]");
            logf( (char*)"   BluetoothManager::scan  %s  %s\n", addr, name);
            string sName = string(name);
            
            if ( sName.find("DSD TECH HC-05") != string::npos )
            {
                if ( bConnect == false )
                {
                    bConnect = true;
                    th_connect = thread(&BluetoothManager::connect_hc05, this);
                    th_connect.detach();
                }
            }
            
        }


        free( ii );
        close( sock );

        sleep(2);
        
        logf( (char*)"Fin du scan ..." );
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
    int s, status;
    char dest[18] = "00:14:03:06:63:02";

    // allocate a socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // set the connection parameters (who to connect to)
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t) 1;
    str2ba( dest, &addr.rc_bdaddr );

    // connect to server
    logf( (char*)"  BT: connexion ..." );
    status = connect( s, (struct sockaddr *)&addr, sizeof(addr) );

    logf( (char*)"  BT: envoi du message ..." );
    // send a message
    int n = 0;
    char mess[255];
    while( !bCancel ) {
        sprintf( mess, "astropilot  %d.... !\n", n++ );
        //logf( (char*)"  BT: Envoi de %s", (char*)mess );
        status = write( s, mess, strlen(mess) );
        sleep( 2 );
    }

    bCancel = false;
    if( status < 0 ) perror("uh oh");

    close(s);

    logf( (char*)"  BT: deconnexion" );

    bConnect = false;

}


//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

