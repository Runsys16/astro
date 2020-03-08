#ifndef CAPTURE_H
#define CAPTURE_H  1

#include "camera.h"
#include <dirent.h>
#include <algorithm>

#include <WindowsManager.h>
#include "main.h"
#include "panel_capture.h"
#include "fits.h"


using namespace std;

class Capture
{
protected:
    bool                        bNewBackground;
    bool                        bFirst;
    bool                        bIcone;
    bool                        bFullScreen;
    bool                        bFits;
    
    vector<string>              filenames;
    string                      filename;
    string                      dirname;
    string                      basename;
    
    PanelWindow*                pW;
    PanelCapture*               panelPreview;
    PanelText*                  pTitre;
    
    struct readBackground       readBgr;
    Fits *                      fits;

public :
    Capture();
    Capture(string);
    Capture(string, string);
    ~Capture();
    
    void                        pooling();
    void                        create_preview();
    void                        resize(int,int);
    void                        resize(int,int,int,int);
    void                        fullscreen();

    void                        onTop();
    void                        addStar(int,int);

    bool                        isMouseOver(int, int);
    
    void                        show();
    void                        hide();
    
    void                        setColor(long);
    
    virtual void                update();
    void                        afficheFits();
    
inline rb_t *                   getRB()                                     { return &readBgr; }

inline void                     setEchelle(float f)                         { panelPreview->setEchelle(f); }
inline float                    getEchelle()                                { return panelPreview->getEchelle(); }

inline void                     setCentX(float f)                           { panelPreview->setCentX(f); }
inline void                     setCentY(float f)                           { panelPreview->setCentY(f); }
inline float                    getCentX()                                  { return panelPreview->getCentX(); }
inline float                    getCentY()                                  { return panelPreview->getCentY(); }

inline string                   getFilename()                               { return filename; }
inline string                   getBasename()                               { return basename; }
inline string                   getDirname()                                { return dirname; }
inline PanelCapture*            getPreview()                                { return panelPreview; }

inline void                     setIcone(bool b)                            { bIcone = b; panelPreview->setIcone(b); }
inline bool                     getIcone()                                  { return bIcone; }

inline void                     setFullScreen(bool b)                       { bFullScreen = b; }
inline bool                     getFullScreen()                             { return bFullScreen; }

inline bool                     isFits()                                    { return bFits; }
};


#endif
