#ifndef CAPTURE_H
#define CAPTURE_H  1

#include "camera.h"
#include <dirent.h>
#include <algorithm>

#include <WindowsManager.h>
#include "main.h"
#include "panel_capture.h"
#include "fits.h"
#include "var_mgr.h"


using namespace std;

class Capture : public PanelWindow
{
protected:
    bool                        bNewBackground;
    bool                        bFirst;
    bool                        bIconized;
    bool                        bFullScreen;
    bool                        bFits;
    bool						bAffInfoFits;
    bool						bAfficheInfoSouris;
    bool						bAfficheGrille;
    bool						bAfficheCorrectionFits;
    
    vector<string>              filenames;
    string                      filename;
    string                      dirname;
    string                      basename;
    
    //PanelWindow*                pW;
    PanelCapture*               panelPreview;
    PanelText*                  pTitre;
    PanelText*                  pNbStars;

    PanelSimple*                pFermer;
    PanelSimple*                pIconiser;
    PanelSimple*                pMaximiser;

    
    struct readBackground       readBgr;
    Fits *                      fits;

public :
    Capture();
    Capture(string);
    Capture(string, string);
    ~Capture();
    
    void                        pooling();
    
    virtual void                update();
    virtual void                updatePos();
    		void                updatePosIcones();

    virtual void                clickLeft( int, int);
    virtual void                releaseLeft( int, int);

    void                        create_icones();
    void                        create_preview();
    
    void                        resize(int,int);
    void                        resize(int,int,int,int);
    void                        fullscreen();

    void                        onTop();
    void                        addStar(int,int);

    //bool                        isMouseOver(int, int);
    
    void                        show();
    void                        hide();
    
    void                        setColor(long);

    void                        afficheFits();
    void                        afficheInfoFits();
    void                        afficheInfoFits(bool);
    void						iconize();
	void						restaure(bool bInfo, bool bGrille, bool bSouris );
    
    
    
    bool                        getAfficheInfoFits()						{ return bAffInfoFits; }
    
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

	   void                     setIconized(bool b);
inline bool                     getIconezed()                               { return bIconized; }
inline bool                     isIconized()                                { return bIconized; }

inline void                     setFullScreen(bool b)                       { bFullScreen = b; }
inline bool                     getFullScreen()                             { return bFullScreen; }

inline bool                     isFits()                                    { return bFits; }
inline Fits*                    getFits()                                   { return fits; }
};


#endif
