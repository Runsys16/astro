#ifndef CAPTURES_H
#define CAPTURES_H  1

#include "main.h"

#include <WindowsManager.h>
#include "main.h"
#include "capture.h"
#include "var_mgr.h"


using namespace std;

#include "Singleton.h"

SINGLETON_BEGIN( Captures )

public:
    Captures();
    bool                        isMouseOverCapture(int, int);
    void                        charge_image(string, string);
    void                        update();
    void                        rotate_capture_plus(bool);
    void                        rotate_capture_moins(bool);

    void                        resize_icone( Capture*, int, int, int, int );
    void                        resize_normal( Capture*, int, int );
    void                        resize_all();
    
    void                        glutSpecialFunc(int key, int x, int y);
    void                        ajoute();
    void                        ajoute(string);
    void                        supprime();
    void                        onTop(Capture*);
    void                        findAllStar();
    void                        deleteAllStars();
    void                        print_list();
    void                        sauve();
    void                        charge();
    void                        charge2();
    void                        position(double, double);
    void                        setCurrent(Capture*);
    
    void                        fullscreen();
    void                        showIcones();
    void                        hideIcones();
    void                        switchAffIcones();
    
    void                        setColor(long);
    vector<Capture*>&           get_captures()          { return captures; }
	Capture*					getCurrentCapture()		{ return current_capture==-1 ? NULL : captures[current_capture]; }
    
    void						afficheInfoFits();
	void						onTop();
inline bool						isCurrent(Capture*p)	{ return p == captures[current_capture]; }
private:
    vector<Capture*>            captures;
    int                         current_capture;
    string                      sCurrentDir;

    bool                        bIcones;
    bool                        bShowIcones;
    bool                        bFullPreview;
    bool                        bShowPreview;
    bool						bAfficheInfoSouris;
    bool						bAfficheInfoFits;
    bool						bAfficheGrille;
    int							mode = 0;

SINGLETON_END()

#endif
