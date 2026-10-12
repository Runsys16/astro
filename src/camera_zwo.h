#ifndef CAMERA_ZWO_H
#define CAMERA_ZWO_H  1

#include "v4l2.h"
#include "stars.h"
#include "timer.h"
#include "panel_camera.h"
#include "camera.h"
//#include "panel_spin_edit_text.h"

#include <WindowsManager.h>
#include <thread>
#include <atomic>
#include <ASICamera2.h>
#include "button_callback.h"
#include "star_compare.h"
#include "notification.h"



typedef enum FB_CALLBACK_KEY {
	FB_CALLBACK_KEY_s = 0,
	FB_CALLBACK_KEY_S,
	FB_CALLBACK_KEY_QUIT = -1
}FB_CALLBACK_KEY;

using namespace std;
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
//class CameraZWO : public Device_cam, ChangeValue, NotificationFindStar 
class CameraZWO : public Camera
{

protected:
	int							iCamID;
	long						width;
	long						height;
	long						iImgSize;
	long						gain;
	long						expo;
	long						wb_r;
	long						wb_b;
	long						bin;
	long						flip;
	
	//string						sName;
	unsigned char* 				pImgBuf0;
	unsigned char* 				pImgBuf1;
	
    atomic<bool>				bThreadCaptureImage;
    atomic<bool>				bThreadCaptureVideo;
    atomic<bool>				bStopThreadVideo;
    atomic<int>					iCharging;

	ASI_CAMERA_INFO				ASI_camera_info;
	ASI_SUPPORTED_MODE			ASI_Supported_Mode;

/*
*/
public :
virtual    ~CameraZWO();
    CameraZWO(int);
    CameraZWO(int, int);
    
    void                        init();
virtual void 					changeValueDouble( double val, void *p );
virtual void 					changeValueDoubleClick( double val, void *p );
    
    void                        save_var_control( ASI_CONTROL_TYPE, long, bool);
    void                        load_var_control();
    long                        get_var_control(ASI_CONTROL_TYPE);
    int                         addControl();
    void                        createControlID( int x, int y, ASI_CONTROL_CAPS*);
    void                        CreateControl();

//    void                        update();

    virtual void                CreatePreview();
    virtual void                iconSizePreview(int, int);
    virtual void                fullSizePreview(int, int);

    virtual void                resizeControl(int, int);
    void                        resizeControlFirst(int, int, int, int);

    virtual void                change_background();
    
    bool                        keyboard( char );
    
    //std::thread                 memberThread();
    
    void                        setVisible(bool b);
    void                        setControlVisible(bool b);
	virtual void				togglePanel();
    
    

    virtual GLubyte*            getPtr();


    vec2*                       getSuivi();
    void						recentreSuivi();
    
    void                        setColor(long);

virtual	void					chargeGaiaDR3()						{};
virtual	void					eraseGaiaDR3()						{};

	void						compareStar();
	void						saveCompareStar();
	
	char*						str_erro( ASI_ERROR_CODE );
	char*						str_bayer( ASI_BAYER_PATTERN );
	char*						str_image( ASI_IMG_TYPE );
	char*						str_mode( ASI_CAMERA_MODE );
	char*						str_caps( ASI_CONTROL_TYPE );
	
	void						print_info( ASI_CAMERA_INFO* );
	void						print_images_spported( ASI_IMG_TYPE* );
	void						print_mode( ASI_SUPPORTED_MODE* );
	
	void						init_zwo( int );
	void						close_zwo();
	void						coloriage( unsigned char* );
	void						thread_capture_image();
	void						thread_capture_video();
virtual	void					enregistre();
	void						video();

virtual void					callback(bool, int, char*);
	int							writeImage(char*, int, int, unsigned char*, char* );
	
inline bool                     getControlVisible()                 { return panelControl!= NULL  ? panelControl->getVisible() : false; }
inline PanelCamera *            getPanelCamera()					{ return panelCamera; }
inline PanelWindow *            getPanelControl()                   { return panelControl; }
inline float                    getHertz()                          { return hz; }
inline PanelText *              getPanelName()                      { return pPanelName; }
inline PanelText *              getpCamFilename()                   { return pCamFilename; }
inline PanelText *              getPanelNbStars()                   { return pNbStars; }
inline int                      getNbStars()                        { return panelCamera->getNbStars(); }

inline void                     add_catalogue(StarCatalog* p)       { panelCamera->add_catalogue(p); }
inline void                     setRefCatalog(double _0, double _1) { panelCamera->setRefCatalog(_0, _1); }

inline	bool					getOnScreen()						{ return bOnScreen;}
inline	void					setOnScreen(bool b)					{ bOnScreen = b;}
inline	bool					getIconized()						{ return bIconized;}
inline	int						get_zwo_ID()						{ return iCamID;}
};

#endif

