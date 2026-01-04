#include "panel_capture.h"
#include "panel_stdout.h"
#include "captures.h"
//--------------------------------------------------------------------------------------------------------------------
//#define DEBUG_WHEEL
//--------------------------------------------------------------------------------------------------------------------
#define DEG_SEXAGESIMAL
//----------------------------------------------
#define TIMEANIM 1.5
#define SIZE_ECH 200.0
//----------------------------------------------
#define SIZEY 14
#define POLICE "fonts/greek-wsi-regular.ttf"   	// 218 glyphe voir alpha ...
//----------------------------------------------
#define CLIP_VIZIER		// Ne traite que  les etoiles visibles
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelCapture::PanelCapture( struct readBackground*  pReadBgr, Capture* pc )
{
	init();
	
    pReadBgr    = pReadBgr;
    pCapture    = pc;

    stars.setView( this );
    stars.setModeMag( 1 );
    
    pFondCoord = new PanelWindow();
    pFondCoord->setBackground( (char*)"images/background.tga" );
    pFondCoord->setSize( 210, 55 );
    pFondCoord->loadSkin( PanelWindow::BLACK );
	pFondCoord->setBorderSize(2);

    this->add( pFondCoord );
    
    pTelescope = new PanelSimple();
    pTelescope->setBackground( (char*)"images/telescope.png" );
    pTelescope->setSize( 45,64 );
    pTelescope->setFantome( true );
    this->add( pTelescope );
    
    int x		= 90;
    int y		= 4;
    int deltax	= 16;

    pColor = new PanelText( (char*)"(0,0)",			PanelText::NORMAL_FONT, 5, y );
    y += SIZEY;
    pCoord = new PanelText( (char*)"(0,0)",			PanelText::NORMAL_FONT, 5, y );


    pJ2000_alpha	= new PanelText( (char*)"a", 		(char*)POLICE , x, y, SIZEY, 0xFFFFFFFF );
    pJ2000_1		= new PanelText( (char*)"(0,0)",	PanelText::NORMAL_FONT, x+deltax, y );

    y += SIZEY+4;
    pJ2000_delta	= new PanelText( (char*)"d", 		(char*)POLICE , x, y, SIZEY, 0xFFFFFFFF );
    pJ2000_2		= new PanelText( (char*)"(0,0)",	PanelText::NORMAL_FONT, x+deltax, y );

    pFondCoord->add( pColor );
    pFondCoord->add( pCoord );
    pFondCoord->add( pJ2000_alpha );
    pFondCoord->add( pJ2000_delta );
    pFondCoord->add( pJ2000_1 );
    pFondCoord->add( pJ2000_2 );

	pInfoVizier = new PanelDebug();
	pInfoVizier->setExtraString("Info Vizier");
	pInfoVizier->setVisible(false);
	pInfoVizier->setSize( 180, 1 );
	pInfoVizier->setTabSize( 60 );
	pInfoVizier->loadSkin( PanelWindow::BLACK );
	pInfoVizier->setBorderSize(2);

	add( pInfoVizier );
	
    
    setExtraString( "PanelCapture" );
    setParentCliping( true );
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelCapture::~PanelCapture()
{
	//---------------------------------------------------------------
	for( int i=0; i<tAD.size(); i++ )
	{
		PanelText* p = tAD[i].pTextCoord;
		if (p->getParent())		p->getParent()->sup(p);
		delete p;
	}
	tAD.clear();
	//---------------------------------------------------------------
	for( int i=0; i<tDE.size(); i++ )
	{
		PanelText* p = tDE[i].pTextCoord;
		if (p->getParent())		p->getParent()->sup(p);
		delete p;
	}
	tDE.clear();
	//---------------------------------------------------------------
    if ( pTelescope != NULL )		delete pTelescope;
    if ( pFondCoord != NULL )		delete pFondCoord;
    if ( pColor != NULL )			delete pColor;
    if ( pCoord != NULL )			delete pCoord;

    if ( pJ2000_alpha != NULL )		delete pJ2000_alpha;
    if ( pJ2000_1 != NULL )			delete pJ2000_1;

    if ( pJ2000_delta != NULL )		delete pJ2000_delta;
    if ( pJ2000_2 != NULL )			delete pJ2000_2;

    stars.setRB( NULL );
    stars.deleteAllStars();

    if ( pVizier != NULL )			delete pVizier;
    if ( pInfoVizier != NULL )		delete pInfoVizier;

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::init()
{
	vTelescopeJ2000 	= vec2(0.0, 0.0);
    ech_geo				= 1.0;
    ech_user			= 1.0;
    dx					= 0.0;
    dy					= 0.0;
    xm_old				= -1;
    ym_old				= -1;
    dTimeAnim			= 0.0;
	bFits				= false;
	    
    pVizier				= NULL;

	bAffCatalogPosition = false;
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
// Positionn ela fenetre info Vizier
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::updatePosInfoVizier()
{
	//------------------------------------
	int dx, dy;
	int ddyy = pCapture->getAfficheInfoSouris() ? 60 : 0;
	
	vec2 vDelta		= vec2(15, 20+ddyy);
	vec2 vPosInfo 	= vec2(vMouse.x, vMouse.y) + vDelta - vec2(getX(), getY());
	vec2 vCmp		= vec2(vMouse.x, vMouse.y) + vDelta + vec2(pInfoVizier->getPosDX(), pInfoVizier->getPosDY());
	
	dx = pCapture->getX() + pCapture->getPosDX();
	dy = pCapture->getY() + pCapture->getPosDY();

	if ( (vCmp.x)>(dx) )				vPosInfo.x -= pInfoVizier->getDX() + 2*vDelta.x;
	if ( (vCmp.y)>(dy) )
	{
		vPosInfo.y -= pInfoVizier->getDY() + 2*vDelta.y;
		
		vCmp		= vec2(vMouse.x, vMouse.y) + vDelta + vec2(pFondCoord->getPosDX(), pFondCoord->getPosDY());
		if ( (vCmp.y-ddyy)<=(dy) )				vPosInfo.y += ddyy;
	}
	
	pInfoVizier->setPos( vPosInfo.x, vPosInfo.y );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::updateInfoVizier()
{
	if ( pVizier == NULL )			return;
	
	double	ad_vizi, de_vizi, ad_star, de_star;
    int 	n		= pVizier->size();
	Fits*	pFits	= pCapture->getFits();
	vec2 	d 		= vec2( getX(), getY() );
	double 	ech 	= ech_user * ech_geo;
	char	str[255];
	vec2	v;

	if ( n==0 )			return;

	//-----------------------------------------------
	// Recherche  de l'etoile sous la souris
	//   idxVizierMouseOver
    idxVizierMouseOver = -1;

    for ( int i=0; i<n; i++ )        {       
		
		PanelText* p = pVizier->get(i)->getInfo();

		p->setVisible(true);

        v.x = pVizier->get(i)->getRA();
        v.y = pVizier->get(i)->getDE();

		pFits->J2000_2_tex( v );
		tex_2_screen(v);
		
		vec2 w = vec2(vMouse.x, vMouse.y);
		w -= v;
		if ( w.length() < (ech*10.0) )		{ idxVizierMouseOver = i; break; }
			
	}
	
	//-----------------------------------------------
	// Etoile trouvéz
	//  idxVizierMouseOver != -1
	if ( idxVizierMouseOver != -1 )
	{
		pInfoVizier->reset_list();
		pInfoVizier->setVisible(true);

		vec2 V, W;
		StarCatalog* pStarViz = pVizier->get(idxVizierMouseOver);
        ad_vizi = v.x = pStarViz->getRA();
        de_vizi = v.y = pStarViz->getDE();

		pFits->J2000_2_tex( v );
		//tex_2_screen(v);
        V = v;
		
		pInfoVizier->add_textf( (char*)"-GAIAdr3\t(%.2lf, %0.2lf)", v.x, v.y );
		pInfoVizier->add_textf( (char*)"    mag\t= %0.2f", (float)pStarViz->getMag() );
		
		//------------------------------------
		
		int iStar = starCompare.getTwinStar( idxVizierMouseOver );
		if ( iStar != -1 )
		{
			Star* pStar = stars.get(iStar);
			ad_star = v.x = pStar->getAD();
			de_star = v.y = pStar->getDE();

			pFits->J2000_2_tex( v );
			W = v;
			pInfoVizier->add_textf( (char*)"-Calc \t(%.2lf, %0.2lf)", v.x, v.y );
			pInfoVizier->add_textf( (char*)"    mag\t= %0.2f", (float)pStar->getMagnitude() );

			W -= V;
			pInfoVizier->add_text( (char*)"-----------------------------------" );

			pInfoVizier->add_textf( (char*)"delta pix \t= %0.2f", (float)W.length() );
			pInfoVizier->add_textf( (char*)"delta mag\t= %0.2f", (float)abs( pStar->getMagnitude()- pStarViz->getMag()) );

			pInfoVizier->add_text( (char*)"-----------------------------------" );
			deg2str_hms( ad_vizi, str, sizeof(str) );		pInfoVizier->add_textf( (char*)"AD : %s", str );
			deg2str_hms( ad_star, str, sizeof(str) );		pInfoVizier->add_textf( (char*)"AD : %s", str );

			deg2str_dms( de_vizi, str, sizeof(str) );		pInfoVizier->add_textf( (char*)"DE : %s", str );
			deg2str_dms( de_star, str, sizeof(str) );		pInfoVizier->add_textf( (char*)"DE : %s", str );

			updatePosInfoVizier();
		}
		else
			pInfoVizier->setVisible(false);
	}
	else
		pInfoVizier->setVisible(false);

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::updateVizier()
{
    int 	n		= pVizier->size();
	Fits*	pFits	= pCapture->getFits();
	vec2 	d 		= vec2( getX(), getY() );
	double 	ech 	= ech_user * ech_geo;
	vec2	v;

    vec2 	vMin	= vec2( pCapture->getX(), pCapture->getY() );
    vec2 	vMax	= vec2( pCapture->getX()+pCapture->getDX()  ,  pCapture->getY()+pCapture->getDY()  );

    idxVizierMouseOver = -1;

    for ( int i=0; i<n; i++ )        {       
		
		PanelText* p = pVizier->get(i)->getInfo();

		p->setVisible(true);

        v.x = pVizier->get(i)->getRA();
        v.y = pVizier->get(i)->getDE();

		pFits->J2000_2_tex( v );
		tex_2_screen(v);
		
		vec2 w = vec2(vMouse.x, vMouse.y);
		w -= v;
		if ( w.length() < (ech*10.0) )		idxVizierMouseOver = i;
			
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
			
			screen_2_tex(v);
	        pVizier->get(i)->setTex( v );
			
	        if ( bNuit )        p->setColor( COLOR32(255,   0, 0, 255) );
	        else                p->setColor( COLOR32(255, 178, 0, 255) );

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
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
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
    setCentre();

    PanelSimple::updatePos();

    
    idxVizierMouseOver = -1;

    if ( pVizier != NULL && bAffCatalog && !pCapture->isIconized() )
    {
       	updateVizier();
	}
   	updateInfoVizier();
    PanelSimple::updatePos();


	if ( pCapture->getAfficheGrille() && !pCapture->isIconized() )	{
		setEchelleVisible(true);
	    updateEchelle();
	}
	else	{
		setEchelleVisible(false);
	}

	if ( bFits )	{
		pCapture->getFits()->J2000_2_tex( vTelescopeJ2000, vTelescopeTex );

		vTelescopePanel = vTelescopeScreen	= vTelescopeTex;

		tex_2_screen( vTelescopeScreen );
		tex_2_panel( vTelescopePanel );
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

	Fits*	pFits	= pCapture->getFits();
	vec2 	d 		= vec2( getX(), getY() );
	double 	ech 	= ech_user * ech_geo;
	vec2 v, w;
	
	if ( pFits == NULL ) 					return;
	//-------------------------------------------------------------------
	glBegin(GL_LINES);

    int n = pVizier->size();
    int nbAff = 0;

    if ( n!= 0)
    {
		vcf4 vNuit    = vcf4( 1.0,  0.0,  0.0, 1.0 );
		vcf4 vOrange  = vcf4( 1.0,  0.7,  0.0, 0.8 );
		vcf4 vSelect  = vcf4( 0.0,  0.8,  1.0, 1.0 );

		if( bNuit )						glColor4fv( (GLfloat*)&vNuit );
		else							glColor4fv( (GLfloat*)&vOrange );

        for ( int i=0; i<n; i++ )
        {
        	if ( starCompare.getMagMax() < pVizier->get(i)->getMag() )	continue;
        	
        	if ( i == idxVizierMouseOver && !bNuit )			glColor4fv( (GLfloat*)&vSelect );
        	
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
        	if ( i == idxVizierMouseOver && !bNuit )			glColor4fv( (GLfloat*)&vOrange );
        }
		
		dDebug5s += Timer::getInstance().getElapsedTime();

		if ( nbNonAff != (n-nbAff) )
		{
			nbNonAff = n-nbAff;
			dDebug5s = 0.0;
			char pStrNbEtoiles[128];
			snprintf( pStrNbEtoiles, sizeof(pStrNbEtoiles), "%d/%d", nbAff, n );
			pCapture->getNbVizier()->changeText( (char*)pStrNbEtoiles );
		}
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


	vec2 d;
	d = vec2( parent->getX(), parent->getY() );

	double ech = ech_user * ech_geo;

	glBegin(GL_LINES);

	vec2 v, w;

	if (bNuit)		glColor4ub( COLOR_R(R), COLOR_G(R), COLOR_B(R), COLOR_A(R) );

	for( int i=0; i<tAD.size(); i++ )
	{
		
		if (!bNuit )			glColor4ub( COLOR_R(b), COLOR_G(b), COLOR_B(b), COLOR_A(b) );

		v = tAD[i].p1 + d;
		w = tAD[i].p2 + d;
		
	    glVertex2i(v.x, v.y);
	    glVertex2i(w.x, w.y);
	}
	
	for( int i=0; i<tDE.size(); i++ )
	{
		if (!bNuit )			glColor4ub( COLOR_R(r), COLOR_G(r), COLOR_B(r), COLOR_A(r) );

		v = tDE[i].p1 + d;
		w = tDE[i].p2 + d;


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

	//-------------------------------------------------	
    
    if ( bNuit )		{
    	setColorBgr( COLOR32(255, 0, 0, 255) );
    	//setColor( COLOR32(255, 0, 0, 255) );
		stars.getPanelNbStars()->setColor( COLOR32(255,   0, 0, 255) );
        pCapture->getNbVizier()->setColor( COLOR32(255,   0, 0, 255) );
        if ( pInfoVizier )		pInfoVizier->setColor( COLOR32(255,   0, 0, 255) );
        if ( pFondCoord )		pFondCoord->setColor( COLOR32(255,   0, 0, 255) );
    }
    else	{
    	setColorBgr( COLOR32(255, 255, 255, 255) );    
    	//setColor( COLOR32(255, 255, 255, 255) );    
        stars.getPanelNbStars()->setColor( COLOR32(  0, 255, 0, 255) );//setColor( 0x00ff00ff );//glColor4f( 0.0,   1.0,  0.0, 0.4 );
        pCapture->getNbVizier()->setColor( COLOR32(255, 178, 0, 255) );
        if ( pInfoVizier )		pInfoVizier->setColor( COLOR32(255, 255, 255, 255) );
        if ( pFondCoord )		pFondCoord->setColor( COLOR32(255, 255, 255, 255) );
    }
    
	//-------------------------------------------------	
    
    PanelSimple::displayGL();
    
    stars.setView( this );

    if ( !pCapture->isIconized() )
    {
    	stars.displayGL();

		if ( bFits )	{
			if ( !pCapture->isIconized() && pCapture->getAfficheGrille() )		displayAxe();
			if ( var.getb( "bAffCatalog") && (pVizier) )						displayCatalog();
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

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::idle(float f)
{
	if ( pCapture->isFits() )			bFits = true;
	else								bFits = false;


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
			passiveMotionFunc( (int)vMouse.x, (int)vMouse.y );
		}
		else
			pFondCoord->setVisible(false);
	
	}
	
	if ( pCapture->isIconized() )		
		pFondCoord->setVisible(false);

	pCapture->afficheFits();

    PanelSimple::idle(f);

	if ( bFits )
	{
		//-------------------------------------------------------------------
		Fits*	pFits	= pCapture->getFits();
		
		pFits->J2000_2_tex( vTelescopeJ2000, vTelescopeTex );
		vTelescopePanel =vTelescopeScreen = vTelescopeTex;
		tex_2_screen( vTelescopeScreen );
		tex_2_panel( vTelescopePanel );
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
				screen_2_tex( vTex );

				vec2 vJ2000;
				pFits->tex_2_J2000( vTex, vJ2000 );

				pStar->setAD( vJ2000.x );
				pStar->setDE( vJ2000.y );
			}
		}
		//-------------------------------------------------------------------
		/*
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
		*/
		//-------------------------------------------------------------------
	}
	else
		pFondCoord->setVisible(false);
	
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
    
    
    updatePos();    
    passiveMotionFunc( xm, ym );
    pCapture->updatePosIcones();
    
#ifdef DEBUG_WHEEL
    logf( (char*)"+Nouvelle echelle %.2f", (float)ech );
#endif

    stars.updateScreenPos( dx+pCapture->getX(), dy+pCapture->getY(), ech);
    
    updatePos();

#ifdef DEBUG_WHEEL
    log_tab(false);
#endif
}
//--------------------------------------------------------------------------------------------------------------------
//
// xx, yy coordonnée écran du pointeur de souris
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::updatePosFondCoord()
{
	// Mise a jour du panelText
	// dans la position de la souris
	// Force la fabrication de la chaine de caractere
	int dx, dy;
	vec2 vDelta = vec2(15, 20);
	vec2 vPosInfo 	= vec2(vMouse.x, vMouse.y) + vDelta - vec2(getX(), getY());
	vec2 vCmp		= vec2(vMouse.x, vMouse.y) + vDelta + vec2(pFondCoord->getPosDX(), pFondCoord->getPosDY());
	
	dx = pCapture->getX() + pCapture->getPosDX();
	dy = pCapture->getY() + pCapture->getPosDY();
	

	if ( (vCmp.x)>(dx) )		vPosInfo.x -= pFondCoord->getDX() + 2*vDelta.x;
	if ( (vCmp.y)>(dy) )		vPosInfo.y -= pFondCoord->getDY() + 2*vDelta.y;
	
	pFondCoord->setPos( vPosInfo.x, vPosInfo.y );
}
//--------------------------------------------------------------------------------------------------------------------
//
// Positionn ela fenetre info Vizier
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::find_star_mouse_over()
{
    int 	n					= stars.size();
	Fits*	pFits				= pCapture->getFits();
    int		idxStarMouseOver	= -1;
    vec2	v, w;

	//-----------------------------------------------
	// Recherche  de l'etoile sous la souris
	//   idxVizierMouseOver

    for ( int i=0; i<n; i++ )        {       
		
		Star* pStar = stars.get(i);
		pStar->setGraph(false);
		
        v.x = pStar->getAD();
        v.y = pStar->getDE();

		pFits->J2000_2_tex( v );
		tex_2_screen(v);
		
		w = vec2(vMouse.x, vMouse.y);
		w -= v;
		if ( w.length() < (ech*10.0) )		{ idxStarMouseOver = i; }
			
	}
	
	if ( idxStarMouseOver != -1 )
	{
		stars.get(idxStarMouseOver)->setGraph(true);
		vector<ivec2>& cmp = starCompare.getCmpViziStar();
		
		for( int i=0; i<cmp.size(); i++ )
		{
			if ( cmp[i].y == idxStarMouseOver )
			{
				//logf( (char*)"PanelCapture::find_star_mouse_over() = %d->%d", idxStarMouseOver, i );
				pCapture->getGraph()->setStarOverMouse(i);
			}
		}
	}

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
	if ( pCapture == NULL  ) 			{ return; }
	
	if ( pCapture->getGraph() != NULL && stars.size() != 0 )		find_star_mouse_over();
	
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
	
	if ( x<0 )	
	{
		//logf( (char*)"[WARNING] x souris dans la texture negatif !!! %s ", pCapture->getBasename().c_str() );
		return;
	}
	
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
	
	if ( ptr == NULL )		
	{
		logf( (char*)"[Erreur] ptr == NULL %s", __LINE__ );
		return;
	}
	// Calcul de l'offset dans le buffer
	long idx = 3 * (w * YY + XX);

	//logf( (char*)"x=%ld y=%ld X=%lf Y=%lf)", x, y, X, Y );
	//logf( (char*)"idx=%ld w=%d XX=%ld YY=%ld)", idx, w, XX, YY );
		
	// Affichage du resultat
	// float l = 0.33 * (float)r + 0.5 * (float)g  + 0.16 * (float)b;
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
	
	if ( bFits )	{
		//---------------------------------------
		// calcul la position J2000 de la souris
    	vec2 vJ2000;
		vec2 vTex = vec2( xm, ym ); 
		screen_2_tex( vTex );
		pCapture->getFits()->tex_2_J2000( vTex, vJ2000 );
		
		#ifdef DEG_SEXAGESIMAL
			struct dms DMS;
			struct hms HMS;
			deg2hms( vJ2000.x, HMS );
			deg2dms( vJ2000.y, DMS );

			snprintf( (char*)coord_ad, sizeof(coord_ad), "=%02.0fh %02.0f' %02.2f\"", HMS.h, HMS.m, HMS.s );
			snprintf( (char*)coord_dc, sizeof(coord_dc), "=%2.0f° %02.0f' %02.2f\"", DMS.d, DMS.m, DMS.s );
		#else
			snprintf( (char*)coord_ad, sizeof(coord_ad), "=%f", vJ2000.x );
			snprintf( (char*)coord_dc, sizeof(coord_dc), "=%f", vJ2000.y );
		#endif

		pJ2000_1->changeText( (char*)coord_ad );
		pJ2000_1->setChangeText(true);

		pJ2000_2->changeText( (char*)coord_dc );
		pJ2000_2->setChangeText(true);

		//------------------------
		updatePosFondCoord();
	}
	else
	{
	}
	
	
	//------------------------------------------------------------
	// si une etoile est survoler dans le graphique
	if ( pCapture->getGraph() )
	{
		int iMouse = pCapture->getGraph()->getIndexMouse();
		if (iMouse != -1 )
		{
			//logf( (char*)"Find %d", iMouse );
		}
	}		
	//log_tab(false);
	//log( (char*)"---" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::clickLeft(int xm, int ym)
{
    logf( (char*)"PanelCapture::clickLeft(%d,%d) ...", xm, ym );
    //if ( pCapture->isIconized() )
	//    Captures::getInstance().active( pCapture );

    //Panel::clickLeft(xm,ym);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::releaseLeft(int xm, int ym)
{
    logf( (char*)"PanelCapture::releaseLeft(%d,%d) glutModifier=%d...", xm, ym, iGlutModifier );

    if ( bFits && iGlutModifier == GLUT_ACTIVE_CTRL)    {
		vec2 vTex = vec2( xm, ym );
		vec2 vJ2000;

		screen_2_tex( vTex );
		pCapture->getFits()->tex_2_J2000( vTex, vJ2000 );
        vJ2000.x = vJ2000.x >180.0 ? -360.0 + vJ2000.x : vJ2000.x;
		
		logf( (char*)"CTRL (%0.6f, %0.6f)", vJ2000.x, vJ2000.y );
		Serveur_mgr::getInstance()._goto( vJ2000.x, vJ2000.y );
    }
    else
    if ( bFits && iGlutModifier == GLUT_ACTIVE_ALT)    {
		vec2 vTex = vec2( xm, ym );
		vec2 vJ2000;

		screen_2_tex( vTex );
		pCapture->getFits()->tex_2_J2000( vTex, vJ2000 );
        vJ2000.x = vJ2000.x >180.0 ? -360.0 + vJ2000.x : vJ2000.x;
		
		logf( (char*)"ALT  (%0.6f, %0.6f)", vJ2000.x, vJ2000.y );
		Serveur_mgr::getInstance()._sync( vJ2000.x, vJ2000.y );
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
			log_tab(true);
		    Captures::getInstance().active( pCapture );
			log_tab(false);
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::clickRight(int xm, int ym)
{
    logf( (char*)"PanelCapture::clickRight(%d,%d) ...", xm, ym );
    //Captures::getInstance().active( pCapture );

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
    
    if ( iGlutModifier == GLUT_ACTIVE_ALT )
    {
    	print_echelle_coordonnees();
    }
    else
    {
    	printObjet();
    }
    
    
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
		    if ( bFits )	       pCapture->afficheFits();
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
    //Captures::getInstance().active( pCapture );

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
	log_tab(true);
	starCompare.setStars(&stars);
	starCompare.setVizier(pVizier);

	if ( stars.size() == 0 )
	{
		logf( (char*)"[ Warning] Reset magnitude 20.0" );
		starCompare.setMagMax(20.0);
		return;
	}
	if ( pVizier == NULL || pVizier->size() == 0 )
	{
		logf( (char*)"[ Warning] Pas de catalogue" );
		log_tab(false);
		return;
	}

	starCompare.setStars(&stars);
	starCompare.setVizier(pVizier);

	starCompare.compareStar();
	
	double d = starCompare.computeDelta();
	stars.setDelta( d );
	log_tab(false);
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
void PanelCapture::setCentre()
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
    setCentre();
    logf( (char*)"PanelCapture::setEchelle() ech_geo=%0.2f ech_user=%0.2f dx=%0.2f dy=%0.2f", ech_geo, ech_user, dx, dy );
    logf( (char*)"  setSize(%0.2lf, %0.2lf)", fDX, fDY );
    
}

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::setCentX(double f)
{
    dx = f;
    setCentre();
    return;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::setCentY(double f)
{
    dy = f;
    setCentre();
    return;
}
//--------------------------------------------------------------------------------------------------------------------
//
// Transformation de coordonnees
//----------------------------------
// (x,y) ecran    =>     (x,y)  dans la texture (image astronomique)
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::screen_2_tex(vec2& v)
{
	vec2 delta   = vec2(getX(), getY());
	v = ( v - delta  ) / ech;
}
//--------------------------------------------------------------------------------------------------------------------
//
// Transformation de coordonnees
//----------------------------------
// (x,y)  dans la texture (image astronomique)   =>    (x,y) ecran
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::tex_2_screen(vec2& v)
{
	vec2 delta = vec2(getX(), getY());
	v = (v * ech) + delta;
}
//--------------------------------------------------------------------------------------------------------------------
//
// Transformation de coordonnees
//----------------------------------
// (x,y)  dans la texture (image astronomique)   =>    (x,y)  dans panelCapture
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::tex_2_panel(vec2& v)
{
	v = (ech * v);
}
//--------------------------------------------------------------------------------------------------------------------
//
// Transformation de coordonnees
//----------------------------------
//  (x,y) dans panelCapture			=>  (x,y)  dans la texture (image astronomique) 
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::panel_2_tex(vec2& v)
{
	v = v / ech;
}
//--------------------------------------------------------------------------------------------------------------------
//
// Transformation de coordonnees
//----------------------------------
//  (x,y) dans panelCapture			=>    (x,y) ecran
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::panel_2_screen(vec2& v)
{
	vec2 delta = vec2(getX(), getY());
	v = v + delta;
}
//--------------------------------------------------------------------------------------------------------------------
//
// Transformation de coordonnees
//----------------------------------
//  (x,y) ecran				=> 		(x,y) dans panelCapture
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::screen_2_panel(vec2& v)
{
	vec2 delta = vec2(getX(), getY());
	v = v - delta;
}
//--------------------------------------------------------------------------------------------------------------------
//
// Transformation de coordonnees
//----------------------------------
// (x,y) ds la fenetre Capture(ps de zoom)		 =>    (x,y) Ascension D. et déclinaison
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::parent_2_J2000(vec2& v)
{
	if ( !bFits )		return;

	v += vec2( -getPosX(), -getPosY() );
	panel_2_tex( v );
	pCapture->getFits()->tex_2_J2000( v );
}
//--------------------------------------------------------------------------------------------------------------------
//
// Transformation de coordonnees
//----------------------------------
// (x,y) ds la fenetre Capture(ps de zoom)		 =>    (x,y) Ascension D. et déclinaison
//
// Sous forme de chaine de caractere en sexagesimal
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::parent_2_str_ad_str_dc(vec2& v, char* str_ad, char* str_dc, int sizeof_str)
{
	if ( !bFits )		return;

	struct hms	HMS;
	struct dms	DMS;

	parent_2_J2000( v );

	deg2hms( v.x, HMS );
	deg2dms( v.y, DMS );

	snprintf( (char*)str_ad, sizeof_str, "%02.0lfh %02.0lf' %02.2lf\"", HMS.h, HMS.m, HMS.s );
	snprintf( (char*)str_dc, sizeof_str, "%02.0lf° %02.0lf' %02.2lf\"", DMS.d, DMS.m, DMS.s );

}
//--------------------------------------------------------------------------------------------------------------------
//
// Transformation de coordonnees
//----------------------------------
// (x,y) J2000		 =>    (x,y) Ascension D. et déclinaison
//
// Sous forme de chaine de caractere en sexagesimal
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::J2000_2_str_ad_str_dc(vec2& v, char* str_ad, char* str_dc, int sizeof_str)
{
	if ( !bFits )		return;

	struct hms	HMS;
	struct dms	DMS;

	deg2hms( v.x, HMS );
	deg2dms( v.y, DMS );

	snprintf( (char*)str_ad, sizeof_str, "%02.0lfh %02.0lf' %02.2lf\"", HMS.h, HMS.m, HMS.s );
	snprintf( (char*)str_dc, sizeof_str, "%02.0lf° %02.0lf' %02.2lf\"", DMS.d, DMS.m, DMS.s );

}
//--------------------------------------------------------------------------------------------------------------------
//
// Transformation de coordonnees
//----------------------------------
// x (J2000)		 =>    (x) Ascension Droite
//
// Sous forme de chaine de caractere en sexagesimal
//
//--------------------------------------------------------------------------------------------------------------------
char* PanelCapture::J2000_2_str_ad(vec2& v, char* str_ad, int sizeof_str)
{
	if ( !bFits )		return str_ad;

	struct hms	HMS;

	deg2hms( v.x, HMS );

	snprintf( (char*)str_ad, sizeof_str, "%02.0lfh %02.0lf' %02.2lf\"", HMS.h, HMS.m, HMS.s );

	return str_ad;
}
//--------------------------------------------------------------------------------------------------------------------
//
// Transformation de coordonnees
//----------------------------------
// x (J2000)		 =>    (x) Declinaison
//
// Sous forme de chaine de caractere en sexagesimal
//
//--------------------------------------------------------------------------------------------------------------------
char* PanelCapture::J2000_2_str_dc(vec2& v, char* str_dc, int sizeof_str)
{
	if ( !bFits )		return str_dc;

	struct dms	DMS;

	deg2dms( v.y, DMS );

	snprintf( (char*)str_dc, sizeof_str, "%02.0lf° %02.0lf' %02.2lf\"", DMS.d, DMS.m, DMS.s );

	return str_dc;
}
//--------------------------------------------------------------------------------------------------------------------
//
// Transformation de coordonnees
//----------------------------------
// (x,y) ds la fenetre Capture(ps de zoom)		 =>    (x,y) Ascension D. et déclinaison
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::J2000_2_parent(vec2& v)
{
	if ( !bFits )		return;

	pCapture->getFits()->J2000_2_tex( v );
	tex_2_panel( v );
	v -= vec2( -getPosX(), -getPosY() );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::print_echelle_coordonnees()                         
{
    log( (char*)"PanelCapture::print_echelle_coordonnees" );
    
    char str_ad[80];
    char str_dc[80];
    char str[80];

	vec2 v0, v1;
	vec2 v, v2000, w;

	double ch, cb, cg, cd;
	double P, p, c;
	double div, epsilon, max;
	//----------------------------------------------------------------------------
    log( (char*)"---" );
    log_tab(true);
	v0 = vec2(      0.0, 0.0 );
	v1 = vec2( SIZE_ECH, 0.0 );

	parent_2_J2000( v0 );	v0.to_str( str );
	logf( (char*)"v0 = %s", str );
	
	parent_2_J2000( v1 );	v1.to_str( str );
	logf( (char*)"v1 = %s", str );

	div = compute_div( abs(v1.x-v0.x) );
	logf( (char*)"Compute Div sur ad  \tdelta=%f   div=%f   1/div=%f", (float)(v1.x-v0.x), (float)div, (float)1.0/div );

	//----------------------------------------------------------------------------
    log( (char*)"" );
	//----------------------------------------------------------------------------
	panelStdOut->change_tab_size();
	panelStdOut->add_tab_size( 90 );
	panelStdOut->add_tab_size( 220 );
	panelStdOut->add_tab_size( 240 );
	panelStdOut->add_tab_size( 400 );
	
	for( int i=0; i < tAD.size(); i++ )
	{
		struct coord_line c = tAD[i];
		v = c.p1; 		parent_2_J2000(v);
		//v.x += 0.000001;
		logf( (char*)"Unite %02d\tp1(%lf, %lf)\tp2(%lf, %lf)\t%s", i, c.p1.x, c.p1.y, c.p2.x, c.p2.y, J2000_2_str_ad(v, str, sizeof(str))  ); 
	}
	//----------------------------------------------------------------------------
    log( (char*)"" );
	v0 = vec2( 0.0, 0.0 );
	v1 = vec2( 0.0, SIZE_ECH );

	parent_2_J2000( v0 );	v0.to_str( str );
	logf( (char*)"v0 = %s", str );
	
	parent_2_J2000( v1 );	v1.to_str( str );
	logf( (char*)"v1 = %s", str );

	div = compute_div( abs(v1.y-v0.y) );
	logf( (char*)"Compute Div sur de  \tdelta=%f   div=%f   1/div=%f", (float)(v1.y-v0.y), (float)div, (float)1.0/div );

	//----------------------------------------------------------------------------
    log( (char*)"" );
	//----------------------------------------------------------------------------
	for( int i=0; i<tDE.size(); i++ )
	{
		struct coord_line c = tDE[i];
		w = c.p1; 		parent_2_J2000(w);
		logf( (char*)"Unite %02d\t p1(%lf, %lf)\tp2(%lf, %lf)    \t%s", i, c.p1.x, c.p1.y, c.p2.x, c.p2.y, J2000_2_str_dc(w, str, sizeof(str))  ); 
	}
	
	//----------------------------------------------------------------------------
	panelStdOut->restaure_tab_size();
    log( (char*)"---" );
    log_tab(false);
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
    logf( (char*)"this->vAD=(%.6lf, %.6lf)", vAD.x, vAD.y );
    logf( (char*)"this->vDE=(%.6lf, %.6lf)", vDE.x, vDE.y );
    logf( (char*)"this->dAngleAD=%0.6lf", dAngleAD );
    logf( (char*)"this->dAngleDE=%0.6lf", dAngleDE );

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

    log( (char*)"" );
    logf( (char*)"%d ligne(s) AD", tAD.size() );
    logf( (char*)"%d ligne(s) DE", tDE.size() );
	//-----------------------------------------------------------------------
	// Position des coins en J2000
	//-----------------------------------------------------------------------
    log( (char*)"" );
	char		str_ad[80];
	char		str_dc[ sizeof(str_ad) ];
	vec2		vParent;

	//-----------------------------------------------------------------------
	panelStdOut->change_tab_size();
	panelStdOut->add_tab_size( 130 );
	panelStdOut->add_tab_size( 200 );
	panelStdOut->add_tab_size( 400 );
	//-----------------------------------------------------------------------

	vParent = vec2( 0.0, 0.0 );
	parent_2_str_ad_str_dc( vParent, str_ad, str_dc, sizeof(str_ad)  );
	logf( (char*)"Haut-Gauche \t(%f, %f)\t(%s, %s)", (float)vParent.x, (float)vParent.y, str_ad, str_dc );

	vParent = vec2( parent->getDX(), 0.0 );
	parent_2_str_ad_str_dc( vParent, str_ad, str_dc, sizeof(str_ad)  );
	logf( (char*)"Haut-Droite \t(%f, %f)\t(%s, %s)", (float)vParent.x, (float)vParent.y, str_ad, str_dc );

	vParent = vec2( 0.0, parent->getDY() );
	parent_2_str_ad_str_dc( vParent, str_ad, str_dc, sizeof(str_ad)  );
	logf( (char*)" Bas-Gauche \t(%f, %f)\t(%s, %s)", (float)vParent.x, (float)vParent.y, str_ad, str_dc );

	vParent = vec2( parent->getDX(), parent->getDY() );
	parent_2_str_ad_str_dc( vParent, str_ad, str_dc, sizeof(str_ad)  );
	logf( (char*)" Bas-Droite \t(%f, %f)\t(%s, %s)", (float)vParent.x, (float)vParent.y, str_ad, str_dc );

	vec2 v0 = vec2(      0.0, 0.0 );	parent_2_J2000( v0 );
	vec2 v1 = vec2( SIZE_ECH, 0.0 );	parent_2_J2000( v1 );

	double div = compute_div( abs(v1.y-v0.y) );
	logf( (char*)"      Div H \t(%.0lf, %lf))", div, 1.0/div );
	
	v0 = vec2( 0.0,      0.0 );	parent_2_J2000( v0 );
	v1 = vec2( 0.0, SIZE_ECH );	parent_2_J2000( v1 );

	div = compute_div( abs(v1.x-v0.x) );
	logf( (char*)"      Div V \t(%.0lf, %lf))", div, 1.0/div );

	//-----------------------------------------------------------------------
	panelStdOut->restaure_tab_size();
	log_tab(false);
	//-----------------------------------------------------------------------
	
	log( (char*)"---" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::findGaiaDR3()
{
    logf( (char*)"PanelCapture::findGaiaDR3()" );
    log_tab(true);

	if ( pVizier )	{
		if ( !bAffCatalogPosition )	{
			bAffCatalogPosition = true;
			pVizier->affiche_position();
		    log_tab(false);
			return;
		}
		else	{
			bAffCatalogPosition = false;
			pVizier->affiche_magnitude();
		    log_tab(false);
			return;
		}

		bAffCatalogPosition = false;
		pVizier->efface();
		delete pVizier;
		pVizier = NULL;
	    log_tab(false);
		return;
	}
	
	if ( !bFits )			{ log_tab(false); return; }
	if ( 	pCapture->getFits()->getCDELT1() == -1.0 || 
			pCapture->getFits()->getCDELT2() == -1.0 )
	{
		log_thread( (char*)" [ Erreur ] Le fichier fit ne contient pas de parametre CDELT" );
		log_tab(false);
		return;
	};

	//--------------------------------------------------------
	// CALCUL LA TAILLE DE L'IMAGE pour le rayon de recherche
	
	// resolution ecran
	double resH = pCapture->getFits()->getNAXIS1();
	double resV = pCapture->getFits()->getNAXIS2();
	logf( (char*)"Calcul du rayon : %0.2lf %0.2lf", resH, resV );
	
	// Coefficient deg/pix
	resH *= pCapture->getFits()->getCDELT1() * 3600.0;
	resV *= pCapture->getFits()->getCDELT2() * 3600.0;
	logf( (char*)"Calcul du rayon : %0.2lf %0.2lf", resH, resV );
	
	// On prend la diagonnale pour rayon de recherche
	double rayon = sqrt( resV*resV + resH*resH );
	rayon /= 2.0;
	logf( (char*)"Calcul du rayon : r = %0.2lf", rayon );
	
	// si plus grand qu 1" d'arc
	if ( rayon > 1800.0 )       	rayon = 1800.0;
	logf( (char*)"  rayon calcule %.2f", rayon );
	
	//---------------------------------------------------
	// RECHERCHE DU CENTRE DE L'IMAGE pour la requete
	vec2 vCentre = vec2( pCapture->getFits()->getCRVAL1(), pCapture->getFits()->getCRVAL2() );

	pVizier = new Catalog(false);
	static char coord[80];
	snprintf( (char*)coord, sizeof(coord), "%0.2f-%0.2f", vCentre.x, vCentre.y );
	string sFilename = "/home/rene/.astropilot/vizier/vizier_" + string(coord) + ".cat";
	//---------------------------------------------------
	// Si le fichier de donne existe
	// on le charge
	if ( pVizier->charge( sFilename ) )		
	{
		updatePos();
		log_tab(false);
		return;
	}


	//---------------------------------------------------
	// lance la requete
	//static char coord[80];
	snprintf( (char*)coord, sizeof(coord), "-r %d% 0.8f %0.8f", (int)(rayon), vCentre.x, vCentre.y );
	logf( (char*)"Requete Gaia DR3 : %s", coord );

	//vizier_load_stars( pVizier, string(coord) );	
	vizier_capture_load_stars( pVizier, string(coord), this );	
    log_tab(false);
	
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::findGaiaDR3_end()
{
    logf_thread( (char*)"PanelCapture::findGaiaDR3_end()" );
    
	vec2 vCentre = vec2( pCapture->getFits()->getCRVAL1(), pCapture->getFits()->getCRVAL2() );

	static char coord[80];
	snprintf( (char*)coord, sizeof(coord), "%0.2f-%0.2f", vCentre.x, vCentre.y );
	string sFilename = "/home/rene/.astropilot/vizier/vizier_" + string(coord) + ".cat";

	//         /home/rene/.astropilot/vizier/vizier_283.38-33.05.cat

 	pVizier->sauve(sFilename );	

	logf_thread( (char*)"Fichier : %s", sFilename.c_str() );

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
    logf( (char*)"PanelCapture::sendStellarium( %d, %d)", xm, ym );

		vec2 vJ2000;
		vec2 vTex = vec2( xm, ym );
		screen_2_tex( vTex );
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
			change_ad_status( vJ2000.x );
			change_dc_status( vJ2000.y );

        	if ( bArduino )	{
		        sprintf( cmd, "ia%lf;id%lf", AD, (vJ2000.y) );
		        logf( (char*)"  Envoi arduino : %s", cmd );
	            Serial::getInstance().write_string(cmd);
        	}
        	else	{
				Serveur_mgr::getInstance().write_stellarium( (double)vJ2000.x, (double)vJ2000.y );
		        logf( (char*)"  Envoi stellarium : %lf, %lf", vJ2000.x, vJ2000.y );
			}
        }

	if ( Serveur_mgr::getInstance().isConnect() );
	{
		Serveur_mgr::getInstance().write_stellarium( (double)vJ2000.x, (double)vJ2000.y );
	}
	/*
	if ( LX200::getInstance().isConnect( (double)vJ2000.x, (double)vJ2000.y );
	{
		LX200::getInstance().write_stellarium( (double)vJ2000.x, (double)vJ2000.y );
	}
	*/
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
		//logf( (char*)"passiveMotionFunc(%d,%d)", vMouse.x, vMouse.y );
		log_tab(true);
		//passiveMotionFunc( vMouse.x, vMouse.y );
		log_tab(false);
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::change_ad( double ad )
{
	if ( !bFits )			return;

	//logf( (char *)"PanelCapture::change_ad(%lf)", ad );
	vTelescopeJ2000.x = ad<0.0 ? 360.0+ad : ad ;
	
	pCapture->getFits()->J2000_2_tex( vTelescopeJ2000, vTelescopeTex );
	vTelescopePanel  = vTelescopeScreen = vTelescopeTex;
	tex_2_screen( vTelescopeScreen );
	tex_2_panel( vTelescopePanel );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::change_dc( double dc )
{
	if ( !bFits )			return;
	//logf( (char *)"PanelCapture::change_dc(%lf)", dc );
	vTelescopeJ2000.y = dc;
		
	pCapture->getFits()->J2000_2_tex( vTelescopeJ2000, vTelescopeTex );
	vTelescopePanel  = vTelescopeScreen = vTelescopeTex;
	tex_2_screen( vTelescopeScreen );
	tex_2_panel( vTelescopePanel );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::setEchelleVisible( bool b )
{
	//logf( (char*)"PanelCapture::setEchelleVisible( %s )", BOOL2STR(b) );
	
	for( int i=0; i<tAD.size(); i++ )		tAD[i].pTextCoord->setVisible(b);
	for( int i=0; i<tDE.size(); i++ )		tDE[i].pTextCoord->setVisible(b);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::clear_AD_DE()
{
	for( int i=0; i<tAD.size(); i++ )
	{
		PanelText* p = tAD[i].pTextCoord;
		sup(p);
		delete p;
	}
	tAD.clear();
	
	
	for( int i=0; i<tDE.size(); i++ )
	{
		PanelText* p = tDE[i].pTextCoord;
		sup(p);
		delete p;
	}
	tDE.clear();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
char* PanelCapture::pin_mode_2_str( pin_mode mode )
{
	switch( mode )
	{
	case pin_mode::HAUT:
		return (char*)"HAUT";
	case pin_mode::GAUCHE:
		return (char*)"GAUCHE";
	case pin_mode::DROITE:
		return (char*)"DROITE";
	case pin_mode::BAS:
		return (char*)"BAS";
	}
	return (char*)"";
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::compute_points( pin_mode mode, struct coord_line& c, double coef )
{
	double			cc;
	vec2			p1, p2;
	bool			b1, b2;

	
	switch( mode )
	{
	case pin_mode::HAUT:
		//----------- Declinaison  (.y)
		cc = (coef - vHG.y) / (vHD.y - vHG.y) * (double)parent->getDX();
		p1 = vec2(round(cc), 0.0);

		cc = (coef - vBG.y) / (vBD.y - vBG.y) * (double)parent->getDX();
		p2 = vec2(round(cc), parent->getDY());
		
		b1	= ( 0.0 <= p1.x ) && ( p1.x <= (double)parent->getDX() );
		b2	= ( 0.0 <= p2.x ) && ( p2.x <= (double)parent->getDX() );
		
		c.bContinue	= !b1 && !b2;
		break;

	case pin_mode::GAUCHE:
		//----------- A. droite  (.x)
		cc = (coef - vHG.x) / (vBG.x - vHG.x) * (double)parent->getDY();
		p1 = vec2(0.0, round(cc));
	
		cc = (coef - vHD.x) / (vBD.x - vHD.x) * (double)parent->getDY();
		p2 = vec2(parent->getDX(), round(cc));

		b1	= ( 0.0 <= p1.y ) && ( p1.y <= (double)parent->getDY() );
		b2	= ( 0.0 <= p2.y ) && ( p2.y <= (double)parent->getDY() );
		
		c.bContinue	= !b1 && !b2;
		break;

	case pin_mode::DROITE:
		//----------- Declinaison  (.y)
		cc = (coef - vHG.y) / (vBG.y - vHG.y) * (double)parent->getDY();
		p1 = vec2(0.0, round(cc));
	
		cc = (coef - vHD.y) / (vBD.y - vHD.y) * (double)parent->getDY();
		p2 = vec2(parent->getDX(), round(cc));

		b1	= ( 0.0 <= p1.y ) && ( p1.y <= (double)parent->getDY() );
		b2	= ( 0.0 <= p2.y ) && ( p2.y <= (double)parent->getDY() );
		
		c.bContinue	= !b1 && !b2;
		break;

	case pin_mode::BAS:
		//----------- A. droite  (.x)
		cc = (coef - vHG.x) / (vHD.x - vHG.x) * (double)parent->getDX();
		p1 = vec2(round(cc), 0.0);
	
		cc = (coef - vBG.x) / (vBD.x - vBG.x) * (double)parent->getDX();
		p2 = vec2(round(cc), parent->getDY());
		
		b1	= ( 0.0 <= p1.x ) && ( p1.x <= (double)parent->getDX() );
		b2	= ( 0.0 <= p2.x ) && ( p2.x <= (double)parent->getDX() );
		
		c.bContinue	= !b1 && !b2;
		break;

	}
	
	//c.bContinue = c.bContinue || c.bBreak;

	c.bBreak = false;

	c.p1 = p1;
	c.p2 = p2;
	
	if ( bOneFrame )
	{
		char		str0[80];
		char		str1[80];
		
		//parent_2_J2000( p1 );
		p1.to_str(str0);
		p2.to_str(str1);
		logf( (char*)"%s)\tcoef=%lf\tp1%s\tp2%s\t%s ", pin_mode_2_str(mode), coef, str0, str1, BOOL2STR(c.bContinue)   );
		//deg2str_hms(cc, str0, sizeof(str0)), deg2str_dms(p1.y, str1, sizeof(str1))
		/*
		p1.to_str(str0);
		logf( (char*)"%s  - %s", str0, BOOL2STR(c.bContinue), BOOL2STR(c.bBreak));
		*/
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::compute_lines( bool bAD, pin_mode mode, double div, double cg )
{
	//vec2 					p1, p2;
	double 					coef;
	double					epsilon = 0.000001/div;
	double					s = bAD ? -1.0 : 1.0;
	vector<coord_line>& 	tLine = bAD ? tAD : tDE;
	char					str[80];
	unsigned				nb = 0;
	
	//if ( cg<0.0 )	logf( (char*)"cg negatif" );
	//s *= -1.0;
	
	for( int i=-30; i<30; i++ )
	{
		struct coord_line c;
		c.pin			= mode;

		coef = cg + s * (double)i/div;

		if ( bOneFrame )
		{
			//logf( (char*)"%s) %lf-%s", pin_mode_2_str(mode), coef, (char*)deg2str_hms( coef, str, sizeof(str)) );
			log_tab(true);
		}
		compute_points( mode, c, coef );
		if ( bOneFrame )
		{
			log_tab(false);
		}

		if ( c.bContinue )
		{
			if ( nb == 0 )			continue;
			else					break;
		}
		nb ++;

		PanelText* p;

		if ( bAD )			p = new PanelText( (char*)deg2str_hms( coef+epsilon, str, sizeof(str)), PanelText::NORMAL_FONT, 0, 0); 
		else				p = new PanelText( (char*)deg2str_dms( coef+epsilon, str, sizeof(str)), PanelText::NORMAL_FONT, 0, 0); 
	    
	    if ( bAD )		    p->setAlpha( dAngleAD > 45.0  ?(float)180.0-dAngleAD : (float)-dAngleAD );
	    else			    p->setAlpha( dAngleAD < -45.0 ?(float)180.0-dAngleDE : (float)-dAngleDE );

	    add(p);
		p->setPos( c.p1.x-getPosX()+15, c.p1.y-getPosY() );
		p->updatePos();
		if (pCapture->getAfficheGrille())		p->setVisible(true);
		else								    p->setVisible(false);

		unsigned long color = bAD ? COLOR32(0, 0, 255, 255) : COLOR32(255, 0,   0, 255);

		if ( bNuit )	p->setColor( COLOR32(255, 0,   0, 128) );
		else			p->setColor( color );

		c.pTextCoord	= p;
		
		tLine.push_back( c );
	}
}
//--------------------------------------------------------------------------------------------------------------------
// Calcul 
// 1h   / 3600 = 0,004166667
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::computeRepere_03()
{
	//log( (char*)"PanelCapture::computeRepere_03()" );
    char str_ad[80];
    char str_dc[80];
    char str[80];

	vec2 v0, v1;
	vec2 v, v2000;
	vec2 p1, p2;

	double ch, cb, cg, cd;
	double P, p, c;
	double div, epsilon, sign;

	if ( bOneFrame )			
	{
		panelStdOut->change_tab_size();
		panelStdOut->add_tab_size( 80 );
		panelStdOut->add_tab_size( 120 );
		panelStdOut->add_tab_size( 220 );
		panelStdOut->add_tab_size( 180 );
	}
	//----------------------------------------------------------------------------
	// Calcul des coordonnée des coins de la fenetre parent (alpha et delta J2000)
	//----------------------------------------------------------------------------
	clear_AD_DE();
	compute_angle();
	
	vHG = vec2( 0.0, 0.0 );								parent_2_J2000( vHG );
	vHD = vec2( parent->getDX(), 0.0 );					parent_2_J2000( vHD );
	vBG = vec2( 0.0, parent->getDY() );					parent_2_J2000( vBG );
	vBD = vec2( parent->getDX(), parent->getDY() );		parent_2_J2000( vBD );


	if (  -45.0 < dAngleAD  && dAngleAD <=   45.0 )
	{
		//----------------------------------------------------------------------------
		//       Declinaison
		//----------------------------------------------------------------------------
		v0 = vec2(      0.0, 0.0 );							parent_2_J2000( v0 );
		v1 = vec2( SIZE_ECH, 0.0 );							parent_2_J2000( v1 );

		div = compute_div( abs(v1.y-v0.y) );
		//----------------------------------------------------------------------------
		epsilon = 0.000001/div;
		cg = floor( vHG.y * div ) / div;

		if ( v1.y-v0.y < 0.0 )		div *= -1.0;
		//----------------------------------------------------------------------------
		compute_lines( false, pin_mode::HAUT,	div, cg );
		//----------------------------------------------------------------------------
		//     Asc Droite
		//----------------------------------------------------------------------------
		v0 = vec2( 0.0, 0.0 );								parent_2_J2000( v0 );
		v1 = vec2( 0.0, SIZE_ECH );							parent_2_J2000( v1 );

		div = compute_div( abs(v1.x-v0.x) );
		//----------------------------------------------------------------------------
		epsilon = 0.000001/div;
		cg = floor( vHG.x * div ) / div;

		if ( v1.x-v0.x > 0.0 )		div *= -1.0;
		
		compute_lines( true, pin_mode::GAUCHE, div, cg );
	}
	else
	if (   45.0 < dAngleAD  || dAngleAD <=  -45.0 )
	{
		//----------------------------------------------------------------------------
		//       Declinaison
		//----------------------------------------------------------------------------
		v0 = vec2(      0.0, 	  0.0 );					parent_2_J2000( v0 );
		v1 = vec2( 		0.0, SIZE_ECH );					parent_2_J2000( v1 );

		div = compute_div( abs(v1.y-v0.y) );
		//----------------------------------------------------------------------------
		epsilon = 0.000001/div;
		cg = floor( vHG.y * div ) / div;

		if ( v1.y-v0.y < 0.0 )		div *= -1.0;
		//----------------------------------------------------------------------------
		compute_lines( false, pin_mode::DROITE,	div, cg );
		
		//----------------------------------------------------------------------------
		//     Asc Droite
		//----------------------------------------------------------------------------
		v0 = vec2(      0.0, 0.0 );							parent_2_J2000( v0 );
		v1 = vec2( SIZE_ECH, 0.0 );							parent_2_J2000( v1 );

		div = compute_div( abs(v1.x-v0.x) );
		//----------------------------------------------------------------------------
		epsilon = 0.000001/div;
		cg = floor( vHG.x * div ) / div;
		
		if ( v1.x-v0.x > 0.0 )		div *= -1.0;
		
		compute_lines( true, pin_mode::BAS,  -div, cg );
	}
		
	if ( bOneFrame )			
	{
		panelStdOut->change_tab_size();
		bOneFrame = false;
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::computeRepere()                         
{
	if ( !bFits )			return;
	
	computeRepere_03();
}
//--------------------------------------------------------------------------------------------------------------------
//
// division normalisée : 	 1°	= 1°   				1/1			2
//							30'	= 0.5°				1/2			3
//							10'	= 0.1666667°		1/6			2
//							 5'	= 0.0833333°		1/12		2
//							 2'	= 0.0333333°		1/30		5/2
//							 1'	= 0.0166667°		1/60		2
//							30"	= 0.0083333°		1/120		2
//							10"	= 0.0027778°		1/360		2
//							 5"	= 0.0013889°		1/720		5/2
//							 2"	= 0.0005556°		1/1800		2
//							 1"	= 0.0002778°		1/3600		2
//--------------------------------------------------------------------------------------------------------------------
double t_div[] = { 1.0, 2.0, 6.0, 12.0, 30.0, 60.0, 120.0, 360.0, 720.0, 1800.0, 3600.0 };
//--------------------------------------------------------------------------------------------------------------------
double PanelCapture::compute_div( double val )
{
	double d = 0.0;
	
	for( int i=0; i<11; i++ )
	{
		d = t_div[i];
		if ( val > (1.0/t_div[i]) )			return d;
	}
	return d;
}
//--------------------------------------------------------------------------------------------------------------------
//
//		Sens trigo
//
//                -----------    ref horizon           ----------------
//							/										   \
//						   /										    \
//						  /	 dAngleAD = -60°			dAngleAD = -120° \
//						 /												  \
//						/												   \
//					   /													\	
//					  /														 \
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCapture::compute_angle()                         
{
    vec2		v, w;

    v = vec2( 0.0, 0.0 );
    parent_2_J2000( v );
	w = v;
	w += vec2( 100.0, 0.0 );
    J2000_2_parent( w );

	w.normalize();
    dAngleAD = asin(w.x);
    dAngleAD = RAD2DEG(dAngleAD);
    
    
	if ( w.y<0.0 )				dAngleAD *= -1.0;
	
    dAngleDE = dAngleAD - 90.0;

	return;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

