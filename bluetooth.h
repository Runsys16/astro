#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <GL/glew.h>
#include <GL/glut.h>


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/rfcomm.h>


#include <WindowsManager.h>
#include <thread>

#include "main.h"
#include "serial.h"

using namespace std;

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#include "Singleton.h"

SINGLETON_BEGIN( BluetoothManager )
    
public:
    BluetoothManager();
    
    void                    start();
    void                    scan();
    void                    connect_hc05();
    void                    th_read_hc05();
    void                    th_write_hc05();
    void                    write_hc05(string);
    void                    disconnect_hc05();
    inline void             disconnect()        { disconnect_hc05(); }
private:
    thread                  th_scan;
    thread                  th_write;
    thread                  th_read;
    bool                    bStart;
    
    bool                    bCancel;
    bool                    bConnect;
    bool                    bScan;
    bool                    bStopScan;
    
    int                     sock;
    int                     x, y, n, b;
    int                     cent;
    int                     nbMess;
    //static int n = 0;
    char                    mess[255];
    char                    buff[255];
    
SINGLETON_END()

#endif

