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
	
	for( int i=0; i<tAbsPanel.size(); i++ )
	{
		delete tAbsPanel[i];
	}
	tAbsPanel.clear();
	
	for( int i=0; i<tOrdPanel.size(); i++ )
	{
		delete tOrdPanel[i];
	}
	tOrdPanel.clear();
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
	pTitre = new PanelText( (char*)"",  PanelText::NORMAL_FONT, 10, 10  );
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

	bLogX = true;
	bLogY = false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::graph2panelX( double& __x )
{
	__x = log10(__x ) * _pasX;
	__x = __x - _debX + DELTA_AXE_X;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::graph2panelY( double& __y )
{
	__y = __y * _pasY;
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
	
	for( int i=1; i<cVizi.size(); i++ )
	{
		vec2 v1 = cVizi[i];
		vec2 v0 = cVizi[i-1];
		
		graph2panel(v0);
		graph2panel(v1);

		vec2 w = v1-v0;

		glLine( v0, w );
		glCroix( v0, vec2(6, 6) );
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::displayCourbeStar()
{
	if( bNuit )						glColor4fv( (GLfloat*)&cRouge );
	else							glColor4fv( (GLfloat*)&cVert );

	double _DY = getPosDY();
	
	for( int i=1; i<cStar.size(); i++ )
	{
		vec2 v1 = cStar[i];
		vec2 v0 = cStar[i-1];
		
		graph2panel(v0);
		graph2panel(v1);
		
		vec2 w = v1-v0;
		
		glLine( v0, w );
		glCroix( v0, vec2(6, 6) );
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::ajoute_ord(double val, vec2 pos)
{
	char	str[21];
	vec2 v = pos;
	graph2panel( v );
	
	
	snprintf( (char*)str, sizeof(str), "%d", (int)val );
	PanelText* p = new PanelText( (char*)str,  PanelText::NORMAL_FONT, v.x+4-DELTA_AXE_X, v.y-8  );
	add( p );
	//log( (char*)"PanelText" );
	tOrdPanel.push_back(p);
	tOrdVec2.push_back(pos);
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
	//double dx = p->getPosDX()/2; 
	//log( (char*)"PanelText" );
	tAbsPanel.push_back(p);
	tAbsVec2.push_back(pos);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::update_ord()
{
	if ( tOrdPanel.size() == 0 )			return;

	for( int i=0; i<tOrdPanel.size(); i++ )
	{
		vec2 v = tOrdVec2[i];
		graph2panel(v);
		int dd = tOrdPanel[i]->getPosDX() + 4;
		
		tOrdPanel[i]->setPos( v.x-dd, v.y-8 );
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
void PanelGraph::displayAxeV()
{
	
	if( bNuit )						glColor4fv( (GLfloat*)&cRougeC );
	else							glColor4fv( (GLfloat*)&cGris );

	double	_ldeb = floor( log10(dXmin) ); 
	double	_log =  pow( 10.0, _ldeb );
	if ( _log <= 1.0 )		_log = 1.0;
	
	double  _dif = log10(dXmax) - log10(dXmin);
	_pasX = ( 1.0 /  _dif ) * ((double)getPosDX()-DELTA_AXE_X);
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

		glLine(v, vec2(0.0, getPosDY()-DELTA_AXE_Y) );
		
		if ( b && i==1.0 )			ajoute_abs( _x, vec2(_x, dYmin) );
		if ( b && i==5.0 )			ajoute_abs( _x, vec2(_x, dYmin) );
		if ( b && i==2.0 )			ajoute_abs( _x, vec2(_x, dYmin) );

	}
	
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::displayAxeH()
{

	if( bNuit )						glColor4fv( (GLfloat*)&cRougeC );
	else							glColor4fv( (GLfloat*)&cGris );

	double  _dif = dYmax - dYmin;
	_pasY = ( 1.0 /  _dif ) * ((double)getPosDY()-DELTA_AXE_Y);
	_debY = dYmin * _pasY;
	_finY = dYmax * _pasY;	
	
	double fM = dYmax;	graph2panelY( fM );
	double fm = dYmin;	graph2panelY( fm );

	bool b = false;
	if ( tOrdPanel.size() == 0)		b = true;
	
	for( double i=1.0; i<=30.0; i+=1.0 )
	{
		vec2 v = vec2( dXmin, i );
		graph2panel( v );
				//logf( (char*)"%lf  %lf<%lf", v.y, fm, fM );
		if ( v.y > fm )	continue;
		if ( v.y < fM )	break;

		glLine(  v, vec2(getPosDX()-DELTA_AXE_X, 0.0) );
		if ( b )			ajoute_ord( i, vec2(dXmin,i) );
	}
	
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

	for( int i=0; i<tAbsPanel.size(); i++ )		sup( tAbsPanel[i] );
	for( int i=0; i<tOrdPanel.size(); i++ )		sup( tOrdPanel[i] );

	for( int i=0; i<tAbsPanel.size(); i++ )		delete tAbsPanel[i];
	for( int i=0; i<tOrdPanel.size(); i++ )		delete tOrdPanel[i];

    tAbsPanel.clear();
    tAbsVec2.clear();
    tOrdPanel.clear();
    tOrdVec2.clear();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::resetCourbeVizi()
{
    cVizi.clear();

	for( int i=0; i<tAbsPanel.size(); i++ )		sup( tAbsPanel[i] );
	for( int i=0; i<tOrdPanel.size(); i++ )		sup( tOrdPanel[i] );

	for( int i=0; i<tAbsPanel.size(); i++ )		delete tAbsPanel[i];
	for( int i=0; i<tOrdPanel.size(); i++ )		delete tOrdPanel[i];

    tAbsPanel.clear();
    tAbsVec2.clear();
    tOrdPanel.clear();
    tOrdVec2.clear();
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
void PanelGraph::addStar( vec2 v )
{	
	if ( v.x < 10.0 )				v.x = 10.0;
	if ( doublons_star(v) )			return;

	//logf( (char*)"Star ajout (%0.4lf, %0.4lf", v.x, v.y );
    cStar.push_back(v);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::addVizi( vec2 v )
{
	if ( v.x < 10.0 )				v.x = 10.0;
	if ( doublons_vizi(v) )			return;

	//logf( (char*)"Vizi ajout (%0.4lf, %0.4lf", v.x, v.y );
    cVizi.push_back(v);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool cmp(vec2 v, vec2 w)
{
    return v.x > w.x;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelGraph::sort_all()
{
    sort(cVizi.begin(), cVizi.end(), cmp );
    sort(cStar.begin(), cStar.end(), cmp );
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
#endif


