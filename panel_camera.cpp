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
    echelle         = 1.0;
    dx              = 0.0;
    dy              = 0.0;
    pReadBgr        = NULL;
    fTime           = 0.5;
    fTime1          = 0.0;
    bTime1          = false;
    fSens           = 1.0;
    fTimeClign      = 0.8;
    
    setExtraString( "panelCamera" );

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCamera::idle(float f)
{
    stars.set_delta( getPosX(), getPosY() );
    
    
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
    if ( !bAffTrace )              return;

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

    //-----------------------------------------------------------------------------
    //      Affichage en mode nuit ?
    if ( bNuit )        glColor4f( 1.0,   0.0,  0.0, gris );
    else                glColor4f( 0.5,   0.4,  0.5, gris );    
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
        glLineStipple(1, 0xFFF0);
        glEnable(GL_LINE_STIPPLE);    
    }

    glCercle( vSuiviScr, echelle*(fLimitCorrection) );

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

    //*
    if ( bModeManuel )
    {
        if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 0.2 );
	    else                            glColor4f( 0.0, 1.0, 0.0, 0.2 );
	    
	    int x = xClick;
	    int y = yClick;

	    //logf( (char*)"--(%d,%d)", x, y );
	    tex2screen(x,y);
	    //logf( (char*)"  (%d,%d)", x, y );

	    glCroix(x, y, 50, 50);
        glCercle(x, y, 25);

    }

    if ( bAfficheVec)
    {
        glVecAD();
        glVecDC();
    }

    displayGLTrace();
    //*/
    
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
    logf( (char*)"getDX=%d RB->w=%0.2f", getDX(), pReadBgr->w.load() );
    
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

