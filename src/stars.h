#ifndef STARS_H
#define STARS_H  1

#include "main.h"
#include "MathlibD.h"
#include "star.h"

#include <WindowsManager.h>
#include <thread>


using namespace std;

class Stars
{
public:
    Stars();
    ~Stars();
    
    void                    add( Star*);
    void                    sup( Star*);

    Star*                   addStar( int, int, int, int, float );
    bool                    starExist(int, int);
    bool                    starExist(int, int, int);

    void                    findAllStars();
    void                    deleteAllStars();
    
    void                    setView( PanelSimple* );
    void                    setPanelNbStars( PanelText * );

    void                    setRB(rb_t*);
    void                    updateRB(rb_t*);
    vec2*                   getSuivi();

	void					setModeMag(int n);
    void                    update_stars(int, int, PanelSimple*, rb_t*);
    void                    update_stars(int, int, PanelSimple*, rb_t*, float ech_usr);
    
    void					updateScreenPos(int, int, double);
    void                    idle();
    void                    displayGL();
    
    void                    selectLeft(int, int);
    void                    selectMiddle(int, int);
    void                    selectStar(int, int);
    void                    suivi(rb_t*);
    
    void                    position(double, double);
    void                    set_delta(float, float);

    int                     getSuiviScreenX();
    int                     getSuiviScreenY();
	void					setVisible(bool);

	void					affiche_position();

	void					setA(double d);
	void					setB(double d);
	void					setC(double d);
	
	void					setDelta(double);
	void					compute_magnitude();

inline bool					getVisible()				{ return bVisible; }
inline Star*				get(int i)					{ return v_tStars[i]; }
inline int					size()						{ return v_tStars.size(); }
inline PanelText*			getPanelNbStars()			{ return pNbStars; }

inline double*				getvA()						{ return &dCoefA; }
inline double*				getvB()						{ return &dCoefB; }
inline double*				getvC()						{ return &dCoefC; }

inline double				getA()						{ return dCoefA; }
inline double				getB()						{ return dCoefB; }
inline double				getC()						{ printf("%lf\n",dCoefC); return dCoefC; }

private:
    vector<Star*>           v_tStars;
    rb_t*                   RB;
    PanelSimple*            pView;
    PanelText*              pNbStars;
    
    int                     dx;
    int                     dy;
    
    float                   ech;
	bool					bVisible;
	bool					bAffPosition;

    int						uModeMag;
    
    double					dCoefA;
    double					dCoefB;
    double					dCoefC;
};


#endif
