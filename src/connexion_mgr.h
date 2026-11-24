#ifndef CONNEXION_MGR_H
#define CONNEXION_MGR_H  1


#ifndef TARGET_ARM
#include <GL/glew.h>
#include <GL/glut.h>
#include "camera_mgr.h"
#endif

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string>

//#include "MathlibD.h"
//#include <WindowsManager.h>
#include <thread>

#include "main.h"
#include "serial.h"
#include "serveur_mgr.h"


using namespace std;

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#include "Singleton.h"

SINGLETON_BEGIN( Connexion_mgr )
   
private:
    std::thread                 th_poll_connexionDEV;
    std::thread                 th_poll_connexionZWO;
    vector<string>              t_port_polling;
    vector<string>              t_port_current;
    vector<string>              exclude;
    
    bool                        bStartDEV;
    bool                        bStartZWO;
    atomic<bool>				bExitThreadDEV;  
    atomic<bool>				bExitThreadZWO;  
    
    int							pCamZWO[16];
    int							nCamZWO;

public:
    Connexion_mgr();

    void                        startDEV();
    void                        startZWO();
//    void                        pooling();
    void                        threadPoolingDEV();
    void                        threadPoolingZWO();
//    std::thread                 startThread();
    bool                        isExclude(string);  
    void                        add_port();
    void                        sup_port();
    
    void                        print_list();
    void						stopThreadDEV()					{ bExitThreadDEV = true; }
    void						stopThreadZWO()					{ bExitThreadZWO = true; }

inline vector<string>&			get_exclude()					{ return exclude; }
SINGLETON_END()

#endif

