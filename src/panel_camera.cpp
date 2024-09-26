#include "panel_camera.h"
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
vec4                colorTraces[] = 
                        {
                        vec4(1.0,0.0,0.0,1.0), vec4(0.0,1.0,0.0,1.0), vec4(0.0,0.0,1.0,1.0),
                        vec4(1.0,1.0,0.0,1.0), vec4(1.0,0.0,1.0,1.0), vec4(0.0,1.0,1.0,1.0),
                        vec4(0.5,0.0,0.0,1.0), vec4(0.0,0.5,0.0,1.0), vec4(0.0,0.0,0.5,1.0),
                        vec4(0.5,0.5,0.0,1.0), vec4(0.5,0.0,0.5,1.0), vec4(0.0,0.5,0.5,1.0)
                        };

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelCamera::PanelCamera()
{
    VarManager& var = VarManager::getInstance();

    echelle         	= 1.0;
    dx              	= 0.0;
    dy              	= 0.0;
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
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#include "camera_mgr.h"
void PanelCamera::idle(float f)
{
    int color = 0;
    if ( bNuit )        color = 0xFFFF0000;
    else                color = 0xFFFFFFFF;

    stars.set_delta( getPosX(), getPosY() );
    
    Camera_mgr& cam_mgr = Camera_mgr::getInstance();
    Camera* cam = cam_mgr.getCurrent();
    cam->getpCamFilename()->setColor(color);
    cam->getPanelNbStars()->setColor(color);
    cam->getPanelName()->setColor(color);

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
    //logf( (char*)" vizier.size() = %d, %s", n, BOOL2STR(bAffCatalog) );;
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
            
            double x = -w.x * ZrefX + fRefCatalogDecalX + getX() + Xref;
            double y = -w.y * ZrefY + fRefCatalogDecalY + getY() + Yref;

    	    //logf( (char*)" set (%0.8f, %0.8f)", fRefCatalogDecalX, ZrefY );;
    	    //logf( (char*)" set (%0.8f, %0.8f)", fRefCatalogDecalX, ZrefY );;
    	    //logf( (char*)" set (%0.8f, %0.8f)", fRefCatalogDecalY, fRefCatalogDecalY );;
            
            vizier.get(i)->setXScreen(x);
            vizier.get(i)->setYScreen(y);
    
        
            PanelText* pInfo = vizier.get(i)->pInfo;
            if ( pInfo->getParent() == NULL )    this->add(pInfo);

            pInfo->setPos(x+10-getX(), y-30-getY());
            pInfo->setVisible(true);

            //logf( (char*)"%08X", (int)color );
            if ( bNuit )        color = 0xFFFF0000;
            else                color = 0xCCFFB200;
            pInfo->setColor(color);
        }
    }    
    else
    if ( n!= 0)
    {
        for ( int i=0; i<n; i++ )
        {
            PanelText* pInfo = vizier.get(i)->pInfo;
            pInfo->setVisible(true);
        }
    }    
    //----------------------------------------------
    
    stars.idle();
    PanelWindow::idle(f);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::findAllStar()
{
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
void PanelCamera::setCentX(float f)
{
    dx = f;

    float fDX = (float)pReadBgr->w / echelle;
    float fDY = (float)pReadBgr->h / echelle;

    int dxp = getParent()->getDX();
    int dyp = getParent()->getDY();
    
    int deltax = (dxp-fDX)/2 + dx*echelle;
    int deltay = (dyp-fDY)/2 + dy*echelle;
    
    setPos( deltax, deltay );
    updatePos();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::setCentY(float f)
{
    dy = f;

    float fDX = (float)pReadBgr->w / echelle;
    float fDY = (float)pReadBgr->h / echelle;

    int dxp = getParent()->getDX();
    int dyp = getParent()->getDY();
    
    int deltax = (dxp-fDX)/2 + dx*echelle;
    int deltay = (dyp-fDY)/2 + dy*echelle;
    
    setPos( deltax, deltay );
    updatePos();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::setEchelle(float f)
{
    logf( (char*)"setEchelle(%0.2f)", f );
    echelle = f;
    float fDX = (float)pReadBgr->w / echelle;
    float fDY = (float)pReadBgr->h / echelle;
    
    int dxp = getParent()->getDX();
    int dyp = getParent()->getDY();

    if ( fDX < dxp )
    {
        echelle = (float)pReadBgr->w/(float)dxp;
        fDX = dxp;
        fDY = dyp;
        logf( (char*)"Stop setEchelle(%0.2f)", echelle );
    }
    
    int deltax = (dxp-fDX)/2 + dx*echelle;
    int deltay = (dyp-fDY)/2 + dy*echelle;
    
    setSize( fDX, fDY );
    setPos( deltax, deltay );
    updatePos();
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
void PanelCamera::compute_echelle()
{
    if  ( panelCourbe==NULL )   return;
    if  ( pReadBgr==NULL )      return;

	WindowsManager& wm = WindowsManager::getInstance();
    
    //      Dimension ecran
    float wSc = (float)wm.getWidth();
    float hSc = (float)wm.getHeight();

    //      Dimension texture
    float wTex = (float)pReadBgr->w;
    float hTex = (float)pReadBgr->h;

    //      Facteur d'echelle
    float ew = wSc / wTex;
    float eh = hSc / hTex;
    //ew = 1600.0/1920.0;
    //eh = 900.0/1080.0;
    echelle = ew<eh ? ew : eh;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::tex2screen(vec2& v)
{
    if  ( panelCourbe==NULL )   return;
    if  ( pReadBgr==NULL )      return;
    
    compute_echelle();

    v.x = echelle * v.x + (float)getX();    
    v.y = echelle * v.y + (float)getY();    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::tex2screen(float& xx, float& yy)
{
    if  ( panelCourbe==NULL )   return;
    if  ( pReadBgr==NULL )      return;

    compute_echelle();

    xx = echelle * xx + (float)getX();    
    yy = echelle * yy + (float)getY();    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::tex2screen(int& xx, int& yy)
{
    if  ( panelCourbe==NULL )   return;
    if  ( pReadBgr==NULL )      return;

    float XX = xx;
    float YY = yy;
    
    tex2screen(XX, YY);

    xx = (int)XX;
    yy = (int)YY;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::screen2tex(float& xx, float& yy)
{
    if  ( panelCourbe==NULL )   return;
    if  ( pReadBgr==NULL )      return;

    compute_echelle();

    xx = (xx - (float)getX()) / echelle;    
    yy = (yy - (float)getY()) / echelle;    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::screen2tex(int& xx, int& yy)
{
    if  ( panelCourbe==NULL )   return;
    if  ( pReadBgr==NULL )      return;

    float XX = xx;
    float YY = yy;
    
    screen2tex(XX, YY);

    xx = (int)XX;
    yy = (int)YY;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::glCercle(int x, int y, int rayon)
{
    float step = 300.0/(float)rayon;
    if ( step < 1.0 )           step = 1.0;
    
    //logf ( (char*)" rayon %d   step %0.2f", rayon, step );
    
	glBegin(GL_LINE_LOOP);

        for( float i=0; i<=360.0; i+=step )
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
void PanelCamera::glVecAD()
{
    VarManager& var = VarManager::getInstance();

    float gris = 0.0;
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
void PanelCamera::glVecDC()
{
    VarManager& var = VarManager::getInstance();

    float gris = 0.0;
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
    float gris = 0.8;

    //      DEBUG
    //logf( (char*)"Screen (%0.2f,%0.2f)  texture(%0.2f,%0.2f)", wSc, hSc, wTex, hTex );
    //logf( (char*)"     suivi (%0.2f,%0.2f) -> (%0.2f,%0.2f)", xSuivi, ySuivi, x, y );
    vec4 color;
    vec4 cBleu = vec4(0.5, 0.5, 1.0, 1.0);

    //-----------------------------------------------------------------------------
    //      Affichage en mode nuit ?
    if ( bNuit )        glColor4f( 1.0,   0.0,  0.0, gris );
    else                glColor4f( 0.5,   0.4,  0.5, gris );

    if ( bNuit )        color = vec4( 1.0,   0.0,  0.0, gris );
    else                color = vec4( 0.5,   0.4,  0.5, gris );
    
    glColor4fv( color );
    //----- Affichage de la croix   ----------------------------------------------
    glCroix( vSuiviScr, 50 );

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

    float l = vDiffText.length();
    float f = fTime/fTimeClign;
    bool bHorsLimite =  l>=fLimitCorrection;
    
    
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
        glLineStipple(1, 0xFF00);
        glEnable(GL_LINE_STIPPLE);    
    }
    else
        glColor4fv( cBleu );

    glCercle( vSuiviScr, echelle*(fLimitCorrection) );
    glColor4fv( color );

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
        glCroix( u.x, u.y, 10, 10 );
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

    float gris = 0.3;
    if ( bNuit )        glColor4f( 1.0,   0.0,  0.0, gris );
    else                glColor4f( 0.0,   1.0,  0.0, gris );    
    
	glBegin(GL_LINES);
        
        glVertex2i(x/2,0);              glVertex2i(x/2, y );
        glVertex2i(0,y/2);              glVertex2i(x, y/2 );

    glEnd();
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
            
		    //logf( (char*)" get (%0.8f, %0.8f)", x, y );;
			//vizier.get(i)->pInfo->setPos(x+10-getX(), y-80);

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
    //logf( (char*)"*** PanelCamera::displayGL() ***" );
    //if ( pReadBgr != NULL )
     //   logf( (char*)"*** PanelCamera::displayGL()  %X", (long)pReadBgr->ptr );

    float gris = 0.3;
    VarManager& var = VarManager::getInstance();
    //if ( var.getb("bNuit") )        glColor4f( 0.5, 0.0, 0.0, 1.0 );    
    //else                            glColor4f( gris, gris, gris, 0.2 );

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

    //float x = getParent()->getX();
    //float y = getParent()->getY();

    float x = getX();
    float y = getY();

    float X = -dx;
    float Y = -dy;

    float dx = getDX()/2;
    float dy = getDY()/2;



    if ( bNuit )        glColor4f( gris,  0.0,  0.0, 1.0 );
    else                glColor4f( gris, gris, gris, 0.2 );    
    
    PanelWindow::displayGL();



    if ( bNuit )        glColor4f( gris,  0.0,  0.0, 1.0 );
    else                glColor4f( 0.0,   1.0,  0.0, 0.4 );    

    stars.displayGL();
    
    if ( bAffCentre )           displayCentre();
    if ( bAffSuivi )            displaySuivi();
    if ( bAffCatalog )          displayVizier();
    if ( bAffTrace )            displayGLTrace();
    if ( bAfficheVec)           { glVecAD(); glVecDC(); }

    if ( bModeManuel )
    {
        if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 0.2 );
	    else                            glColor4f( 0.0, 1.0, 0.0, 0.2 );
	    
	    int x = xClick;
	    int y = yClick;

	    tex2screen(x,y);

		vDeplaceDepuis.x = x;
		vDeplaceDepuis.y = y;
		
	    glCroix(x, y, 50, 50);
        glCercle(x, y, 25);
    }

    if ( bMouseDeplaceVers )
    {
        if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 0.2 );
	    else                            glColor4f( 0.0, 1.0, 0.0, 0.2 );
	    
	    int x = mouse.x;
	    int y = mouse.y;

		glBegin(GL_LINES);
		    glVertex2i(x,y);                glVertex2i(vDeplaceDepuis.x, vDeplaceDepuis.y);
		glEnd();        

	    glCroix(x, y, 50, 50);
        //glCercle(x, y, 25);
    }
    
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

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
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::releaseLeft(int xm, int ym)
{
    logf( (char*)"PanelCamera::releaseLeft(%d,%d) :%d", xm, ym, __LINE__ );
    if ( pReadBgr == NULL )     { logf( (char*)" return Pointeur NULL" ); return; }
    
    log_tab(true);
    logf( (char*)"getDX=%d RB->w=%0.2f", getDX(), (float)pReadBgr->w.load() );
    
    float e = (float)getDX() / (float)pReadBgr->w.load(); 
    //float e = (float)getDX() / (float)1920.0; 
    
    int xx = ((float)xm-(float)getX()) / e;
    int yy = ((float)ym-(float)getY()) / e;
    
    stars.setView( this );
    stars.setRB( pReadBgr );
    
    if ( !bModeManuel && stars.addStar( xm, ym, getX(), getY(), e ) == NULL )
        stars.selectLeft(xx, yy);

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
    
    //float e = (float)getDX() / (float)pReadBgr->w; 
    float e = (float)getDX() / (float)1920.0; 
    
    int xx = ((float)xm-(float)getX()) / e;
    int yy = ((float)ym-(float)getY()) / e;
    
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

