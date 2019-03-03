#ifndef PANEL_CAPTURRE_H
#define PANEL_CAPTURE_H  1

#include "camera.h"
#include <dirent.h>
#include <algorithm>

#include <WindowsManager.h>
#include "main.h"



using namespace std;

class PanelCapture : public PanelSimple
{
public:
    PanelCapture();
    virtual void		displayGL();
    
inline void             setEchelle(float f)                         { echelle = f; }
inline float            getEchelle()                                { return echelle; }

inline void             setDX(float f)                              { dx = f; }
inline void             setDY(float f)                              { dy = f; }
inline float            getDX()                                     { return dx; }
inline float            getDY()                                     { return dy; }

protected:
    float               echelle;
    float               dx;
    float               dy;
};


#endif
