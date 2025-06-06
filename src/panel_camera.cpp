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
	
	if ( pCurrentCamera != NULL ) {
		bIsCurrentCamera  = this == pCurrentCamera->getPanelPreview();
	}
	else
		bIsCurrentCamera = false;
	
 
    int color = 0;
//    if ( bNuit )        color = 0xFF0000FF;
//    else                color = 0xFFFFFFFF;

    stars.set_delta( getPosX(), getPosY() );
    
    Camera_mgr& cam_mgr = Camera_mgr::getInstance();
    Camera* cam = cam_mgr.getCurrent();
    if ( cam != NULL )  {
        cam->getpCamFilename()->setColor(color);
        cam->getPanelNbStars()->setColor(color);
        cam->getPanelName()->setColor(color);
    }
    //pCamFilename
    
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
    //                 Vizier
    //----------------------------------------------
    int n = vizier.size();

    if ( bIsCurrentCamera )
    {
		if ( bAffCatalog && n!=0 )
		{
		    for ( int i=0; i<n; i++ )
		    {

		        double xx = (vizier.get(i)->fRA - fRefCatalogX );
		        double yy = (vizier.get(i)->fDE - fRefCatalogY );

		        vec3 v = vec3( xx, yy, 1.0 );
		        mat3 m;
		        m.rotate( vec3(0.0,0.0,1.0), Wref );
		        vec3 w = m * v;
		        
		        double x = -w.x * ZrefX  + fRefCatalogDecalX + getX() + Xref;
		        double y = -w.y * ZrefY  + fRefCatalogDecalY + getY() + Yref;

			    //logf( (char*)" set (%0.8f, %0.8f)", fRefCatalogDecalX, ZrefY );;
			    //logf( (char*)" set (%0.8f, %0.8f)", fRefCatalogDecalX, ZrefY );;
			    //logf( (char*)" set (%0.8f, %0.8f)", fRefCatalogDecalY, fRefCatalogDecalY );;
		        
		        vizier.get(i)->setXScreen(x);
		        vizier.get(i)->setYScreen(y);
		
		    
		        PanelText* pInfo = vizier.get(i)->pInfo;
		        if ( pInfo->getParent() == NULL )    this->add(pInfo);

		        pInfo->setPos(x+10-getX(), y-30-getY());
		        pInfo->setVisible( true );

		        //logf( (char*)"%08X", (int)color );
		        if ( bNuit )        color = 0xFF0000FF;
		        else                color = 0xFFB200CC;
		        pInfo->setColor(color);
		        pInfo->setVisible( bIsCurrentCamera );
		    }
		}    
		else
		if ( n!= 0)
		{
		    for ( int i=0; i<n; i++ )
		    {
		        PanelText* pInfo = vizier.get(i)->pInfo;
		        pInfo->setVisible(false);
		    }
		}    
	}
    //----------------------------------------------
	if ( bAffStar   )	{
		stars.setVisible(bAffStar && bIsCurrentCamera );
		stars.idle();
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
    if  ( pReadBgr==NULL )      logf( (char*)"PanelCamera::update_stars()   pointeur RB NULL" );
    //else            logf((char*)"PanelCamera::update() w=%d", pReadBgr->w);
    
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
    
    if ( !bModeManuel && stars.addStar( xm, ym, getX(), getY(), e ) == NULL )
        stars.selectLeft(xx, yy);

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
    echelle = ech_user * ech_geo;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::wheelUp(int xm, int ym)
{
    logf( (char*)"PanelCapture::wheelUp(%d,%d) ...", xm, ym );
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
    
    //stars.update_stars( getX(), getY(), this, pReadBgr, ech );

    log_tab(false);
    
    updatePos();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::wheelDown(int xm, int ym)
{
    logf( (char*)"PanelCapture::wheelDown(%d,%d) ...", xm, ym );
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

    //stars.updateScreenPos( dx+pCapture->getX(), dy+pCapture->getY(), ech);
    //stars.updateScreenPos( dx+getX(), dy, echelle);

    log_tab(false);
}
/*
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::wheelUp(int xm, int ym)
{
	logf( (char*)"PanelCamera::wheelUp(%d,%d) :%d", xm, ym, __LINE__ );
    log_tab(true);
    
    double k = 1.1;
    ech_user *= k;
    
    Panel* p = getParent();

    double XM =  ( xm- getX() );
    double X0 = -( getPosX() ); 
    double X1 = k*X0 + (k-1)*XM;
    
    double YM =  ( ym- getY() );
    double Y0 = -( getPosY() ); 
    double Y1 = k*Y0 + (k-1)*YM;
    
    double ech = ech_user * ech_geo;
    double max_x = (ech*(double)pReadBgr->w) - getDX();
    double max_y = (ech*(double)pReadBgr->h) - getDY();
    echelle = ech;

    setPos( -X1, -Y1 );
    dx = -X1;
    dy = -Y1;
    
    //stars.update_stars( getX(), getY(), this, pReadBgr, ech );

    log_tab(false);
    
    updatePos();

    stars.updateScreenPos( dx+getX(), dy, ech);
}
*/
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::tex2screen(vec2& v)
{
    //if  ( panelCourbe==NULL )   return;
    if  ( pReadBgr==NULL )      return;
    
    //compute_echelle();

    v.x = echelle * v.x + (double)getX();    
    v.y = echelle * v.y + (double)getY();    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::tex2screen(double& xx, double& yy)
{
    //if  ( panelCourbe==NULL )   return;
    if  ( pReadBgr==NULL )      return;

    //compute_echelle();

    xx = echelle * xx + (double)getX();    
    yy = echelle * yy + (double)getY();    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::tex2screen(int& xx, int& yy)
{
    //if  ( panelCourbe==NULL )   return;
    if  ( pReadBgr==NULL )      return;

    double XX = xx;
    double YY = yy;
    
    tex2screen(XX, YY);

    xx = (int)XX;
    yy = (int)YY;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::screen2tex(double& xx, double& yy)
{
    //if  ( panelCourbe==NULL )   return;
    if  ( pReadBgr==NULL )      return;

    //compute_echelle();

    xx = (xx - (double)getX()) / echelle;    
    yy = (yy - (double)getY()) / echelle;    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::screen2tex(int& xx, int& yy)
{
    //if  ( panelCourbe==NULL )   return;
    if  ( pReadBgr==NULL )      return;

    double XX = xx;
    double YY = yy;
    
    screen2tex(XX, YY);

    xx = (int)XX;
    yy = (int)YY;
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
    //if ( bNuit )        glColor4f( 1.0,   0.0,  0.0, gris );
    //else                glColor4f( 0.5,   0.4,  0.5, gris );

    if ( bNuit )        color = vcf4( 1.0,   0.0,  0.0, gris );
    
    if ( !bNuit )			color = cVert;
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
    if ( !bCorrection )
    {
        glLineStipple(2, 0xFF00);
        glEnable(GL_LINE_STIPPLE);    
    }
    else
        glColor4fv( cRoug );

	glLineWidth( SIZE_CERCLE_COLLIMATION );
    glColor4fv( color );
    glCercle( vSuiviScr, echelle*(fLimitCorrection0) );
    
    glColor4fv( cRoug );
    glCercle( vSuiviScr, echelle*(fDiamSuivi1) );
    
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
    //logf( (char*)"PanelCamera::displayVizier() %d etoiles", n );;
    if ( n!= 0)
    {
        if ( bNuit )        glColor4f( 1.0,  0.0,  0.0, 1.0 );
        else                glColor4f( 1.0, 0.7, 0.0, 0.8 );

        for ( int i=0; i<n; i++ )
        {

            double x = vizier.get(i)->getXScreen();
            double y = vizier.get(i)->getYScreen();
            
            double r = (12.0 - vizier.get(i)->fMag ) * 1.8;
            glCercle( x, y, r );
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
	bool bIsCurrentCamera;
	
	if ( pCurrentCamera != NULL ) {
		bIsCurrentCamera  = this == pCurrentCamera->getPanelPreview();
	}
	else
		bIsCurrentCamera = false;
	
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
    pReadBgr = p;
    stars.setRB(p);
    update_stars();

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
    /*
    double xx = (p->getRA() - fRefCatalogX );
    double yy = (p->getDE() - fRefCatalogY );

    vec3 v = vec3( xx, yy, 1.0 );
    mat3 m;
    m.rotate( vec3(0.0,0.0,1.0), Wref );
    vec3 w = m * v;
    
    double x = -w.x * ZrefX + fRefCatalogDecalX + getX() + Xref;
    double y = -w.y * ZrefY + fRefCatalogDecalY + getY() + Yref;
    
    logf( (char*)"PanelCamera::add_catalogue(%0.4f,%0.4f) ...", x, y );
    //p->getInfo()->setX(x);
    //p->getInfo()->setY(y);
    p->getInfo()->setPos(x,y);
    */
    
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
    
    //Xref = 0.0;
    //Yref = 0.0;
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

