#ifndef CAMERA_MGR_H
#define CAMERA_MGR_H  1


#include "camera.h"
#include "camera_zwo.h"
#include "serial.h"
#include "star_catalogue.h"

#include <WindowsManager.h>
#include <thread>




//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#include "Singleton.h"

class Camera_mgr : public ButtonCallBack
{
public :
	inline static Camera_mgr& getInstance() {
		if (!s_pInstance)
			s_pInstance = new Camera_mgr();

		return *s_pInstance;
	}

	inline static void DestroyInstance() {
		if(s_pInstance) delete s_pInstance;
		s_pInstance = 0;
	}


private :
	static Camera_mgr* s_pInstance;	// Instance de la classe

	//void operator =(Singleton&);

	
private:
    vector<Camera*>             pCameras;
    Camera*                     pCurrent;
    Camera*                     pAsserv;
    
    int                         nActive;
    vec2                        vSuivi;
    

public:
    Camera_mgr();
    void						idleGL( float fElapsedTIme );
    bool                        camera_exist( string dev_name );
    void                        add( string dev_name );
    void                        add( Camera* );
    void                        add_zwo( int );
    void                        sup_zwo( int );
    void                        addImages( Camera* );
    void                        sup( string );
    void                        sup( Camera * );

    void                        change_background_camera();
    void                        start_camera();
    void                        stop_camera();
    bool                        keyboard( char key );
    void                        glutSpecialFunc(int key, int x, int y);
    //void                        idleGL();
    void                        onBottom();
    void                        resize( int, int );
    void                        active();
    void                        active( Camera*);
    void                        active( Camera*, bool);
    void                        reOrder();
    void                        togglePanel();
    void						fullscreen();

    GLubyte*                    getPtr();

    int                         get_xCam();
    int                         get_yCam();
    int                         get_dxCam();
    int                         get_dyCam();
    ivec2                       get_vCameraSize();
    vec2*                       getSuivi();
    void						setRefSuivi(vec2&);
    
    int                         getCount();
    int                         getNum(Camera *);
    Camera*                     getCamera(string dev);
    Camera*                     getCamera(int n);
    
    void                        print_list();
 
    bool                        haveNewFrame();
    void                        haveUseFrame(bool);
    
    void                        findAllStars();
    void                        deleteAllStars();
    
    void                        suivi();

    rb_t *                      getRB();

   
    void                        update();   
    void                        position(double, double);
    
    void                        setColor(long);

    void                        tex2screen(int&,int&);
    void                        tex2screen(double&,double&);

    void                        screen2tex(int&,int&);
    void                        screen2tex(double&,double&);
    
    void                        start_threads();
    void                        add_catalogue(StarCatalog*);
    void                        setRefCatalog(double, double);
   
	void						stopAllCameras();
	void						cam_full_screen();
	
	void						compareStar();
	void						callback( bool, int, char* );
	void						enregistre_name();
	
	void		              	setAsserv(Camera* p);
	
inline Camera*              	getCurrent()						{ return pCurrent; }   
inline bool 					is_current( Camera * p )			{ return (pCurrent == p); }
inline Camera*              	getAsserv()							{ return pAsserv; }   
    
};



#endif

