#ifndef PANEL_CAMERA_H
#define PANEL_CAMERA_H  1

//#include "camera.h"
#include <dirent.h>
#include <algorithm>

#include "main.h"
#include "var_mgr.h"
#include "stars.h"
//#include "capture.h"
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


public:
    PanelCamera();
    
    virtual void		update();
            void		displaySuivi();
            void		displayCentre();
    virtual void		displayGL();
    virtual void        releaseLeft( int, int);
    virtual void        releaseMiddle( int, int);

    virtual void        wheelUp( int, int)                              {;};
    virtual void        wheelDown( int, int)                            {;};

    void                tex2screen(int&,int&);
    void                tex2screen(float&,float&);

    void                screen2tex(int&,int&);
    void                screen2tex(float&,float&);

    void                glCercle(int x, int y, int rayon);
    void                glCarre( int x,  int y,  int dx,  int dy );
    void                glCroix( int x,  int y,  int dx,  int dy );

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

};


#endif
