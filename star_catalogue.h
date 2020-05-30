#ifndef STAR_CATALOG_H
#define STAR_CATALOG_H  1

#include "main.h"
#include "Mathlib.h"

#include <WindowsManager.h>
#include <thread>


using namespace std;

class star_catalogue 
{
public :
        double          fRA;
        double          fDE;
        double          fMag;
        string          name;

public :
        star_catalogue( double, double, double, string );
};

#endif
