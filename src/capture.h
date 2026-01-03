#ifndef CAPTURE_H
#define CAPTURE_H  1

#include "camera.h"
#include <dirent.h>
#include <algorithm>

#include <WindowsManager.h>
#include "main.h"
#include "panel_capture.h"
#include "panel_debug.h"
#include "fits.h"
#include "var_mgr.h"
#include "catalog.h"


using namespace std;

class Capture : public PanelWindow
{
protected:
    bool                        bNewBackground;
    bool                        bFirst;
    bool                        bIconized;
    bool                        bFullScreen;
    bool                        bFits;
    bool						bAfficheInfoFits;
    bool						bAfficheInfoSouris;
    bool						bAfficheGrille;
    bool						bAfficheCorrectionFits;
    bool						bAfficheGraph;
    
    sPanelPos					sPosSvg;
    
    vector<string>              filenames;
    string                      filename;
    string                      dirname;
    string                      basename;
    
    PanelGraph*					pGraph;
    PanelDebug*					pInfoGraph;
    PanelCapture*               panelCapture;
    PanelText*                  pTitre;
    PanelText*                  pNbStars;
    PanelText*                  pNbVizier;

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
    
    void						init();
    void						charge( string, string );
    
    void                        pooling();
    
    virtual void                update();
    virtual void                updatePos();
    		void                updatePosIcones();

    virtual void                clickLeft( int, int);
    virtual void                releaseLeft( int, int);

    virtual void                callback(void *p);

    void                        create_icones();
    void                        create_preview();
    
    void                        resize(int,int);
    void                        resize(int,int,int,int);
    void                        fullscreen();
    void						iconize(int, int, int, int);
	void						restaure();

    void                        onTop();
    void                        addStar(int,int);

    void                        show();
    void                        hide();
    
    void                        setColor(long);

    void                        afficheFitsDic();
    void                        afficheFits();
    void                        afficheInfoFits();
    void                        afficheInfoFits(bool);

	void						export_stars();
	void						export_vizier();
	
	void						setNbVizier(unsigned);
	void						setAffGraph(bool);
	bool						cmp(vec2, vec2);
	void						compareStar();

	void						update_info_graph();
	void						create_info_graph();
	void						create_graph();

    
inline bool	 					getAffGraph()                               { return bAfficheGraph; }
inline rb_t *                   getRB()                                     { return &readBgr; }

inline void                     setEchelle(float f)                         { panelCapture->setEchelle(f); }
inline float                    getEchelle()                                { return panelCapture->getEchelle(); }

inline void                     setCentX(float f)                           { panelCapture->setCentX(f); }
inline void                     setCentY(float f)                           { panelCapture->setCentY(f); }
inline float                    getCentX()                                  { return panelCapture->getCentX(); }
inline float                    getCentY()                                  { return panelCapture->getCentY(); }

inline string                   getFilename()                               { return filename; }
inline string                   getBasename()                               { return basename; }
inline string                   getDirname()                                { return dirname; }
inline PanelCapture*            getPreview()                                { return panelCapture; }
inline PanelGraph*            	getGraph()									{ return pGraph; }
inline PanelText*	            getNbVizier()                               { return pNbVizier; }

//	   void                     setIconized(bool b);
inline bool                     getIconezed()                               { return bIconized; }
inline bool                     isIconized()                                { return bIconized; }

inline void                     setFullScreen(bool b)                       { bFullScreen = b; }
inline bool                     getFullScreen()                             { return bFullScreen; }

inline bool                     isFits()                                    { return bFits; }
inline Fits*                    getFits()                                   { return fits; }

inline void						setAfficheGrille(bool b)					{ bAfficheGrille = b; }
inline void						setAfficheInfoSouris(bool b)				{ bAfficheInfoSouris = b; }
inline void						setAfficheInfoFits(bool b)					{ bAfficheInfoFits = b; }

inline bool						getAfficheGrille()							{ return bAfficheGrille; }
inline bool						getAfficheInfoSouris()						{ return bAfficheInfoSouris; }
inline bool						getAfficheInfoFits()						{ return bAfficheInfoFits; }
inline void						invalide_panel()							{ sPosSvg.X = -1; }
};


#endif
