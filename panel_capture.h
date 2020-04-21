#ifndef PANEL_CAPTURE_H
#define PANEL_CAPTURE_H  1

#include "camera.h"
#include <dirent.h>
#include <algorithm>

#include <WindowsManager.h>
#include "main.h"
#include "var_mgr.h"
#include "stars.h"
//#include "capture.h"

class Capture;
using namespace std;

class PanelCapture : public PanelSimple
{
protected:
    float               ech_geo;
    float               ech_user;
    float               dx;
    float               dy;
    bool                bIcone;
    bool                bHaveMove;
    
    int                 xm_old;
    int                 ym_old;
    
    rb_t *              pReadBgr;
    
    Stars               stars;
    Capture*            pCapture;
    
public:
    PanelCapture( rb_t *, Capture* );
    ~PanelCapture();    
    
    virtual void		update();
    virtual void		updatePos();
    virtual void		displayGL();

    //virtual Panel*      isMouseOver( int, int);

    virtual void        clickLeft( int, int);
    virtual void        releaseLeft( int, int);

    virtual void        clickRight( int, int);
    virtual void        releaseRight( int, int);

    virtual void        clickMiddle( int, int);
    virtual void        motionMiddle( int, int);
    virtual void        releaseMiddle( int, int);

    virtual void        wheelUp( int, int);
    virtual void        wheelDown( int, int);

    void                findAllStars();
    void                deleteAllStars();
    //bool                starExist(int, int);    
    void                addStar(int,int);
    void                clip(int&, int&);

    void                setCent();
    void                setEchelle(float f);
    void                setCentX(float f);
    void                setCentY(float f);
    
    int                 screen2texX( int );
    int                 screen2texY( int );
    void                screen2tex( int&, int& );

    int                 tex2screenX( int );
    int                 tex2screenY( int );
    void                tex2screen( int&, int& );
    
    void                printObjet();
    
inline void             setRB(struct readBackground*p)                  { pReadBgr = p; }
inline float            getEchelle()                                    { return ech_user; }
inline float            getCentX()                                      { return dx; }
inline float            getCentY()                                      { return dy; }
inline Stars*           getStars()                                      { return &stars; }
inline void             setIcone(bool b)                                { bIcone = b; }
inline bool             getIcone()                                      { return bIcone; }

};


#endif
