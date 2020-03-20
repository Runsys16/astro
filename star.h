#ifndef STAR_H
#define STAR_H  1

#include "main.h"
#include "Mathlib.h"

#include <WindowsManager.h>
#include <thread>

#include "panel_zoom.h"


using namespace std;

class Star
{
public:
    ~Star();
    Star();
    Star(int, int);
    void                init(int,int);

    void                setPtr(GLubyte*);
    void                setWidth(int);
    void                setHeight(int);
    void                setRB(rb_t*);
    void                setView(Panel*);

    int                 getOffset(int,int);
    void                computeMag();
    float               computeRayon();

    float               getLumLimit(int offset, float limit );
    float               getLum(int offset );
    float               getLum(int X, int Y, int limit );

    bool                cherche(int X, int Y, int size);
    bool                cherche(int X, int Y, int size, float lum);
    bool                chercheLum(int X, int Y, int size);
    bool                chercheLum(int X, int Y, int size, int limit);

    bool                find(int);
    bool                find();
    
    
    float               getColorR(int);
    float               getColorG(int);
    float               getColorB(int);

    void                getFWHMLine( int x, int y, int size);
    void                getFWHM( int x, int y, int size);

    void                glCercle(int rayon);
    void                glCarre(int dx,  int dy );
    void                glCarre(int dx);
    void                glMark(int dx,  int dy );
    void                glCroix(int dx,  int dy );

    void                screen2tex(int&, int&);
    void                screen2tex(int&, int&, int, int, int, int);
    void                updatePos(int, int, float);
    void                updatePos(int, int);
    void                suivi();


    void                displayGL();
    
    void                setZoom(bool);
    
    void                position(double, double);
    
    inline bool         getPos(vec2* v)                                 { v->x=pos.x; v->y=pos.y; }
    inline bool         getPos(vec2& v)                                 { v.x=pos.x; v.y=pos.y; }
    inline bool         getSuivi(vec2& v)                               { v.x=xSuivi; v.y=ySuivi; }
    inline float        getSuiviX()                                     { xSuivi; }
    inline float        getSuiviY()                                     { ySuivi; }

    inline bool         getSuivi()                                      { return bSuivi; }
    inline void         toggleSuivi()                                   { bSuivi = !bSuivi; }
    inline void         setSuivi(bool b)                                { bSuivi = b; }

    inline bool         getZoom()                                       { return bZoom; }
    inline void         toggleZoom()                                    { bZoom = !bZoom; }

    inline int          getX()                                          { return x; }
    inline int          getY()                                          { return y; }
    inline void         setXY( int xx, int yy)                          { x = xx; y = yy; pInfo->setPos(xx,yy); }
    inline float        getMagnitude()                                  { return magnitude; }
    inline PanelText*   getInfo()                                       { return pInfo; }
    inline void         select()                                        { bSelect = !bSelect; }
    
    inline bool         haveCoord()                                      { return ra_rad != 9999.0; }
    inline double       getRA()                                          { return ra_rad; }
    inline double       getDC()                                          { return dc_rad; }
    
private:
    vec2                pos;
    
    int                 width;
    int                 height;
    
    int                 xFound;
    int                 yFound;

    int                 x;
    int                 y;

    float               xSuivi;
    float               ySuivi;

    int                 dx_screen;
    int                 dy_screen;
    int                 x_screen;
    int                 y_screen;

    vec2                vFWHM;
    float               ponderation;
    
    float               magnitude;
    float               maxLum;
    float               limitLum;
    float               ech;
    double              ra_rad;
    double              dc_rad;
    
    GLubyte*            ptr;
    rb_t*               RB;

    PanelText *         pInfo;
    char                p_sInfo[225];
    
    bool                bSelect;
    bool                bSuivi;
    bool                bZoom;
    
    PanelZoom*          panelZoom;
    Panel*              pView;
};


#endif
