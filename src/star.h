#ifndef STAR_H
#define STAR_H  1

#include "main.h"
#include "MathlibD.h"

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
    void                setRBzoom();
    void                setRB(rb_t*);
    void                setView(PanelSimple*);

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

    void                set_delta(float, float);
    //void                screen2tex(int&, int&, int, int, int, int);
    
    void                updatePos(int, int, float, float);
    void                updatePos(int, int);
    void                suivi();


    void                displayGL();
    
    void                setZoom(bool);
    void                position(double, double);
    void				setVisible(bool);
    
    void				affiche_position(bool);
    
    inline void         getPos(vec2* v)                                 { v->x=pos.x; v->y=pos.y; }
    inline void         getPos(vec2& v)                                 { v.x=pos.x; v.y=pos.y; }
    inline void         getSuivi(vec2& v)                               { v.x=xSuivi; v.y=ySuivi; }
    inline float        getSuiviX()                                     { return xSuivi; }
    inline float        getSuiviY()                                     { return ySuivi; }

    inline bool         getSuivi()                                      { return bSuivi; }
    inline void         toggleSuivi()                                   { bSuivi = !bSuivi; }
    inline void         setSuivi(bool b)                                { bSuivi = b; }

    inline bool         getSelect()                                     { return bSelect; }
    inline void         toggleSelect()                                  { bSelect = !bSelect; }
    inline void         setSelect(bool b)                               { bSelect = b; }

    inline bool         getZoom()                                       { return bZoom; }
    inline void         toggleZoom()                                    { bZoom = !bZoom; }

    inline int          getX()                                          { return x; }
    inline int          getY()                                          { return y; }
    inline int          getXScreen()                                    { return x_screen; }
    inline int          getYScreen()                                    { return y_screen; }
    inline void         setXY( int xx, int yy)                          { x = xx; y = yy; pInfo->setPos(xx,yy); }
    inline float        getMagnitude()                                  { return magnitude; }
    inline PanelText*   getInfo()                                       { return pInfo; }
    inline void         select()                                        { bSelect = !bSelect; }
    
    inline bool         haveCoord()                                     { return ad != 9999.0; }
    inline double       getAD()                                         { return ad; }
    inline double       getDE()                                         { return de; }

    inline void         setAD(double d)                                 { ad = d; }
    inline void         setDE(double d)                                 { de = d; }

    inline int          getNotFound()                                   { return nbNotFound; }
    inline void         resetNotFound()                                 { nbNotFound = 0; }

    
private:
    vec2                pos;
    
    GLubyte*            ptr;
    rb_t*               RB;
    int                 width;
    int                 height;
    int                 bitplane;
    
    int                 xFound;
    int                 yFound;
    int                 nbNotFound;

    int                 x;
    int                 y;

    float               xSuivi;
    float               ySuivi;

    float               dx_screen;
    float               dy_screen;
    
    int                 x_screen;
    int                 y_screen;

    vec2                vFWHM;
    float               ponderation;
    
    float               magnitude;
    float               maxLum;
    float               limitLum;
    float               ech_x;
    float               ech_y;
    double              ad;
    double              de;
    

    PanelText *         pInfo;
    char                p_sInfo[225];
    
    bool                bSelect;
    bool                bSuivi;
    bool                bZoom;
    bool				bVisible;
    
    PanelZoom*          panelZoom;
    PanelSimple*        pView;
};


#endif
