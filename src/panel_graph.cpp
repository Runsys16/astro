#ifndef PANEL_GRAPH_CPP
#define PANEL_GRAPH_CPP
//--------------------------------------------------------------------------------------------------------------------
#include "panel_graph.h"
//--------------------------------------------------------------------------------------------------------------------
#define DELTA_AXE_Y	16.0
#define DELTA_AXE_X	20.0
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelGraph::~PanelGraph()
{
    logf( (char*)"Destructeur  PanelGraph::~PanelGraph()" );

	sup( pTitre );
	delete pTitre;
	
	for( int i=0; i<tAbsPanel.size(); i++ )			sup( tAbsPanel[i] );
	for( int i=0; i<tAbsPanel.size(); i++ )			delete tAbsPanel[i];
	tAbsPanel.clear();
	
	for( int i=0; i<tOrdPanelG.size(); i++ )		sup( tOrdPanelG[i] );
	for( int i=0; i<tOrdPanelG.size(); i++ )		delete tOrdPanelG[i];
	tOrdPanelG.clear();
	
	for( int i=0; i<tOrdPanelD.size(); i++ )		sup( tOrdPanelD[i] );
	for( int i=0; i<tOrdPanelD.size(); i++ )		delete tOrdPanelD[i];
	tOrdPanelD.clear();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelGraph::PanelGraph()
{
    logf( (char*)"Constructeur  PanelGraph::PanelGraph()" );
    log_tab(true);
    
	WindowsManager&     wm  = WindowsManager::getInstance();
    VarManager&         var = VarManager::getInstance();

    init_var();
    setSize( 1000, 400 );

    setBackground((char*)"images/background.tga");
	pTitre = new PanelText( (char*)"",  PanelText::NORMAL_FONT, 10, 5  );
	pTitre->setAlign( PanelText::CENTER );
	add( pTitre );
    
    setExtraString("PanelGraph");

    log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::init_var()
{
    VarManager&         var = VarManager::getInstance();
    
    dXmin	=   500.0;
    dXmax	= 55000.0;
    dYmin	=     7.0;
    dYmax	=    16.0;

	bLogX = false ;
	bLogY = false;
	
	pPanelCallback = NULL;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::panel2graphX( double& __x )
{
	__x = __x + _debX - DELTA_AXE_X;
	if ( bLogX )		__x = pow(10.0, __x / _pasX);
	else				__x = __x  / _pasX;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::panel2graphY( double& __y )
{
	__y = getPosDY() -__y + _debY - DELTA_AXE_Y;
	if ( bLogY )		__y = pow(10.0, __y / _pasX);
	else				__y = __y / _pasY;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::panel2graph( vec2& __v )
{
	panel2graphX(__v.x);
	panel2graphY(__v.y);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::graph2panelX( double& __x )
{
	if ( bLogX )		__x = log10(__x ) * _pasX;
	else				__x = __x  * _pasX;
	__x = __x - _debX + DELTA_AXE_X;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::graph2panelY( double& __y )
{
	if ( bLogY )		__y = log10(__y ) * _pasX;
	else				__y = __y * _pasY;
	__y = getPosDY() -__y + _debY - DELTA_AXE_Y;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::graph2panel( vec2& __v )
{
	graph2panelX(__v.x);
	graph2panelY(__v.y);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::glCercle(int x, int y, int rayon)
{
    double step = 80.0/(double)rayon;
    if ( step < 1.0 )           step = 1.0;
    
    //logf ( (char*)" rayon %d   step %0.2f", rayon, step );
    
	glBegin(GL_LINE_LOOP);

        for( double i=0; i<=360.0; i+=step )
        {
            double fx = (double)x+ (double)rayon*cos(DEG2RAD(i)) + getPosX(); 
            double fy = (double)y+ (double)rayon*sin(DEG2RAD(i)) + getPosY();
            glVertex2i(fx,fy);
        }
        
    glEnd();        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::glCroix( int x,  int y,  int dx,  int dy )
{
    x += getPosX();// + dx_screen;
    y += getPosY();// + dy_screen;

	glBegin(GL_LINES);

	    glVertex2i(x, y-dy);         glVertex2i(x, y+dy);
	    glVertex2i(x-dx, y);         glVertex2i(x+dx, y);

    glEnd();        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::glCroix( vec2 v, vec2 delta )
{
	glCroix( (int)v.x, (int)v.y, (int)delta.x, (int)delta.y );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::glLine( int x, int y, int dx,  int dy )
{
    int delta_x = 0;
    int delta_y = 0;
    
    x += getPosX();// + dx_screen;
    y += getPosY();// + dy_screen;
    
	glBegin(GL_LINES);

        glVertex2i(x,y);                glVertex2i(x+dx,y+dy);

    glEnd();        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::glLine( vec2 v, vec2 delta )
{
	glLine( (int)v.x, (int)v.y, (int)delta.x, (int)delta.y );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::glCarre( int x, int y, int dx,  int dy )
{
    int delta_x = 0;
    int delta_y = 0;
    
    x += getPosX();// + dx_screen;
    y += getPosY();// + dy_screen;
    
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
void PanelGraph::glEchelleAxe( int AXE, int SIZE, float max, float min, PanelText* pMax, PanelText* pMin )
{
    VarManager& var = VarManager::getInstance();

    float gris = 0.3;
    vcf4 color, colorLimit, colorAxe;
    
    if ( var.getb("bNuit") )
    {
        color       = vcf4( gris, 0.0, 0.0, 1.0 );    
        colorLimit  = vcf4( 0.8, 0.0, 0.0, 1.0 );    
        colorAxe    = vcf4( 1.0, 0.0, 0.0, 1.0 );
    }
    else                            
    {
        //gris = 0.4;
        color       = vcf4( gris, gris, gris, 1.0 );
        colorLimit  = vcf4( 1.0, 0.0, 0.0, 1.0 );
        colorAxe    = vcf4( 1.0, 1.0, 1.0, 1.0 );
    }
    
    glBegin(GL_LINES);

    glEnd();        


    glBegin(GL_LINES);
    glEnd();        
        glDisable(GL_LINE_STIPPLE);    
        glColor4fv( (GLfloat*)&colorAxe );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::glEchelle()
{
    VarManager& var = VarManager::getInstance();

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::displayCourbeVizi()
{

	if( bNuit )						glColor4fv( (GLfloat*)&cRouge );
	else							glColor4fv( (GLfloat*)&cOrange );

	double _DY = getPosDY();
	
	for( int i=1; i<cSort.size(); i++ )
	{
		int j0 = cSort[i];
		int j1 = cSort[i-1];

		vec2 v0 = cVizi[j0];
		vec2 v1 = cVizi[j1];
		
		graph2panel(v0);
		graph2panel(v1);

		vec2 w = v1-v0;

		glLine( v0, w );
		glCroix( v0, vec2(6, 6) );
		
		if ( i==1 ) 			glCroix( v1, vec2(6, 6) );
	}

	if ( iMouseCapture != -1 )
	{
		vec2 v0 = cVizi[iMouseCapture];
		graph2panel(v0);

		glColor4fv( (GLfloat*)&cRouge );
		glCercle( (int)v0.x, (int)v0.y , 15 );
		//iMouseCapture = -1;
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::displayCourbeStar()
{
	vcf4 color;
	if( bNuit )						color = cRouge;
	else							color = cVert;
	
	glColor4fv( (GLfloat*)&color );

	double _DY = getPosDY();
	
	for( int i=1; i<cSort.size(); i++ )
	{
		int j0 = cSort[i];
		int j1 = cSort[i-1];

		vec2 v0 = cStar[j0];
		vec2 v1 = cStar[j1];
		
		graph2panel(v0);
		graph2panel(v1);
		
		vec2 w = v1-v0;
		
		glLine( v0, w );
		
		glCroix( v0, vec2(6, 6) );

		if ( i==1 ) 			glCroix( v1, vec2(6, 6) );
	}
	
	if ( iMouseCapture != -1 )
	{
		vec2 v0 = cStar[iMouseCapture];
		graph2panel(v0);

		glColor4fv( (GLfloat*)&cRouge );
		glCarre( (int)v0.x, (int)v0.y , 15, 15 );
		//iMouseCapture = -1;
	}

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::ajoute_ordG(double val, vec2 pos)
{
	char	str[21];
	vec2 v = pos;
	graph2panel( v );
	
	
	snprintf( (char*)str, sizeof(str), "%d", (int)val );
	PanelText* p = new PanelText( (char*)str,  PanelText::NORMAL_FONT, v.x+4-DELTA_AXE_X, v.y-8  );
	add( p );
	//log( (char*)"PanelText" );
	tOrdPanelG.push_back(p);
	tOrdVec2G.push_back(pos);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::ajoute_ordD(double val, vec2 pos)
{
	char	str[21];
	vec2 v = pos;
	graph2panel( v );
	
	
	snprintf( (char*)str, sizeof(str), "%d", (int)val );
	PanelText* p = new PanelText( (char*)str,  PanelText::NORMAL_FONT, v.x+4+DELTA_AXE_X, v.y-8  );
	add( p );
	//log( (char*)"PanelText" );
	tOrdPanelD.push_back(p);
	tOrdVec2D.push_back(pos);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::ajoute_abs(double val, vec2 pos)
{
	char	str[21];
	vec2 v = pos;
	graph2panel( v );
	
	snprintf( (char*)str, sizeof(str), "%d", (int)val );
	PanelText* p = new PanelText( (char*)str,  PanelText::NORMAL_FONT, v.x, v.y  );
	
	add( p );

	tAbsPanel.push_back(p);
	tAbsVec2.push_back(pos);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::update_ord()
{
	if ( tOrdPanelG.size() == 0 )			return;

	for( int i=0; i<tOrdPanelG.size(); i++ )
	{
		vec2 v = tOrdVec2G[i];
		graph2panel(v);
		int dd = tOrdPanelG[i]->getPosDX() + 4;
		
		tOrdPanelG[i]->setPos( v.x-dd, v.y-8 );
	}
	
	for( int i=0; i<tOrdPanelD.size(); i++ )
	{
		vec2 v = tOrdVec2D[i];
		graph2panel(v);

		tOrdPanelD[i]->setPos( v.x+4, v.y-8 );
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::update_abs()
{
	if ( tAbsPanel.size() == 0 )			return;
	
	for( int i=0; i<tAbsPanel.size(); i++ )
	{
		vec2 v = tAbsVec2[i];
		graph2panel(v);
		int dd = tAbsPanel[i]->getPosDX()/2;
		
		tAbsPanel[i]->setPos( v.x-dd, v.y+4 );
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::displayAxeVlog()
{
	
	if( bNuit )						glColor4fv( (GLfloat*)&cRougeC );
	else							glColor4fv( (GLfloat*)&cGris );

	double _log;
	double	_ldeb = floor( log10(dXmin) ); 
	_log =  pow( 10.0, _ldeb );
	if ( _log <= 1.0 )		_log = 1.0;
	
	double  _dif = log10(dXmax) - log10(dXmin);
	_pasX = ( 1.0 /  _dif ) * ((double)getPosDX()-2*DELTA_AXE_X);
	_debX = log10(dXmin) * _pasX;
	_finX = log10(dXmax) * _pasX;	

	double fM = dXmax;	graph2panelX( fM );
	double fm = dXmin;	graph2panelX( fm );
		
	bool b = false;
	if ( tAbsPanel.size() == 0)		b = true;

	for( double i=1.0; i<=10.0; i+=1.0 )
	{
		if ( i == 10.0 )	{
			_log *= 10.0;
			if ( isinf(_log) )	{ log( (char*)"[ Erreur ] Infini PanelGraph::displayAxeV()" ); setVisible(false); break; }
			i = 1.0;
		}
		
		double _x = _log*i;
		vec2 v = vec2(_x, dYmax);
		graph2panel( v );
		
		if ( v.x < fm )	continue;
		if ( v.x > fM )	break;

		glLine(v, vec2(0.0, getPosDY()-2*DELTA_AXE_Y) );
		
		if ( b && i==1.0 )			ajoute_abs( _x, vec2(_x, dYmin) );
		if ( b && i==5.0 )			ajoute_abs( _x, vec2(_x, dYmin) );
		if ( b && i==2.0 )			ajoute_abs( _x, vec2(_x, dYmin) );

	}
	
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::displayAxeVlin()
{
	
	if( bNuit )						glColor4fv( (GLfloat*)&cRougeC );
	else							glColor4fv( (GLfloat*)&cGris );

	double  _dif = (dXmax - dXmin);
	_pasX = ( 1.0 /  _dif ) * ((double)getPosDX()-2*DELTA_AXE_X);
	_debX = dXmin * _pasX;
	_finX = dXmax * _pasX;	

	double _ech = _dif / 5.0;
	int n = 0;
	
	while( _ech >=10.0 )	{
		n++;
		_ech /= 10.0;
	}
	_ech = floor( _ech );
	for( int i=0; i<n; i++ )	_ech *= 10.0;
	

	double fM = dXmax;	graph2panelX( fM );
	double fm = dXmin;	graph2panelX( fm );
		
	bool b = false;
	if ( tAbsPanel.size() == 0)		b = true;

	for( double i=1.0; i<=10.0; i+=1.0 )
	{
		vec2 v = vec2(i*_ech, dYmax);
		graph2panel( v );

		if ( v.x < fm )	continue;
		if ( v.x > fM )	break;

		glLine(v, vec2(0.0, getPosDY()-2*DELTA_AXE_Y) );
		
		if ( b )			ajoute_abs( i*_ech, vec2(i*_ech, dYmin) );
	}
	
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::displayAxeV()
{
	if ( bLogX )		displayAxeVlog();
	else				displayAxeVlin();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::displayAxeHlog()
{
	double  _dif = dYmax - dYmin;
	_pasY = ( 1.0 /  _dif ) * ((double)getPosDY()-2*DELTA_AXE_Y);
	_debY = dYmin * _pasY;
	_finY = dYmax * _pasY;	
	
	double fM = dYmax;	graph2panelY( fM );
	double fm = dYmin;	graph2panelY( fm );

	bool b = false;
	if ( tOrdPanelG.size() == 0)		b = true;
	
	for( double i=1.0; i<=30.0; i+=1.0 )
	{
		vec2 v = vec2( dXmin, i );
		graph2panel( v );
				//logf( (char*)"%lf  %lf<%lf", v.y, fm, fM );
		if ( v.y > fm )	continue;
		if ( v.y < fM )	break;

		glLine(  v, vec2(getPosDX()-2*DELTA_AXE_X, 0.0) );
		if ( b )			ajoute_ordG( i, vec2(dXmin,i) );
		if ( b )			ajoute_ordD( i, vec2(dXmax,i) );
	}
	
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::displayAxeHlin()
{
	double  _dif = dYmax - dYmin;
	_pasY = ( 1.0 /  _dif ) * ((double)getPosDY()-2*DELTA_AXE_Y);
	_debY = dYmin * _pasY;
	_finY = dYmax * _pasY;	
	
	double fM = dYmax;	graph2panelY( fM );
	double fm = dYmin;	graph2panelY( fm );

	bool b = false;
	if ( tOrdPanelG.size() == 0)		b = true;
	
	for( double i=1.0; i<=30.0; i+=1.0 )
	{
		vec2 v = vec2( dXmin, i );
		graph2panel( v );
				//logf( (char*)"%lf  %lf<%lf", v.y, fm, fM );
		if ( v.y > fm )	continue;
		if ( v.y < fM )	break;

		glLine(  v, vec2(getPosDX()-2*DELTA_AXE_X, 0.0) );
		if ( b )			ajoute_ordG( i, vec2(dXmin,i) );
		if ( b )			ajoute_ordD( i, vec2(dXmax,i) );
	}
	
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::displayAxeH()
{

	if( bNuit )						glColor4fv( (GLfloat*)&cRougeC );
	else							glColor4fv( (GLfloat*)&cGris );

	if ( bLogY )	displayAxeHlog();
	else			displayAxeHlin();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::displayAxes()
{
	if (dXmin < 10.0 )		    dXmin = 10.0;

	//glLineStipple(1, 0xCCCC);
	glLineStipple(1, 0xAAAA);
	glEnable(GL_LINE_STIPPLE);    
	glLineWidth( 0.5 );
		displayAxeV();
		displayAxeH();
	glDisable(GL_LINE_STIPPLE);    
	glLineStipple(1, 0xFFFF);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::displayGL(void)
{
	if ( !visible )			return;

	WindowsManager& wm  = WindowsManager::getInstance();
    VarManager&     var = VarManager::getInstance();

    PanelWindow::displayGL();
    

    float height = (float)wm.getHeight();
	int   scx  = getX();
	int   scy  = height - getDY() - getY();
	int   scdx = getDX();
	int   scdy = getDY();
    
    glScissor( scx, scy, scdx, scdy );
    glEnable( GL_SCISSOR_TEST );

    	displayAxes();
    	if ( cStar.size() != 0 )		displayCourbeStar();
    	if ( cVizi.size() != 0 )		displayCourbeVizi();

    glDisable( GL_SCISSOR_TEST );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::resetCourbeStar()
{
    cStar.clear();
    cSort.clear();

	for( int i=0; i<tAbsPanel.size(); i++ )			sup( tAbsPanel[i] );
	for( int i=0; i<tOrdPanelG.size(); i++ )		sup( tOrdPanelG[i] );
	for( int i=0; i<tOrdPanelD.size(); i++ )		sup( tOrdPanelD[i] );

	for( int i=0; i<tAbsPanel.size(); i++ )			delete tAbsPanel[i];
	for( int i=0; i<tOrdPanelG.size(); i++ )		delete tOrdPanelG[i];
	for( int i=0; i<tOrdPanelD.size(); i++ )		delete tOrdPanelD[i];

    tAbsPanel.clear();
    tAbsVec2.clear();
    tOrdPanelG.clear();
    tOrdVec2G.clear();
    tOrdPanelD.clear();
    tOrdVec2D.clear();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::resetCourbeVizi()
{
    cVizi.clear();
    cSort.clear();

	for( int i=0; i<tAbsPanel.size(); i++ )			sup( tAbsPanel[i] );
	for( int i=0; i<tOrdPanelG.size(); i++ )		sup( tOrdPanelG[i] );
	for( int i=0; i<tOrdPanelD.size(); i++ )		sup( tOrdPanelD[i] );

	for( int i=0; i<tAbsPanel.size(); i++ )			delete tAbsPanel[i];
	for( int i=0; i<tOrdPanelG.size(); i++ )		delete tOrdPanelG[i];
	for( int i=0; i<tOrdPanelD.size(); i++ )		delete tOrdPanelD[i];

    tAbsPanel.clear();
    tAbsVec2.clear();
    tOrdPanelG.clear();
    tOrdVec2G.clear();
    tOrdPanelD.clear();
    tOrdVec2D.clear();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::resetCourbes()
{
	resetCourbeStar();
	resetCourbeVizi();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool PanelGraph::doublons_star( vec2 v )
{	
	for( int i=0; i<cStar.size(); i++ )		if ( v.x == cStar[i].x )	return true;
	return false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool PanelGraph::doublons_vizi( vec2 v )
{	
	for( int i=0; i<cVizi.size(); i++ )		if ( v.x == cVizi[i].x )	return true;
	return false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::addViziStar( vec2 v, vec2 w )
{	
	if ( v.x < 10.0 )				v.x = 10.0;
	//if ( doublons_star(v) )			v.x = -1.0;//return;

	if ( w.x < 10.0 )				w.x = 10.0;
	//if ( doublons_vizi(w) )			w.x = -1.0;//return;

    cVizi.push_back(v);
    cStar.push_back(w);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::addStar( vec2 v )
{	
	if ( v.x < 10.0 )				v.x = 10.0;
	if ( doublons_star(v) )			v.x = -1.0;//return;

    cStar.push_back(v);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::addVizi( vec2 v )
{
	if ( v.x < 10.0 )				v.x = 10.0;
	if ( doublons_vizi(v) )			v.x = -1.0;//return;

	//logf( (char*)"Vizi ajout (%0.4lf, %0.4lf", v.x, v.y );
    cVizi.push_back(v);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
vector<vec2>	triStar;
bool cmp(int i, int j)
{
    return triStar[i].x > triStar[j].x;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::sort_all()
{
    //sort(cVizi.begin(), cVizi.end(), cmp );
    //sort(cStar.begin(), cStar.end(), cmp );
    triStar.clear();
    cSort.clear();
    for( int i=0; i<cStar.size(); i++ )	{ triStar.push_back( cStar[i] ); cSort.push_back( i ); }
    
    sort(cSort.begin(), cSort.end(), cmp );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::setStarOverMouse(int is)
{
	iMouseCapture = is;
	//printf( "%d\n", iMouseCapture );
	return;
	for( int i=0; i<cSort.size(); i++ )
		if ( cSort[i] == is )			iMouseCapture = i;
//	iMouseCapture = cSort[is];
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::updatePos()
{
    PanelWindow::updatePos();
	update_abs();
	update_ord();
    PanelWindow::updatePos();
    
    //int x = getPosDX()/2 - pTitre->getPosDX()/2;
    //pTitre->setPos( x, 10 );
    
	if ( bNuit )
	{
		setColor( 0xFF0000FF );
		pTitre->setColor( 0xFF0000FF );
	}
	else
	{
		setColor( 0xFFffffFF );
		pTitre->setColor( 0xFFffffFF );
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::passiveMotionFunc( int mx, int my)
{
	iMouse = -1;

	if ( !getVisible())					return;
	if ( cStar.size() == 0 )			return;
	if ( cVizi.size() == 0 )			return;
	
	double dMini = 100000.0;
	
	vec2 vMouse = vec2(mx, my) - vec2(getPosX(), getPosY());// - vec2(_debX, _debY);
	panel2graph(vMouse);	
	
	for( int i=0; i<cStar.size(); i++ )
	{
		vec2 w = vec2(cStar[i].x, cStar[i].y) - vMouse;
		
		if ( w.length() < dMini )		
		{
			dMini = w.length();
			iMouse = i;
			iMouseCapture = i;
		}
	}
	
	if (iMouse != -1)	{
		pPanelCallback->callback( (void*)&iMouse );
	}
}
/*
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::clickLeft( int xm, int ym )
{
    logf( (char*)"PanelGraph::clickLeft( %d, %d )", xm, ym );

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::motionLeft( int xm, int ym )
{
    //logf( (char*)"PanelGraph::motionLeft( %d, %d )", xm, ym );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::releaseLeft( int xm, int ym )
{
    logf( (char*)"PanelGraph::releaseLeft( %d, %d )", xm, ym );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::clickMiddle( int xm, int ym )
{
    //logf( (char*)"PanelGraph::clickMiddle( %d, %d )", xm, ym );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::motionMiddle( int xm, int ym )
{
    //logf( (char*)"PanelGraph::motionMiddle( %d, %d )", sc2winX(xm), sc2winY(ym) );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::releaseMiddle( int xm, int ym )
{
    logf( (char*)"PanelGraph::releaseMiddle( %d, %d )", xm, ym );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
*/
#endif


