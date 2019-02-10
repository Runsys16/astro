#ifndef CONNEXION_MGR_H
#define CONNEXION_MGR_H  1


//#include "camera.h"
#include <GL/glew.h>
#include <GL/glut.h>

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string>

#include <WindowsManager.h>
#include <thread>

#include "main.h"
#include "camera_mgr.h"
#include "serial.h"


using namespace std;

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#include "Singleton.h"

SINGLETON_BEGIN( Connexion_mgr )

public:
    Connexion_mgr();

    void                        start();
    void                        pooling();
    void                        threadPooling();
    std::thread                 startThread();
    void                        add_port();
    void                        sup_port();
    
    void                        print_list();
    
   
private:
    std::thread                 th_poll_connexion;
    vector<string>              t_port_polling;
    vector<string>              t_port_current;
    
    bool                        bStart;
    
SINGLETON_END()

#endif

