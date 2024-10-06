#include "panel_capture.h"
#include "captures.h"
//--------------------------------------------------------------------------------------------------------------------
#define DEG_MS
#ifdef DEG_MS
#else
#endif
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelCapture::PanelCapture( struct readBackground*  pReadBgr, Capture* pc )
{
	dTelescopeAD = 0.0;
	dTelescopeDC = 0.0;
    ech_geo     = 1.0;
    ech_user    = 1.0;
    dx          = 0.0;
    dy          = 0.0;
    xm_old      = -1;
    ym_old      = -1;
    
    pReadBgr    = pReadBgr;
    pCapture    = pc;
    bIcone      = false;
	bInfoSouris = false;
	bAffGrille	= false;
    stars.setView( this );
    
    pFondCoord = new PanelSimple();
    pFondCoord->setBackground( (char*)"images/background.tga" );
    pFondCoord->setSize( 200,50 );
    this->add( pFondCoord );
    
    pTelescope = new PanelSimple();
    pTelescope->setBackground( (char*)"images/telescope.png" );
    pTelescope->setSize( 45,64 );
    this->add( pTelescope );
    
    pCoord = new PanelText( (char*)"(0,0)",			PanelText::NORMAL_FONT, 20, 10 );
    pJ2000_1 = new PanelText( (char*)"(0,0)",		PanelText::NORMAL_FONT, 20, 10 );
    pJ2000_2 = new PanelText( (char*)"(0,0)",		PanelText::NORMAL_FONT, 20, 10 );
	/*
    this->add( pCoord );
    this->add( pJ2000_1 );
    this->add( pJ2000_2 );
	*/    
    pFondCoord->add( pCoord );
    pFondCoord->add( pJ2000_1 );
    pFondCoord->add( pJ2000_2 );
    
    pVizier = NULL;
    
    setExtraString( "PanelViewCapture" );
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
void PanelCapture::deleteAllStars()
{
    stars.setRB( pReadBgr );

    stars.deleteAllStars();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::findAllStars()
{
    stars.setView( this );
    stars.setRB( pReadBgr );
    stars.findAllStars();
    return;
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
    computeEchelle();
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
        logf( (char*)"PanelCapture::updatePos() Changement d'echelle" );
        logf( (char*)"  %s", pCapture!=NULL? (char*)pCapture->getBasename().c_str() : (char*)"" );
        logf( (char*)"  ech_geo=%0.2lf ech_user=%0.2lf", coef, ech_user );
        logf( (char*)"  alphaAD=%0.2lf alphaED=%0.2lf", dAngleAD, dAngleDE );
        
        ech = ech_user * ech_geo;
        dx /= ech;
        dy /= ech;

        ech_geo = coef;

        ech = ech_user * ech_geo;

        dx *= ech;
        dy *= ech;
        
        setPos(dx, dy);
		setSize( (double)pReadBgr->w * ech, (double)pReadBgr->h * ech );
        
        logf( (char*)" dx=%0.2lf dy=%0.2lf", dx, dy );
    }

    if ( coef != ech_geo )    {
        //stars.update_stars( getPosX(), getPosY(), this, pReadBgr, ech_geo*ech_user );
    }
    
    ech = ech_user * ech_geo;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::updatePos()
{
    //PanelSimple::updatePos();
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

    if ( pVizier != NULL )
    {
	    int n = pVizier->size();
		Fits*	pFits	= pCapture->getFits();
		vec2 	d 		= vec2( getX(), getY() );
		double 	ech 	= ech_user * ech_geo;
		vec2	v, w;

        for ( int i=0; i<n; i++ )        {
            //pVizier->get(i)->setXScreen(x);
            //pVizier->get(i)->setYScreen(y);

            w.x = pVizier->get(i)->getRA();
            w.y = pVizier->get(i)->getDE();
			pFits->J2000_2_screen( w, v );
			
			v = ech * v;
            //v -= d;
            PanelText * p = pVizier->get(i)->getInfo();
            if ( bNuit )        p->setColor( COLOR32(255, 255, 0, 0) );
            else                p->setColor(  COLOR32(255, 255, 178, 0) );

            if ( p->getParent() == NULL )    this->add(p);
            vec2 vEch = vec2( 6.0, 6.0 );
            vEch *= ech;	vEch.y += 12; 
            p->setPos( v.x+vEch.x, v.y-vEch.y );
            p->updatePos();
            p->setAlpha(0.0);
            //p->setAlpha(45.0);

		    VarManager& var = VarManager::getInstance();
			if ( !pCapture->isIconized() && var.getb( "bAffCatalog"))	{
	            p->setVisible(true);
	        }
	        else	{
	            p->setVisible(false);
	        }
        }
    }    

    //stars.update_stars(getParent()->getX(), getParent()->getY(), this, NULL );

	if ( bAffGrille && !bIcone )	{
		setEchelleVisible(true);
	    updateEchelle();
	}
	else	{
		setEchelleVisible(false);
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::glCroix( int x,  int y,  int dx,  int dy )
{
	glBegin(GL_LINES);

	    glVertex2i(x, y-dy);         glVertex2i(x, y+dy);
	    glVertex2i(x-dx, y);         glVertex2i(x+dx, y);

    glEnd();        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::glCercle(int x, int y, int rayon)
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
void PanelCapture::displayTelescope()
{
	unsigned color_old = getColor();
	unsigned j = 0xFFFFFF00;

	vec2 	vTelescopeJ2000 = vec2( dTelescopeAD, dTelescopeDC );
	vec2 	vTelescopeScreen;
	vec2 	d 				= vec2( getX(), getY() );
	vec2	v;
	double 	ech 			= ech_user * ech_geo;

	pCapture->getFits()->J2000_2_screen( vTelescopeJ2000, vTelescopeScreen );
	v = ech * vTelescopeScreen + d;

    if ( bNuit )		{
		pTelescope->setColor( COLOR32(255, 255, 0, 0) );
		glColor4f( 255, 0, 0, 255 );
    }
    else	{
		pTelescope->setColor( j );
		glColor4f( COLOR_R(j), COLOR_G(j), COLOR_B(j), COLOR_A(j) );
    }
    glCercle( v.x, v.y, 15 );
    glCroix( v.x, v.y, 20, 20 );
	glColor4f( COLOR_A(color_old), COLOR_R(color_old), COLOR_G(color_old), COLOR_B(color_old) );
	
	
	pCapture->getFits()->J2000_2_screen( vTelescopeJ2000, vTelescopeScreen );
	vTelescopeScreen *= ech;
	pTelescope->setPos( vTelescopeScreen.x+20, vTelescopeScreen.y+20);
	pTelescope->onTop();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::displayCatalog()
{
	//logf( (char*)"PanelCapture::displayCatalog() %d etoiles", vizier.size() );
	// Sauvegarde de l'ancienne couleur
	unsigned color_old = getColor();
	unsigned c  = 0xFFFF00FF;
	unsigned r  = 0xFF0000FF;
	unsigned vv = 0x00FF00FF;
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
    if ( n!= 0)
    {
        if ( bNuit )        glColor4f( 1.0,  0.0,  0.0, 1.0 );
        else                glColor4f( 1.0, 0.7, 0.0, 0.8 );

        for ( int i=0; i<n; i++ )
        {
			PanelText* p = pVizier->get(i)->getInfo();

            w.x = pVizier->get(i)->getRA();
            w.y = pVizier->get(i)->getDE();

			//v = m * w;
			pFits->J2000_2_screen( w, v );
			//p->setPos( w.x, w.y );
            
			v = ech * v + d;
			
			//glVertex2i(v.x, v.y);			//vizier.get(i)->pInfo->setPos(x+10-getX(), y-80);

            double r = 13.5 - 0.675*pVizier->get(i)->fMag;
            r *= ech;
            glCercle( v.x, v.y, r );

			p->setPos( v.x+15, w.y+15 );
			p->updatePos();

            if ( bNuit )        p->setColor( COLOR32(255, 255, 0, 0) );
            else                p->setColor(  COLOR32(255, 255, 178, 0) );
        }
    }    

    glEnd();
	// retour a l'ancienne couleur
	glColor4f( COLOR_A(color_old), COLOR_R(color_old), COLOR_G(color_old), COLOR_B(color_old) );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::displayAxe()
{
	// Sauvegarde de l'ancienne couleur
	unsigned color_old = getColor();
	unsigned c  = 0xFFFF00FF;
	unsigned r  = 0xFF0000FF;
	unsigned vv = 0x00FF00FF;
	unsigned b  = 0x0000FFFF;

	//logf( (char*)"Old Color=%0lX", color_old );
	//logf( (char*)"New Color=%0lX", c );

	vec2 d = vec2( getX(), getY() );
	double ech = ech_user * ech_geo;

	glBegin(GL_LINES);

	vec2 v, w;

	if (bNuit)		glColor4f( COLOR_A(r), COLOR_R(r), COLOR_G(r), COLOR_B(r) );

	for( int i=0; i<p1.size(); i++ )
	{
		if (!bNuit )	{
			if ( (i%2) == 1)    glColor4f( COLOR_A(r), COLOR_R(r), COLOR_G(r), COLOR_B(r) );
			else			    glColor4f( COLOR_A(b), COLOR_R(b), COLOR_G(b), COLOR_B(b) );
		}
		v = ech * p1[i] + d;
		w = ech * p2[i] + d;
	    glVertex2i(v.x, v.y);
	    glVertex2i(w.x, w.y);


	}

    glEnd();
	// retour a l'ancienne couleur
	glColor4f( COLOR_A(color_old), COLOR_R(color_old), COLOR_G(color_old), COLOR_B(color_old) );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::displayGL()
{
    double gris = 0.3;
    VarManager& var = VarManager::getInstance();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	/*
    if ( bNuit )        glColor4f( gris,  0.0,  0.0, 1.0 );
    else                glColor4f( gris, gris, gris, 0.2 );    
    */
    
    if ( bNuit )		{
    	setColorBgr( COLOR32(255, 255, 0, 0) );
    	pCoord->setColor( COLOR32(255, 255, 0, 0) );
    	pJ2000_1->setColor( COLOR32(255, 255, 0, 0) );
    	pJ2000_2->setColor( COLOR32(255, 255, 0, 0) );
    }
    else	{
    	setColorBgr( COLOR32(255, 255, 255, 255) );    
    	pCoord->setColor( COLOR32(255, 255, 255, 255) );
    	pJ2000_1->setColor( COLOR32(255, 255, 255, 255) );
    	pJ2000_2->setColor( COLOR32(255, 255, 255, 255) );
    }
    
	
	
    //if ( bNuit )        glColor4f( gris,  0.0,  0.0, 1.0 );
    //else                glColor4f( gris, gris, gris, 0.2 );    
    
    PanelSimple::displayGL();
    
    stars.setView( this );
    //stars.setRB( pReadBgr );
    if ( !bIcone )					stars.displayGL();

	if ( !pCapture->isIconized() )	{
		if ( pCapture->isFits() )	{
			if ( bAffGrille )									displayAxe();

			if ( var.getb( "bAffCatalog") && (pVizier) )		displayCatalog();
		}
	}	
	
	if ( 0<xTelescope && xTelescope<getDX() && 0<yTelescope && yTelescope<getDY()  )	
	{
		pTelescope->setVisible(true);
		displayTelescope();
	}
	else
		pTelescope->setVisible(false);
	

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::idle(float f)
{
    //VarManager& var = VarManager::getInstance();
    //----------------------------------------------
	if ( !bIcone  )	{
		stars.setVisible(true);
		stars.updateScreenPos( dx+pCapture->getX(), dy+pCapture->getY(), ech);
	}	
	else	{
		stars.setVisible(false);
	}    
	stars.idle();
    PanelSimple::idle(f);
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
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::wheelUp(int xm, int ym)
{
    logf( (char*)"PanelCapture::wheelUp(%d,%d) ... 2tex(%d,%d)", xm, ym, screen2texX(xm), screen2texY(ym) );
    log_tab(true);
    //Captures::getInstance().glutSpecialFunc(104, xm, ym);
    
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

    setPos( -X1, -Y1 );
    dx = -X1;
    dy = -Y1;
    
    //stars.update_stars( getX(), getY(), this, pReadBgr, ech );

    log_tab(false);
    
    updatePos();
    passiveMotionFunc( xm, ym );
    pCapture->updatePosIcones();

    stars.updateScreenPos( dx+pCapture->getX(), dy+pCapture->getY(), ech);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::wheelDown(int xm, int ym)
{
    logf( (char*)"PanelCapture::wheelDown(%d,%d) ... 2tex(%d,%d)", xm, ym, screen2texX(xm), screen2texY(ym) );
    //log_tab(true);
    //Captures::getInstance().glutSpecialFunc(105, xm, ym);
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
    
    /*    
    if ( X1<0 )             X1 = 0.0;
    else
    if ( X1>max_x )         X1 = max_x;
    
    if ( Y1<0 )             Y1 = 0.0;
    else
    if ( Y1>max_y )         Y1 = max_y;
    */
    int x1 = X1;
    int y1 = Y1;
    
    clip( x1, y1 );
    setPos( -x1, -y1 );
    dx = -x1;
    dy = -y1;
    
    //stars.update_stars( getX(), getY(), this, pReadBgr, ech_geo*ech_user );

    //log_tab(false);
    updatePos();    
    passiveMotionFunc( xm, ym );
    pCapture->updatePosIcones();

    stars.updateScreenPos( dx+pCapture->getX(), dy+pCapture->getY(), ech);
}
//--------------------------------------------------------------------------------------------------------------------
//
// xx, yy coordonnée écran du pointeur de souris
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::passiveMotionFunc(int xm, int ym)
{
	if ( !bInfoSouris || bIcone ) 			{ return; }
	
	static char coord[80];
	//logf( (char*)"PanelCapture::passiveMotionFunc( %d, %d )", xm, yy );
	log_tab(true);

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
	
	// Calculde l'offset dans le buffer
	long idx = 3 * (w * YY + XX);
	
	// Recuperation de la couleur du pixel
	r = ptr[ idx + 0 ];
	g = ptr[ idx + 1 ];
	b = ptr[ idx + 2 ];
	
	// Affichage du resultat	
	snprintf( (char*)coord, sizeof(coord), "rvb = (%d, %d, %d)", (unsigned)r, (unsigned)g, (unsigned)b );
	pCoord->changeText( (char*)coord );

	vec2 vJ2000;
	
	if ( pCapture->isFits() )	{
		vec2 sc = vec2( XX, YY ); 
		pCapture->getFits()->screen_2_J2000( sc, vJ2000 );
		
		struct dms DMS;
		struct hms HMS;
		deg2hms( vJ2000.x, HMS );
		deg2dms( vJ2000.y, DMS );
		
		
#ifdef DEG_MS
		snprintf( (char*)coord, sizeof(coord), "%ld %ld \tAD:%02.0fh %02.0f' %02.2f\"", XX, YY, HMS.h, HMS.m, HMS.s );
#else
		snprintf( (char*)coord, sizeof(coord), "%ld %ld \t%f", XX, YY, vJ2000.x );
#endif
		pJ2000_1->changeText( (char*)coord );
		pJ2000_1->setChangeText(true);
		//pJ2000_1->setPos( x+15, y+30);
		pJ2000_1->setPos( 5, 17);

#ifdef DEG_MS
		snprintf( (char*)coord, sizeof(coord), "\t\tDE:%2.0fd %02.0f' %02.2f\"", DMS.d, DMS.m, DMS.s );
#else
		snprintf( (char*)coord, sizeof(coord), "\t\t%f", vJ2000.y );
#endif
		pJ2000_2->changeText( (char*)coord );
		pJ2000_2->setChangeText(true);
		pJ2000_2->setPos( 5, 32);
	}
	// Mise a jour du panelText
	// ans la position de la souris
	// Force la fabrication de la chaine de caractere
	pFondCoord->setPos( x+15, y+15);
	pFondCoord->onTop();

	pCoord->setPos( 5, 2);
	pCoord->setChangeText(true);
	
	log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::clickLeft(int xm, int ym)
{
    logf( (char*)"PanelCapture::clickLeft(%d,%d) ...", xm, ym );
    if ( pCapture->isIconized() )
	    Captures::getInstance().setCurrent( pCapture );

    Panel::clickLeft(xm,ym);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::releaseLeft(int xm, int ym)
{
    logf( (char*)"PanelCapture::releaseLeft(%d,%d) ...", xm, ym );

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
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::clickRight(int xm, int ym)
{
    Captures::getInstance().setCurrent( pCapture );
    PanelSimple::clickRight(xm,ym);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::motionRight(int xm, int ym)
{
    logf( (char*)"PanelCapture::motionRight(%d,%d) ...", xm, ym );
	PanelSimple::motionRight( xm, ym );
    updatePos();    
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
        stars.setView( this );
        stars.setRB( pReadBgr );
        if ( stars.addStar( xm, ym, getX(), getY(), e ) == NULL )
        {
            stars.selectStar(xx, yy);
            logf( (char*)"releaseMiddle(%d,%d) selects star...", xm, ym );
        }
        else
            stars.selectStar(xx, yy);
    }
    bHaveMove = false;
    
    if ( pCapture->isFits() )    {
		sendStellarium( xm, ym );
    }
    log_tab(false);

    computeEchelle();
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

    //setPos( deltax, deltay );
    Panel* p = getParent();
    if ( p!= NULL )
    {
        //logf( (char*)"delta : %d", p->getPosY() );
        stars.set_delta( deltax + p->getPosX(), deltay + p->getPosY() );
    }
    
    /*
    if ( Captures::getInstance().isCurrent(pCapture) )
    {
		logf( (char*)"PanelCapture::setCent()" );
		log_tab(true);

		logf( (char*)"(fDX,fDY):\t(%0.2f,%0.2f)", fDX, fDY );
		logf( (char*)"(fw,fh):\t(%0.2f,%0.2f)", fw, fh );
		logf( (char*)"(dxp,dyp):\t(%d,%d)", dxp, dyp );
		logf( (char*)"(deltax,deltay):\t(%0.2f,%0.2f)", deltax, deltay );
		
		log_tab(false);
	}
	*/
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
int PanelCapture::screen2texX( int x )
{
	double X = x - pCapture->getX();
    return ( (double)X-dx) / (ech);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int PanelCapture::screen2texY( int y )
{
	double Y = y - pCapture->getY();
    return ( (double)Y-dy) / (ech);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::screen2tex(int& x, int& y)
{
    x = screen2texX(x);
    y = screen2texY(y);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::screen2tex(vec2& v)
{
	v.x = screen2texX(v.x);
	v.y = screen2texY(v.y);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int PanelCapture::tex2screenX( int x )
{
    return (double)x * ( ech_geo*ech_user) + dx;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int PanelCapture::tex2screenY( int y )
{
    return (double)y * (ech_geo*ech_user) + dy;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::tex2screen(int& x, int& y)
{
    x = tex2screenX(x);
    y = tex2screenY(y);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::tex2screen(vec2& v)
{
	v.x = tex2screenX(v.x);
	v.y = tex2screenY(v.y);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::printObjet()
{
    logf( (char*)"PanelCapture::printObjet()" );
    log_tab(true);
    logf( (char*)"this->pos (%d, %d)", getPosX(), getPosY() );
    logf( (char*)"this->dim %d x %d", getDX(), getDY() );
    logf( (char*)"this->ech_geo=%0.2lf",  ech_geo );
    logf( (char*)"this->ech_user=%0.2lf", ech_user );
    if ( pReadBgr == NULL )    {
        logf( (char*)"pReadBgr = NULL" );
    } else {
        logf( (char*)"this->pReadBgr->w %d", pReadBgr->w.load() );
        logf( (char*)"this->pReadBgr->h %d", pReadBgr->h.load() );
        logf( (char*)"this->pReadBgr->d %d", pReadBgr->d.load() );
    }
    log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::findGaiaDR3()
{
    logf( (char*)"PanelCapture::findGaiaDR3()" );

	if ( pVizier )	{
	    //logf( (char*)"|  Efface le catalog" );
		pVizier->efface();
		delete pVizier;
		pVizier = NULL;
		return;
	}

	// CALCUL LA TAILLE DE L'IMAGE
	double resH = pCapture->getFits()->getNAXIS1();
	double resV = pCapture->getFits()->getNAXIS2();
	
	resH *= pCapture->getFits()->getCDELT1() * 3600.0;
	resV *= pCapture->getFits()->getCDELT2() * 3600.0;
	
	double res;
	if ( resH > resV )			res = resH;
	else						res = resV;
	if ( res>3500.0 )       res = 3500.0;
	// RECHERCHE DU CENTRE DE L'IMAGE
	static char coord[80];
	double xCentre = pCapture->getFits()->getCRVAL1();
	double yCentre = pCapture->getFits()->getCRVAL2();

	snprintf( (char*)coord, sizeof(coord), "-r %d% 0.8f %0.8f", (int)res, xCentre, yCentre );
	logf( (char*)"Requete Gaia DR3 : %s", coord );	

	pVizier = new Catalog(false);

	// lance la requete
	vizier_load_stars( pVizier, string(coord) );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::sendStellarium( int xm, int ym)
{
    logf( (char*)"PanelCapture::sendStellarium()" );

		long x = Screen2x(xm);
		long y = Screen2y(ym);
		
		// coordonnées image	
		double X = (double)x/(ech_user*ech_geo);
		double Y = (double)y/(ech_user*ech_geo);
		
		long XX = X;
		long YY = Y;

		
		// Recuperation des infos image largueur hauteur et pointeur sur RGBRGBRGBRGBRGBRGB
		vec2 vJ2000;
		vec2 screen = vec2( XX, YY );
		pCapture->getFits()->screen_2_J2000( screen, vJ2000 );
		
		struct dms DMS;
		struct hms HMS;
		deg2hms( vJ2000.x, HMS );
		deg2dms( vJ2000.y, DMS );
		
				
		static char coord[80];
		snprintf( (char*)coord, sizeof(coord), "(%0.6f, %0.6f)", vJ2000.x, vJ2000.y );
		log( (char*)coord );	
		
		
        VarManager& var= VarManager::getInstance();
        if ( var.getb("bModeManuel") )  {
            char cmd[255];
            double AD = vJ2000.x >180.0 ? -360.0 + vJ2000.x : vJ2000.x;
            
            sprintf( cmd, "A%lf;D%lf", AD, (vJ2000.y) );
            logf( (char*)"  Envoi arduino : %s", cmd );
            Serial::getInstance().write_string(cmd);
        }
        else    {
    		Serveur_mgr::getInstance().write_stellarium( (double)vJ2000.x, (double)vJ2000.y );
        }

		//change_ad_status( vJ2000.x );
		//change_dc_status( vJ2000.y );
		
		/*
		snprintf( (char*)coord, sizeof(coord), "screen (%f, %f)", screen.x, screen.y );
		log( (char*)coord );	
		
		mat2 m = pCapture->getFits()->getMatrix();
		vJ2000 = m * screen;
		//pCapture->getFits()->screen_2_J2000( screen, vJ2000 );
		snprintf( (char*)coord, sizeof(coord), "to J2000(%f, %f)", vJ2000.x, vJ2000.y );
		log( (char*)coord );	

		mat2 n = m.inverse();
		screen = n * vJ2000;
		//pCapture->getFits()->J2000_2_screen( vJ2000, screen );
		snprintf( (char*)coord, sizeof(coord), "to screen (%0.6f, %0.6f)", screen.x, screen.y );
		log( (char*)coord );	
		*/
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::setInfoSouris(bool b)  
{
	//logf( (char*)"void PanelCapture::setInfoSouris(%s)", BOOL2STR(b) );
	bInfoSouris = b; 
	
	if (b) 	{
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
		
		pCoord->setPos( 5, 2);
		pCoord->updatePos();
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
	bAffGrille = false;
	bInfoSouris = false;
	setInfoSouris( false );
	bIcone = true;
	
	//updateEchelleGeo();
	
	logf( (char*)" setPos(%lfd, %lf)", (dx), (dy) );
	logf( (char*)" ech_geo=%lf, ech_user=%lf)", ech_geo, ech_user );
	setPos( dx, dy );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::restaure(bool bGril, bool bbSour )
{
	logf( (char*)"PanelCapture::restaure()" );
	
	bIcone = false;
	bAffGrille = bGril;
	bInfoSouris = bbSour;
	setInfoSouris( bbSour );
	//setPos(dx, dy);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::change_ad( double ad )
{
	//logf( (char *)"PanelCapture::change_ad(%lf)", ad );
	dTelescopeAD = ad<0.0 ? 360.0+ad : ad ;
	
	vec2 vTelescopeJ2000  = vec2( dTelescopeAD, dTelescopeDC );
	vec2 vTelescopeScreen;
	
	
	vec2 	d 				= vec2( getX(), getY() );
	double 	ech 			= ech_user * ech_geo;
	vec2	v;

	pCapture->getFits()->J2000_2_screen( vTelescopeJ2000, vTelescopeScreen );
	v = (ech * vTelescopeScreen );
	xTelescope = v.x;

	char STRJ[50];
	char STRV[50];
	vTelescopeJ2000.to_str(STRJ);
	v.to_str(STRV);

	logf( (char*)"PanelCapture::change_ad()  J2000%s vScreen%s  ech=%lf", STRJ, STRV, ech );
	logf( (char*)" change_ad %s",BOOL2STR( 0<v.x && v.x<getDX()) );
	
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::change_dc( double dc )
{
	//logf( (char *)"PanelCapture::change_dc(%lf)", dc );
	dTelescopeDC = dc;
	
	vec2 vTelescopeJ2000  = vec2( dTelescopeAD, dTelescopeDC );
	vec2 vTelescopeScreen;
	
	vec2 	d 				= vec2( getX(), getY() );
	vec2	v;
	double 	ech 			= ech_user * ech_geo;

	pCapture->getFits()->J2000_2_screen( vTelescopeJ2000, vTelescopeScreen );
	v = vTelescopeScreen * ech;
	yTelescope = v.y;

	char STRJ[50];
	char STRV[50];
	vTelescopeJ2000.to_str(STRJ);
	v.to_str(STRV);

	logf( (char*)"PanelCapture::change_ad()  J2000%s vScreen%s  ech=%lf", STRJ, STRV, ech );
	logf( (char*)" change_dc %s", BOOL2STR( 0<v.x && v.x<getDX())  );
	
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
    vec2 	vDelta	= vec2( getPosX(), getPosY() );
	    	
	vec2 pt1 = p1;			tex2screen(pt1);
	vec2 pt2 = p2;			tex2screen(pt2);
	vec2 v = (pt2 - pt1);

	double c = -pt1.y/v.y;
	double X = c*v.x + pt1.x;
	
	V = vec2( X, 0.0 );
	V = V - vDelta;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::computeIntersectionBas(vec2& V, vec2 p1, vec2 p2 )                         
{
    double  ech 	= ech_geo * ech_user;
    vec2 	vDelta	= vec2( getPosX(), getPosY() );
	    	
	vec2 pt1 = p1;			tex2screen(pt1);
	vec2 pt2 = p2;			tex2screen(pt2);
	vec2 v = (pt2 - pt1);

	double c = ((double)getDY()-pt1.y)/v.y;
	double X = c*v.x + pt1.x;
	
	V = vec2( X, (double)getDY() );
	V = V - vDelta;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::computeIntersectionGau(vec2& V, vec2 p1, vec2 p2 )                         
{
    vec2 	vDelta	= vec2( getPosX(), getPosY() );
	    	
	vec2 pt1 = p1;			tex2screen(pt1);
	vec2 pt2 = p2;			tex2screen(pt2);
	vec2 v = (pt2 - pt1);

	double c = -pt1.x/v.x;
	double Y = c*v.y + pt1.y;
	
	V = vec2( 0.0, Y );
	V = V - vDelta;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::computeEchelle()                         
{
    vec2 	vDelta	= vec2( getPosX(), getPosY() );
    double  ech 	= ech_geo * ech_user;
	    	
	for ( int i=0; i<p1.size(); i+=2 )	{
		vec2 V;
		if ( dAngleAD > 45.0 )				computeIntersectionHau( V, p1[i], p2[i] );
		else								computeIntersectionGau( V, p1[i], p2[i] );
		
		pTextEch[i]->setPos( V.x, V.y);
		
		if ( bNuit )			pTextEch[i]->setColor( COLOR32(255, 255, 0, 0) );
		else					pTextEch[i]->setColor( COLOR32(255,   0, 0, 255) );
	}


	for ( int i=1; i<p1.size(); i+=2 )	{
		vec2 V;
		if ( dAngleAD>45.0  )						computeIntersectionGau( V, p1[i], p2[i] );
		else										computeIntersectionHau( V, p1[i], p2[i] );

		pTextEch[i]->setPos( V.x, V.y );
		if ( bNuit )			pTextEch[i]->setColor( COLOR32(255, 255, 0, 0) );
		else					pTextEch[i]->setColor( COLOR32(255, 255, 0, 0) );
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::addP1P2(vec2 v, vec2 w)                         
{
	p1.push_back(vec2(v));
	p2.push_back(vec2(w)); 

    double angle;
    vec2 V0, V1;
    V0 = w-v;
    V0.normalize();
    angle = acos(V0.x);
    angle = RAD2DEG(angle);
	if ( V0.y<0.0 )			angle *= -1.0;

	char str[40];    
    PanelText * p;
    vec2 pt, vJ2000;
    
    p = new PanelText( (char*)str, PanelText::NORMAL_FONT, 0, 0); 


   	if ( (p1.size()%2) != 0 )	{
	    if ( angle >90.0 )          angle = -180.0 + angle;
	    dAngleAD = angle;
	    p->setAlpha( (float)dAngleAD );
	    p->setColor( 0xFF0000FF );

   		pt = v;
		pCapture->getFits()->screen_2_J2000( pt, vJ2000 );
		struct hms HMS;
		deg2hms( vJ2000.x, HMS );

		#ifdef DEG_MS
		snprintf( (char*)str, sizeof(str), "A.D.:%02.0lfh %02.0lf' %02.2lf\"", HMS.h, HMS.m, HMS.s );
		#else
		snprintf( (char*)str, sizeof(str), "A.D.:%lf alpha:%lf", vJ2000.x, angle);
		#endif
	}
	else	{
	    angle = dAngleAD + 90.0;
	    if ( angle >90.0 )          angle = -180.0 + angle;
	    dAngleDE = angle;
	    p->setAlpha( (float)dAngleDE );

	    p->setColor( 0xFFFF0000 );
   		pt = v;
		pCapture->getFits()->screen_2_J2000( pt, vJ2000 );
		struct dms DMS;
		deg2dms( vJ2000.y, DMS );

		#ifdef DEG_MS
		snprintf( (char*)str, sizeof(str), "Dec:%02.0lfd %02.0lf' %02.2lf\"", DMS.d, DMS.m, DMS.s );
		#else
		snprintf( (char*)str, sizeof(str), "Dec:%lf alpha:%lf", vJ2000.y, angle );
		#endif
    }
    
    char STR0[40];
    char STR1[40];
    v.to_str(STR0);
    w.to_str(STR1);
    logf( (char*)"Angle = %lf v%s w%s", angle, STR0, STR1 );

    p->setPos( w.x, w.y );
    p->updatePos();

    
    
    if ( p->getParent() == NULL )	    add(p);
	pTextEch.push_back( p ); 
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

