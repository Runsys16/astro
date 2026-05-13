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
#include "find_star.h"
#include "notification.h"
#include "notification_capture.h"



using namespace std;

class Capture : public PanelWindow, PanelButtonCallback, Notification
{
protected:
	int							ID;
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
    bool						bTraiteReleaseLeft;
    
    sPos						sPosSvg;
    
    vector<string>              filenames;
    string                      filename;
    string                      dirname;
    string                      basename;
    
    bool						bAffGraph;

	FindStar*					pFindStar;
    PanelGraph*					pGraph;
    sPos						sPosGraph;
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
    void						charge_findstar();
	void						charge_graph();
    
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
	void                     	iconize();
	void						restaure();

    void                        on_top();
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
	void						createGraph();
	void						setAffGraph(bool);
	void						deleteGraph();
	bool						cmp(vec2, vec2);
	void						compareStar();
	void						affine_compareStar(bool);

	void						update_info_graph();
	void						create_info_graph();
	void						create_graph();
	void						graph_on_top();
	void						create_find_star();
	
virtual void					cb_button_mouse_up(PanelButton*);
	void						setGraphPosAndSize(int, int, int, int);
	void						save_vars();
	void						delete_vars();
virtual void					notifie( unsigned, void* );
	void						printObjet();

virtual int						getInfo()									{ return ID; }
    
inline bool	 					getAffGraph()                               { return bAfficheGraph; }
inline rb_t *                   getRB()                                     { return &readBgr; }

inline void                     setEchelle(float f)                         { panelCapture->setEchelle(f); }
inline float                    getEchelle()                                { return panelCapture->getEchelle(); }

inline void                     setCentX(float f)                           { panelCapture->setCentX(f); }
inline void                     setCentY(float f)                           { panelCapture->setCentY(f); }
inline float                    getCentX()                                  { return panelCapture->getCentX(); }
inline float                    getCentY()                                  { return panelCapture->getCentY(); }

inline string&                  getFilename()                               { return filename; }
inline string&                  getBasename()                               { return basename; }
inline string&                  getDirname()                                { return dirname; }
inline PanelCapture*            getPanelCapture()							{ return panelCapture; }
inline PanelGraph*            	getGraph()									{ return pGraph; }
inline PanelText*	            getNbVizier()                               { return pNbVizier; }
inline FindStar*	            getFindStar()                               { return pFindStar; }

//	   void                     setIconized(bool b);
inline bool                     getIconized()                               { return bIconized; }
inline void                     setIconized(bool b)                         { bIconized = b; }
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
inline bool						getTraiteReleaseLeft()						{ return bTraiteReleaseLeft; }

inline void						setID( int i )								{ ID = i; }
inline int						getID()										{ return ID; }

};


#endif
