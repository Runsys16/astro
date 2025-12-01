#ifndef LX200_MGR_H
#define LX200_MGR_H  1


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
#define PANEL_LX200_DEBUG
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
SINGLETON_BEGIN( LX200 )

protected:
    std::thread                 th_lx200_listen;

    int                         sock_lx200;
    int                         sock_listen_lx200;
    unsigned					uPort;
    
    double						dRA;
    double						dDC;

    bool                        bListen;
    bool                        bLx200;
    
    string						sIP_lx200;
    
#ifdef PANEL_LX200_DEBUG
    PanelDebug					panel_debug;
#endif

public:
							    LX200();
    
    void                        traite_command_G( char*, int);
    void                        traite_command_M( char*, int);
    void                        traite_command_Q( char*, int);
    void                        traite_command_R( char*, int);
    void                        traite_command_S( char*, int);
    void                        traite_connexion_lx200();
    void                        thread_listen_lx200();
    void                        start_lx200();
   
    void                        write_lx200( char* s);
    void                        write_lx200( int, char* s, int);
    void                        write_lx200( int, char* s, int, bool );
    
    void                        close_all();

    void                        print_list();

inline bool                     is_connect()						{ return sock_lx200 != -1; }

#ifdef PANEL_LX200_DEBUG
inline PanelDebug*				getPanel()							{ return &panel_debug; }
	void						setColor( uint32_t );
#endif
    
    
    
SINGLETON_END()

#endif

