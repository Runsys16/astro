#include "panel_capture.h"
#include "captures.h"
//--------------------------------------------------------------------------------------------------------------------
//#define DEBUG_WHEEL
//--------------------------------------------------------------------------------------------------------------------
#define DEG_MS
#ifdef DEG_MS
#else
#endif
//--------------------------------------------------------------------------------------------------------------------
#define TIMEANIM 1.5
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelCapture::PanelCapture( struct readBackground*  pReadBgr, Capture* pc )
{
	vTelescopeJ2000 = vec2(0.0, 0.0);
    ech_geo     = 1.0;
    ech_user    = 1.0;
    dx          = 0.0;
    dy          = 0.0;
    xm_old      = -1;
    ym_old      = -1;
    dTimeAnim	= 0.0;
    
    pReadBgr    = pReadBgr;
    pCapture    = pc;
/*
    bIcone      = false;
	bInfoSouris = false;
	bAffGrille	= false;
*/
	bAffCatalogPosition = false;
    stars.setView( this );
    
    pFondCoord = new PanelSimple();
    pFondCoord->setBackground( (char*)"images/background.tga" );
    pFondCoord->setSize( 210, 65 );
    this->add( pFondCoord );
    
    pTelescope = new PanelSimple();
    pTelescope->setBackground( (char*)"images/telescope.png" );
    pTelescope->setSize( 45,64 );
    pTelescope->setFantome( true );
    this->add( pTelescope );
    
    #define SIZEY 14
	#define POLICE "fonts/greek-wsi-regular.ttf"   	// 218 glyphe voir alpha ...

    int x = 90, y = SIZEY, deltax = 16;
    pColor = new PanelText( (char*)"(0,0)",			PanelText::NORMAL_FONT, 5, y );
    y += SIZEY;
    pCoord = new PanelText( (char*)"(0,0)",			PanelText::NORMAL_FONT, 5, y );
    //y += SIZEY;


    pJ2000_alpha = new PanelText( 	(char*)"a", (char*)POLICE , x, y, 14, 0xFFFFFFFF );
    pJ2000_1 = new PanelText( (char*)"(0,0)",		PanelText::NORMAL_FONT, x+deltax, y );

    y += SIZEY+4;
    pJ2000_delta = new PanelText( 	(char*)"d", (char*)POLICE , x, y, 14, 0xFFFFFFFF );
    pJ2000_2 = new PanelText( (char*)"(0,0)",		PanelText::NORMAL_FONT, x+deltax, y );

    pFondCoord->add( pColor );
    pFondCoord->add( pCoord );
    pFondCoord->add( pJ2000_alpha );
    pFondCoord->add( pJ2000_delta );
    pFondCoord->add( pJ2000_1 );
    pFondCoord->add( pJ2000_2 );
    
    pVizier = NULL;
    
    setExtraString( "PanelCapture" );
    setParentCliping( true );
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelCapture::~PanelCapture()
{
    stars.setRB( NULL );
    stars.deleteAllStars();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::update_stars()
{
    if  ( pReadBgr==NULL )      logf( (char*)"PanelCapture::update()   pointeur RB NULL" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::updateEchelle()
{
    computeRepere();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::updateEchelleGeo()
{
    double coef;
    double coef0 = (double)pReadBgr->w / getParent()->getDX();// * ech_user;
    double coef1 = (double)pReadBgr->h / getParent()->getDY();// * ech_user;

    if ( coef0 > coef1 )        coef = 1.0/coef1;
    else                        coef = 1.0/coef0;
    
    if ( coef != ech_geo )
    {
        // Ancienne valeur
        ech = ech_user * ech_geo;
        dx /= ech;
        dy /= ech;

		// Nouvelle valeur
        ech_geo = coef;

        ech = ech_user * ech_geo;
        dx *= ech;
        dy *= ech;
		
		//----------------------        
        setPos(dx, dy);
		setSize( (double)pReadBgr->w * ech, (double)pReadBgr->h * ech );
    }

    if ( coef != ech_geo )    {
        //stars.update_stars( getPosX(), getPosY(), this, pReadBgr, ech_geo*ech_user );
    }
    
    ech = ech_user * ech_geo;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#define CLIP_VIZIER
void PanelCapture::updatePos()
{
    if ( pReadBgr == NULL )
    {
        PanelSimple::updatePos();
        return;
    }
	
	updateEchelleGeo();

    double fDX = (double)pReadBgr->w;// * ech_geo;
    double fDY = (double)pReadBgr->h;// * ech_geo;


    setSize( fDX*ech, fDY*ech );
    setCent();

    PanelSimple::updatePos();

    vec2 vMin = vec2( pCapture->getX(), pCapture->getY() );
    vec2 vMax = vec2( pCapture->getX()+pCapture->getDX()  ,  pCapture->getY()+pCapture->getDY()  );

    if ( pVizier != NULL && bAffCatalog && !pCapture->isIconized() )
    {
	    int n = pVizier->size();
		Fits*	pFits	= pCapture->getFits();
		vec2 	d 		= vec2( getX(), getY() );
		double 	ech 	= ech_user * ech_geo;
		vec2	v;

        for ( int i=0; i<n; i++ )        {       
			
			PanelText* p = pVizier->get(i)->getInfo();
	
			p->setVisible(true);

            v.x = pVizier->get(i)->getRA();
            v.y = pVizier->get(i)->getDE();

			pFits->J2000_2_tex( v );
			tex2screen(v);
			
#ifdef CLIP_VIZIER
			if ( 	v.x> vMin.x   &&	v.x < vMax.x  
				&& 	v.y> vMin.y   &&	v.y < vMax.y  )
			{
#endif			
		        pVizier->get(i)->setScreen( v );

		        double r = 17.5 - 0.675*pVizier->get(i)->fMag;
				p->setPos( v.x + ech*r*0.707,    v.y - ech*r*0.707 - 15.0 );
				p->updatePos();
				p->setAlpha(0.0);
				
				screen2tex(v);
		        pVizier->get(i)->setTex( v );
				
		        if ( bNuit )        p->setColor( COLOR32(255,   0, 0, 255) );
		        else                p->setColor( COLOR32(255, 178, 0, 255) );

				//logf( (char*)"%d-etoile dans ecran (%.2f, %.2f) (%.2f, %.2f)", i, (float)w.x, (float)w.y, (float)v.x, (float)v.y );
#ifdef CLIP_VIZIER
			}
			else
			{
				vec2 v = vec2(-1.0,-1.0);
		        pVizier->get(i)->setScreen( v );
		        
				p->setVisible(false);
			}
#endif			
			

        }
    }    

    //stars.update_stars(getParent()->getX(), getParent()->getY(), this, NULL );

	if ( pCapture->getAfficheGrille() && !pCapture->isIconized() )	{
		setEchelleVisible(true);
	    updateEchelle();
	}
	else	{
		setEchelleVisible(false);
	}

	if ( pCapture->isFits() )	{
		pCapture->getFits()->J2000_2_tex( vTelescopeJ2000, vTelescopeTex );

		vTelescopePanel = vTelescopeScreen	= vTelescopeTex;

		tex2screen( vTelescopeScreen );
		tex2panel( vTelescopePanel );
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::glCercleAnimation( int X,  int Y,  int rayon, int pp, int mm )
{
	glBegin(GL_LINE_SMOOTH);

	    glVertex2i(x, y-dy);         glVertex2i(x, y+dy);
	    glVertex2i(x-dx, y);         glVertex2i(x+dx, y);

    double step = 80.0/(double)rayon;
    if ( step < 1.0 )           step = 1.0;
    
    //logf ( (char*)" rayon %d   step %0.2f", rayon, step );
    double alpha = 90.0/TIMEANIM*dTimeAnim;

	double x0, y0, x1, y1, r, i;
	glBegin(GL_LINES);
		i = alpha;
		r = rayon + pp;
        x0 = (double)X+ (double)r*cos(DEG2RAD(i));
        y0 = (double)Y+ (double)r*sin(DEG2RAD(i));
		r = rayon + mm;
        x1 = (double)X+ (double)r*cos(DEG2RAD(i));
        y1 = (double)Y+ (double)r*sin(DEG2RAD(i));
        glVertex2i(x0, y0);         glVertex2i(x1, y1);
        
		i = alpha + 90.0;
		r = rayon + pp;
        x0 = (double)X+ (double)r*cos(DEG2RAD(i));
        y0 = (double)Y+ (double)r*sin(DEG2RAD(i));
		r = rayon + mm;
        x1 = (double)X+ (double)r*cos(DEG2RAD(i));
        y1 = (double)Y+ (double)r*sin(DEG2RAD(i));
        glVertex2i(x0, y0);         glVertex2i(x1, y1);
        
		i = alpha + 180.0;
		r = rayon + pp;
        x0 = (double)X+ (double)r*cos(DEG2RAD(i));
        y0 = (double)Y+ (double)r*sin(DEG2RAD(i));
		r = rayon + mm;
        x1 = (double)X+ (double)r*cos(DEG2RAD(i));
        y1 = (double)Y+ (double)r*sin(DEG2RAD(i));
        glVertex2i(x0, y0);         glVertex2i(x1, y1);
        
		i = alpha + 270.0;
		r = rayon + pp;
        x0 = (double)X+ (double)r*cos(DEG2RAD(i));
        y0 = (double)Y+ (double)r*sin(DEG2RAD(i));
		r = rayon + mm;
        x1 = (double)X+ (double)r*cos(DEG2RAD(i));
        y1 = (double)Y+ (double)r*sin(DEG2RAD(i));
        glVertex2i(x0, y0);         glVertex2i(x1, y1);
        
    glEnd();        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::glCroix( int x,  int y,  int dx,  int dy )
{
	glBegin(GL_LINE_SMOOTH);

	    glVertex2i(x, y-dy);         glVertex2i(x, y+dy);
	    glVertex2i(x-dx, y);         glVertex2i(x+dx, y);

    glEnd();        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::glCercle(int x, int y, int rayon)
{
    double step = 80.0/(double)rayon;
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
void PanelCapture::displayTelescope()
{
	unsigned color_old = getColor();
	unsigned jaune = 0xFFFF00FF;

	/*
	pCapture->getFits()->J2000_2_tex( vTelescopeJ2000, vTelescopeTex );

	vTelescopeScreen	= vTelescopeTex;
	vTelescopePanel		= vTelescopeTex;

	tex2screen( vTelescopeScreen );
	tex2panel( vTelescopePanel );

	*/
	vec2	v = vTelescopeScreen;// + vec2(getX(), getY());
	//-------------------------------------
	//   Couleur Nuit ?
    if ( bNuit )		{
		pTelescope->setColor( COLOR32(255, 0, 0, 255) );
		glColor4f( 255, 0, 0, 255 );
    }
    else	{
		pTelescope->setColor( jaune );
		glColor4f( COLOR_R(jaune), COLOR_G(jaune), COLOR_B(jaune), COLOR_A(jaune) );
    }
	//-------------------------------------
    // Affichage
    glCercleAnimation( v.x, v.y, 15, 3, -10 );
    glCercle( v.x, v.y, 15 );
    glCercle( v.x, v.y, 10 );
    //glCroix( v.x, v.y, 20, 20 );
	
	pTelescope->setPos( vTelescopePanel.x+10, vTelescopePanel.y+10);
	pTelescope->onTop();

	glColor4f( COLOR_R(color_old), COLOR_G(color_old), COLOR_B(color_old), COLOR_A(color_old) );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::displayCatalog()
{
	//logf( (char*)"PanelCapture::displayCatalog() %d etoiles", pVizier->size() );
	// Sauvegarde de l'ancienne couleur
	unsigned color_old = getColor();
	unsigned r  = 0xFF0000FF;
	unsigned b  = 0x0000FFFF;

	Fits*	pFits	= pCapture->getFits();
	vec2 	d 		= vec2( getX(), getY() );
	double 	ech 	= ech_user * ech_geo;
	vec2 v, w;
	
	if ( pFits == NULL ) 					return;
	//-------------------------------------------------------------------
	glBegin(GL_LINES);

	//glColor4f( COLOR_A(r), COLOR_R(r), COLOR_G(r), COLOR_B(r) );
    
    int n = pVizier->size();
    int nbAff = 0;

    if ( n!= 0)
    {
        if ( bNuit )        glColor4f( 1.0,  0.0,  0.0, 1.0 );
        else                glColor4f( 1.0, 0.7, 0.0, 0.8 );

        for ( int i=0; i<n; i++ )
        {
            v.x = pVizier->get(i)->getXScreen();
            v.y = pVizier->get(i)->getYScreen();

			if ( 	v.x != -1.0  )
			{
				PanelText* p = pVizier->get(i)->getInfo();
		        double r = 17.5 - 0.675*pVizier->get(i)->fMag;
			
		        glCercle( v.x, v.y, r*ech );
				p->displayGL();
				nbAff++;
			}
        }
		
		dDebug5s += Timer::getInstance().getElapsedTime();
		//if ( dDebug5s > 5.0 || (nbNonAff != (n-nbAff)) )
		if ( nbNonAff != (n-nbAff) )
		{
			nbNonAff = n-nbAff;
			dDebug5s = 0.0;
			char pStrNbEtoiles[128];
			//snprintf( pStrNbEtoiles, sizeof(pStrNbEtoiles), "Etoiles affiches %d/%d", nbAff, n );
			//log( (char*)pStrNbEtoiles );
			snprintf( pStrNbEtoiles, sizeof(pStrNbEtoiles), "%d/%d", nbAff, n );
			pCapture->getNbVizier()->changeText( (char*)pStrNbEtoiles );
		}
        
    	//pCapture->setNbVizier( 	pVizier->size() );
        if ( bNuit )        pCapture->getNbVizier()->setColor( COLOR32(255,   0, 0, 255) );
        else                pCapture->getNbVizier()->setColor( COLOR32(255, 178, 0, 255) );
    }    
    
    glEnd();
  	
	// retour a l'ancienne couleur
	glColor4f( COLOR_R(color_old), COLOR_G(color_old), COLOR_B(color_old), COLOR_A(color_old) );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::displayAxe()
{
	// Sauvegarde de l'ancienne couleur
	unsigned color_old = getColor();
	unsigned R  = 0xFF000080;
	unsigned r  = 0xFF000080;
	unsigned b  = 0x0000FF80;

	//logf( (char*)"Old Color=%0lX", color_old );
	//logf( (char*)"New Color=%0lX", c );

	vec2 d = vec2( getX(), getY() );
	double ech = ech_user * ech_geo;

	glBegin(GL_LINES);

	vec2 v, w;

	if (bNuit)		glColor4ub( COLOR_R(R), COLOR_G(R), COLOR_B(R), COLOR_A(R) );

	for( int i=0; i<p1.size(); i++ )
	{
		if (!bNuit )	{
			if ( (i%2) == 1)    glColor4ub( COLOR_R(r), COLOR_G(r), COLOR_B(r), COLOR_A(r) );
			else			    glColor4ub( COLOR_R(b), COLOR_G(b), COLOR_B(b), COLOR_A(b) );
		}
		v = ech * p1[i] + d;
		w = ech * p2[i] + d;
	    glVertex2i(v.x, v.y);
	    glVertex2i(w.x, w.y);


	}

    glEnd();
	// retour a l'ancienne couleur
	glColor4f( COLOR_R(color_old), COLOR_G(color_old), COLOR_B(color_old), COLOR_A(color_old) );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::displayGL()
{
    double gris = 0.3;
    VarManager& var = VarManager::getInstance();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	/*
    if ( bNuit )        glColor4f( gris,  0.0,  0.0, 1.0 );
    else                glColor4f( gris, gris, gris, 0.2 );    
    */
    
    if ( bNuit )		{
    	setColorBgr( COLOR32(255, 0, 0, 255) );
    	pColor->setColor( COLOR32(255, 0, 0, 255) );
    	pCoord->setColor( COLOR32(255, 0, 0, 255) );
    	pJ2000_1->setColor( COLOR32(255, 0, 0, 255) );
    	pJ2000_2->setColor( COLOR32(255, 0, 0, 255) );
    	pJ2000_alpha->setColor( COLOR32(255, 0, 0, 255) );
    	pJ2000_delta->setColor( COLOR32(255, 0, 0, 255) );
    }
    else	{
    	setColorBgr( COLOR32(255, 255, 255, 255) );    
    	pColor->setColor( COLOR32(255, 255, 255, 255) );
    	pCoord->setColor( COLOR32(255, 255, 255, 255) );
    	pJ2000_1->setColor( COLOR32(255, 255, 255, 255) );
    	pJ2000_2->setColor( COLOR32(255, 255, 255, 255) );
    	pJ2000_alpha->setColor( COLOR32(255, 255, 255, 255) );
    	pJ2000_delta->setColor( COLOR32(255, 255, 255, 255) );
    }
    
	
	
    //if ( bNuit )        glColor4f( gris,  0.0,  0.0, 1.0 );
    //else                glColor4f( gris, gris, gris, 0.2 );    
    
    PanelSimple::displayGL();
    
    stars.setView( this );
    //stars.setRB( pReadBgr );
    if ( !pCapture->isIconized() )
    {
    	stars.displayGL();

		if ( pCapture->isFits() )	{
			if ( !pCapture->isIconized() && pCapture->getAfficheGrille() )		displayAxe();

			if ( var.getb( "bAffCatalog") && (pVizier) )		displayCatalog();
		}
	
		if ( 0<vTelescopeScreen.x && vTelescopeScreen.x<getDX() && 0<vTelescopeScreen.y && vTelescopeScreen.y<getDY()  )	
		{
			pTelescope->setVisible(true);
			displayTelescope();
		}
		else
			pTelescope->setVisible(false);
		
	}
	else
	{
		pTelescope->setVisible(false);
	}	
	// Affichage de debogage
	/*
	if ( bAffStar && !bIcone )		{
		logf( (char*)"(X, Y)=(%d, %d)  (x_raw, y_raw)=(%d, %d )", getPosX(), getPosY(), getX(), getY() );
	}
	*/
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::idle(float f)
{
    VarManager& var = VarManager::getInstance();
    //----------------------------------------------
    dTimeAnim += f;
	if ( dTimeAnim>TIMEANIM )			dTimeAnim -= TIMEANIM;
    
	if ( !pCapture->isIconized()   )	{
		stars.setModeMag(1);
		stars.setVisible(bAffStar);
		stars.updateScreenPos( dx+pCapture->getX(), dy+pCapture->getY(), ech);
		stars.idle();
	}	
	else	{
		stars.setVisible(false);
	}    
    
    
	if ( pCapture->getAfficheInfoSouris() != pFondCoord->getVisible() )
	{
		
		if ( pCapture->getAfficheInfoSouris() )
		{
			pFondCoord->setVisible(true);
			passiveMotionFunc( (int)mouse.x, (int)mouse.y );
		}
		else
			pFondCoord->setVisible(false);
	
	}
	
	if ( pCapture->isIconized() )		
		pFondCoord->setVisible(false);

	pCapture->afficheFits();

    PanelSimple::idle(f);

	if ( pCapture->isFits() )
	{
		//-------------------------------------------------------------------
		Fits*	pFits	= pCapture->getFits();
		
		pFits->J2000_2_tex( vTelescopeJ2000, vTelescopeTex );
		vTelescopePanel =vTelescopeScreen = vTelescopeTex;
		tex2screen( vTelescopeScreen );
		tex2panel( vTelescopePanel );
		//---------------------------------------
		if ( !pCapture->isIconized() && stars.size() != 0 )
		{

			for( int i; i<stars.size(); i++ )	
			{
				Star* pStar = stars.get(i);

				if ( pStar->haveCoord() )	continue;
				//logf( (char*)"assignation coordonneees %d", i );

				vec2 vScreen = vec2( pStar->getXScreen(), pStar->getYScreen() );
				vec2 vTex = vScreen;
				screen2tex( vTex );

				vec2 vJ2000;
				pFits->tex_2_J2000( vTex, vJ2000 );

				pStar->setAD( vJ2000.x );
				pStar->setDE( vJ2000.y );
			}
		}
		//-------------------------------------------------------------------
		if ( !pCapture->isIconized() )
		{
			if ( var.getb("bAffFitsCorrection") )	pFits->getPanelCorrectionFits()->setVisible(true);
			else 									pFits->getPanelCorrectionFits()->setVisible(false);
		}
		else
			pFits->getPanelCorrectionFits()->setVisible(false);
		//----------------------------------------------
		if ( bNuit )		pFits->getPanelCorrectionFits()->setColor(0xFF0000FF);
		else				pFits->getPanelCorrectionFits()->setColor(0xFFFFFFFF);
		//-------------------------------------------------------------------
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::wheelUp(int xm, int ym)
{
#ifdef DEBUG_WHEEL
    logf( (char*)"PanelCapture::wheelUp(%d,%d) ...", xm, ym );
    log_tab(true);
#endif
    
    double k = 1.1;
    ech_user *= k;
    
    Panel* p = getParent();

    double XM =  ( xm- p->getX() );
    double X0 = -( getPosX() ); 
    double X1 = k*X0 + (k-1)*XM;
    
    double YM =  ( ym-p->getY() );
    double Y0 = -( getPosY() ); 
    double Y1 = k*Y0 + (k-1)*YM;
    
    double ech = ech_user * ech_geo;
    double max_x = (ech*(double)pReadBgr->w) - p->getDX();
    double max_y = (ech*(double)pReadBgr->h) - p->getDY();

    int x1 = X1;
    int y1 = Y1;
    
    clip( x1, y1 );
    setPos( -x1, -y1 );

    dx = -X1;
    dy = -Y1;
    
    
    updatePos();
    passiveMotionFunc( xm, ym );
    pCapture->updatePosIcones();

    stars.updateScreenPos( dx+pCapture->getX(), dy+pCapture->getY(), ech);

#ifdef DEBUG_WHEEL
	logf( (char*)"+Nouvelle echelle %.2f", (float)ech );

    log_tab(false);
#endif
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::wheelDown(int xm, int ym)
{
#ifdef DEBUG_WHEEL
    logf( (char*)"PanelCapture::wheelDown(%d,%d) ...", xm, ym );
    log_tab(true);
#endif

    double k = 0.9;
    ech_user *= k;
    if (ech_user<=1.0)           ech_user = 1.0;
    
    Panel* p = getParent();

    double XM =  ( xm- p->getX() );
    double X0 = -( getPosX() ); 
    double X1 = k*X0 + (k-1)*XM;
    
    double YM =  ( ym-p->getY() );
    double Y0 = -( getPosY() ); 
    double Y1 = k*Y0 + (k-1)*YM;

    double ech = ech_user * ech_geo;
    double max_x = ech*pReadBgr->w - p->getDX();
    double max_y = ech*pReadBgr->h - p->getDY();
    

    int x1 = X1;
    int y1 = Y1;
    
    clip( x1, y1 );
    setPos( -x1, -y1 );
    dx = -x1;
    dy = -y1;
    
    //stars.update_stars( getX(), getY(), this, pReadBgr, ech_geo*ech_user );

    
    updatePos();    
    passiveMotionFunc( xm, ym );
    pCapture->updatePosIcones();
    
#ifdef DEBUG_WHEEL
    logf( (char*)"+Nouvelle echelle %.2f", (float)ech );
#endif

    stars.updateScreenPos( dx+pCapture->getX(), dy+pCapture->getY(), ech);

#ifdef DEBUG_WHEEL
    log_tab(false);
#endif
}
//--------------------------------------------------------------------------------------------------------------------
//
// xx, yy coordonnée écran du pointeur de souris
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::passiveMotionFunc(int xm, int ym)
{
	//logf( (char*)"PanelCapture::passiveMotionFunc( %d, %d ) %s", xm, ym, pCapture->getBasename().c_str() );
	//logf( (char*)"  ech_user = %0.2f, ech_geo = %0.2f )", (float)ech_user, (float)ech_geo );
	//log_tab(true);

	//if ( !bInfoSouris || bIcone ) 			{ return; }
	if ( !pCapture->getAfficheInfoSouris() || pCapture->isIconized() )		
	{
		if ( pFondCoord )	pFondCoord->setVisible(false);
		return;
	}
	
	static char coord[80];
	static char coord_lum[80];
	static char coord_ad[80];
	static char coord_dc[80];

	// Coordonne fenetre 
	long x = Screen2x(xm);
	long y = Screen2y(ym);
	
	// coordonnées image	
	double X = (double)x/(ech_user*ech_geo);
	double Y = (double)y/(ech_user*ech_geo);
	
	long XX = X;
	long YY = Y;

	// Recuperation des infos image largueur hauteur et pointeur sur RGBRGBRGBRGBRGBRGB
	int w = pReadBgr->w;
	int h = pReadBgr->h;
	
	GLubyte*	ptr = pReadBgr->ptr;
	GLubyte		r, g, b;
	
	if ( ptr == NULL )		logf( (char*)"Erreur ptr NULL %s", __LINE__ );
	// Calcul de l'offset dans le buffer
	long idx = 3 * (w * YY + XX);
		
	// Affichage du resultat
	//float l = 0.33 * (float)r + 0.5 * (float)g  + 0.16 * (float)b;
	// Recuperation de la couleur du pixel
	r = ptr[ idx + 0 ];
	g = ptr[ idx + 1 ];
	b = ptr[ idx + 2 ];
	
	float l = LUM(r,g,b);
	//
	snprintf( (char*)coord_lum, sizeof(coord_lum), "rvb=(%d, %d, %d)   lum=%0.1f", (int)r, (int)g, (int)b, l );
	pColor->changeText( (char*)coord_lum);
	pColor->setChangeText(true);

	snprintf( (char*)coord, sizeof(coord), "(%ld,%ld)", XX, YY );
	pCoord->changeText( (char*)coord );
	pCoord->setChangeText(true);
	
	if ( pCapture->isFits() )	{
		//---------------------------------------
		// calcul la position J2000 de la souris
    	vec2 vJ2000;
		vec2 vTex = vec2( xm, ym ); 
		screen2tex( vTex );
		pCapture->getFits()->tex_2_J2000( vTex, vJ2000 );
		
		#ifdef DEG_MS
			struct dms DMS;
			struct hms HMS;
			deg2hms( vJ2000.x, HMS );
			deg2dms( vJ2000.y, DMS );

			snprintf( (char*)coord_ad, sizeof(coord_ad), "=%02.0fh %02.0f' %02.2f\"", HMS.h, HMS.m, HMS.s );
			snprintf( (char*)coord_dc, sizeof(coord_dc), "=%2.0fd %02.0f' %02.2f\"", DMS.d, DMS.m, DMS.s );
		#else
			snprintf( (char*)coord_ac, sizeof(coord_ad), "=%f", vJ2000.x );
			snprintf( (char*)coord_dc, sizeof(coord_dc), "=%f", vJ2000.y );
		#endif

		pJ2000_1->changeText( (char*)coord_ad );
		pJ2000_1->setChangeText(true);

		pJ2000_2->changeText( (char*)coord_dc );
		pJ2000_2->setChangeText(true);
	}
	else
	{
	}
	// Mise a jour du panelText
	// dans la position de la souris
	// Force la fabrication de la chaine de caractere
	pFondCoord->setPos( x+15, y+15);
	pFondCoord->onTop();
		
	//log_tab(false);
	//log( (char*)"---" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::clickLeft(int xm, int ym)
{
    logf( (char*)"PanelCapture::clickLeft(%d,%d) ...", xm, ym );
    if ( pCapture->isIconized() )
	    Captures::getInstance().setCurrent( pCapture );

    //Panel::clickLeft(xm,ym);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::releaseLeft(int xm, int ym)
{
    logf( (char*)"PanelCapture::releaseLeft(%d,%d) glutModifier=%d...", xm, ym, iGlutModifier );

    if ( pCapture->isFits() && iGlutModifier == GLUT_ACTIVE_CTRL)    {
		sendStellarium( xm, ym );
    }
    else
    if ( pCapture->isFits() && iGlutModifier == GLUT_ACTIVE_ALT)    {
		sendStellarium( xm, ym );
    }
    else
    {
		double e = (double)getDX() / (double)pReadBgr->w; 
		
		int xx = ((double)xm-(double)getX()) / e;
		int yy = ((double)ym-(double)getY()) / e;


		// Si en plein ecran on ajoute une etoile
		    
		if ( !pCapture->isIconized()     )
		{
		    //stars.setView( (PanelSimple*)this->getParent() );
		    stars.setView( this );
		    stars.setRB( pReadBgr );
		    if ( stars.addStar( xm, ym, getX(), getY(), e ) == NULL )
		        stars.selectLeft(xx, yy);
		}
		else
		{            
		    captureOnTop(pCapture);
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::clickRight(int xm, int ym)
{
    logf( (char*)"PanelCapture::clickRight(%d,%d) ...", xm, ym );
    Captures::getInstance().setCurrent( pCapture );

    if ( pCapture->getFullScreen() )
    {
		setCanMove(false);
		pCapture->setCanMove(false);
	}
	else
	{
	    PanelSimple::clickRight(xm,ym);
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::motionRight(int xm, int ym)
{
    //logf( (char*)"PanelCapture::motionRight(%d,%d) ...", xm, ym );
    if ( pCapture->getFullScreen() )
    {
		setCanMove(false);
		pCapture->setCanMove(false);
	}
	else
	{
		PanelSimple::motionRight( xm, ym );
		updatePos();    
	}
	//stars.update_stars( getX(), getY(), this, pReadBgr, ech_geo*ech_user );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::releaseRight(int xm, int ym)
{
    logf( (char*)"PanelCapture::releaseRight(%d,%d) ...", xm, ym );
    printObjet();
    if ( pReadBgr == NULL )     { logf( (char*)"Pointeur NULL" ); return; }
    
    if ( pCapture->getFullScreen() )
    {
		setCanMove(false);
		pCapture->setCanMove(true);
	    logf( (char*)"PanelCapture:: canMove" );
	}
	else
    {
		double e = (double)getDX() / (double)pReadBgr->w; 
		//double e = (double)getDX() / (double)1920.0; 
		
		int xx = ((double)xm-(double)getX()) / e;
		int yy = ((double)ym-(double)getY()) / e;
		
		stars.setView( (PanelSimple*)this->getParent() );
		stars.setRB( pReadBgr );
		if ( pCapture != NULL )
		{
		    if ( pCapture->isFits() )       pCapture->afficheFits();
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::clickMiddle(int xm, int ym)
{
    xm_old = xm;
    ym_old = ym;
    logf( (char*)"PanelCapture::clickMiddle(%d,%d) ...", xm, ym );
    Captures::getInstance().setCurrent( pCapture );

	motionMiddle( xm, ym);
    //bHaveMove = false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::motionMiddle(int xm, int ym)
{
    //logf( (char*)"PanelCapture::motionMiddle(%d,%d) ...", xm, ym );
    //log_tab(true);
    //logf( (char*)"    delta (%d,%d) ...", xm-xm_old, ym-ym_old );
    double deltaX, deltaY;
    double ech = ech_geo * ech_user;
    Panel* p = getParent();
    
    deltaX = (double) (xm-xm_old) / ech;
    deltaY = (double) (ym-ym_old) / ech;

    double X0 = ( getPosX() ); 
    double X1 = X0 + (xm-xm_old);
    
    double Y0 = ( getPosY() ); 
    double Y1 = Y0 + (ym-ym_old);
    
    double max_x = ech*pReadBgr->w - p->getDX();
    double max_y = ech*pReadBgr->h - p->getDY();

    int x1 = -X1;
    int y1 = -Y1;

    clip(x1, y1);
    setPos( -x1, -y1 );
    dx = -x1;
    dy = -y1;
    double e = (double)getDX() / (double)pReadBgr->w; 
    
    xm_old = xm;
    ym_old = ym;
    
    bHaveMove = true;
 
 
    //stars.update_stars( getX(), getY(), this, pReadBgr, ech_geo*ech_user );
    //log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::releaseMiddle(int xm, int ym)
{
	motionMiddle( xm, ym);
    xm_old = -1;
    ym_old = -1;
    logf( (char*)"PanelCapture::releaseMiddle(%d,%d) ...", xm, ym );
    log_tab(true);
    logf( (char*)"ech_geo=%0.2f ech_user=%0.2lf dx=%0.2lf dy=%0.2lf", ech_geo, ech_user, dx, dy );


    if ( pReadBgr == NULL )     { logf( (char*)"Pointeur NULL" ); log_tab(false); return; }
    
    
    //double e = (double)getDX() / (double)pReadBgr->w; 
    double e = (double)getDX() / (double)pReadBgr->w; 
    int xx = ((double)xm-(double)getX()) / e;
    int yy = ((double)ym-(double)getY()) / e;
    
    logf( (char*)"echelle%0.2lf", e );
    logf( (char*)"Pos (%0.2lf, %0.2lf)", (double)getX(), (double)getY()  );
    
    if ( !bHaveMove )
    {
		stars.setModeMag(1);
        stars.setView( this );
        stars.setRB( pReadBgr );
        if ( stars.addStar( xm, ym, getX(), getY(), e ) == NULL )
        {
            stars.selectStar(xx, yy);
            logf( (char*)"selects star" );
        }
        else
            stars.selectStar(xx, yy);
    }
    bHaveMove = false;
	
    computeRepere();

    log_tab(false);
	log( (char*)"---" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::findAllStars()
{
	if ( stars.size() != 0 )				{
		stars.affiche_position();
		return;
	}
	stars.setModeMag(1);
    stars.setView( this );
    stars.setRB( pReadBgr );
    stars.findAllStars();
    return;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::deleteAllStars()
{
    stars.setRB( pReadBgr );

    stars.deleteAllStars();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::addStar(int xm, int ym)
{
    logf( (char*)"PanelCapture::addStar(%d,%d) ...", xm, ym );
    stars.setView( this );
    stars.setRB( pReadBgr );
    //stars.addStar( xm, ym );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::clip(int& xm, int& ym)	{
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
// Compare les etoiles trouvées par le programme// aux etoiles du catalogue vizier
// Affiche le resultat dans la console
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::compareStar()
{
   	log( (char*)"PanelCapture::compareStar()");
   	if ( stars.size() == 0 )		return;
   	if ( pVizier == NULL || pVizier->size() == 0 )		return;

	starCompare.setStars(&stars);
	starCompare.setVizier(pVizier);

	starCompare.compareStar();
}
//--------------------------------------------------------------------------------------------------------------------
//
// Compare les etoiles trouvées par le programme// aux etoiles du catalogue vizier
// Affiche le resultat dans la console
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::saveCompareStar()
{
   	log( (char*)"PanelCapture::saveCompareStar()");
   	if ( stars.size() == 0 )							return;
   	if ( pVizier == NULL || pVizier->size() == 0 )		return;

	starCompare.setStars(&stars);
	starCompare.setVizier(pVizier);

	starCompare.saveCompareStar();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::setCent()
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
void PanelCapture::setEchelle(double f)
{
    if ( f == 0.0 )     
    {
        logf( (char*)"PanelCapture::setEchelle(%0.2f)", f );
        return;
    }
    if ( pReadBgr == NULL )         return;
    
    double fDX = (double)pReadBgr->w * ech_geo;
    double fDY = (double)pReadBgr->h * ech_geo;
    
    int dxp = getParent()->getDX();
    int dyp = getParent()->getDY();

    if ( fDX*f < dxp || fDY*f < dyp )
    {
        logf( (char*)"Stop setEchelle(%0.2lf)", ech_user );
        return;
    }

    ech_user = f;
    
    
    setSize( fDX*ech_user, fDY*ech_user );
    setCent();
    logf( (char*)"PanelCapture::setEchelle() ech_geo=%0.2f ech_user=%0.2f dx=%0.2f dy=%0.2f", ech_geo, ech_user, dx, dy );
    logf( (char*)"  setSize(%0.2lf, %0.2lf)", fDX, fDY );
    
}

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::setCentX(double f)
{
    dx = f;
    setCent();
    return;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::setCentY(double f)
{
    dy = f;
    setCent();
    return;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::screen2tex(vec2& v)
{
	vec2 delta   = vec2(getX(), getY());
	v = ( v - delta  ) / ech;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::tex2screen(vec2& v)
{
	vec2 delta = vec2(getX(), getY());
	v = (v * ech) + delta;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::tex2panel(vec2& v)
{
	v = (ech * v);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::panel2tex(vec2& v)
{
	v = v / ech;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::panel2screen(vec2& v)
{
	vec2 delta = vec2(getX(), getY());
	v = v + delta;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::screen2panel(vec2& v)
{
	vec2 delta = vec2(getX(), getY());
	v = v - delta;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::printObjet()
{
    logf( (char*)"PanelCapture::printObjet() \"%s\"", pCapture->getBasename().c_str() );
    log_tab(true);
    logf( (char*)"this->pos (%d, %d)", getPosX(), getPosY() );
    logf( (char*)"this->dim %d x %d", getDX(), getDY() );
    logf( (char*)"this->ech_geo=%0.2lf",  ech_geo );
    logf( (char*)"this->ech_user=%0.2lf", ech_user );
    logf( (char*)"this->ech=%0.2lf", ech );
    if ( pReadBgr == NULL )    {
        logf( (char*)"pReadBgr = NULL" );
    } else {
        logf( (char*)"this->pReadBgr->w %d", pReadBgr->w.load() );
        logf( (char*)"this->pReadBgr->h %d", pReadBgr->h.load() );
        logf( (char*)"this->pReadBgr->d %d", pReadBgr->d.load() );
    }
    logf( (char*)"this->vAD=(%.2lf, %.2lf)", vAD.x, vAD.y );
    logf( (char*)"this->vDE=(%.2lf, %.2lf)", vDE.x, vDE.y );
    logf( (char*)"this->dAngleAD=%0.2lf", dAngleAD );
    logf( (char*)"this->dAngleDE=%0.2lf", dAngleDE );

	if ( pVizier )
	    logf( (char*)"this->pVizier=%d", pVizier->size() );
    logf( (char*)"this->stars=%d", stars.size() );

    log( (char*)"" );
    logf( (char*)"this->bAffCatalogPosition=%s", BOOL2STR(bAffCatalogPosition) );
    logf( (char*)"this->pTelescope->bVisible=%s", BOOL2STR(pTelescope->getVisible()) );
    logf( (char*)"this->pFondCoord->bVisible=%s", BOOL2STR(pFondCoord->getVisible()) );
    logf( (char*)"this->pColor->bVisible=%s", BOOL2STR(pColor->getVisible()) );
    logf( (char*)"this->pCoord->bVisible=%s", BOOL2STR(pCoord->getVisible()) );

    log( (char*)"" );
    logf( (char*)"this->pCapture->isIconized=%s", BOOL2STR(pCapture->isIconized()) );
    logf( (char*)"this->pCapture->bAfficheInfoSouris=%s", BOOL2STR(pCapture->getAfficheInfoSouris()) );
    logf( (char*)"this->pCapture->bAfficheGrille=%s", BOOL2STR(pCapture->getAfficheGrille()) );
    logf( (char*)"this->pCapture->bAfficheInfoFits=%s", BOOL2STR(pCapture->getAfficheInfoFits()) );
    logf( (char*)"this->pCapture->bFullScreen=%s", BOOL2STR(pCapture->getFullScreen()) );

    log_tab(false);
	log( (char*)"---" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::findGaiaDR3()
{
    logf( (char*)"PanelCapture::findGaiaDR3()" );

	if ( pVizier )	{
		if ( !bAffCatalogPosition )	{
			bAffCatalogPosition = true;
			pVizier->affiche_position();
			return;
		}
		else	{
			bAffCatalogPosition = false;
			pVizier->affiche_magnitude();
			return;
		}

		bAffCatalogPosition = false;
		pVizier->efface();
		delete pVizier;
		pVizier = NULL;
		return;
	}
	
	if ( !pCapture->isFits() )	return;

	//---------------------------------------------------
	// CALCUL LA TAILLE DE L'IMAGE pour le rayon de recher
	
	// resolution ecran
	double resH = pCapture->getFits()->getNAXIS1();
	double resV = pCapture->getFits()->getNAXIS2();
	
	// Coefficient deg/pix
	resH *= pCapture->getFits()->getCDELT1() * 3600.0;
	resV *= pCapture->getFits()->getCDELT2() * 3600.0;
	
	// On prend la diagonnale pour rayon de recherche
	double rayon = sqrt( resV*resV + resH*resH );
	rayon /= 2.0;
	
	// si plus grand qu 1" d'arc
	if ( rayon > 3600.0 )       	rayon = 3600.0;
	logf( (char*)"  rayon calcule %.2f", rayon );
	
	//---------------------------------------------------
	// RECHERCHE DU CENTRE DE L'IMAGE pour la requete
	vec2 vCentre = vec2( pCapture->getFits()->getCRVAL1(), pCapture->getFits()->getCRVAL2() );

	pVizier = new Catalog(false);

	//---------------------------------------------------
	// lance la requete
	static char coord[80];
	snprintf( (char*)coord, sizeof(coord), "-r %d% 0.8f %0.8f", (int)(rayon), vCentre.x, vCentre.y );
	logf( (char*)"Requete Gaia DR3 : %s", coord );

	//vizier_load_stars( pVizier, string(coord) );	
	vizier_capture_load_stars( pVizier, string(coord), this );	
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::findGaiaDR3_end()
{
    logf( (char*)"PanelCapture::findGaiaDR3_end()" );
	updatePos();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::eraseGaiaDR3()
{
    logf( (char*)"PanelCapture::eraseGaiaDR3()" );

	if ( pVizier )	{
		bAffCatalogPosition = false;
		pVizier->efface();
		delete pVizier;
		pVizier = NULL;
		return;
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::sendStellarium( int xm, int ym)
{
    logf( (char*)"PanelCapture::sendStellarium()" );

		vec2 vJ2000;
		vec2 vTex = vec2( xm, ym );
		screen2tex( vTex );
		pCapture->getFits()->tex_2_J2000( vTex, vJ2000 );
		
		struct dms DMS;
		struct hms HMS;
		deg2hms( vJ2000.x, HMS );
		deg2dms( vJ2000.y, DMS );
		
				
		static char coord[80];
		snprintf( (char*)coord, sizeof(coord), "(%0.6f, %0.6f)", vJ2000.x, vJ2000.y );
		log( (char*)coord );	
		
		
        VarManager& var= VarManager::getInstance();
        char cmd[255];
        double AD = vJ2000.x >180.0 ? -360.0 + vJ2000.x : vJ2000.x;

        if ( var.getb("bModeManuel") )  {
            sprintf( cmd, "A%lf;D%lf", AD, (vJ2000.y) );
            logf( (char*)"  Envoi arduino : %s", cmd );
            Serial::getInstance().write_string(cmd);
        }
        else    {
        	if ( bArduino )	{
		        sprintf( cmd, "ia%lf;id%lf", AD, (vJ2000.y) );
		        logf( (char*)"  Envoi arduino : %s", cmd );
	            Serial::getInstance().write_string(cmd);
        	}
        	else	{
				Serveur_mgr::getInstance().write_stellarium( (double)vJ2000.x, (double)vJ2000.y );
		        logf( (char*)"  Envoi stellarium : %lf, %lf", vJ2000.x, vJ2000.y );
				change_ad_status( vJ2000.x );
				change_dc_status( vJ2000.y );
			}
        }

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::setInfoSouris(bool b)  
{
	logf( (char*)"void PanelCapture::setInfoSouris(%s)", BOOL2STR(b) );
	//bInfoSouris = b; 
	
	if ( pCapture->getAfficheInfoSouris() ) 	{
		pTelescope->setVisible(true);
		pFondCoord->setVisible(true);
		pCoord->setVisible(true);
		pJ2000_1->setVisible(true);
		pJ2000_2->setVisible(true);
		stars.setVisible(true);
		setEchelleVisible(true);

		int xx = WindowsManager::getInstance().getMouseX();
		int yy = WindowsManager::getInstance().getMouseY();
		
		int x = Screen2x(xx);
		int y = Screen2y(yy);
	}
	else
	{	
		pTelescope->setVisible(false);
		pFondCoord->setVisible(false);
		pCoord->setVisible(false);
		pJ2000_1->setVisible(false);
		pJ2000_2->setVisible(false);
		setEchelleVisible(false);
		stars.setVisible(false);
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::iconize()
{
	logf( (char*)"PanelCapture::iconize()" );
	/*
	bAffGrille = false;
	bInfoSouris = false;
	setInfoSouris( false );
	bIcone = true;
	*/
	//updateEchelleGeo();
	//logf( (char*)"setPos(%lfd, %lf)", (dx), (dy) );
	//logf( (char*)"ech_geo=%lf, ech_user=%lf)", ech_geo, ech_user );
	setPos( dx, dy );

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::restaure()
{
	logf( (char*)"PanelCapture::restaure()" );
	
	//dTimeAnim = 0.0;
	if ( !pCapture->isIconized() && pCapture->getAfficheInfoSouris() )
	{
		//logf( (char*)"passiveMotionFunc(%d,%d)", mouse.x, mouse.y );
		log_tab(true);
		passiveMotionFunc( mouse.x, mouse.y );
		log_tab(false);
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::change_ad( double ad )
{
	if ( !pCapture->isFits() )			return;

	//logf( (char *)"PanelCapture::change_ad(%lf)", ad );
	vTelescopeJ2000.x = ad<0.0 ? 360.0+ad : ad ;
	
	pCapture->getFits()->J2000_2_tex( vTelescopeJ2000, vTelescopeTex );
	vTelescopePanel  = vTelescopeScreen = vTelescopeTex;
	tex2screen( vTelescopeScreen );
	tex2panel( vTelescopePanel );

	/*
	if ( Captures::getInstance().isCurrent( pCapture ) )
	{
		logf( (char*)"PanelCapture::change_ad()  ech=%lf", ech );
		logf( (char*)"     J2000           (%lf, %lf)", vTelescopeJ2000.x, vTelescopeJ2000.y );
		logf( (char*)"     Tex             (%lf, %lf)", vTelescopeTex.x, vTelescopeTex.y );
		logf( (char*)"     Screen          (%lf, %lf)", vTelescopeScreen.x, vTelescopeScreen.y );
		//logf( (char*)"     Panel           (%lf, %lf)", vTelescopeJ2000.x, vTelescopeJ2000.y );
	}
	*/	
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::change_dc( double dc )
{
	if ( !pCapture->isFits() )			return;
	//logf( (char *)"PanelCapture::change_dc(%lf)", dc );
	vTelescopeJ2000.y = dc;
		
	pCapture->getFits()->J2000_2_tex( vTelescopeJ2000, vTelescopeTex );
	vTelescopePanel  = vTelescopeScreen = vTelescopeTex;
	tex2screen( vTelescopeScreen );
	tex2panel( vTelescopePanel );

	/*
	if ( Captures::getInstance().isCurrent( pCapture ) )
	{
		logf( (char*)"PanelCapture::change_dc()  ech=%lf", ech );
		logf( (char*)"     J2000           (%lf, %lf)", vTelescopeJ2000.x, vTelescopeJ2000.y );
		logf( (char*)"     Tex             (%lf, %lf)", vTelescopeTex.x, vTelescopeTex.y );
		logf( (char*)"     Screen          (%lf, %lf)", vTelescopeScreen.x, vTelescopeScreen.y );
		//logf( (char*)"     Panel           (%lf, %lf)", vTelescopeJ2000.x, vTelescopeJ2000.y );
	}	
	*/
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::setEchelleVisible( bool b )
{
	for( int i=0; i<pTextEch.size(); i++ )		pTextEch[i]->setVisible(b);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::computeIntersectionHau(vec2& V, vec2 p1, vec2 p2 )                         
{
    //vec2 	vDelta	= vec2( getPosX(), getPosY() );
    double 	dPosY = -getPosY();
	    	
	vec2 pt1 = p1;			tex2panel(pt1);
	vec2 pt2 = p2;			tex2panel(pt2);
	vec2 v = (pt2 - pt1);

	double c = (dPosY-pt1.y)/v.y;
	double X = c*v.x + pt1.x;
	
	V = vec2( X, dPosY );
	V = V * ech;// - vDelta;
	panel2tex(V);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::computeIntersectionBas(vec2& V, vec2 p1, vec2 p2 )                         
{
    double 	dPosY = -getPosY() + getDY()/ech;
	    	
	vec2 pt1 = p1;			tex2panel(pt1);
	vec2 pt2 = p2;			tex2panel(pt2);
	vec2 v = (pt2 - pt1);

	double c = (dPosY-pt1.y)/v.y;
	double X = c*v.x + pt1.x;
	
	V = vec2( X, dPosY );
	V = V * ech;
	panel2tex(V);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::computeIntersectionGau(vec2& V, vec2 p1, vec2 p2 )                         
{
    double 	dPosX = -getPosX();
	    	
	vec2 pt1 = p1;			tex2panel(pt1);
	vec2 pt2 = p2;			tex2panel(pt2);
	vec2 v = (pt2 - pt1);

	double c = (dPosX-pt1.x)/v.x;
	double Y = c*v.y + pt1.y;
	
	V = vec2( dPosX, Y );
	V = V * ech;// - vDelta;
	panel2tex(V);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::computeEchAD_00( int idx, vec2 v )
{
	char		str[40];    
	pTextEch[idx]->setPos( v.x, v.y);

	vec2 pt = v;
	panel2tex(pt);
	vec2 vJ2000;
	pCapture->getFits()->tex_2_J2000( pt, vJ2000 );
	struct hms HMS;
	deg2hms( vJ2000.x, HMS );

	#ifdef DEG_MS
		snprintf( (char*)str, sizeof(str), "A.D.:%02.0lfh %02.0lf' %02.2lf\"", HMS.h, HMS.m, HMS.s );
	#else
		snprintf( (char*)str, sizeof(str), "A.D.:%lf alpha:%lf", vJ2000.x, angle);
	#endif

	//logf( (char*)"computeEchAD : (%.2lf, %.2lf) %s", pt.x, pt.y, (char*)str );	
	pTextEch[idx]->changeText( (char*)str ); 

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::computeEchDE_00( int idx, vec2 v )
{
	char		str[40];    
	pTextEch[idx]->setPos( v.x, v.y);

	vec2 pt = v;
	panel2tex(pt);
	vec2 vJ2000;
	pCapture->getFits()->tex_2_J2000( pt, vJ2000 );
	struct dms DMS;
	//if ( vJ2000.y >180.0
	deg2dms( vJ2000.y, DMS );

	#ifdef DEG_MS
		snprintf( (char*)str, sizeof(str), "Dec:%02.0lf° %02.0lf' %02.2lf\"", DMS.d, DMS.m, DMS.s );
	#else
		snprintf( (char*)str, sizeof(str), "Dec:%lf alpha:%lf", vJ2000.y, angle );
	#endif

	//logf( (char*)"computeEchAD : (%.2lf, %.2lf) %s", pt.x, pt.y, (char*)str );	
	pTextEch[idx]->changeText( (char*)str ); 

}
//--------------------------------------------------------------------------------------------------------------------
//
// 1deg / 3600 = 0,000277778
// 1h   / 3600 = 0,004166667
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::computeRepere_00()                         
{
    vec2 	vDelta	= vec2( getPosX(), getPosY() );
    double  ech 	= ech_geo * ech_user;
	    	
	for ( int i=0; i<p1.size(); i+=2 )	{
		vec2 V;
		if ( dAngleAD > 45.0 )				computeIntersectionHau( V, p1[i], p2[i] );
		else								computeIntersectionGau( V, p1[i], p2[i] );
		
		computeEchAD_00( i, V );

		if ( bNuit )						pTextEch[i]->setColor( COLOR32(255, 0, 0, 128) );
		else								pTextEch[i]->setColor( COLOR32(0, 0, 255, 255) );
	}


	for ( int i=1; i<p1.size(); i+=2 )	{
		vec2 V;
		if ( dAngleAD>45.0  )				computeIntersectionGau( V, p1[i], p2[i] );
		else								computeIntersectionHau( V, p1[i], p2[i] );

		if ( dAngleDE > 0.0 )   {
		    double size = pTextEch[i]->getTextLenght();
		    V += size * -vAD;
		}

		computeEchDE_00( i, V );
		
		if ( bNuit )						pTextEch[i]->setColor( COLOR32(255,   0,   0, 255) );
		else								pTextEch[i]->setColor( 0xFF0000FF );
	}
	
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::computeRepereAxe_01(int n)
{
	//log( (char*)" Calcul du repere" );
	int nb = p1.size();
	vec2 P1 = vec2( p1[n] );
	vec2 P2 = vec2( p2[n] );
	vec2 v = P2-P1;
	double a;

	for ( int i=n; i<nb; i+=2 )	{
		vec2 v1 = vec2( i*80.0 - getPosX(),   40.0 - getPosY() );
		v1 /= ech;
		vec2 v2;

		v2.y = getDY() - 40.0 - getPosY();
		v2.y /= ech;
		a = v.y / (v2.y-v1.y);
		v2.x =  ( v.x + a*v1.x ) / a;

		if ( v2.x<(-getPosX()/ech) )	{
			v2.x = 40.0 - getPosX();
			v2.x /= ech;
			a    =  v.x / (v2.x-v1.x);
			v2.y =  (v.y + a*v1.y) / a;
		}
		else if ( v2.x>(getDX()-40.0-getPosX())/ech )	{
			v2.x = getDX()  - 40.0 - getPosX();
			v2.x /= ech;
			a    =  v.x / (v2.x-v1.x);
			v2.y =  (v.y + a*v1.y) / a;
		}

		p1[i] = v1;
		p2[i] = v2;
	}		
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::computeRepere_01()
{
	computeRepereAxe_01(0);
	computeRepereAxe_01(1);
}
//--------------------------------------------------------------------------------------------------------------------
// 1deg / 3600 = 0,000277778
// 1h   / 3600 = 0,004166667
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::computeRepere_02()
{
	//log( (char*)" Calcul du repere" );
	
	double sec = 0.000277778;
	//double sec = 0,004166667;
		
	vec2 vAD_tex, v, v0, v1;
	v = vec2( getDY(), 0.0);

	screen2tex(v);
	
	pCapture->getFits()->tex_2_J2000( v, vAD_tex );

	logf( (char*)"----------------------------------------------------" );
	logf( (char*)"      v(%0.4lf, %0.4lf) ech=%lf", v.x, v.y, ech );
	logf( (char*)"     v1(%0.4lf, %0.4lf) ech=%lf", v1.x, v1.y, ech );
	logf( (char*)"vAD_tex(%.8lf, %.8lf)", vAD_tex.x, vAD_tex.y );
	//pCapture->getFits()->J2000_2_tex( vAD_tex, v );
	logf( (char*)"  mod v = %.8lf   0.000277778", fmod(vAD_tex.y, 0.004166667) );
	logf( (char*)"  div v = %.8lf   0.000277778", fdiv(vAD_tex.y, 0.004166667) );
	double _floor = floor(fdiv(vAD_tex.y, (double)0.004166667));
	logf( (char*)"floor v = %.8lf   0.000277778", _floor );
	double val = _floor * 0.004166667;
	logf( (char*)"    val = %.8lf   0.000277778", val );

	//struct hms HMS;
	struct dms DMS;

	//deg2hms( val, HMS );
	deg2dms( val, DMS );

	//logf( (char*)"  AD:%02.0lfh %02.0lf' %02.2lf\"", HMS.h, HMS.m, HMS.s );
	logf( (char*)"  DE:%02.0lf° %02.0lf' %02.2lf\"", DMS.d, DMS.m, DMS.s );


	computeRepere_00();
	
		
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::computeRepere()                         
{
	if ( ! pCapture->isFits() )			return;
	computeRepere_00();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::addP1P2(vec2 v, vec2 w)                         
{
	//-----------------------------------------------------------
	p1.push_back(vec2(v));
	p2.push_back(vec2(w)); 
	//-----------------------------------------------------------
    double 		angle;
    vec2		V0, V1;
    vec2		pt, vJ2000;
	char		str[40];    
    PanelText*	pInfo;
	//-----------------------------------------------------------
    V0 = w-v;
    V0.normalize();
    angle = acos(V0.x);
    angle = RAD2DEG(angle);
	if ( V0.y<0.0 )			angle *= -1.0;

    
    pInfo = new PanelText( (char*)str, PanelText::NORMAL_FONT, 0, 0); 


   	if ( (p1.size()%2) == 0 )	{
	    if ( angle >90.0 )          angle = -180.0 + angle;
	    dAngleAD = angle;
	    pInfo->setAlpha( (float)dAngleAD );
	    pInfo->setColor( 0xFF0000FF );

   		pt = v;
   		//panel2tex(pt);
		pCapture->getFits()->tex_2_J2000( pt, vJ2000 );
		struct hms HMS;
		deg2hms( vJ2000.x, HMS );

		//logf( (char*)"pt(%.2lf, %.2lf) => A.D.:%02.0lfh %02.0lf' %02.2lf\"", pt.x, pt.y, HMS.h, HMS.m, HMS.s );

		#ifdef DEG_MS
			snprintf( (char*)str, sizeof(str), "A.D.:%02.0lfh %02.0lf' %02.2lf\"", HMS.h, HMS.m, HMS.s );
		#else
			snprintf( (char*)str, sizeof(str), "A.D.:%lf alpha:%lf", vJ2000.x, angle);
		#endif
		if ( p1.size() <3 )	{
			vAD = w-v;
			vAD.normalize();
		}
	}
	else	{
	    angle = dAngleAD + 90.0;
	    if ( angle >90.0 )          angle = -180.0 + angle;
	    dAngleDE = angle;
	    pInfo->setAlpha( (float)dAngleDE );
	    pInfo->setColor( 0x0000FFFF );
   		pt = v;
		pCapture->getFits()->tex_2_J2000( pt, vJ2000 );
		struct dms DMS;
		deg2dms( vJ2000.y, DMS );

		#ifdef DEG_MS
			snprintf( (char*)str, sizeof(str), "Dec:%02.0lf° %02.0lf' %02.2lf\"", DMS.d, DMS.m, DMS.s );
		#else
			snprintf( (char*)str, sizeof(str), "Dec:%lf alpha:%lf", vJ2000.y, angle );
		#endif
		if ( p1.size() <3 )	{
			vDE = w-v;
			vDE.normalize();
		}
    }
    
    char STR0[40];
    char STR1[40];
    v.to_str(STR0);
    w.to_str(STR1);
    //logf( (char*)"Angle = %lf v%s w%s", angle, STR0, STR1 );

    pInfo->setPos( w.x, w.y );
    pInfo->updatePos();

    
    
    if ( pInfo->getParent() == NULL )	    add(pInfo);
	pTextEch.push_back( pInfo ); 
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

