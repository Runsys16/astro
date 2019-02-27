#ifndef CAPtuRE_H
#define CAPtuRE_H  1

#include "camera.h"
#include <dirent.h>

#include <WindowsManager.h>
#include "main.h"



using namespace std;

class Capture
{
protected:
    bool                        bNewBackground;
    bool                        bFirst;
    vector<string>              filenames;
    string                      filename;
    
    PanelWindow*                panelPreview;
    PanelText*                  pTitre;
    
    struct readBackground       readBgr;

public :
    Capture();
    ~Capture();
    
    void                        pooling();
    void                        create_preview();


};


#endif
