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
    
    rb_t *              pReadBgr;
    
    Stars               stars;
    Capture*            pCapture;

public:
    PanelCapture( rb_t *, Capture* );
    
    
    virtual void		update();
    virtual void		updatePos();
    virtual void		displayGL();
    virtual void        releaseLeft( int, int);
    virtual void        releaseRight( int, int);

    void                findAllStars();
    void                deleteAllStars();
    //bool                starExist(int, int);    
    void                addStar(int,int);

    void                setCent();
    void                setEchelle(float f);
    void                setCentX(float f);
    void                setCentY(float f);
    
inline void             setRB(struct readBackground*p)                  { pReadBgr = p; }
inline float            getEchelle()                                    { return ech_user; }
inline float            getCentX()                                      { return dx; }
inline float            getCentY()                                      { return dy; }
inline Stars*           getStars()                                      { return &stars; }
inline void             setIcone(bool b)                                { bIcone = b; }
inline bool             getIcone()                                      { return bIcone; }

};


#endif
