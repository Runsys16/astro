#ifndef PANEL_ZOOM_H
#define PANEL_ZOOM_H  1

//#include "camera.h"
#include <dirent.h>
#include <algorithm>

#include "main.h"
#include "var_mgr.h"
//#include "capture.h"
#include <WindowsManager.h>


using namespace std;

class PanelZoom : public PanelWindow
{

protected:
        int             xPanel;
        int             yPanel;

        float           echelle;
        float           ratio;
        
        vec2            vStar;
        
        float           xWin;
        float           yWin;
        float           dxWin;
        float           dyWin;
        
        PanelSimple*    pFond;
        float           xFond;
        float           yFond;
        float           dxFond;
        float           dyFond;
        
        float           h;
        float           w;
        
        Panel*          pView;
        
        rb_t*           pReadBgr;
    
public:
        ~PanelZoom();
        PanelZoom();
        
        
virtual void            update();
        void            glCroix(float,float,float);
virtual void	        displayGL();
virtual void            releaseLeft( int, int);

        void            setView(Panel*);
        void            setCentX(float f);
        void            setCentY(float f);
        void            setEchelle(float f);
        void            setRatio(float f);

        void            ajuste();
        void            setPosAndSize(int, int, int, int);
        void            setPos(int, int);
        void            setPosStar(float, float);
        void            active(int, int, Panel*);

        void            setRB(rb_t* p);
        void            setBackground(_Texture2D*);
    
inline  float           getEchelle()            { return echelle; }
inline  float           getCentX()              { return dx; }
inline  float           getCentY()              { return dy; }
        void            setTextWidth(int w );
        void            setTextHeight(int h );
};


#endif
