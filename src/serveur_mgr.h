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
    std::thread                 th_depl;
    std::thread                 th_init;

    int                         sock_deplacement;
    int                         sock_listen_deplacement;
    unsigned					uPort_deplacement;
    
    int                         sock_init;
    int                         sock_listen_init;
    unsigned					uPort_init;

    bool                        listen_1;
    bool                        listen_2;
    bool                        bTraite_depl;
    bool                        bTraite_init;
    
    string						sIP_listen_init;
    string						sIP_init;
    string						sIP_listen_depl;
    string						sIP_depl;

public:
    Serveur_mgr();
    
    double						com2rad( int);
    void                        decode(struct stellarium& ss, unsigned char* buffer);

    void						traite_connexion_deplacement();
    void                        thread_listen_deplacement();
    void                        start_deplacement();

    void                        traite_connexion_init();
    void                        thread_listen_init();
    void                        start_init();
   
    void                        write_stellarium( char* s);
    void                        write_stellarium( int, char* s, int);
    void                        write_stellarium( int, char* s, int, bool );
    void                        write_stellarium( double, double );

    void                        _goto( double, double );
    void                        _sync( double, double );
    void                        _goto_rad( double, double );
    void                        _sync_rad( double, double );
    
    void                        close_all();

    void                        print_list();

inline bool                     isConnect()                             { return sock_deplacement !=-1; }
    
    
SINGLETON_END()

#endif

