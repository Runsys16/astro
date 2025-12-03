#ifndef SYNSCAN_H
#define SYNSCAN_H  1


//#include "camera.h"
#include <GL/glew.h>
#include <GL/glut.h>

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string>

#include <limits.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>


#include "MathlibD.h"
#include <WindowsManager.h>
#include <thread>

#include "main.h"
#include "serial.h"
#include "panel_debug.h"

using namespace std;

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#include "Singleton.h"
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
SINGLETON_BEGIN( SYNSCAN )

protected:
    std::thread                 th_synscan_listen;

    int                         sock_listen_synscan;  
    int                         sock_synscan;
    unsigned					uPort;

    bool                        listen_1;
    bool                        traite_1;
    
    double						AD_deg;
    double						DC_deg;
    
    int							AA;
    int							MM;
    int							JJ;
    int							HH;
    int							MN;
    int							SS;
    int							GMT;
    int							HETE;
    
    string						sIP_listen_synscan;
    string						sIP_synscan;
    
    char						sHex[91];     // 3*30 + '0' terminal

public:
							    SYNSCAN();
    
    void                        decode_time(char*, int);
    void                        encode_time(char*, int);
    void                        decode_precise(char*, int);
    void                        encode_precise(char*, int);

    void                        traite_command( char*, int);
	void						traite_connexion_synscan();
    void                        thread_listen_synscan();
    void                        start_synscan();
   
    void                        write_synscan( char* s);
    void                        write_synscan( int, char* s, int);
    void                        write_synscan( int, char* s, int, bool );
    void                        to_hex( char*, int );

    void                        close_all();

    void                        print_list();

inline bool                     is_connect()						{ return sock_synscan != -1; }
//inline PanelWindow*				getPanelWindow()					{ return &panel_win; }
    
    
    
SINGLETON_END()

#endif

