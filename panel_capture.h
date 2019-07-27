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


using namespace std;

class PanelCapture : public PanelSimple
{
protected:
    float               echelle;
    float               dx;
    float               dy;
    
    rb_t *              pReadBgr;
    
    Stars               stars;

public:
    PanelCapture( rb_t * );
    
    
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
inline float            getEchelle()                                    { return echelle; }
inline float            getCentX()                                      { return dx; }
inline float            getCentY()                                      { return dy; }
inline Stars*           getStars()                                      { return &stars; }

};


#endif
