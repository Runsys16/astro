#ifndef CAPTURES_H
#define CAPTURES_H  1

#include "main.h"

#include <WindowsManager.h>
#include "main.h"
#include "capture.h"


using namespace std;

#include "Singleton.h"

SINGLETON_BEGIN( Captures )

public:
    Captures();
    bool                        isMouseOverCapture(int, int);
    void                        change_file(string, string);
    void                        update();
    void                        rotate_capture_plus(bool);
    void                        rotate_capture_moins(bool);
    void                        glutSpecialFunc(int key, int x, int y);
    void                        fullscreen();
    void                        ajoute();
    void                        ajoute(string);
    void                        supprime();
    void                        onTop(Capture*);
    void                        findAllStar();
    void                        deleteAllStars();
    void                        print_list();
    void                        sauve();
    void                        charge();
    void                        position(double, double);
    
private:
    vector<Capture*>            captures;
    int                         current_capture;
    bool                        bIcones;

SINGLETON_END()

#endif
