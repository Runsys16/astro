#ifndef STARS_H
#define STARS_H  1

#include "main.h"
#include "Mathlib.h"
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

    void                    findAllStars();
    void                    deleteAllStars();
    
    void                    setView( Panel* );

    void                    setRB(rb_t*);
    void                    updateRB(rb_t*);
    vec2*                   getSuivi();

    void                    update(int, int, Panel*, rb_t*);
    void                    displayGL();
    
    void                    selectLeft(int, int);
    void                    selectRight(int, int);
    void                    suivi(rb_t*);
    
    void                    position(double, double);
           
private:
    vector<Star*>           v_tStars;
    rb_t*                   RB;
    Panel*                  pView;
    
    int                     dx;
    int                     dy;
    
    float                   ech;

};


#endif
