#include "panel_camera.h"
//--------------------------------------------------------------------------------------------------------------------
#define SIZE_CERCLE_COLLIMATION	2
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
//#define TRUE_TYPE_TEST
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
vcf4                colorTraces[] = 
                        {
                        vcf4(1.0,0.0,0.0,1.0), vcf4(0.0,1.0,0.0,1.0), vcf4(0.0,0.0,1.0,1.0),
                        vcf4(1.0,1.0,0.0,1.0), vcf4(1.0,0.0,1.0,1.0), vcf4(0.0,1.0,1.0,1.0),
                        vcf4(0.5,0.0,0.0,1.0), vcf4(0.0,0.5,0.0,1.0), vcf4(0.0,0.0,0.5,1.0),
                        vcf4(0.5,0.5,0.0,1.0), vcf4(0.5,0.0,0.5,1.0), vcf4(0.0,0.5,0.5,1.0)
                        };

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelCamera::PanelCamera(Camera* p)
{
    VarManager& var = VarManager::getInstance();

	pCamera				= p;
    echelle         	= 1.0;
    ech_geo				= 1.0;
    ech_user			= 1.0;
    _dx              	= 0.0;
    _dy              	= 0.0;
    
    pReadBgr        	= NULL;
    fTime           	= 0.5;
    fTime1          	= 0.0;
    bTime1         		= false;
    fSens          		= 1.0;
    fTimeClign      	= 0.8;
	fRefCatalogX    	= 0.0;
	fRefCatalogY    	= 0.0;
	fRefCatalogDecalX	= 0.0;
	fRefCatalogDecalY	= 0.0;

    setExtraString( "panelCamera" );
	
    if ( var.existe("bAffCatalog") )         bAffCatalog  = var.getb( "bAffCatalog");
    
    if (        Xref == -1.0 
            &&  Yref == -1.0
            &&  ZrefX == -1.0
            &&  ZrefY == -1.0
            &&  Wref == -1.0 )  {
        Xref = 525.0;
        Yref = 438.0;
        ZrefX = 934.0;
        ZrefY = 1018.0;
        Wref = 5.50;
    }
#ifdef TRUE_TYPE_TEST
	#define POLICE0 "fonts/almosnow.ttf"
	#define POLICE1 "fonts/baileysc.ttf"
	#define POLICE2 "fonts/biometri.ttf"
	#define POLICE3 "fonts/boron2.ttf"
	#define POLICE4 "fonts/grec.ttf"
	#define POLICE5 "fonts/burnstow.ttf"
	#define POLICE6 "fonts/ghostmea.ttf"
	#define POLICE7 "fonts/riotact.ttf"
	#define POLICE8 "fonts/rsfs10.ttf"
	#define POLICE9 "fonts/Oh_Beloved.ttf"
	#define POLICE10 "fonts/Magiera-Script.ttf"
	#define POLICE11 "fonts/GFSSolomos.otf"   			// 360 glyphe voir alpha ...
	#define POLICE12 "fonts/greek-wsi-regular.ttf"   	// 218 glyphe voir alpha ...
	#define POLICE13 "/usr/share/fonts/truetype/larabie/betsy.ttf"
	//add(new PanelText( 	(char*)"abcdefghijklmnopqrstuvwxyz", (char*)POLICE7 , 100, y, 150, 0xFF0095FA )  );


	#define TEXT00 "abcdefghijklmnopqrstuvwxyz"
	#define TEXT01 "ABCDEFGHIJKLMNOPQRSTUBWXYZ"
	#define TEXT02 "a c e g i k m o q s u w"
	#define TEXT03 "abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUBWXYZ"
	#define TEXT04 "Essai"
	#define TEXT05 
	int size, i;
	
	size = 100; y = 0;
	PanelText* t = new PanelText( 	(char*)TEXT04, (char*)POLICE1 , 0, y, size, 0x40FF00FF, true );
	add( t );
	t->setAlign( PanelText::CENTER );
	logf( (char*)"Size : %d", t->getPosDX() );
	y += size + 10;

	size = 100;	
	t = new PanelText( 	(char*)TEXT01, (char*)POLICE2 , 100, y, size, 0x40906043, true );
	add( t );
	y += size + 10;

	t = new PanelText( 	(char*)TEXT02, (char*)POLICE3 , 0, y, size, 0xC040FC83, true );
	add( t );
	t->setAlign( PanelText::RIGHT );
	y += size + 10;

	size = 60;	
	t = new PanelText( 	(char*)TEXT03, (char*)POLICE12, 0, y,  size, 0xFF00FFFF, true );
	add( t );
	t->setAlign( PanelText::CENTER );
	logf( (char*)"Size : %d", t->getPosDX() );
	y += size + 10;

	size = 50;	
	add(new PanelText( 	(char*)TEXT00, (char*)POLICE4 , 100, y, size, 0x40FF0010, true )  );
	y += size + 10;

	size = 50;	
	t = new PanelText( 	(char*)TEXT01, (char*)POLICE5 , 100, y, size, 0xFFFFFC89, true );
	add( t );
	t->setAlign( PanelText::CENTER );
	y += size + 10;

	size = 30;	
	add(new PanelText( 	(char*)TEXT02, (char*)POLICE13 , 100, y, size, 0xA030F100, true )  );
	y += size + 10;

	size = 80;	
	t = new PanelText( 	(char*)"Degouline", (char*)POLICE7 , 0, y, size, 0xFF0095FA, true );
	add( t );
	t->setAlign( PanelText::RIGHT );
	y += size + 10;

	size = 20;	
	add(new PanelText( 	(char*)"R.N.", (char*)POLICE8 ,        100, y, size, 0xFF0095FA, true )  );
	y += size + 10;

	size = 40;	
	add(new PanelText( 	(char*)"abcdefghijklmnopqrstuvwxyz", (char*)POLICE9 , 0, y, size, 0xFF0095FA, true )  );
	y += size + 10;

	size = 40;	
	t = new PanelText( 	(char*)"Nouvelle police.", (char*)POLICE10 ,        0, y, size, 0x8FFA00F6, true );
	add( t );
	t->setAlign( PanelText::CENTER );
	y += size + 10;

	size = 60;	
	t = new PanelText( 	(char*)"Rene Negre", (char*)POLICE10 ,        0, y, size, 0xFFFFFFFF, true ) ;
	add( t );
	t->setAlign( PanelText::RIGHT );
	y += size + 10;
#else
	#define TEXT03 "abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUBWXYZ &é(-è_çà)=+°^$¨£ù*%µ,;:!?./§²²"
	
	int y = 100;
	//add(new PanelText( 	(char*)TEXT03, 	PanelText::NORMAL_FONT, 10 , y  ) );
	y += 14;
#endif
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#include "camera_mgr.h"
void PanelCamera::idle(float f)
{
    VarManager& var = VarManager::getInstance();

	Camera* pCurrentCamera = Camera_mgr::getInstance().getCurrent();
	bool bIsCurrentCamera;
	
	bIsCurrentCamera  = (pCamera == pCurrentCamera);
	
	updatePos();
    stars.set_delta( getPosX(), getPosY() );
    computeColor();
    
    //----------------------------------------------
    //                 Vizier
    //----------------------------------------------
    if ( bIsCurrentCamera )
    {
	    //logf( (char*)"PanelCamera::displayVizier() %d etoiles", vizier.size() );;
		//compute_echelle();
	    //updatePos();
		updateVizizePos();	
	}
    //----------------------------------------------
	
    fTime += fSens * f;
    fTime1 += f;
    
    if ( fTime1 > 1.0 )
    {
        bTime1 = true;
        fTime1 -= 1.0;
    }
    
    if ( fTime > fTimeClign )
    {
        fSens = -1.0;
    }
    else if ( fTime < 0.0 )
    {
        fSens = 1.0;
    }
    //----------------------------------------------
	if ( bAffStar   )	{
		stars.setVisible(bAffStar && bIsCurrentCamera );
		stars.idle();
		stars.setModeMag(0);
		if ( pReadBgr!=NULL)		stars.suivi(pReadBgr);
		//log( (char*)"Stars::Idle()" );
	}	
	else	{
		stars.setVisible(false);
	}    
   
    PanelWindow::idle(f);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::update_stars()
{
    if  ( pReadBgr==NULL )      {
    	//logf( (char*)"PanelCamera::update_stars()   pointeur RB NULL" );
    	return;
    }
    //else            logf((char*)"PanelCamera::update() w=%d", pReadBgr->w);
    
	stars.setModeMag(0);
    stars.update_stars( getX(), getY(), this, pReadBgr );
    //logf((char*)"PanelCamera::update_stars() dx=%d dy=%d", getX(), getY() );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::releaseLeft(int xm, int ym)
{
    logf( (char*)"PanelCamera::releaseLeft(%d,%d) :%d", xm, ym, __LINE__ );
    if ( pReadBgr == NULL )     { logf( (char*)" return Pointeur NULL" ); return; }
    
    VarManager& var = VarManager::getInstance();

    log_tab(true);
    logf( (char*)"getDX=%d RB->w=%0.2f", getDX(), (double)pReadBgr->w.load() );
    
    double e = (double)getDX() / (double)pReadBgr->w.load(); 
    
    int xx = ((double)xm-(double)getX()) / e;
    int yy = ((double)ym-(double)getY()) / e;
    
    stars.setView( this );
    stars.setRB( pReadBgr );
    
    if( (iGlutModifier & GLUT_ACTIVE_CTRL ) )
    {
		double xx = xm, yy = ym;
		
		screen2tex(xx, yy);
		
		xSuivi = (double)xx;
		ySuivi = (double)yy;

		var.set( "xSuivi", xSuivi );		
		var.set( "ySuivi", ySuivi );		
		
		change_joy( xSuivi, ySuivi );

    	logf( (char*)"Touche control" );
    }
	else
	{
		if ( !bModeManuel && stars.addStar( xm, ym, getX(), getY(), e ) == NULL )
			stars.selectLeft(xx, yy);
	}

    log_tab(false);
    logf( (char*)"PanelCamera::releaseLeft(%d,%d) ...", xm, ym );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::releaseMiddle(int xm, int ym)
{
    if ( bMouseDeplace ) 
    {
        logf( (char*)"PanelCamera::releaseMiddle(%d,%d) ... deplacement MOUSE", xm, ym );
        return;
    }

    logf( (char*)"PanelCamera::releaseMiddle(%d,%d) ...", xm, ym );
    if ( pReadBgr == NULL )     { logf( (char*)"Pointeur NULL" ); return; }
    log_tab(true);
        
    logf( (char*)"panelCamera->getDX()=%d pReadBgr->w=%0.2f", getDX(), pReadBgr->w.load() );
    
    //double e = (double)getDX() / (double)pReadBgr->w; 
    double e = (double)getDX() / (double)1920.0; 
    
    int xx = ((double)xm-(double)getX()) / e;
    int yy = ((double)ym-(double)getY()) / e;
    
    stars.setView( this );
    stars.setRB( pReadBgr );
    if ( stars.addStar( xm, ym, getX(), getY(), e ) == NULL )
    {
        stars.selectMiddle(xx, yy);
        logf( (char*)"releaseMiddle(%d,%d) selects star...", xm, ym );
    }
    else
        stars.selectMiddle(xx, yy);
    
    log_tab(false);
    logf( (char*)"PanelCamera::releaseMiddle(%d,%d) ...", xm, ym );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::motionRight(int xm, int ym)
{
    Camera_mgr&     mgr = Camera_mgr::getInstance(); 
	mgr.onBottom();
	updatePos();    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::wheelUp(int xm, int ym)
{
    logf( (char*)"PanelCamera::wheelUp(%d,%d) ...", xm, ym );
    log_tab(true);
    
    
    _dx = getDX() / echelle;
    _dy = getDY() / echelle;
    
    double XM0 =  (double)xm;
    double YM0 =  (double)ym;
    screen2tex(XM0, YM0);
    
    double k = 1.1;
    ech_user *= k;
    echelle = ech_user * ech_geo;
    setSize( _dx*echelle, _dy*echelle );
    
    double X1 =  XM0 * echelle - (double)xm;
    double Y1 =  YM0 * echelle - (double)ym;

    setPos( -X1, -Y1 );
    updatePos();
    
    logf( (char*)"Nouvelle echelle %.2f  geo=%0.2f user=%0.2f", (float)echelle, (float)ech_geo, (float)ech_user );
    log_tab(false);
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::wheelDown(int xm, int ym)
{
    logf( (char*)"PanelCamera::wheelDown(%d,%d) ...", xm, ym );
    log_tab(true);

    _dx = getDX() / echelle;
    _dy = getDY() / echelle;

    double XM0 =  (double)xm;
    double YM0 =  (double)ym;
    screen2tex(XM0, YM0);
    
    double k = 0.9;
    ech_user *= k;
    if (ech_user<=0.1)           ech_user = 0.1;

    echelle = ech_user * ech_geo;
    setSize( _dx*echelle, _dy*echelle );

    double X1 =  XM0 * echelle - (double)xm;
    double Y1 =  YM0 * echelle - (double)ym;

    setPos( -X1, -Y1 );
    updatePos();    

    logf( (char*)"Nouvelle echelle %.2f  geo=%0.2f user=%0.2f", (float)echelle, (float)ech_geo, (float)ech_user );
    log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::updatePos()
{
    #ifdef DEBUG
    logf( (char*)"PanelCamera::updatePos() : %d", __LINE__ );
    #endif
    PanelWindow::updatePos();
    if (    getX()  != x_old  ||
            getY()  != y_old  ||
            getDX() != dx_old || 
            getDY() != dy_old )
    {
        update_stars();
    }
    x_old = getX();
    y_old = getY();
    dx_old = getDX();
    dy_old = getDY();
    
    stars.update_stars( getX(), getY(), this, pReadBgr, echelle );

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::clip(int& xm, int& ym)	{
    Panel* p = getParent();

    double ech = ech_user * ech_geo;

    double max_x = ech*pReadBgr->w - p->getDX();
    double max_y = ech*pReadBgr->h - p->getDY();
    
    double X1 = xm;
    double Y1 = ym;
    
    if ( X1<0 )             X1 = 0.0;
    else
    if ( X1>max_x )         X1 = max_x;
    
    if ( Y1<0 )             Y1 = 0.0;
    else
    if ( Y1>max_y )         Y1 = max_y;
    
    xm = X1;
    ym = Y1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::compute_centre()
{
    double fDX = (double)pReadBgr->w * ech_geo;
    double fDY = (double)pReadBgr->h * ech_geo;
    
    double fw = (double)pReadBgr->w;
    double fh = (double)pReadBgr->h;

    int dxp = getParent()->getDX();
    int dyp = getParent()->getDY();
    
    int deltax = ((fw/2.0 ) - ech_user * (fw/2.0 - dx )) * ech_geo;
    int deltay = ((fh/2.0 ) - ech_user * (fh/2.0 - dy )) * ech_geo;
    
    if ( deltax > 0 )           deltax = 0;
    if ( deltay > 0 )           deltay = 0;
    
    int maxX  = -(fDX*ech_user - dxp);
    int maxY  = -(fDY*ech_user - dyp);
    
    if ( deltax < maxX )        deltax = maxX;
    if ( deltay < maxY )        deltay = maxY;

    Panel* p = getParent();
    if ( p!= NULL )
    {
        //logf( (char*)"delta : %d", p->getPosY() );
        stars.set_delta( deltax + p->getPosX(), deltay + p->getPosY() );
    }
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::compute_echelle()
{
    //if  ( panelCourbe==NULL )   return;
    if  ( pReadBgr==NULL )      return;

	WindowsManager& wm = WindowsManager::getInstance();
    
    //      Dimension ecran
    double wSc = (double)wm.getWidth();
    double hSc = (double)wm.getHeight();

    //      Dimension texture
    double wTex = (double)pReadBgr->w;
    double hTex = (double)pReadBgr->h;

    //      Facteur d'echelle
    double ew = wSc / wTex;
    double eh = hSc / hTex;
    //ew = 1600.0/1920.0;
    //eh = 900.0/1080.0;
    ech_geo = ew<eh ? ew : eh;

	// Echelle totale
    echelle = ech_user * ech_geo;

    logf( (char*)"PanelCamera::compute_echelle %.2f  (%.2fx,%.2f) ...", (float)echelle, (float)ech_user, (float)ech_geo );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::computeColor()
{
    //logf( (char*)"PanelCamera::setColor()" );

    Camera_mgr& cam_mgr = Camera_mgr::getInstance();
    Camera* cam = cam_mgr.getCurrent();

    if ( cam == pCamera )  {
    	
    	if ( bNuit )	
    	{
			uint32_t color = 0xff0000ff;
			cam->getpCamFilename()->setColor(color);
			cam->getPanelNbStars()->setColor(color);
			cam->getPanelName()->setColor(color);
		}
		else
    	{
			uint32_t color = 0xffFFffFF;
			cam->getpCamFilename()->setColor(color);
			cam->getPanelNbStars()->setColor(color);
			cam->getPanelName()->setColor(color);
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::tex2screen(vec2& v)
{
    if  ( pReadBgr==NULL )      return;
    
    v.x = echelle * v.x + (double)getX();    
    v.y = echelle * v.y + (double)getY();    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::tex2screen(double& xx, double& yy)
{
    if  ( pReadBgr==NULL )      return;

	vec2 v = vec2( xx, yy );
	tex2screen(v);
	xx = v.x; yy = v.y;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::tex2screen(int& xx, int& yy)
{
    if  ( pReadBgr==NULL )      return;

	vec2 v = vec2( xx, yy );
	tex2screen(v);
	xx = (int)v.x; yy = (int)v.y;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::screen2tex(vec2& v)
{
    if  ( pReadBgr==NULL )      return;

	v -= vec2( getX(), getY() );
	v /= echelle;    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::screen2tex(double& xx, double& yy)
{
    if  ( pReadBgr==NULL )      return;

	vec2 v = vec2( xx, yy );
	screen2tex(v);
	xx = v.x; yy = v.y;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::screen2tex(int& xx, int& yy)
{
    if  ( pReadBgr==NULL )      return;

	vec2 v = vec2( xx, yy );
	screen2tex(v);
	xx = v.x; yy = v.y;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::glCercle(int x, int y, int rayon)
{
    double step = 300.0/(double)rayon;
    if ( step < 1.0 )           step = 1.0;
    
    //logf ( (char*)" rayon %d   step %0.2f", rayon, step );
    
	glBegin(GL_LINE_LOOP);

        for( double i=0; i<=360.0; i+=step )
        {
            double fx = (double)x+ (double)rayon*cos(DEG2RAD(i));
            double fy = (double)y+ (double)rayon*sin(DEG2RAD(i));
            glVertex2i(fx,fy);
        }
        
    glEnd();        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::glCarre( int x,  int y,  int dx,  int dy )
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
void PanelCamera::glCroix( int x,  int y,  int dx,  int dy )
{
	glBegin(GL_LINES);

	    glVertex2i(x, y-dy);         glVertex2i(x, y+dy);
	    glVertex2i(x-dx, y);         glVertex2i(x+dx, y);

    glEnd();        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::displayGLTrace(void)
{
    //if ( !bAffTrace )              return;

    //logf( (char*)"Affiche les traces" );
    int nbv = t_vTrace.size();
    
    glBegin(GL_LINES);
        
    int m = sizeof(colorTraces) / 16;
    //int m = 12;
    //logf( (char*)"Modulo : %d", m );

    for( int j=0; j<nbv; j++ )
    {
        
        glColor4fv( (GLfloat*)&colorTraces[j%m] );
            
        vector<vec2> *   trace = t_vTrace[j];
        int nb = trace->size();
        if ( nb == 1 )                  continue;
    
        for ( int i=0; i<nb-1; i++ )
        {
	        int x;
	        int y;
	        
	        x = round((*trace)[i].x);
	        y = round((*trace)[i].y);
	        tex2screen(x,y);
            glVertex2i(x,y);

	        x = round((*trace)[i+1].x);
	        y = round((*trace)[i+1].y);
	        tex2screen(x,y);
            glVertex2i(x,y);
        }
    }    
    glEnd();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::displayVecAD()
{
    VarManager& var = VarManager::getInstance();

    double gris = 0.0;
    if ( var.getb("bNuit") )        glColor4f( 0.5, 0.0, 0.0, 1.0 );
    else                            glColor4f( 1.0, gris, gris, 1.0 );
    
    glBegin(GL_LINES);
        int x = vecAD[0].x;
        int y = vecAD[0].y;
        tex2screen(x, y);
        glVertex2i( x, y );

        x = vecAD[1].x;
        y = vecAD[1].y;
        tex2screen(x, y);
        glVertex2i( x, y );

        glVertex2i( vecAD[1].x, vecAD[1].y );
    glEnd();        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::displayVecDC()
{
    VarManager& var = VarManager::getInstance();

    double gris = 0.0;
    if ( var.getb("bNuit") )        glColor4f( 0.5, 0.0, 0.0, 1.0 );
    else                            glColor4f( gris, 1.0, gris, 1.0 );
    
    glBegin(GL_LINES);
        int x = vecDC[0].x;
        int y = vecDC[0].y;
        tex2screen(x, y);
        glVertex2i( x, y );

        x = vecDC[1].x;
        y = vecDC[1].y;
        tex2screen(x, y);
        glVertex2i( x, y );

        glVertex2i( vecDC[1].x, vecDC[1].y );

    glEnd();        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::displaySuivi()
{
    if  ( panelCourbe==NULL )   return;
    if  ( pReadBgr==NULL )      return;

	WindowsManager& wm = WindowsManager::getInstance();

    vec2 vSuiviTex = vec2(xSuivi, ySuivi);
    vec2 vSuiviScr = vec2(xSuivi, ySuivi);
    tex2screen(vSuiviScr);
    
    //      Convertion des coordonnées
    double gris = 0.8;

    //      DEBUG
    //logf( (char*)"Screen (%0.2f,%0.2f)  texture(%0.2f,%0.2f)", wSc, hSc, wTex, hTex );
    //logf( (char*)"     suivi (%0.2f,%0.2f) -> (%0.2f,%0.2f)", xSuivi, ySuivi, x, y );
    vcf4 color;
    vcf4 cBleu = vcf4(0.2, 0.2, 1.0, 1.0);
    vcf4 cRoug = vcf4(1.0, 0.0, 0.0, 1.0);
    vcf4 cVert = vcf4(0.0, 1.0, 0.0, 1.0);

    //-----------------------------------------------------------------------------
    //      Affichage en mode nuit ?
    if ( bNuit )        color = vcf4( 1.0,   0.0,  0.0, gris );
    else				color = cVert;
    
    glColor4fv( color );
    
    //----- Affichage de la croix   ----------------------------------------------
	glLineWidth( SIZE_CERCLE_COLLIMATION );
    glCroix( vSuiviScr, 50 );
	glLineWidth( 1 );

    if ( !bNuit )			color = cBleu;
    glColor4fv( color );

    //----- Cercle de correction --------------------------------------------------
    vec2 vStarTex, vStarScr, vDiffText;
    
    vec2* pvStar = getSuivi();
    
    if ( pvStar != NULL )
    {
        vStarTex = vec2(pvStar->x, pvStar->y);
        vStarScr = vec2(pvStar->x, pvStar->y);
        tex2screen(vStarScr);

        vDiffText = vSuiviTex - vStarTex;
    }
    else
    {

        vStarScr = vec2(0.0,0.0);
        vDiffText = vec2(0.0,0.0);
    }     

    double l = vDiffText.length();
    double f = fTime/fTimeClign;
    bool bHorsLimite =  l>=fLimitCorrection0;
    
    
    if ( f>1.0)     f = 1.0;
    else
    if ( f<0.0)     f = 0.0;
        
    f = f * 0.33 + 0.66;

    if ( bNuit )                                    glColor4f( 1.0,   0.0,  0.0, gris/2.0 );
    else
    {
        if (!bHorsLimite )                          glColor4f( 0.0,   1.0,  0.0, f );
        else                                        glColor4f( 1.0,   0.0,  0.0, f );
    }
    
    //----- Cercle de correction --------------------------------------------------
	glLineWidth( SIZE_CERCLE_COLLIMATION );


    glLineStipple(2, 0xFF00);
    glEnable(GL_LINE_STIPPLE);    

    if ( bAffColimation )
    {
    	glColor4fv( cRoug );
	    glCercle( vSuiviScr, echelle*(fDiamSuivi1) );
	}

    if ( !bCorrection )			glEnable(GL_LINE_STIPPLE);    
    else						glDisable(GL_LINE_STIPPLE);    

    glColor4fv( color );
    glCercle( vSuiviScr, echelle*(fLimitCorrection0) );
    
    glColor4fv( color );
	glLineWidth( 1 );

    //----- Droite de correction --------------------------------------------------
    if ( pvStar!=NULL && vStarTex!=vec2(0.0,0.0) && vSuiviTex!=vec2(0.0,0.0)  )
    {
	    glBegin(GL_LINES);
            glVertex2i( (int)vStarScr.x,  (int)vStarScr.y);                  
            glVertex2i((int)vSuiviScr.x, (int)vSuiviScr.y );
        glEnd();

    }

    if ( !bCorrection )
        glDisable(GL_LINE_STIPPLE);    

    
    if ( bNuit )                                    glColor4f( 1.0,   0.0,  0.0, gris/2.0 );
    else
    {
                                                    glColor4f( 1.0,   1.0,  0.0, f );
    }
    
    if ( bCentrageSuivi )
    {
        vec2 u = vec2(xSuiviSvg, ySuiviSvg);
        tex2screen( u );
        //glCroix( u.x, u.y, 10, 10 );
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::displayCentre()
{
    if  ( pReadBgr==NULL )  return;

	WindowsManager& wm = WindowsManager::getInstance();

    int x = wm.getWidth();
    int y = wm.getHeight();

    double gris = 0.3;
    if ( bNuit )        glColor4f( 1.0,   0.0,  0.0, gris );
    else                glColor4f( 0.0,   1.0,  0.0, gris );    
    
	glLineWidth( 1 );
	glBegin(GL_LINES);
        
        glVertex2i(x/2,0);              glVertex2i(x/2, y );
        glVertex2i(0,y/2);              glVertex2i(x, y/2 );

    glEnd();
	glLineWidth( 1 );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::displayVizier()
{
    int n = vizier.size();
    vec2 v;
    
    if ( n!= 0)
    {
	    //logf( (char*)"PanelCamera::displayVizier() %d etoiles", n );;
        if ( bNuit )        glColor4f( 1.0,  0.0,  0.0, 1.0 );
        else                glColor4f( 1.0, 0.7, 0.0, 0.8 );

        for ( int i=0; i<n; i++ )
        {

            v.x = vizier.get(i)->getXScreen();
            v.y = vizier.get(i)->getYScreen();

			if ( 	v.x != -1.0  )
			{
				PanelText* p = vizier.get(i)->getInfo();
		        double r = 17.5  - 0.675*vizier.get(i)->fMag;
		        if ( bAffStar && stars.size() != 0 ) 	r += 5;
			
		        glCercle( v.x, v.y, r*echelle );
				p->displayGL();
				//p->setVisible( true );
			}
        }
    }    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::displayGL()
{

    double gris = 0.3;
    VarManager& var = VarManager::getInstance();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

    if ( bNuit )        glColor4f( gris,  0.0,  0.0, 1.0 );
    else                glColor4f( gris, gris, gris, 0.2 );    
    
    PanelWindow::displayGL();

    if ( bNuit )        glColor4f( gris,  0.0,  0.0, 1.0 );
    else                glColor4f( 0.0,   1.0,  0.0, 0.4 );    

    
	Camera* pCurrentCamera = Camera_mgr::getInstance().getCurrent();
	bool bIsCurrentCamera  = (pCamera == pCurrentCamera);

    if ( bIsCurrentCamera )
    {
		
		stars.displayGL();
    
		if ( bAffCentre )           displayCentre();
		if ( bAffSuivi )            displaySuivi();
		if ( bAffCatalog )          displayVizier();
		if ( bAffTrace )            displayGLTrace();
		if ( bAfficheVec)           { displayVecAD(); displayVecDC(); }

		if ( bModeManuel )
		{
		    if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 0.2 );
			else                            glColor4f( 0.0, 1.0, 0.0, 0.2 );
			
			int x = xClick;
			int y = yClick;

			tex2screen(x,y);

			vDeplaceDepuis.x = x;
			vDeplaceDepuis.y = y;
			
			//glColor4f( 0.0, 1.0, 0.0, 1.0 );
			glCroix(x, y, 50, 50);
		    glCercle(x, y, 25);
		}

		if ( bMouseDeplaceVers )
		{
		    if ( var.getb("bNuit") )			glColor4f( 1.0, 0.0, 0.0, 0.2 );
			else								glColor4f( 0.0, 1.0, 0.0, 0.2 );
			
			int x = mouse.x;
			int y = mouse.y;

			glBegin(GL_LINES);
				glVertex2i(x,y);                glVertex2i(vDeplaceDepuis.x, vDeplaceDepuis.y);
			glEnd();        

			glCroix(x, y, 50, 50);
		}
		
    }
    //else
    //	log( (char*)"Pleiade ss not current" );
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::findAllStar()
{
    logf( (char*)"PanelCamera::findAllStar()" );
    stars.setView( this );
    stars.setRB( pReadBgr );
    stars.findAllStars();
    update_stars();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool PanelCamera::starExist(int x, int y)
{
    return stars.starExist(x,y);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::addStar(int x, int y)
{
    stars.setView( this );
    stars.setRB( pReadBgr );
    //stars.addStar(x, y);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::setRB(rb_t* p)
{
	bool bCompute = false;
	if ( pReadBgr == NULL )		bCompute = true;
	if ( p		  == NULL )		bCompute = false;
	
    pReadBgr = p;
    stars.setRB(p);
    update_stars();
    if( bCompute)			compute_echelle();

    //logf((char*)"PanelCamera::setRB() w=%d", pReadBgr->w);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
vec2* PanelCamera::getSuivi()
{
    return stars.getSuivi();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::add_catalogue(StarCatalog* p)
{
    vizier.add( p );
    this->add(p->getInfo());
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::setRefCatalog(double _0, double _1)
{
    logf( (char*)"PanelCamera::setRefCatalog(%0.4f,%0.4f) ...", _0, _1 );
    fRefCatalogX = _0;
    fRefCatalogY = _1;
    
    fRefCatalogDecalX = stars.getSuiviScreenX();
    fRefCatalogDecalY = stars.getSuiviScreenY();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::updateVizizePos()
{
	//logf( (char*)"PanelCamera::updateVizizePos()" );

    if ( !bAffCatalog )					return;

    int n = vizier.size();
	vec2 v;
	if ( n!=0 )
	{
		//logf( (char*)"PanelCamera::updateVizizePos()" );
		vec2 vMin = vec2( getX()			, getY() );
		vec2 vMax = vec2( getX()+getDX()  	, getY()+getDY()  );

	    for ( int i=0; i<n; i++ )
	    {
			PanelText* p = vizier.get(i)->getInfo();
			p->setVisible(true);

	        mat2 m;		       m.rotate( Wref );
	        vec2 vHomothetie = vec2(ZrefX, ZrefY);
	        vec2 vTranslatio = vec2(Xref , Yref);

	        vec2 v = vec2( vizier.get(i)->fRA, vizier.get(i)->fDE) - vec2(fRefCatalogX, fRefCatalogY);
	        vec2 vViz = m * v;
	        vViz = -vViz * vHomothetie  + vTranslatio;
	        
	        vec2 vScreen = vec2(vViz);
			tex2screen(vScreen);
			/*
			*/
			if ( 	vScreen.x> vMin.x   &&	vScreen.x < vMax.x  
				&& 	vScreen.y> vMin.y   &&	vScreen.y < vMax.y  )
			{
				vizier.get(i)->setScreen( vScreen );
				vizier.get(i)->setTex( vViz );

				double r = 17.5 - 0.675*vizier.get(i)->fMag;
		        if ( bAffStar && stars.size() != 0 ) 	r += 5;
				p->setPos( vScreen.x + echelle*r*0.707,    vScreen.y - echelle*r*0.707 - 15.0 );
				p->updatePos();
				p->setAlpha(0.0);
		        p->setVisible(true);
			}
			else
			{
				vec2 vOut = vec2(-1.0,-1.0);
			    vizier.get(i)->setScreen( vOut );
			    vizier.get(i)->setTex(    vOut );
			    
				p->setVisible(false);
			}

	        if ( bNuit )        p->setColor( COLOR32(255,   0, 0, 255) );
	        else                p->setColor( COLOR32(255, 178, 0, 255) );	    }
	}    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::recentreSuivi()
{
    logf( (char*)"PanelCamera::recentreSuivi()" );
    log_tab(true);

	WindowsManager& wm = WindowsManager::getInstance();

    int w = wm.getWidth();
    int h = wm.getHeight();

    vec2 vSuiviTex = vec2(xSuivi, ySuivi);
    tex2screen( vSuiviTex );
    vec2 vSize( getDX(), getDY() );
    vec2 vPos( getPosX(), getPosY() );
    vec2 vScr2( w/2, h/2 );
    
    vPos -= vSuiviTex - vScr2;
    setPos( vPos.x, vPos.y );
    updatePos();
    
    logf( (char*)"Suivi (%0.2lf,%0.2lf)", vSuiviTex.x, vSuiviTex.y );

    log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

