#ifndef PANEL_CAPTURRE_H
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
public:
    PanelCapture(struct readBackground*);
    virtual void		displayGL();
    virtual void        releaseLeft( int, int);

    void                findAllStar();
    bool                starExist(int, int);    
    void                addStar(int,int);

    void                setEchelle(float f);
    void                setCentX(float f);
    void                setCentY(float f);

inline void             setRB(struct readBackground*p)                  { pReadBgr = p; }
inline float            getEchelle()                                    { return echelle; }
inline float            getCentX()                                      { return dx; }
inline float            getCentY()                                      { return dy; }

protected:
    float               echelle;
    float               dx;
    float               dy;
    
struct readBackground*  pReadBgr;
    
    vector<Star*>       v_tStars;
};


#endif
