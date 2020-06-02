#ifndef PANEL_CAMERA_H
#define PANEL_CAMERA_H  1

//#include "camera.h"
#include <dirent.h>
#include <algorithm>

#include "main.h"
#include "var_mgr.h"
#include "stars.h"
#include "star_catalogue.h"
#include <WindowsManager.h>

using namespace std;

class PanelCamera : public PanelWindow
{

protected:
    float               echelle;
    float               dx;
    float               dy;
    
    rb_t*               pReadBgr;
    
    Stars               stars;
    float               fTime;
    float               fSens;
    float               fTimeClign;
    float               fTime1;
    bool                bTime1;
    
    int                 x_old;
    int                 y_old;
    int                 dx_old;
    int                 dy_old;

    vector<star_catalogue*>      catalog;

public:
    PanelCamera();
    
    virtual void		idle(float);
    virtual void		update_stars();
            void		displaySuivi();
            void		displayCentre();
    virtual void		displayGL();
    virtual void		updatePos();
    virtual void        releaseLeft( int, int);
    virtual void        releaseMiddle( int, int);

    virtual void        wheelUp( int, int)                              {;};
    virtual void        wheelDown( int, int)                            {;};

    void                compute_echelle();
    void                tex2screen(vec2&);
    void                tex2screen(int&,int&);
    void                tex2screen(float&,float&);

    void                screen2tex(int&,int&);
    void                screen2tex(float&,float&);

    void                glCercle(int x, int y, int rayon);
    inline void         glCercle(vec2 v, int r)
                        { glCercle( (int)v.x, (int)v.y, r ); }
    void                glCarre( int x,  int y,  int dx,  int dy );
    void                glCroix( int x,  int y,  int dx,  int dy );
    inline void         glCroix( int x,  int y,  int dxy )
                        { glCroix( x, y, dxy, dxy ); }
    inline void         glCroix(vec2 v, int dxy)
                        { glCroix( v.x, v.y, dxy, dxy ); }

    void                displayGLTrace(void);

    void                glVecAD();
    void                glVecDC();

    void                findAllStar();
    bool                starExist(int, int);    
    void                addStar(int,int);

    void                setEchelle(float f);
    void                setCentX(float f);
    void                setCentY(float f);

    void                setRB(rb_t* p);
    vec2*               getSuivi();
    
inline float            getEchelle()                                    { return echelle; }
inline float            getCentX()                                      { return dx; }
inline float            getCentY()                                      { return dy; }
inline Stars*           getStars()                                      { return &stars; }
inline rb_t*            getRB()                                         { return pReadBgr; }
inline int              getNbStars()                                    { return stars.size(); }

inline void             add_catalogue(star_catalogue* p)                { catalog.push_back(p); }
};


#endif
