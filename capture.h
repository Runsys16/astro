#ifndef CAPtuRE_H
#define CAPtuRE_H  1

#include "camera.h"
#include <dirent.h>
#include <algorithm>

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
    void                        resize(int,int);
    void                        resize(int,int,int,int);

    void                        onTop();
};


#endif
