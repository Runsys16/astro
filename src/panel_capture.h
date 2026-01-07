#ifndef PANEL_CAPTURE_H
#define PANEL_CAPTURE_H  1

#include "camera.h"
#include <dirent.h>
#include <algorithm>
//----------------------------------------------------------------------------------------------
#include "MathlibD.h"
#include <WindowsManager.h>
#include "main.h"
#include "var_mgr.h"
#include "stars.h"
#include "catalog.h"
#include "stars.h"
#include "star_compare.h"
#include "timer.h"
#include "panel_graph.h"
#include "panel_debug.h"
//----------------------------------------------------------------------------------------------
class Capture;
class Captures;
class Fits;
//----------------------------------------------------------------------------------------------
using namespace std;
//----------------------------------------------------------------------------------------------
enum pin_mode 
{
    HAUT,
    DROITE,
    BAS,
    GAUCHE
};
//----------------------------------------------------------------------------------------------
struct coord_line
{
    vec2		p1;
    vec2		p2;
    pin_mode	pin;
    PanelText*	pTextCoord;
    bool		bContinue;
    bool		bBreak;
};
//----------------------------------------------------------------------------------------------
class PanelCapture : public PanelSimple
{

protected:
	vec2				vHG;
	vec2				vHD;
	vec2				vBG;
	vec2				vBD;

	vec2				vTelescopeScreen;
	vec2				vTelescopeTex;
	vec2				vTelescopeJ2000;
	vec2				vTelescopePanel;

	double				ech;
    double              ech_geo;
    double              ech_user;
    double              dx;
    double              dy;

    //bool                bIcone;
    bool                bHaveMove;
    //bool                bInfoSouris;
    bool				bAffCatalogPosition;
    
    int                 xm_old;
    int                 ym_old;
    
    rb_t *              pReadBgr;
    
    Capture*            pCapture;

    StarCompare			starCompare;
    Stars               stars;
    Catalog*            pVizier;
    int					idxVizierMouseOver;
    
    PanelDebug*			pInfoVizier;
    PanelSimple*		pTelescope;
    PanelWindow*		pFondCoord;
    PanelText*			pColor;
    PanelText*			pCoord;
    PanelText*			pJ2000_1;
    PanelText*			pJ2000_2;
    PanelText*			pJ2000_alpha;
    PanelText*			pJ2000_delta;
    
    vector<coord_line>	tAD;
    vector<coord_line>	tDE;
    
    vec2				vAD;
    vec2				vDE;
    double				dAngleAD;
    double				dAngleDE;
    double				dTimeAnim;
    
    double				dDebug5s;
    int					nbNonAff;
    bool				bFits;
    int					maxAD;
    int					maxDC;

public:
    PanelCapture( rb_t *, Capture* );
    ~PanelCapture();    
    		void		init();
    
    virtual void		update_stars();
    		void		updateEchelle();
    		void		updateEchelleGeo();
    		
    		void		updatePosInfoVizier();
    		void		updateInfoVizier();
    		void		updateVizier();
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
    		void		updatePosFondCoord();
    		
    		void		find_star_mouse_over();
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

	void				compareStar();
	void				saveCompareStar();


    void                setCentre();
    void                setEchelle(double f);
    void                setCentX(double f);
    void                setCentY(double f);

    void                screen_2_tex( vec2& );
    void                screen_2_panel( vec2& );
    void                tex_2_screen( vec2& );
    void                tex_2_panel( vec2& );
    void                panel_2_tex( vec2& );
    void                panel_2_screen( vec2& );
    void				parent_2_J2000( vec2& );
	void				parent_2_str_ad_str_dc(vec2&, char*, char*, int );
	char*				J2000_2_str_ad(vec2&, char*, int );
	char*				J2000_2_str_dc(vec2&, char*, int );
	void				J2000_2_str_ad_str_dc(vec2&, char*, char*, int );
    void				J2000_2_parent( vec2& );

	void				print_echelle_coordonnees();
    void                printObjet();
    void				findGaiaDR3();
    void				findGaiaDR3_end();
    void				eraseGaiaDR3();
    void				sendStellarium( int, int );
    
	void             	setInfoSouris(bool);
	void				iconize();
	void				restaure();

	void				change_ad( double );
	void				change_dc( double );

	void				setEchelleVisible(bool);
	void				clear_AD_DE();
	char*				pin_mode_2_str( pin_mode );
	void				compute_points( pin_mode, struct coord_line&, double );
	void				compute_lines( bool, pin_mode, double, double );
	void				computeRepere_03();
	void				computeRepere();
	
	double				compute_div(double);
	void				compute_angle();

inline void             setRB(struct readBackground*p)                  { pReadBgr = p; }
inline int				getDX()											{ return pReadBgr->w; }
inline int				getDY()											{ return pReadBgr->h; }
inline double           getEchelle()                                    { return ech_user; }
inline double           getCentX()                                      { return dx; }
inline double           getCentY()                                      { return dy; }
inline Stars*           getStars()                                      { return &stars; }
inline void             setVecteurAD(vec2 v)                            { vAD = v; }
inline void             setVecteurDE(vec2 v)                            { vDE = v; }
inline Catalog*			getCatalog()									{ return pVizier; }
inline Capture*			getCapture()									{ return pCapture; }
inline StarCompare&		getStarCompare()								{ return starCompare; }
//inline void				tex_2_J2000(vec2& v)							{ pCapture->getFits()->tex_2_J2000(v); }
//inline void				J2000_2_tex(vec2& v)							{ pCapture->getFits()->J2000_2_tex(v); }
//inline string&			getFilename()									{ if (pCapture) return pCapture->getBasename(); else return ""; }

};


#endif
