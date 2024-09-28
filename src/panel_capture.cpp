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
    
    pCoord = new PanelText( (char*)"(0,0)",		PanelText::NORMAL_FONT, 20, 10 );
    this->add( pCoord );
    pJ2000_1 = new PanelText( (char*)"(0,0)",		PanelText::NORMAL_FONT, 20, 10 );
    this->add( pJ2000_1 );
    pJ2000_2 = new PanelText( (char*)"(0,0)",		PanelText::NORMAL_FONT, 20, 10 );
    this->add( pJ2000_2 );
    
    pVizier = NULL;
    
    setExtraString( "PanelViewCapture" );
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
		pJ2000_1->setPos( x+15, y+30);

#ifdef DEG_MS
		snprintf( (char*)coord, sizeof(coord), "\t\tDE:%2.0fd %02.0f' %02.0f\"", DMS.d, DMS.m, DMS.s );
#else
		snprintf( (char*)coord, sizeof(coord), "\t\t%f", vJ2000.y );
#endif
		pJ2000_2->changeText( (char*)coord );
		pJ2000_2->setChangeText(true);
		pJ2000_2->setPos( x+15, y+45);
	}
	// Mise a jour du panelText
	// ans la position de la souris
	// Force la fabrication de la chaine de caractere
	pCoord->setPos( x+15, y+15);
	pCoord->setChangeText(true);
	
	log_tab(false);
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
        logf( (char*)"Stop setEchelle(%0.2f)", ech_user );
        return;
    }

    ech_user = f;
    
    
    setSize( fDX*ech_user, fDY*ech_user );
    setCent();
    logf( (char*)"PanelCapture::setEchelle() ech_geo=%0.2f ech_user=%0.2f dx=%0.2f dy=%0.2f", ech_geo, ech_user, dx, dy );
    logf( (char*)"  setSize(%0.2f, %0.2f)", fDX, fDY );
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::update_stars()
{
    if  ( pReadBgr==NULL )      logf( (char*)"PanelCapture::update()   pointeur RB NULL" );

    //Panel* pParent = getParent();
    stars.update_stars( getPosX(), getPosY(), this, pReadBgr, ech_user );

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::updatePosLigne()
{
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

    double coef0 = (double)pReadBgr->w / getParent()->getDX();// * ech_user;
    double coef1 = (double)pReadBgr->h / getParent()->getDY();// * ech_user;
    double coef;
    if ( coef0 > coef1 )        coef = 1.0/coef1;
    else                        coef = 1.0/coef0;
    
    if ( coef != ech_geo )
    {
        logf( (char*)"PanelCapture::updatePos() Changement d'echelle" );
        logf( (char*)"  %s", pCapture!=NULL? (char*)pCapture->getBasename().c_str() : (char*)"" );
        logf( (char*)"  ech_geo=%0.2f ech_user=%0.2f", coef, ech_user );
        ech_geo = coef;
    }
    
    //setEchelle(coef);
    double fDX = (double)pReadBgr->w * ech_geo;
    double fDY = (double)pReadBgr->h * ech_geo;

    setSize( fDX*ech_user, fDY*ech_user );
    setCent();


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
            if ( p->getParent() == NULL )    this->add(p);
            vec2 vEch = vec2( 6.0, 6.0 );
            vEch *= ech;	vEch.y += 12; 
            p->setPos( v.x+vEch.x, v.y-vEch.y );
            p->updatePos();

		    VarManager& var = VarManager::getInstance();
			if ( !pCapture->isIconized() && var.getb( "bAffCatalog"))		
	            p->setVisible(true);
	        else
	            p->setVisible(false);
        }
    }    

    //stars.update_stars(getParent()->getX(), getParent()->getY(), this, NULL );

    PanelSimple::updatePos();

    if ( coef != ech_geo )
    {
        stars.update_stars( getPosX(), getPosY(), this, pReadBgr, ech_geo*ech_user );
    }
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
	
	if ( pFits == NULL ) 					return;
	//-------------------------------------------------------------------

	glBegin(GL_LINES);

	vec2 v, w;
	glColor4f( COLOR_A(r), COLOR_R(r), COLOR_G(r), COLOR_B(r) );

    int n = pVizier->size();
    if ( n!= 0)
    {
        if ( bNuit )        glColor4f( 1.0,  0.0,  0.0, 1.0 );
        else                glColor4f( 1.0, 0.7, 0.0, 0.8 );

        for ( int i=0; i<n; i++ )
        {

            w.x = pVizier->get(i)->getRA();
            w.y = pVizier->get(i)->getDE();

			//v = m * w;
			pFits->J2000_2_screen( w, v );
			PanelText* p = pVizier->get(i)->getInfo();
			p->setPos( w.x, w.y );
			p->updatePos();
			//logf( (char*)"PanelCapture::displayCatalog() etoile (%0.6f, %0.6f) %d/%d",  w.x, w.y, i, n );
            
			v = ech * v + d;
			
			
			glVertex2i(v.x, v.y);			//vizier.get(i)->pInfo->setPos(x+10-getX(), y-80);

            double r = ( 30.0 - 1.5*pVizier->get(i)->fMag ) * 0.45;
            r *= ech;
            glCercle( v.x, v.y, r );
        }
    }    

    glEnd();
	// retour a l'ancienne couleur
	glColor4f( COLOR_A(color_old), COLOR_R(color_old), COLOR_G(color_old), COLOR_B(color_old) );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::displayEchelle()
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
	glColor4f( COLOR_A(r), COLOR_R(r), COLOR_G(r), COLOR_B(r) );

	for( int i=0; i<p1.size(); i++ )
	{
		if ( (i%2) == 1)    glColor4f( COLOR_A(b), COLOR_R(b), COLOR_G(b), COLOR_B(b) );
		else			    glColor4f( COLOR_A(r), COLOR_R(r), COLOR_G(r), COLOR_B(r) );

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


    if ( bNuit )        glColor4f( gris,  0.0,  0.0, 1.0 );
    else                glColor4f( gris, gris, gris, 0.2 );    
    
    PanelSimple::displayGL();

    stars.setView( this );
    //stars.setRB( pReadBgr );
    stars.displayGL();

	if ( !pCapture->isIconized() )	{
		if ( pCapture->isFits() )	{
			if ( bAffGrille )									displayEchelle();

			if ( var.getb( "bAffCatalog") && (pVizier) )		displayCatalog();
		}
	}	
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
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
    
    stars.update_stars( getX(), getY(), this, pReadBgr, ech_geo*ech_user );
    
    logf( (char*)"XY    (%0.2f, %0.2f)  (%0.2f, %0.2f)", X0, Y0, X1, Y1 );
    logf( (char*)"Mouse (%0.2f, %0.2f)  (%0.2f, %0.2f)", (double)xm, (double)ym, XM, YM );
    logf( (char*)"MAX   (%0.2f, %0.2f)", max_x, max_y );
    logf( (char*)"ech   (%0.2f, %0.2f) bgr(%d, %d)", ech_geo, ech_user, (int)pReadBgr->w, (int)pReadBgr->h );
    logf( (char*)"win   (%0.2f, %0.2f) (%0.2f, %0.2f)", (double)getX(), (double)getY(), (double)p->getDX(), (double)p->getDY() );
    printObjet();

    log_tab(false);
    
    passiveMotionFunc( xm, ym );
    updatePos();
    stars.update_stars( getX(), getY(), this, pReadBgr, ech_geo*ech_user );
    pCapture->updatePosIcones();
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
    
    stars.update_stars( getX(), getY(), this, pReadBgr, ech_geo*ech_user );
    /*
    logf( (char*)"XY    (%0.2f, %0.2f)  (%0.2f, %0.2f)", X0, Y0, X1, Y1 );
    logf( (char*)"Mouse (%0.2f, %0.2f)  (%0.2f, %0.2f)", (double)xm, (double)ym, XM, YM );
    logf( (char*)"MAX   (%0.2f, %0.2f)", max_x, max_y );
    logf( (char*)"ech   (%0.2f, %0.2f) bgr(%d, %d)", ech_geo, ech_user, (int)pReadBgr->w, (int)pReadBgr->h );
    logf( (char*)"win   (%0.2f, %0.2f) (%0.2f, %0.2f)", (double)getX(), (double)getY(), (double)p->getDX(), (double)p->getDY() );
    printObjet();
    */
    //log_tab(false);
    passiveMotionFunc( xm, ym );
    updatePos();    stars.update_stars( getX(), getY(), this, pReadBgr, ech_geo*ech_user );
    pCapture->updatePosIcones();
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
    updatePos();    stars.update_stars( getX(), getY(), this, pReadBgr, ech_geo*ech_user );
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
    double e = (double)getDX() / (double)pReadBgr->w; 
    
    //logf( (char*)"XY    (%0.2f, %0.2f)  (%0.2f, %0.2f)", X0, Y0, X1, Y1 );
    //logf( (char*)"Mouse (%0.2f, %0.2f) ", (double)xm, (double)ym );
    //logf( (char*)"MAX   (%0.2f, %0.2f)", max_x, max_y );
    //logf( (char*)"ech   (%0.2f, %0.2f, %0.2f) bgr(%d, %d)", ech_geo, ech_user, e, (int)pReadBgr->w, (int)pReadBgr->h );
    //logf( (char*)"win   (%0.2f, %0.2f)", (double)p->getDX(), (double)p->getDY() );
    //logf( (char*)"Pos   (%0.2f, %0.2f)", (double)getPosX(), (double)getPosY() );
    //stars.set_delta( deltaX, deltaY );
    
    xm_old = xm;
    ym_old = ym;
    
    bHaveMove = true;
 
 
    stars.update_stars( getX(), getY(), this, pReadBgr, ech_geo*ech_user );
    //log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::releaseMiddle(int xm, int ym)
{
    xm_old = -1;
    ym_old = -1;
    logf( (char*)"PanelCapture::releaseMiddle(%d,%d) ...", xm, ym );
    log_tab(true);
    logf( (char*)"ech_geo=%0.2f ech_user=%0.2f dx=%0.2f dy=%0.2f", ech_geo, ech_user, dx, dy );

    if ( pReadBgr == NULL )     { logf( (char*)"Pointeur NULL" ); log_tab(false); return; }
    
    
    //double e = (double)getDX() / (double)pReadBgr->w; 
    double e = (double)getDX() / (double)pReadBgr->w; 
    int xx = ((double)xm-(double)getX()) / e;
    int yy = ((double)ym-(double)getY()) / e;
    
    logf( (char*)"echelle%0.2f", e );
    logf( (char*)"Pos (%0.2f, %0.2f)", (double)getX(), (double)getY()  );
    
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
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int PanelCapture::screen2texX( int x )
{
    return (double)x / ( ech_geo*ech_user) + dx;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int PanelCapture::screen2texY( int y )
{
    return (double)y / (ech_geo*ech_user) + dy;
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
int PanelCapture::tex2screenX( int x )
{
    return (double)( x-dx) * ech_user * ech_geo;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int PanelCapture::tex2screenY( int y )
{
    return (double)( y-dy) * ech_user * ech_geo;
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
void PanelCapture::printObjet()
{
    logf( (char*)"PanelCapture::printObjet()" );
    log_tab(true);
    logf( (char*)"this->pos (%d, %d)", getPosX(), getPosY() );
    logf( (char*)"this->dim %d x %d", getDX(), getDY() );
    logf( (char*)"this->ech_geo=%0.2f",  ech_geo );
    logf( (char*)"this->ech_user=%0.2f", ech_user );
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
		Serveur_mgr::getInstance().write_stellarium( (double)vJ2000.x, (double)vJ2000.y );
		
		
		change_ad_status( vJ2000.x );
		change_dc_status( vJ2000.y );
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
		pCoord->setVisible(true);
		pJ2000_1->setVisible(true);
		pJ2000_2->setVisible(true);

		int xx = WindowsManager::getInstance().getMouseX();
		int yy = WindowsManager::getInstance().getMouseY();
		
		int x = Screen2x(xx);
		int y = Screen2y(yy);
		
		pCoord->setPos( x+15, y+15);
		pCoord->updatePos();
	}
	else
	{	
		pCoord->setVisible(false);
		pJ2000_1->setVisible(false);
		pJ2000_2->setVisible(false);
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::iconize()
{
	bAffGrille = false;
	bInfoSouris = false;
	setInfoSouris( false );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::restaure(bool bGril, bool bbSour )
{
	bAffGrille = bGril;
	bInfoSouris = bbSour;
	setInfoSouris( bbSour );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

