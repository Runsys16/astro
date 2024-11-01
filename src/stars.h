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

inline bool					getVisible()				{ return bVisible; }
inline Star*				get(int i)					{ return v_tStars[i]; }
inline int					size()						{ return v_tStars.size(); }

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
};


#endif
