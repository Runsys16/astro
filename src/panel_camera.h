#ifndef PANEL_CAMERA_H
#define PANEL_CAMERA_H  1

//#include "camera.h"
#include <dirent.h>
#include <algorithm>

#include "main.h"
#include "var_mgr.h"
#include "stars.h"
#include "catalog.h"
#include <WindowsManager.h>
#include "convert.h"

class Camera;
class Camera_mgr;

using namespace std;


class PanelCamera : public PanelWindow, public Convert
{

protected:
    double              echelle;
    double				ech_user;
    double				ech_geo;
    double              _x;
    double              _y;
    double              _dx;
    double              _dy;
    
    rb_t*				pReadBgr;
    
    Stars               stars;
    double              fTime;
    double              fSens;
    double              fTimeClign;
    double              fTime1;
    bool                bTime1;
    
    int                 x_old;
    int                 y_old;
    int                 dx_old;
    int                 dy_old;
    
    double              fRefCatalogX;
    double              fRefCatalogY;
    int                 fRefCatalogDecalX;
    int                 fRefCatalogDecalY;
    ivec2				vDeplaceDepuis;
    
    Camera*				pCamera;

    //vector<StarCatalog*>      catalog;

public:
    PanelCamera(Camera*);

    virtual void		idle(float);
    virtual void		update_stars();
    virtual void        releaseLeft( int, int);
    virtual void        releaseMiddle( int, int);
    virtual void        motionRight( int, int);
    virtual void        wheelUp( int, int);//                              {;};
    virtual void        wheelDown( int, int);//                            {;};
	//-----------------------------------------------------------------
    virtual void		updatePos();

	void				clip(int&, int&);
	void				compute_centre();
    void                compute_echelle();
    void				computeColor();
/*	//-----------------------------------------------------------------
    void                tex2screen(vec2&);


*/	//-----------------------------------------------------------------
    void                tex2screen(int&,int&);
    void                tex2screen(double&,double&);
    void                screen2tex(int&,int&);
    void                screen2tex(double&,double&);

/*
    void                screen2tex( vec2& );
    void                screen2panel( vec2& );
    void                tex2screen( vec2& );
    void                tex2panel( vec2& );
    void                panel2tex( vec2& );
    void                panel2screen( vec2& );
*/    
virtual void			screen_2_tex( vec2& );
virtual void			screen_2_panel( vec2& );
virtual void			tex_2_screen( vec2& );
virtual void			tex_2_panel( vec2& );
virtual void			panel_2_tex( vec2& );
virtual void			panel_2_screen( vec2& );
    
	//-----------------------------------------------------------------
    void                glCercle(int x, int y, int rayon);
    inline void         glCercle(vec2 v, int r)							{ glCercle( (int)v.x, (int)v.y, r ); }

    void                glCarre( int x,  int y,  int dx,  int dy );

    void                glCroix( int x,  int y,  int dx,  int dy );
    inline void         glCroix( int x,  int y,  int dxy )				{ glCroix( x, y, dxy, dxy ); }
    inline void         glCroix(vec2 v, int dxy)						{ glCroix( v.x, v.y, dxy, dxy ); }

    void                displayGLTrace(void);
    void                displayVecAD();
    void                displayVecDC();

    void				displayLigneSuivi();
    void				displaySuivi();
    void				displayCentre();
    void				displayVizier();

    virtual void		displayGL();
	//-----------------------------------------------------------------
    void                findAllStar();
    bool                starExist(int, int);    
    void                addStar(int,int);

    void                setRB(rb_t* p);
    vec2*               getSuivi();
    void                add_catalogue(StarCatalog*);
    void                setRefCatalog(double _0, double _1);
    void				updateVizizePos();
	//-----------------------------------------------------------------
	void				recentreSuivi();   
	//-----------------------------------------------------------------
inline void				raz_ech_usr()                                   { ech_user = 1.0; }
inline double           getEchelle()                                    { return echelle; }
inline double           getCentX()                                      { return dx; }
inline double           getCentY()                                      { return dy; }
inline Stars*           getStars()                                      { return &stars; }
inline rb_t*            getRB()                                         { return pReadBgr; }
inline int              getNbStars()                                    { return stars.size(); }

};

#endif
