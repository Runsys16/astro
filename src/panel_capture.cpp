#include "panel_capture.h"
#include "captures.h"

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
    
    pCoord = new PanelText( (char*)"(0,0)",		PanelText::LARGE_FONT, 20, 10 );
    this->add( pCoord );
    pJ2000_1 = new PanelText( (char*)"(0,0)",		PanelText::LARGE_FONT, 20, 10 );
    this->add( pJ2000_1 );
    pJ2000_2 = new PanelText( (char*)"(0,0)",		PanelText::LARGE_FONT, 20, 10 );
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
void PanelCapture::passiveMotionFunc(int xx, int yy)
{
	if ( !bInfoSouris || bIcone ) 			{ return; }
	
	static char coord[80];
	//logf( (char*)"PanelCapture::passiveMotionFunc( %d, %d )", xx, yy );
	log_tab(true);

	// Coordonne fenetre 
	long x = Screen2x(xx);
	long y = Screen2y(yy);
	
	// coordonnées image	
	float X = (float)x/(ech_user*ech_geo);
	float Y = (float)y/(ech_user*ech_geo);
	
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
		pCapture->getFits()->screen2J2000( vJ2000, XX, YY );
		
		struct dms DMS;
		struct hms HMS;
		rad2hms( HMS, vJ2000.x / 180.0 * M_PI );
		rad2dms( DMS, vJ2000.y / 180.0 * M_PI );
		
		
		snprintf( (char*)coord, sizeof(coord), "%ld %ld \tAD:%02.0fh %02.0f' %02.2f\"", XX, YY, HMS.h, HMS.m, HMS.s );
		pJ2000_1->changeText( (char*)coord );
		pJ2000_1->setChangeText(true);
		pJ2000_1->setPos( x+15, y+30);

		snprintf( (char*)coord, sizeof(coord), "\t\tDE:%2.0fd %02.0f' %02.2f\"", DMS.d, DMS.m, DMS.s );
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
    float fDX = (float)pReadBgr->w * ech_geo;
    float fDY = (float)pReadBgr->h * ech_geo;
    
    float fw = (float)pReadBgr->w;
    float fh = (float)pReadBgr->h;

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
void PanelCapture::setCentX(float f)
{
    dx = f;
    setCent();
    return;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::setCentY(float f)
{
    dy = f;
    setCent();
    return;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::setEchelle(float f)
{
    if ( f == 0.0 )     
    {
        logf( (char*)"PanelCapture::setEchelle(%0.2f)", f );
        return;
    }
    if ( pReadBgr == NULL )         return;
    
    float fDX = (float)pReadBgr->w * ech_geo;
    float fDY = (float)pReadBgr->h * ech_geo;
    
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

    float coef0 = (float)pReadBgr->w / getParent()->getDX();// * ech_user;
    float coef1 = (float)pReadBgr->h / getParent()->getDY();// * ech_user;
    float coef;
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
    float fDX = (float)pReadBgr->w * ech_geo;
    float fDY = (float)pReadBgr->h * ech_geo;

    setSize( fDX*ech_user, fDY*ech_user );
    setCent();


    if ( pVizier != NULL )
    {
	    int n = pVizier->size();
		Fits*	pFits	= pCapture->getFits();
		vec2 	d 		= vec2( getX(), getY() );
		float 	ech 	= ech_user * ech_geo;
		vec2	v, w;

        for ( int i=0; i<n; i++ )        {
            w.x = pVizier->get(i)->getRA();
            w.y = pVizier->get(i)->getDE();
			pFits->J2000_2_screen( v, w );
			v = ech * v;
            //v -= d;
            PanelText * p = pVizier->get(i)->getInfo();
            p->setVisible(true);
            p->setPos( v.x+10, v.y+10 );
            p->updatePos();
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
	//vec2 	crval	= vec2( pFits->getCRVAL1(), pFits->getCRVAL2() );
	//vec2 	crpix	= vec2( pFits->getCRPIX1(), pFits->getCRPIX2() );
	//mat2 	m		= pFits->getMatrix();
	float 	ech 	= ech_user * ech_geo;
	
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
			pFits->J2000_2_screen( v, w );
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
	float ech = ech_user * ech_geo;

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
	/*
	//-------------------------------------------------
	// Affichage d'un carre jaune
	glColor4f( COLOR_A(c), COLOR_R(c), COLOR_G(c), COLOR_B(c) );
    glVertex2i(500, 500);
    glVertex2i(550, 500);

    glVertex2i(550, 500);
    glVertex2i(550, 550);

    glVertex2i(550, 550);
    glVertex2i(500, 550);

    glVertex2i(500, 550);
    glVertex2i(500, 500);
	//-------------------------------------------------
	*/
    glEnd();
	// retour a l'ancienne couleur
	glColor4f( COLOR_A(color_old), COLOR_R(color_old), COLOR_G(color_old), COLOR_B(color_old) );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::displayGL()
{
    float gris = 0.3;
    VarManager& var = VarManager::getInstance();

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

    /*
    float x = getX();
    float y = getY();

    float X = -dx;
    float Y = -dy;

    float dx = getDX()/2;
    float dy = getDY()/2;
    */

    if ( bNuit )        glColor4f( gris,  0.0,  0.0, 1.0 );
    else                glColor4f( gris, gris, gris, 0.2 );    
    
    PanelSimple::displayGL();

    stars.setView( this );
    //stars.setRB( pReadBgr );
    stars.displayGL();

	if ( pCapture->isFits() )	{
		if ( bAffGrille )												displayEchelle();
		if ( Captures::getInstance().isCurrent(pCapture)	)
		{
			if ( var.existe("bAffCatalog") && var.getb( "bAffCatalog"))		if (pVizier) displayCatalog();
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

    float ech = ech_user * ech_geo;

    float max_x = ech*pReadBgr->w - p->getDX();
    float max_y = ech*pReadBgr->h - p->getDY();
    
    float X1 = xm;
    float Y1 = ym;
    
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
    
    float k = 1.1;
    ech_user *= k;
    
    Panel* p = getParent();

    float XM =  ( xm- p->getX() );
    float X0 = -( getPosX() ); 
    float X1 = k*X0 + (k-1)*XM;
    
    float YM =  ( ym-p->getY() );
    float Y0 = -( getPosY() ); 
    float Y1 = k*Y0 + (k-1)*YM;
    
    float ech = ech_user * ech_geo;
    float max_x = (ech*(float)pReadBgr->w) - p->getDX();
    float max_y = (ech*(float)pReadBgr->h) - p->getDY();

    setPos( -X1, -Y1 );
    
    stars.update_stars( getX(), getY(), this, pReadBgr, ech_geo*ech_user );
    
    logf( (char*)"XY    (%0.2f, %0.2f)  (%0.2f, %0.2f)", X0, Y0, X1, Y1 );
    logf( (char*)"Mouse (%0.2f, %0.2f)  (%0.2f, %0.2f)", (float)xm, (float)ym, XM, YM );
    logf( (char*)"MAX   (%0.2f, %0.2f)", max_x, max_y );
    logf( (char*)"ech   (%0.2f, %0.2f) bgr(%d, %d)", ech_geo, ech_user, (int)pReadBgr->w, (int)pReadBgr->h );
    logf( (char*)"win   (%0.2f, %0.2f) (%0.2f, %0.2f)", (float)getX(), (float)getY(), (float)p->getDX(), (float)p->getDY() );
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
    float k = 0.9;
    ech_user *= k;
    if (ech_user<=1.0)           ech_user = 1.0;
    
    Panel* p = getParent();

    float XM =  ( xm- p->getX() );
    float X0 = -( getPosX() ); 
    float X1 = k*X0 + (k-1)*XM;
    
    float YM =  ( ym-p->getY() );
    float Y0 = -( getPosY() ); 
    float Y1 = k*Y0 + (k-1)*YM;

    float ech = ech_user * ech_geo;
    float max_x = ech*pReadBgr->w - p->getDX();
    float max_y = ech*pReadBgr->h - p->getDY();
    
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
    logf( (char*)"Mouse (%0.2f, %0.2f)  (%0.2f, %0.2f)", (float)xm, (float)ym, XM, YM );
    logf( (char*)"MAX   (%0.2f, %0.2f)", max_x, max_y );
    logf( (char*)"ech   (%0.2f, %0.2f) bgr(%d, %d)", ech_geo, ech_user, (int)pReadBgr->w, (int)pReadBgr->h );
    logf( (char*)"win   (%0.2f, %0.2f) (%0.2f, %0.2f)", (float)getX(), (float)getY(), (float)p->getDX(), (float)p->getDY() );
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
    
    Captures::getInstance().setCurrent( pCapture );
    Panel::clickLeft(xm,ym);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::releaseLeft(int xm, int ym)
{
    logf( (char*)"PanelCapture::releaseLeft(%d,%d) ...", xm, ym );

    float e = (float)getDX() / (float)pReadBgr->w; 
    
    int xx = ((float)xm-(float)getX()) / e;
    int yy = ((float)ym-(float)getY()) / e;


    // Si en plein ecran on ajoute une etoile
        
    if ( !bIcone     )
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
void PanelCapture::releaseRight(int xm, int ym)
{
    logf( (char*)"PanelCapture::releaseRight(%d,%d) ...", xm, ym );
    printObjet();
    if ( pReadBgr == NULL )     { logf( (char*)"Pointeur NULL" ); return; }
    
    float e = (float)getDX() / (float)pReadBgr->w; 
    //float e = (float)getDX() / (float)1920.0; 
    
    int xx = ((float)xm-(float)getX()) / e;
    int yy = ((float)ym-(float)getY()) / e;
    
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
    float deltaX, deltaY;
    float ech = ech_geo * ech_user;
    Panel* p = getParent();
    
    deltaX = (float) (xm-xm_old) / ech;
    deltaY = (float) (ym-ym_old) / ech;

    float X0 = ( getPosX() ); 
    float X1 = X0 + (xm-xm_old);
    
    float Y0 = ( getPosY() ); 
    float Y1 = Y0 + (ym-ym_old);
    
    float max_x = ech*pReadBgr->w - p->getDX();
    float max_y = ech*pReadBgr->h - p->getDY();

    int x1 = -X1;
    int y1 = -Y1;

    clip(x1, y1);
    setPos( -x1, -y1 );
    float e = (float)getDX() / (float)pReadBgr->w; 
    
    //logf( (char*)"XY    (%0.2f, %0.2f)  (%0.2f, %0.2f)", X0, Y0, X1, Y1 );
    //logf( (char*)"Mouse (%0.2f, %0.2f) ", (float)xm, (float)ym );
    //logf( (char*)"MAX   (%0.2f, %0.2f)", max_x, max_y );
    //logf( (char*)"ech   (%0.2f, %0.2f, %0.2f) bgr(%d, %d)", ech_geo, ech_user, e, (int)pReadBgr->w, (int)pReadBgr->h );
    //logf( (char*)"win   (%0.2f, %0.2f)", (float)p->getDX(), (float)p->getDY() );
    //logf( (char*)"Pos   (%0.2f, %0.2f)", (float)getPosX(), (float)getPosY() );
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
    
    
    //float e = (float)getDX() / (float)pReadBgr->w; 
    float e = (float)getDX() / (float)pReadBgr->w; 
    int xx = ((float)xm-(float)getX()) / e;
    int yy = ((float)ym-(float)getY()) / e;
    
    logf( (char*)"echelle%0.2f", e );
    logf( (char*)"Pos (%0.2f, %0.2f)", (float)getX(), (float)getY()  );
    
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
    
    if ( pCapture->isFits() )
    {
		long x = Screen2x(xm);
		long y = Screen2y(ym);
		
		// coordonnées image	
		float X = (float)x/(ech_user*ech_geo);
		float Y = (float)y/(ech_user*ech_geo);
		
		long XX = X;
		long YY = Y;

		
		// Recuperation des infos image largueur hauteur et pointeur sur RGBRGBRGBRGBRGBRGB
		vec2 vJ2000;
		pCapture->getFits()->screen2J2000( vJ2000, XX, YY );
		
		struct dms DMS;
		struct hms HMS;
		rad2hms( HMS, vJ2000.x / 180.0 * M_PI );
		rad2dms( DMS, vJ2000.y / 180.0 * M_PI );
		
		/*		
		static char coord[80];
		snprintf( (char*)coord, sizeof(coord), "(%0.6f, %0.6f)", vJ2000.x, vJ2000.y );
		log( (char*)coord );	
		Serveur_mgr::getInstance().write_stellarium( (double)vJ2000.x, (double)vJ2000.y );
		*/
		
		double resH = pCapture->getFits()->getNAXIS1();
		double resV = pCapture->getFits()->getNAXIS2();
		
		resH *= pCapture->getFits()->getCDELT1() * 3600.0;
		resV *= pCapture->getFits()->getCDELT2() * 3600.0;
		
		float res;
		if ( resH > resV )			res = resH;
		else						res = resV;
		
		static char coord[80];
		double xCentre = pCapture->getFits()->getCRVAL1();
		double yCentre = pCapture->getFits()->getCRVAL2();

		//snprintf( (char*)coord, sizeof(coord), "-r %d% 0.8f %0.8f", (int)res, vJ2000.x, vJ2000.y );
		snprintf( (char*)coord, sizeof(coord), "-r %d% 0.8f %0.8f", (int)res, xCentre, yCentre );
		logf( (char*)"Requete Gaia DR3 : %s", coord );	

		if ( pCapture->getFits()->getPanelFits()->isVisible() )
		{
			if ( pVizier )	{
				pVizier->efface();
				delete pVizier;
				pVizier = NULL;
			}
			pVizier = new Catalog(false);
		
			// lance la requete
			if ( bAffGrille )
				vizier_load_stars( pVizier, string(coord) );

			for( int i=0; i<pVizier->size(); i++ )
			{
				PanelText* p = pVizier->get(i)->getInfo();
				p->setPos( 100, 100 );
				this->add( p );
				p->setChangeText(true);
				p->setVisible(true);
			}
		}		
		
		change_ad( vJ2000.x );
		change_dc( vJ2000.y );
		
    	
    }
    log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int PanelCapture::screen2texX( int x )
{
    return (float)x / ( ech_geo*ech_user) + dx;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int PanelCapture::screen2texY( int y )
{
    return (float)y / (ech_geo*ech_user) + dy;
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
    return (float)( x-dx) * ech_user * ech_geo;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int PanelCapture::tex2screenY( int y )
{
    return (float)( y-dy) * ech_user * ech_geo;
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
void PanelCapture::setInfoSouris(bool b)  
{
	logf( (char*)"void PanelCapture::setInfoSouris(%s)", BOOL2STR(b) );
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
		//logf( (char*)"Mouse '%d, %d)", x, y );
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

