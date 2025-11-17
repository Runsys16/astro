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
SINGLETON_BEGIN( LX200 )

protected:
    std::thread                 th_1;

    std::thread                 connect_1;

    int                         sock_ref;
    
    int                         sock_1;

    bool                        listen_1;
    bool                        traite_1;
    
    string						sIP_lx200;

public:
							    LX200();
    
    float                       com2rad( int);
    void                        decode(struct stellarium& ss, unsigned char* buffer);

    void                        traite_connexion_lx200();
    void                        thread_listen_lx200();
    void                        start_lx200();
   
    void                        write_stellarium( char* s);
    void                        write_stellarium( int, char* s, int);
    void                        write_stellarium( int, char* s, int, bool );
    void                        write_stellarium( double, double );
    
    void                        close_all();

    void                        print_list();

inline bool                     is_connect()                          { return sock_1 != -1; }
    
    
    
SINGLETON_END()

#endif

