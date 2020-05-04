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

        vec2            vCamView;
        float           ech;
        
        PanelSimple*    pFond;
        float           xFond;
        float           yFond;
        float           dxFond;
        float           dyFond;
        
        float           h;
        float           w;
        
        Panel*          pView;
        
        rb_t*           pReadBgr;
        
        bool            bFreePos = false;
        vec2            vClickRight;
    
public:
        ~PanelZoom();
        PanelZoom();
        
        
virtual void            update();
        void            glCroix(float,float,float,vec4);
virtual void	        displayGL();

virtual void            releaseLeft( int, int);

virtual void            clickRight( int, int);
virtual void            motionRight( int, int);
virtual void            releaseRight( int, int);

        void            setView(Panel*);
        void            setCentX(float f);
        void            setCentY(float f);
        void            setEchelle(float f);
        void            setRatio(float f);

        void            setPosAndSize(int, int, int, int);
        void            setPos(int, int);
        void            setPosStar(float, float);

        void            setRB(rb_t* p);
        void            setBackground(_Texture2D*);
		void		    setBackground( GLubyte*, unsigned, unsigned, unsigned );
    
        void            setTextWidth(int w );
        void            setTextHeight(int h );

inline  float           getEchelle()                    { return echelle; }
inline  float           getCentX()                      { return dx; }
inline  float           getCentY()                      { return dy; }
inline  void            setCamView(float x, float y, float e)    
                        { vCamView.x = x; vCamView.y = y; ech = e; }

};


#endif
