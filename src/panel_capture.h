#ifndef PANEL_CAPTURE_H
#define PANEL_CAPTURE_H  1

#include "camera.h"
#include <dirent.h>
#include <algorithm>

#include <WindowsManager.h>
#include "main.h"
#include "var_mgr.h"
#include "stars.h"
#include "catalog.h"


class Capture;
class Captures;
class Fits;

using namespace std;

class PanelCapture : public PanelSimple
{
protected:
	double				dTelescopeAD;
	double				dTelescopeDC;
	int					xTelescope;
	int					yTelescope;
	double				ech;
    double              ech_geo;
    double              ech_user;
    double              dx;
    double              dy;
    bool                bIcone;
    bool                bHaveMove;
    bool                bInfoSouris;
    bool				bAffGrille;
    
    int                 xm_old;
    int                 ym_old;
    
    rb_t *              pReadBgr;
    
    Stars               stars;
    Catalog*            pVizier;
    Capture*            pCapture;
    
    PanelText*			pCoord;
    PanelText*			pJ2000_1;
    PanelText*			pJ2000_2;
    
    vector<vec2>		p1;
    vector<vec2>		p2;
    vector<PanelText*>	pTextEch;
    vec2				vAD;
    vec2				vDE;
    double				dAngleAD;
    double				dAngleDE;
    
public:
    PanelCapture( rb_t *, Capture* );
    ~PanelCapture();    
    
    
    virtual void		update_stars();
    		void		updateEchelle();
    		void		updateEchelleGeo();
    virtual void		updatePos();

			void		glCroix( int,  int,  int,  int );
    		void		glCercle(int, int, int);
    		void		displayTelescope();
    		void		displayCatalog();
    		void		displayAxe();
    virtual void		displayGL();

    virtual void        wheelUp( int, int);
    virtual void        wheelDown( int, int);
    virtual void		passiveMotionFunc(int, int);

    virtual void        clickLeft( int, int);
    virtual void        releaseLeft( int, int);

    virtual void        clickRight( int, int);
    virtual void        motionRight( int, int);
    virtual void        releaseRight( int, int);

    virtual void        clickMiddle( int, int);
    virtual void        motionMiddle( int, int);
    virtual void        releaseMiddle( int, int);

    void                findAllStars();
    void                deleteAllStars();
    //bool                starExist(int, int);    
    void                addStar(int,int);
    void                clip(int&, int&);

    void                setCent();
    void                setEchelle(double f);
    void                setCentX(double f);
    void                setCentY(double f);
    
    int                 screen2texX( int );
    int                 screen2texY( int );
    void                screen2tex( int&, int& );
    void                screen2tex( vec2& );

    int                 tex2screenX( int );
    int                 tex2screenY( int );
    void                tex2screen( int&, int& );
    void                tex2screen( vec2& );
    
    void                printObjet();
    void				findGaiaDR3();
    void				sendStellarium( int, int );
    
	void             	setInfoSouris(bool);
	void				iconize();
	void				restaure(bool bGril, bool bbSour );

	void				change_ad( double );
	void				change_dc( double );

	void				setEchelleVisible(bool);
	void				computeIntersectionHau(vec2&, vec2, vec2);
	void				computeIntersectionBas(vec2&, vec2, vec2);
	void				computeIntersectionGau(vec2&, vec2, vec2);
	void				computeEchelle();
	void             	addP1P2(vec2 , vec2);

inline void             setRB(struct readBackground*p)                  { pReadBgr = p; }
inline double           getEchelle()                                    { return ech_user; }
inline double           getCentX()                                      { return dx; }
inline double           getCentY()                                      { return dy; }
inline Stars*           getStars()                                      { return &stars; }
inline void             setIcone(bool b)                                { bIcone = b; }
inline bool             getIcone()                                      { return bIcone; }
inline void             setAffGrille(bool b)                           	{ bAffGrille = b; }
inline bool             getAffGrille()                           		{ return bAffGrille; }
inline bool             getInfoSouris()                                 { return bInfoSouris; }
inline void             setVecteurAD(vec2 v)                            { vAD = v; }
inline void             setVecteurDE(vec2 v)                            { vDE = v; }

};


#endif
