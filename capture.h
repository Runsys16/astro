#ifndef CAPtuRE_H
#define CAPtuRE_H  1

#include "camera.h"
#include <dirent.h>
#include <algorithm>

#include <WindowsManager.h>
#include "main.h"
#include "panel_capture.h"


using namespace std;

class Capture
{
protected:
    bool                        bNewBackground;
    bool                        bFirst;
    vector<string>              filenames;
    string                      filename;
    
    PanelWindow*                pW;
    PanelCapture*               panelPreview;
    PanelText*                  pTitre;
    
    struct readBackground       readBgr;

public :
    Capture();
    Capture(string, string);
    ~Capture();
    
    void                        pooling();
    void                        create_preview();
    void                        resize(int,int);
    void                        resize(int,int,int,int);

    void                        onTop();

inline void             setEchelle(float f)                         { panelPreview->setEchelle(f); }
inline float            getEchelle()                                { return panelPreview->getEchelle(); }
inline void             setDX(float f)                              { panelPreview->setDX(f); }
inline void             setDY(float f)                              { panelPreview->setDY(f); }
inline float            getDX(float f)                              { return panelPreview->getDX(); }
inline float            getDY(float f)                              { return panelPreview->getDY(); }

};


#endif
