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
    std::thread                 th_1;

    std::thread                 connect_1;

    int                         sock_ref;  
    int                         sock_1;

    bool                        listen_1;
    bool                        traite_1;
    
    string						sIP_synscan;
    //PanelDebug					panel_debug;

public:
							    SYNSCAN();
    
    float                       com2rad( int);
    void                        decode(struct stellarium& ss, unsigned char* buffer);

	void						traite_connexion_synscan();
    void                        thread_listen_synscan();
    void                        start_synscan();
   
    void                        write_stellarium( char* s);
    void                        write_stellarium( int, char* s, int);
    void                        write_stellarium( int, char* s, int, bool );
    void                        write_stellarium( double, double );
    
    void                        close_all();

    void                        print_list();

inline bool                     is_connect()						{ return sock_ref != -1; }
//inline PanelWindow*				getPanelWindow()					{ return &panel_win; }
    
    
    
SINGLETON_END()

#endif

