#include "main.h"
#include "v4l2.h"
#include "camera.h"
#include "camera_mgr.h"
#include "connexion_mgr.h"
#include "Mathlib.h"
#include "timer.h"
#include "pleiade.h"
#include "panel_console_serial.h"


//#define DEBUG 1
#define SIZEPT  20



//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelConsole *      pConsoleSerial;
PanelWindow *       panelHelp;
PanelWindow *       panelResultat;
PanelWindow *       panelCourbe;

//PanelSimple *       panelPreView;
PanelSimple *       panelStatus;

PanelScrollText*    panelStdOut;

PanelText*          pCamFilename;
PanelText*          R;
PanelText*          G;
PanelText*          B;
PanelText*          L;
PanelText*          SP;

PanelText*          pHertz;
PanelText*          pFPS;
PanelText *         pErr;


//Device_cam          camera      = Device_cam();

char                background[]="frame-0.raw";
GLenum err;
float               prevTime    = -1.0f;

int                 width  = 1422;
int                 height = 800;
//int                 width  = 950;
//int                 height = 534;
int                 widthScreen  = 0;
int                 heightScreen = 0;

int                 xPos;
int                 yPos;

GLdouble            fov = 60.0;

char                dev_name[] = "/dev/video0000";

bool                bPrintLum = true;

bool                bPng    = false;
bool                bFull   = false;
bool                bPause  = false;
bool                bSuivi  = false;
bool                bAutorisationSuivi = true;

bool                bPanelControl  = true;
bool                bPanelHelp     = false;
bool                bPanelResultat = false;
bool                bPanelCourbe   = false;
bool                bPanelStdOut   = false;


int                 wImg;
int                 hImg;
int                 nPlanesImg;

ivec2               vCameraSize;
GLubyte*             ptr;
vector<string>      exclude;

int                 _r[256];
int                 _g[256];
int                 _b[256];

float               xSuivi;
float               ySuivi;
//--------------------------------------------------------------------------------------------------------------------
//              Ratio Witdh Height
//--------------------------------------------------------------------------------------------------------------------
double              rw;
double              rh;

int                 xCam;
int                 yCam;
int                 dxCam;
int                 dyCam;

float               zoom;

#define             GET_OFFSET(x,y,width)   (3*y*width+x)
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
vector<vec2>        t_vResultat;
float               offset_x;
float               offset_y;
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
struct etoile   atlas   = { 3, 49, 9.7425852,  24, 3, 12.300277 };
struct etoile   electre = { 3, 44, 52.5368818, 24, 6, 48.011217 };

vec2            calibre[2];
int             nCalibre = 0;
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static const char short_options[] = "d:hfs:lpe:";
static const struct option
long_options[] = {
        { "device", required_argument, NULL, 'd' },
        { "help",   no_argument,       NULL, 'h' },
        { "format", no_argument,       NULL, 'f' },
        { "size",   required_argument, NULL, 's' },
        { "log",    no_argument,       NULL, 'l' },
        { "pleiade",no_argument,       NULL, 'p' },
        { "exclude",required_argument, NULL, 'e' },
        { 0, 0, 0, 0 }
};
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void usage(FILE *fp, int argc, char **argv)
{
        fprintf(fp,
                 "Usage: %s [options]\n\n"
                 "Version 1.3\n"
                 "Options:\n"
                 "-d | --device name   Video device name [/dev/video(n)]\n"
                 "-h | --help          Print this message\n"
                 "-f | --full          Full size windows\n"
                 "-l | --log           Affiche les logs\n"
                 "-e | --exc           Exclude device\n"
                 "",
                 argv[0] );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
std::vector<string>& getExclude()
{
    return exclude;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
float hms2rad(struct hms& HMS)
{
    return (HMS.h + HMS.m/60.0 + HMS.s/3600.0) / 12.0*M_PI;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
float dms2rad(struct dms& DMS)
{
    return (DMS.d + DMS.m/60.0 + DMS.s/3600.0) / 180.0*M_PI;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void rad2hms(struct hms& HMS, float r)
{
    float h = r / M_PI * 12.0;
    int H = h;
    HMS.h = H;
    
    float m = (h-H) * 60.0;
    int M = m;
    HMS.m = M;
    
    float s = (m-M) * 60.0;
    HMS.s = s;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void rad2dms(struct dms& DMS, float r)
{
    float d = r / M_PI * 180.0;
    int D = d;
    DMS.d = D;
    
    float m = (d-D) * 60.0;
    int M = m;
    DMS.m = M;
    
    float s = (m-M) * 60.0;
    DMS.s = s;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void glCercle(int x, int y, int rayon)
{
	glBegin(GL_LINE_LOOP);

        for( float i=0; i<=360.0; i+=1.0 )
        {
            float fx = (float)x+ (float)rayon*cos(DEG2RAD(i));
            float fy = (float)y+ (float)rayon*sin(DEG2RAD(i));
            glVertex2i(fx,fy);
        }
        
    glEnd();        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void glCarre( int x,  int y,  int dx,  int dy )
{
	glBegin(GL_LINES);
        x = x-dx;
        y = y-dy;
        
        glVertex2i(x,y);                glVertex2i(x+2*dx,y);
        glVertex2i(x+2*dx,y);           glVertex2i(x+2*dx,y+2*dy);
        glVertex2i(x+2*dx,y+2*dy);      glVertex2i(x,y+2*dy);
        glVertex2i(x,y+2*dy);           glVertex2i(x,y);

    glEnd();        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void glCroix( int x,  int y,  int dx,  int dy )
{
	glBegin(GL_LINES);

	    glVertex2i(x, y-dy);         glVertex2i(x, y+dy);
	    glVertex2i(x-dx, y);         glVertex2i(x+dx, y);

    glEnd();        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void displayGL_cb(void)
{
    if ( bSuivi && bAutorisationSuivi )   {
		    glColor4f( 0.0, 1.0, 0.0, 0.2 );
		    
		    int x = round(xSuivi+0.5);
		    int y = round(ySuivi+0.5);
		    
		    tex2screen(x,y);

		    glCroix(x, y, 50, 50);
            glCarre(x, y, SIZEPT, SIZEPT);
            //glCercle(xSuivi, ySuivi, SIZEPT-10);
            //glCercle(xSuivi, ySuivi, SIZEPT-5);
            //glCercle(xSuivi, ySuivi, SIZEPT+10);

    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void glEchelle()
{
    float gris = 0.3;
    glColor4f( gris, gris, gris, 1.0 );
    
    glBegin(GL_LINES);
        int y = 100.0;
        int x = 0;

        panelCourbe->x2Screen(x);
        panelCourbe->y2Screen(y);

        glVertex2i( x, y );
        
        x += panelCourbe->getPosDX();

        glVertex2i( x, y );
     
        y = 200.0;
        x = 0;

        panelCourbe->x2Screen(x);
        panelCourbe->y2Screen(y);

        glVertex2i( x, y );
        
        x = panelCourbe->getDX() + x;
        panelCourbe->x2Screen(x);

        glVertex2i( x, y );


        for( int i=0; i<2000; i+=60)
        {
            int x = i;
            int y = 0;
            panelCourbe->x2Screen(x);
            panelCourbe->y2Screen(y);
            glVertex2i( x, y );
            
            y += panelCourbe->getPosDX();
            glVertex2i( x, y );
            
        }
    glEnd();        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void displayCourbeGL_cb(void)
{
    int DY = panelCourbe->getY();
    int n = t_vResultat.size();
    
    glEchelle();
    
    if ( n != 0  )
    {
        glColor4f( 0.0, 0.0, 1.0, 1.0 );
        
        glBegin(GL_LINE_STRIP);
        for( int i=0; i<t_vResultat.size(); i++ )
        {
            int y = 1.0*(t_vResultat[i].x-offset_x) + 100.0;
            int x = (n-i)*1;

            panelCourbe->x2Screen(x);
            panelCourbe->y2Screen(y);

            glVertex2i( x, y );
            //printf( "%d %0.2f %0.2f y=%0.2f\n", i, t_vResultat[i].x, offset_x, y );
        }
        glEnd();        

        
        glColor4f( 1.0, 0.0, 0.0, 1.0 );
        glBegin(GL_LINE_STRIP);
        for( int i=0; i<t_vResultat.size(); i++ )
        {
            int y = 1.0*(t_vResultat[i].y-offset_y) + 200.0;
            int x = (n-i)*1;

            panelCourbe->x2Screen(x);
            panelCourbe->y2Screen(y);

            glVertex2i( x, y );
        }
        glEnd();   
        
    }     
    
    glColor4f( 1.0, 1.0, 1.0, 1.0 );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int getOffset( int x, int y, int width )
{
    return 3*(x) + 3*(y)* width;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void screen2tex( int& x, int& y )
{
    x = (float)(x-xCam) * rw;
    y = (float)(y-yCam) * rh;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void tex2screen( int& x, int& y )
{
    x = (float)x / rw + xCam;
    y = (float)y / rh + yCam;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void getSkyPointLine(struct sky_point* point, int x, int y, int size)
{
    int min = x - size;
    int max = x + size;
    
    //int offset = GET_OFFSET(0,y,vCameraSize.x);
    
    //printf( "getPointLine x%d y%d\n", x, y);
    ptr = Camera_mgr::getInstance().getPtr();
    for( x=min; x<max; x++) 
    {
        int offset = getOffset(x,y,vCameraSize.x);

        float r = ptr[offset+0]; 
        float g = ptr[offset+1]; 
        float b = ptr[offset+2]; 

        float l = 0.33 * r + 0.5 * g  + 0.16 * b;
        if ( l<20.0 )       l = 0.0;
        
        if (bPrintLum)      printf( "%-3d-", (int)l);
        
        point->xAverage    += l * (float)x;
        point->yAverage    += l * (float)y;
        point->ponderation += l;
    }  

    if (bPrintLum)      printf( "\n" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void getSkyPoint(struct sky_point* point, int x, int y, int size)
{
    //int X = (float)(x-xCam) * rw;
    //int Y = (float)(y-yCam) * rh;
    int min = y - size;
    int max = y + size;
    
    //printf( "getPoint\n");
    for(int Y=min; Y<max; Y ++)        getSkyPointLine(point, x, Y, size);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void change_fov( void )
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	GLdouble ratio = (GLdouble)width / (GLdouble)height;
	
	gluPerspective((GLdouble)fov, (GLdouble)ratio, (GLdouble)0.1, (GLdouble) 1000.0);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void displayGL(void)
{
	WindowsManager::getInstance().clearBufferGL(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

    glColor4f( 0.0, 0.0, 0.0, 1.0 );
	WindowsManager::getInstance().displayGL();

	glutSwapBuffers();


}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void reshapeGL(int newWidth, int newHeight)
{
    logf((char*) "-------- reshapeGL(%d, %d)", newWidth, newHeight);

	WindowsManager& wm = WindowsManager::getInstance();
	wm.setScreenSize( newWidth, newHeight );

    //resizePreview(newWidth, newHeight);
    //resizeControl(newWidth, newHeight);
    resizeHelp(newWidth, newHeight);
    resizeCourbe(newWidth, newHeight);

	panelStatus->setPosAndSize( 0, newHeight-20, newWidth, 20 );
	pFPS->setPos( newWidth-100, 5);
	pHertz->setPos( newWidth-150, 5);

	glViewport(0, 0, (GLint)newWidth, (GLint)newHeight);

	change_fov();
	if ( !bFull )
    {
	    width = newWidth;
	    height = newHeight;
    }
    Camera_mgr::getInstance().resize( newWidth, newHeight );
    //cout << "reshapeGL("<< newWidth <<" "<< newHeight <<")"<< endl;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void write_image(void)
{
    //camera.mainloop();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void change_hertz(float hz)
{
    static char sHz[255];
    sprintf( sHz, "%.0fHz", hz);
    pHertz->changeText(sHz);
    //camera.mainloop();
}

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void suivi(void)
{
    //change_background_pleiade();
    getSuiviParameter();   
    
    char   sSkyPoint[100];
    
    struct sky_point point;
    point.xAverage = 0.0;
    point.yAverage = 0.0;
    point.ponderation = 0.0;
    
    //printf( "1-Suivi x=%0.2f, y=%0.2f\n", xSuivi, ySuivi);

    //getSkyPoint(&point, (int)xSuivi-xCam, (int)ySuivi-yCam, SIZEPT);
    getSkyPoint(&point, (int)xSuivi, (int)ySuivi, SIZEPT);
    
    float xx;
    float yy;

    if ( point.ponderation > 0.1 ) {

        xx = point.xAverage / point.ponderation;
        yy = point.yAverage / point.ponderation;
        
        sprintf( sSkyPoint, "x=%0.2f, y=%0.2f, pond=%0.2f", xx, yy, point.ponderation);
        SP->changeText(sSkyPoint);
        //panelResultat->setVisible(true);

        xSuivi = xx;
        ySuivi = yy;
        //printf( "2-Suivi x=%0.2f, y=%0.2f\n", xSuivi, ySuivi);
    }

    if ( t_vResultat.size()>20000)      t_vResultat.clear();

    vec2 v;
    v.x = xx;
    v.y = yy;
    t_vResultat.push_back(v);
    
    
    //printf( "3-Suivi x=%0.2f, y=%0.2f\n", xSuivi, ySuivi);
    int xPanel = xSuivi;
    int yPanel = ySuivi;
    tex2screen(xPanel,yPanel);
    panelResultat->setPos(xPanel+20 , yPanel+20);
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void getSuiviParameter(void)
{
    Camera_mgr&  cam_mgr = Camera_mgr::getInstance();
    //cam_mgr.active();

    xCam = cam_mgr.get_xCam();
    yCam = cam_mgr.get_yCam();
    dxCam = cam_mgr.get_dxCam();
    dyCam = cam_mgr.get_dyCam();

    vCameraSize.x = 1920.0;
    vCameraSize.y = 1080;
    vCameraSize = cam_mgr.get_vCameraSize();

    //rw = (float)vCameraSize.x/(float)panelPreView->getDX();
    //rh = (float)vCameraSize.y/(float)panelPreView->getDY();
    rw = (float)vCameraSize.x/(float)dxCam;
    rh = (float)vCameraSize.y/(float)dyCam;

    /*
    logf( (char*)"-------------------------------------" );
    logf( (char*)"width=%d height=%d" , width, height );
    logf( (char*)"xCam=%d yCam=%d dxCam=%d dyCam=%d", xCam, yCam, dxCam, dyCam );
    logf( (char*)"rw=%0.2f rh=%0.2f" , rw, rh );
    */
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void idleGL(void)
{
    Timer&          timer = Timer::getInstance();
	float time = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

    char sFPS[] = "fps 0000";
    sprintf( sFPS,"fps %d", *Timer::getInstance().getvFPSCounter() );
    pFPS->changeText((char*)sFPS);
    
	timer.Idle();




    if (!bPause)    {
        Camera_mgr::getInstance().change_background_camera();
    }

    if (bSuivi)    suivi();

	
	if ( prevTime < 0 )	{
		prevTime = time;
	}
	else 	{
		float elapsedTime = time - prevTime;
		prevTime = time;
		WindowsManager::getInstance().idleGL( elapsedTime );
	}

    //WindowsManager::getInstance().onBottom(panelPreView);

    //Camera_mgr::getInstance().idleGL();
	glutPostRedisplay();
    //Camera_mgr::getInstance().idleGL();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void quit(void)
{
	exit(0);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void initGL(int argc, char **argv)
{
	glClearColor(.8f,.8f,.8f,1.0f);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glDisable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void rotateVisible()
{
    Panel*  pFocus = WindowsManager::getInstance().getFocus();

    if ( pFocus == panelHelp)       pFocus->setVisible(!pFocus->getVisible());
    //if ( pFocus == panelControl)    pFocus->setVisible(!pFocus->getVisible());
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void glutKeyboardFunc(unsigned char key, int x, int y) {
    //std::cout << (int)key << std::endl;
	Control* pControl;
	
	switch(key){ 
	
	case 27: quit(); break;
	case 9:
		//WindowsManager::getInstance().swapVisible();
		{
	    logf( (char*)"-------------- Touche 'TAB'" );
		Camera_mgr&  cam_mgr = Camera_mgr::getInstance();
		cam_mgr.active();

        xCam = cam_mgr.get_xCam();
        yCam = cam_mgr.get_yCam();
        dxCam = cam_mgr.get_dxCam();
        dyCam = cam_mgr.get_dyCam();
    
        vCameraSize.x = 1920.0;
        vCameraSize.y = 1080;
        vCameraSize = cam_mgr.get_vCameraSize();

        //rw = (float)vCameraSize.x/(float)panelPreView->getDX();
        //rh = (float)vCameraSize.y/(float)panelPreView->getDY();
        rw = (float)vCameraSize.x/(float)dxCam;
        rh = (float)vCameraSize.y/(float)dyCam;
		
	    logf( (char*)"vCameraSize.x=%d vCameraSize.y=%d" , vCameraSize.x, vCameraSize.y );
	    logf( (char*)"width=%d height=%d" , width, height );
	    logf( (char*)"xCam=%d yCam=%d dxCam=%d dyCam=%d", xCam, yCam, dxCam, dyCam );
	    logf( (char*)"rw=%0.2f rh=%0.2f" , rw, rh );
		//change_cam();
        }
		break;
	case 10:
		break;
	case 13:
	    bFull = !bFull;

        if (bFull ){
     		glutFullScreen();
            //reshapeGL(vCameraSize.x,1200);
            logf( (char*)"w=%d h=%d\n", width, height );
            log( (char*)"FullScreen !!!" );
        }
        else    {
            logf( (char*)"w=%d h=%d\n", width, height );
		    glutPositionWindow( (glutGet(GLUT_SCREEN_WIDTH)-width)/2, (glutGet(GLUT_SCREEN_HEIGHT)-height)/2  );
		    glutReshapeWindow( width, height );
            log( (char*)"NormalScreen !!!" );
		}
    	break;
    case 'O':
    	bAutorisationSuivi = !bAutorisationSuivi;
    	break;

    case 'l':
        {            
        Connexion_mgr::getInstance().print_list();
        Camera_mgr::getInstance().print_list();
        }
        break;

    case 'g':
        {            
        Serial::getInstance().write_string( "g" );
        }
        break;

    case 'p':  // '-'
        bPause = !bPause;

        if ( bPause )   {
            pErr->changeText((char*)"Pause" );
        }
        else {
            pErr->changeText((char*)"" );
        }
        break;

    case '2':
        bPanelHelp = !bPanelHelp;
        panelHelp->setVisible(bPanelHelp);
        log( (char*)"Toggle panelHelp !!!" );
        break;
    case '3':
        bPanelResultat = !bPanelResultat;
        panelResultat->setVisible(bPanelResultat);
        log( (char*)"Toggle panelResultat !!!" );
        break;
    case '4':
        bPanelCourbe = !bPanelCourbe;
        panelCourbe->setVisible(bPanelCourbe);
        log( (char*)"Toggle panelCourbe !!!" );
        break;
    case '5':
        bPanelStdOut = !bPanelStdOut;
        panelStdOut->setVisible(bPanelStdOut);
        log( (char*)"Toggle panelStdOut !!!" );
        break;
    case ' ':
        bPng = !bPng;
        if ( bPng )
        {
            log( (char*)"Affiche le suivi sauvegarde" );
            //pthread_chargement_camera->join();
            //camera.stop_capturing();
        }
        else
        {
            log( (char*)"Affiche la camera" );
            //pthread_chargement_pleiades->join();
            //camera.start_capturing();
        }
        break;
    case 'A':
        if ( nCalibre == 0 )
        {
            logf( (char*)"Atlas :    AD %.0fh%.0fm%0.2fs  %frad", atlas.ad.h,  atlas.ad.m,  atlas.ad.s,  hms2rad(atlas.ad) );
            logf( (char*)"          DEC %.0fd%.0fm%0.2fs  %frad", atlas.dec.d, atlas.dec.m, atlas.dec.s, dms2rad(atlas.dec) );
            calibre[0].x = hms2rad(atlas.ad);
            calibre[0].y = dms2rad(atlas.dec);
        }
        else
        {
            logf( (char*)"Electre :  AD %.0fh%.0fm%0.2fs  %frad", electre.ad.h,  electre.ad.m,  electre.ad.s,  hms2rad(electre.ad) );
            logf( (char*)"          DEC %.0fd%.0fm%0.2fs  %frad", electre.dec.d, electre.dec.m, electre.dec.s, dms2rad(electre.dec) );
            calibre[1].x = hms2rad(electre.ad);
            calibre[1].y = dms2rad(electre.dec);
        }
        nCalibre = ++nCalibre % 2;
        break;
    case 'a':
        {
        vec2 r;
        //r.x =  calibre[1].x - calibre[0].x;
        //r.y =  calibre[1].y - calibre[0].y;
        r =  calibre[1] - calibre[0];
        float l = r.length();
        struct dms DMS;
        rad2dms( DMS, l );
        logf( (char*)"Longueur : %f   %.0fd%.0fm%.2fs", l, DMS.d, DMS.m, DMS.s );
        }
        break;

    default:
        {
        logf((char*)"key: %d", key);
        }
        break;
	}
	
	Camera_mgr::getInstance().keyboard( key );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void glutKeyboardUpFunc(unsigned char key, int x, int y)	{
	WindowsManager::getInstance().keyboardUpFunc( key, x, y);
    //WindowsManager::getInstance().onBottom(panelPreView);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void glutSpecialFunc(int key, int x, int y)	{
	WindowsManager::getInstance().keyboardSpecialFunc( key, x, y);
    //WindowsManager::getInstance().onBottom(panelPreView);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void glutSpecialUpFunc(int key, int x, int y)	{
	WindowsManager::getInstance().keyboardSpecialUpFunc( key, x, y);
    //WindowsManager::getInstance().onBottom(panelPreView);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void glutMouseFunc(int button, int state, int x, int y)	{
	WindowsManager::getInstance().mouseFunc(button, state, x, y);
    //WindowsManager::getInstance().onBottom(panelPreView);

	//if ( bPause && button == 0 && state == 0 )	{
	if ( button == 0 && state == 0 )	{

        getSuiviParameter();

        Camera_mgr::getInstance().onBottom();
        
	    int X = x;
	    int Y = y;
	    
	    screen2tex(X,Y);
	    logf( (char*)"---------------------- Click x=%d y=%d  X=%d Y=%d vCameraSize.x=%d" , x, y, X, Y, vCameraSize.x );
	    
        ptr = Camera_mgr::getInstance().getPtr();
        int N = getOffset(X, Y, vCameraSize.x);
	    int r = ptr[N];
	    int g = ptr[N+1];
	    int b = ptr[N+2];

        float fL = 0.33 * (float)r + 0.5 * (float)g  + 0.16 * (float)b;
        int l = fL;

	    char sR[10];
	    char sG[10];
	    char sB[10];
	    char sL[100];
	    char skyPoint[100];
	    
	    sprintf( sR, "r=%03d", r );
	    sprintf( sG, "g=%03d", g );
	    sprintf( sB, "b=%03d", b );
	    sprintf( sL, "l=%03d", l );
	    
	    R->changeText(sR);
	    G->changeText(sG);
	    B->changeText(sB);
	    L->changeText(sL);
	    
	    panelResultat->setPos(x+20 , y+20);

    
        struct sky_point point;
        point.xAverage = 0.0;
        point.yAverage = 0.0;
        point.ponderation = 0.0;
        
        //rw = (float)vCameraSize.x.0/(float)panelPreView->getDX();
        //rh = (float)vCameraSize.y/(float)panelPreView->getDY();
        bPrintLum = true;
        getSkyPoint(&point, X, Y, SIZEPT);
        bPrintLum = false;
        
        if ( point.ponderation > 0.1 ) {

            float xx = point.xAverage / point.ponderation;
            float yy = point.yAverage / point.ponderation;
            
            sprintf( skyPoint, "x=%0.2f, y=%0.2f, pond=%0.2f", xx, yy, point.ponderation);
	        SP->changeText(skyPoint);
	        //panelResultat->setVisible(true);

	        printf( "%s\n", skyPoint);
	        
	        if ( bAutorisationSuivi)    bSuivi = true;
	        xSuivi = xx;
	        ySuivi = yy;
	        offset_x = xx;
	        offset_y = yy;
	        t_vResultat.clear();
        }
        else {
            sprintf( skyPoint, "Rien");
	        SP->changeText(skyPoint);
	        bSuivi = false;
        }

	    logf( (char*)"xSuivi=%0.2f ySuivi=%0.2f   " , xSuivi, ySuivi );
	    logf( (char*)"    l=%d rgb=%d,%d,%d\n" , r,g,b,  l );

	} 

    Camera_mgr::getInstance().onBottom();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void glutMotionFunc(int x, int y)	{	
	WindowsManager::getInstance().motionFunc(x, y);
    //WindowsManager::getInstance().onBottom(panelPreView);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void glutPassiveMotionFunc(int x, int y)	{
	WindowsManager::getInstance().passiveMotionFunc(x, y);
    //WindowsManager::getInstance().onBottom(panelPreView);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void resizeHelp(int width, int height)	{
    int dx = panelHelp->getDX();
    int dy = panelHelp->getDY();
    
    if ( dx==0)     return;

    int x = width - dx - 20;
    int y = 20 + 200 + 20 ;

    panelHelp->setPos( x,  y );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void resizeCourbe(int width, int height)	{
    int dx = width - 20;
    int dy = 300;
    int x = 10;
    int y = height - 10 - 20 - dy;

    panelCourbe->setPosAndSize( x,  y, dx, dy );
    //printf("resizeControl(%d, %d\n", width, height);
    //printf("  x=%d y=%d dx=%d dy=%d\n", x, y, dx, dy);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void CreateCourbe()	{
	WindowsManager& wm = WindowsManager::getInstance();

    panelCourbe = new PanelWindow();
    resizeCourbe( width, height );

    wm.add(panelCourbe);
    
    panelCourbe->setVisible(bPanelCourbe);
 	panelCourbe->setBackground((char*)"background.tga");
    panelCourbe->setDisplayGL( displayCourbeGL_cb );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void CreateResultat()	{
	WindowsManager& wm = WindowsManager::getInstance();

    panelResultat = new PanelWindow();
    int dx = 700;
    int dy = 30;
    int x = 10;
    int y = height - 100;

    panelResultat->setPosAndSize( x, y, dx, dy);
    wm.add(panelResultat);
    
	int y0 = 5;
	int x0 = 10;
	int dx0 = 50;
	int l = 0;

	R = new PanelText( (char*)"r=0",  		PanelText::NORMAL_FONT, x0 + dx0*l++, y0  );
	panelResultat->add( R ); 
	G = new PanelText( (char*)"g=0",  		PanelText::NORMAL_FONT, x0 + dx0*l++, y0  );
	panelResultat->add( G ); 
	B = new PanelText( (char*)"b=0",  		PanelText::NORMAL_FONT, x0 + dx0*l++, y0  );
	panelResultat->add( B ); 
	L = new PanelText( (char*)"l=0",  		PanelText::NORMAL_FONT, x0 + dx0*l++, y0  );
	panelResultat->add( L ); 

	SP = new PanelText( (char*)" ",  		PanelText::NORMAL_FONT, x0, y0+16  );
	panelResultat->add( SP ); 

    panelResultat->setSize( x0 + l*dx0+100, dy+16);
    panelResultat->setBackground((char*)"background.tga");
    panelResultat->setVisible(bPanelResultat);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void CreateHelp()	
{
    int X = width - 50;
    int Y = 50;
    int DX = 150;
    int DY = 300;
    panelHelp = new PanelWindow();
	panelHelp->setPosAndSize( X, Y, DX, DY);
	panelHelp->setVisible(bPanelHelp);
	//panelHelp->setBackground( (char*)"/home/rene/programmes/opengl/video/background.tga");
	panelHelp->setBackground( (char*)"background.tga");
    
	int y = 10;
	int dy = 15;
	int l=0;

	panelHelp->add( new PanelTextOmbre( (char*)"Brightness",  		PanelText::LARGE_FONT, 5, y + l++*dy ) );
	panelHelp->add( new PanelText( (char*)"touche B/b",		    PanelText::NORMAL_FONT,20, y + l++*dy ) );
	
	panelHelp->add( new PanelTextOmbre( (char*)"Contrast",  		PanelText::LARGE_FONT, 5, y + l++*dy ) );
	panelHelp->add( new PanelText( (char*)"touche C/c",		    PanelText::NORMAL_FONT,20, y + l++*dy ) );
	
	panelHelp->add( new PanelTextOmbre( (char*)"Saturation",  		PanelText::LARGE_FONT, 5, y + l++*dy ) );
	panelHelp->add( new PanelText( (char*)"touche S/s",		    PanelText::NORMAL_FONT,20, y + l++*dy ) );
	
	panelHelp->add( new PanelTextOmbre( (char*)"Hue",  		    PanelText::LARGE_FONT, 5, y + l++*dy ) );
	panelHelp->add( new PanelText( (char*)"touche H/h",		    PanelText::NORMAL_FONT,20, y + l++*dy ) );
	
	panelHelp->add( new PanelTextOmbre( (char*)"Gamma",  		    PanelText::LARGE_FONT, 5, y + l++*dy ) );
	panelHelp->add( new PanelText( (char*)"touche G/g",		    PanelText::NORMAL_FONT,20, y + l++*dy ) );
	
	panelHelp->add( new PanelTextOmbre( (char*)"Sharpness",  		PanelText::LARGE_FONT, 5, y + l++*dy ) );
	panelHelp->add( new PanelText( (char*)"touche Z/z",		    PanelText::NORMAL_FONT,20, y + l++*dy ) );
	
	panelHelp->add( new PanelTextOmbre( (char*)"Exposure",  		PanelText::LARGE_FONT, 5, y + l++*dy ) );
	panelHelp->add( new PanelText( (char*)"touche E/e",		    PanelText::NORMAL_FONT,20, y + l++*dy ) );
	
	panelHelp->add( new PanelTextOmbre( (char*)"WhiteBalance",  	PanelText::LARGE_FONT, 5, y + l++*dy ) );
	panelHelp->add( new PanelText( (char*)"touche W/w",		    PanelText::NORMAL_FONT,20, y + l++*dy ) );
	
	panelHelp->add( new PanelTextOmbre( (char*)"Camera",  	PanelText::LARGE_FONT, 5, y + l++*dy ) );
	panelHelp->add( new PanelText( (char*)"touche P/' '",		    PanelText::NORMAL_FONT,20, y + l++*dy ) );
	
	panelHelp->add( new PanelTextOmbre( (char*)"Visibilite Controle",PanelText::LARGE_FONT, 5, y + l++*dy ) );
	panelHelp->add( new PanelText( (char*)"touche 1",		        PanelText::NORMAL_FONT,20, y + l++*dy ) );
	
	panelHelp->add( new PanelTextOmbre( (char*)"Visibilite Help",PanelText::LARGE_FONT, 5, y + l++*dy ) );
	panelHelp->add( new PanelText( (char*)"touche 2",		        PanelText::NORMAL_FONT,20, y + l++*dy ) );

	panelHelp->add( new PanelTextOmbre( (char*)"Pause",PanelText::LARGE_FONT, 5, y + l++*dy ) );
	panelHelp->add( new PanelText( (char*)"touche p",		        PanelText::NORMAL_FONT,20, y + l++*dy ) );

	panelHelp->add( new PanelTextOmbre( (char*)"Suivi (o/n)",PanelText::LARGE_FONT, 5, y + l++*dy ) );
	panelHelp->add( new PanelText( (char*)"touche O",		        PanelText::NORMAL_FONT,20, y + l++*dy ) );

	WindowsManager::getInstance().add(panelHelp);
	
    panelHelp->setSize( DX, ++l*dy);
    //panelHelp->loadSkin((char*)"fen-2");
    resizeHelp(width, height);

    X = width - 20 - DX;
    Y = 20 + 20 + 192;
	panelHelp->setPos(X, Y);

    logf((char*)"** CreateHelp()  panelHelp  %d,%d %dx%d", X, Y, DX, DY);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void CreateStatus()	{
	WindowsManager& wm = WindowsManager::getInstance();
	wm.setScreenSize( width, height );

	int x=0;
	int dx=width;
	int dy=20;
	int y=height-dy;

	panelStatus = new PanelSimple();
	panelStatus->setPosAndSize( x, y, dx, dy );

	pErr = new PanelText( (char*)"Status",		PanelText::NORMAL_FONT, 10, 5 );
	panelStatus->add( pErr );

    pFPS = new PanelText( (char*)"0",		PanelText::NORMAL_FONT, width-100, 5 );
	panelStatus->add( pFPS );

    pHertz = new PanelText( (char*)"0",		PanelText::NORMAL_FONT, width-150, 5 );
	panelStatus->add( pHertz );

	string sErr = "Status !!!";

 	wm.add( panelStatus );
 	panelStatus->setBackground((char*)"background.tga");

    logf((char*)"** CreateStatus()  panelSatuts  %d,%d %dx%d", x, y, dx, dy);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void CreateStdOut()	{
	WindowsManager& wm = WindowsManager::getInstance();

	int x=10;
	int y=50;
	int dx=350;
	int dy=400;

	panelStdOut = new PanelScrollText(dy/13,50);
	panelStdOut->setPrompt(string(">"));
	panelStdOut->setPosAndSize( x, y, dx, dy );


 	wm.add( panelStdOut );
 	panelStdOut->setBackground((char*)"background.tga");
 	panelStdOut->setVisible(bPanelStdOut);
 	panelStdOut->setTabSize(20);

    string st = string("Bonjour\n");
       
    logf((char*)"** CreateStdOut()  panelStdOut  %d,%d %dx%d", x, y, dx, dy);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
static void CreateAllWindows()	{
    //CreatePreview();
    //CreateControl();
    CreateHelp();
    CreateResultat();
    CreateStatus();
    CreateCourbe();
    CreateStdOut();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void log( char* chaine )
{
    if ( panelStdOut )          panelStdOut->affiche( chaine );
    
    printf( "log : %s\n", chaine);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void logf(char *fmt, ...)
{
    char chaine[255];
    va_list arglist;

    va_start( arglist, fmt );
    vsprintf( chaine, fmt, arglist );
    va_end( arglist );
    
    log((char*)chaine);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void parse_size( string s )
{
    std::size_t found = s.find("x");
    int l = s.size();
    
    if (found!=std::string::npos)
    {
        std::cout << "Period found at: " << found << '\n';

        char buff[100];
        sprintf( buff, "%s", s.c_str() );
        buff[found] = 0;
        buff[l] = 0;
        
        char *sWidth = buff;
        char *sHeight = buff + found +1;
        
        width  = atoi(sWidth);
        height = atoi(sHeight);
        
        std::cout<<"Parse "<< sWidth <<"x"<< sHeight << '\n';
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void parse_option( int argc, char**argv )
{
    bool bOK = false;
    /*
    for(int i=0; i<20; i++ )    {
        if (camera.isDevice(i))    {
            cout <<"Camera sur /dev/video"<< i << endl;
            bOK = true;
            break;
        }
    }
    */
    
    if (!bOK)           cout <<"Pas de camera !!!" << endl;

    char        cSize[100];

    for (;;) {
            int idx;
            int c;

            c = getopt_long(argc, argv, short_options, long_options, &idx);

            if (-1 == c)
                    break;

            switch (c) {
            case 0: /* getopt_long() flag */
                    break;

            case 'd':
                    strncpy( dev_name, optarg, sizeof(dev_name));
                    Camera_mgr::getInstance().add(dev_name);
                    //log((char*)"Fin**");
                    bPng = false;
                    break;

            case 'e':
                    strncpy( dev_name, optarg, sizeof(dev_name));
                    exclude.push_back( dev_name );
                    break;

            case 'h':
                    usage(stdout, argc, argv);
                    exit(EXIT_SUCCESS);

            case 'f':
                    bFull = true;
                    width  = widthScreen;
                    height = heightScreen;
                    break;

            case 's':
                    strncpy( cSize, optarg, sizeof(cSize) );

                    //string      sSize=string((const char*)cSize);
                    printf( "%s\n", cSize );
                    parse_size(cSize);
                    break;
            case 'p':
                {
                Camera_mgr::getInstance().add( new Pleiade() );
                }
                break;
    
            /*
            case 'm':
                    io = IO_METHOD_MMAP;
                    break;

            case 'r':
                    io = IO_METHOD_READ;
                    break;

            case 'u':
                    io = IO_METHOD_USERPTR;
                    break;

            case 'o':
                    out_buf++;
                    break;


            case 'c':
                    errno = 0;
                    frame_count = strtol(optarg, NULL, 0);
                    if (errno)
                            errno_exit(optarg);
                    break;
            */
            default:
                    usage(stderr, argc, argv);
                    exit(EXIT_FAILURE);
                    break;
            }
    }
    
    //camera.setDevName(dev_name);

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void parse_option_size( int argc, char**argv )
{
    char        cSize[100];

    for (;;) {
            int idx;
            int c;

            c = getopt_long(argc, argv, short_options, long_options, &idx);

            if (-1 == c)
                    break;

            switch (c) {
            case 's':
                    strncpy( cSize, optarg, sizeof(cSize) );

                    //string      sSize=string((const char*)cSize);
                    printf( "%s\n", cSize );
                    parse_size(cSize);
                    break;
            }
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#include <X11/Xlib.h>
void getX11Screen()
{
    Display *display;
    Screen *screen;

    // open a display
    display = XOpenDisplay(NULL);

    // return the number of available screens
    int count_screens = ScreenCount(display);

    printf("Total count screens: %d\n", count_screens);


    for (int i = 0; i < count_screens; ++i) {
        screen = ScreenOfDisplay(display, i);
        printf("\tScreen %d: %dX%d\n", i + 1, screen->width, screen->height);
        widthScreen = screen->width;
        heightScreen = screen->height;
    }

    // close the display
    XCloseDisplay(display);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
    //parse_option_size(argc, argv);
    
    vCameraSize.x = 1280;
    vCameraSize.y = 720;

    vCameraSize.x = 1920;
    vCameraSize.y = 1080;
    
    xCam = 0;
    yCam = 0;
    dxCam = 1920;
    dyCam = 1080;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    cout <<"Screen size "<< glutGet(GLUT_SCREEN_WIDTH) <<"x"<< glutGet(GLUT_SCREEN_HEIGHT) << endl;

	xPos = (glutGet(GLUT_SCREEN_WIDTH)-width)/2;
	yPos = (glutGet(GLUT_SCREEN_HEIGHT)-height)/2;

	glutInitWindowPosition(xPos, yPos);
	glutInitWindowSize( width, height );

	if (glutCreateWindow("Astro Pilot") == 0){ 
		return 1;
	}

	glutReshapeFunc(reshapeGL);
	glutDisplayFunc(displayGL);
	glutIdleFunc(idleGL);

	
	cout << "Ecran   wxh " << glutGet(GLUT_SCREEN_WIDTH) << "x" << glutGet(GLUT_SCREEN_HEIGHT) << endl;
	cout << "Fenetre x,y wxh "<< xPos <<","<< yPos <<" " << width << "x" << height << endl;

	glutKeyboardFunc(glutKeyboardFunc);
	glutKeyboardUpFunc(glutKeyboardUpFunc);
	glutSpecialFunc(glutSpecialFunc);
	glutSpecialUpFunc(glutSpecialUpFunc);
	glutMouseFunc(glutMouseFunc);
	glutMotionFunc(glutMotionFunc);
	glutPassiveMotionFunc(glutPassiveMotionFunc);

    if ( bFull )        glutFullScreen();

	initGL(argc, argv);
    glewInit();
    
    CreateAllWindows();

    getX11Screen();
    parse_option(argc, argv);
    
    
    logf ((char*)"############## START MANAGER ###################");
    
    Camera_mgr::getInstance().reOrder();
    Connexion_mgr::getInstance().start();
    PanelConsoleSerial::getInstance();

    float gris = 0.2;
    glClearColor( gris, gris, gris,1.0);
    glutMainLoop();


	return 0;
}


