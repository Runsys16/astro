#ifndef SERVEUR_MGR_H
#define SERVEUR_MGR_H  1


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
#include "serveur_mgr.h"

using namespace std;

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#include "Singleton.h"
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
struct stellarium
{
    long long   tz;
    int         ra;
    int         dc;
};
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
SINGLETON_BEGIN( Serveur_mgr )

protected:
    std::thread                 th_1;
    std::thread                 th_2;

    std::thread                 connect_1;

    int                         sock_stellarium;
    int                         sock_ref;
    
    int                         sock_1;
    int                         sock_2;

    bool                        listen_1;
    bool                        listen_2;
    bool                        traite_1;
    bool                        traite_2;
    
    string						sIPref;
    string						sIPstellarium;

public:
    Serveur_mgr();
    
    float                       com2rad( int);
    void                        decode(struct stellarium& ss, unsigned char* buffer);

    void                        traite_connexion_deplacement();
    void                        thread_listen_deplacement();
    void                        start_deplacement();

    void                        traite_connexion_init();
    void                        thread_listen_init();
    void                        start_init();
   
    void                        write_stellarium( char* s);
    void                        write_stellarium( double, double );
    
    void                        close_all();

    void                        print_list();

inline bool                     is_running_1()                          { return sock_1 != -1; }
inline bool                     is_running_2()                          { return sock_2 != -1; }
    
inline bool                     isConnect()                             { return sock_stellarium!=-1; }
    
    
SINGLETON_END()

#endif

