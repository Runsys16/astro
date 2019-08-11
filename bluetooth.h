#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <GL/glew.h>
#include <GL/glut.h>


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/rfcomm.h>


#include <WindowsManager.h>
#include <thread>

#include "main.h"

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
    inline void             disconnect()            { bCancel = true; }
private:
    thread                  th_scan;
    thread                  th_connect;
    bool                    bStart;
    
    bool                    bCancel;
    bool                    bConnect;
    bool                    bScan;
    bool                    bStopScan;
    
SINGLETON_END()

#endif

