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
#include "stars.h"


class Capture;
class Captures;
class Fits;

using namespace std;

class PanelCapture : public PanelSimple
{
protected:
	vec2				vTelescopeScreen;
	vec2				vTelescopeTex;
	vec2				vTelescopeJ2000;
	vec2				vTelescopePanel;

	double				ech;
    double              ech_geo;
    double              ech_user;
    double              dx;
    double              dy;

    bool                bIcone;
    bool                bHaveMove;
    bool                bInfoSouris;
    bool				bAffGrille;
    bool				bAffCatalogPosition;
    
    int                 xm_old;
    int                 ym_old;
    
    rb_t *              pReadBgr;
    
    Stars               stars;
    Catalog*            pVizier;
    Capture*            pCapture;
    
    PanelSimple*		pTelescope;
    PanelSimple*		pFondCoord;
    PanelText*			pColor;
    PanelText*			pCoord;
    PanelText*			pJ2000_1;
    PanelText*			pJ2000_2;
    PanelText*			pJ2000_alpha;
    PanelText*			pJ2000_delta;
    
    vector<vec2>		p1;
    vector<vec2>		p2;
    vector<PanelText*>	pTextEch;
    vec2				vAD;
    vec2				vDE;
    double				dAngleAD;
    double				dAngleDE;
    double				dTimeAnim;
public:
    PanelCapture( rb_t *, Capture* );
    ~PanelCapture();    
    
    
    virtual void		update_stars();
    		void		updateEchelle();
    		void		updateEchelleGeo();
    virtual void		updatePos();

    		void		glCercleAnimation(int, int, int, int, int);
			void		glCroix( int,  int,  int,  int );
    		void		glCercle(int, int, int);
    		void		displayTelescope();
    		void		displayCatalog();
    		void		displayAxe();
    virtual void		displayGL();

    virtual void		idle(float);

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
    void                addStar(int,int);
    void                clip(int&, int&);

    void                setCent();
    void                setEchelle(double f);
    void                setCentX(double f);
    void                setCentY(double f);

    void                screen2tex( vec2& );
    void                tex2screen( vec2& );
    void                tex2panel( vec2& );
    void                panel2tex( vec2& );
    void                screen2panel( vec2& );
    void                panel2screen( vec2& );

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
	void				computeEchAD_00(int, vec2);
	void				computeEchDE_00(int, vec2);
	void				computeRepere_00();
	void				computeRepereAxe_01(int);
	void				computeRepere_01();
	void				computeRepere_02();
	void				computeRepere();
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
inline Catalog*			getCatalog()									{ return pVizier; }

};


#endif
