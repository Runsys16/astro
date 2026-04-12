#ifndef FIND_STAR_CPP
#define FIND_STAR_CPP
//--------------------------------------------------------------------------------------------------------------------
#define OFFSET(__x, __y)		((int__x+(int)__y*pRB->w)
//--------------------------------------------------------------------------------------------------------------------
#include "find_star.h"
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
FindStar::FindStar()
{
	log( (char*)"FindStar::FindStar()" );
	WindowsManager&     wm  = WindowsManager::getInstance();
	
	pGraphDistri	= NULL;
	create_graph_distri();
	pGraphLum		= NULL;
	create_graph_lum();

	pRB				= NULL;
	pView			= NULL;
	pConvert		= NULL;
	pInfo			= NULL;
	//thFindStar		= -1;

	dOffsetLow		= 5.0;
	max_lum			= 0.0;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
FindStar::~FindStar()
{
	log( (char*)"FindStar::~FindStar()" );

	WindowsManager&     wm  = WindowsManager::getInstance();

	if ( pGraphDistri )
	{
		wm.sup(pGraphDistri);
		delete pGraphDistri;
	}
	
	if ( pGraphLum )
	{
		wm.sup(pGraphLum);
		delete pGraphLum;
	}
	
	if ( pInfo )
	{
		wm.sup(pInfo);
		delete pInfo;
	}
	
	pRB				= NULL;
	pView			= NULL;
	pConvert		= NULL;
	pGraphDistri	= NULL;
	pGraphLum		= NULL;
	pInfo			= NULL;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FindStar::create_graph_lum()
{
	if( pGraphLum != NULL )		return;

	log( (char*)"FindStar::create_graph_lum()" );
	log_tab(true);

	WindowsManager&     wm  = WindowsManager::getInstance();
	
	pGraphLum = new PanelGraph();
	
	pGraphLum->setExtraString( "pGraph Etoiles" );
	pGraphLum->setVisible( false );
	pGraphLum->setPosAndSize( 10, 10, width/2 -40, height/2 -40 );

	wm.add( pGraphLum );

	log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FindStar::create_graph_distri()
{
	if( pGraphDistri != NULL )		return;

	log( (char*)"FindStar::create_graph_distri()" );
	log_tab(true);

	WindowsManager&     wm  = WindowsManager::getInstance();
	
	pGraphDistri = new PanelGraph();
	
	pGraphDistri->setExtraString( "pGraph distribution" );
	pGraphDistri->setPosAndSize( 10, 10, width/2 -40, height/2 -40 );
	pGraphDistri->setVisible( false );

	wm.add( pGraphDistri );

	log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FindStar::create_info()
{
	if( pInfo != NULL )		return;

	log( (char*)"FindStar::create_info()" );
	log_tab(true);

	WindowsManager&     wm  = WindowsManager::getInstance();
	
	pInfo = new PanelDebug();
	
	pInfo->setExtraString( "pInfo findStar" );
	pInfo->loadSkin( PanelWindow::BLACK );
	pInfo->setBorderSize(2);
	pInfo->setSize( 160, 2 );
	pInfo->setTabSize( 70 );
	wm.add( pInfo );

	log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
double FindStar::get_li_low_lvl( int li)
{
	//logf( (char*)"FindStar::get_limite(%d)", li );
	
	if ( pRB == NULL )		{ log( (char*)"[ Erreur ] pRG = NULL" ); return 0.0; }
	
	int 	distri[256];
	double 	imax	= 0;
	int 	vmax	= 0, j;
	int 	offset	= getOffset( 0, li );

	memset( distri, 0, sizeof(distri) );
	
	for( int co=0; co<pRB->w; co++ )
	{
		int o = offset + co*pRB->d;
		double l = getLum(o);
		
		if ( l >= 255.0 )		l = 255.0;
		
		j = (int)l;
		distri[j]++;
		
		if ( distri[j] > vmax )		{ vmax = distri[j]; imax = j; } 
	}
	
	imax += dOffsetLow;
	//logf( (char*)"Lum = %0.4lf", imax );
	return imax;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
double FindStar::getLum(int offset )
{
    double r;
    double g;
    double b;
    try
    {
        r = ptr[offset+0];
        g = ptr[offset+1];
        b = ptr[offset+2];
    }
    catch ( const std::exception& e )
    {
        std::cout << e.what() << std::endl;
        return -1;
    }

    return  (double)LUM(r,g,b);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
double FindStar::getLum( vec2 v )
{
	if ( 0.0 > v.x || v.x >= pRB->w )			return 0.0;
	if ( 0.0 > v.y || v.y >= pRB->h )			return 0.0;
    int offset = getOffset( v.x, v.y );
    return getLum( offset );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FindStar::get_li_distribution( int li)
{
	//logf( (char*)"FindStar::get_li_distribution(%d)", li );
	
	if ( pRB == NULL )		{ log( (char*)"[ Erreur ] pRG = NULL" ); return; }
	
	int distri[256];
	for( int i=0; i<256; i++ )		distri[i] = 0;
	
	int imax	= 0, 
		vmax	= 0, j;
	int offset	= getOffset( 0, li );
	
	for( int co=0; co<pRB->w; co++ )
	{
		int o = offset + co*pRB->d;
		double l = getLum(o);
		
		//logf( (char*)"Lum = %0.4lf", l );
		if ( l >= 255.0 )		l = 255.0;
		
		j = (int)l;
		distri[j]++;
		
		if ( distri[j] > vmax )		{ vmax = distri[j]; imax = j; } 
	}

	double ratio = 0.0;
	for( int i=0; i<256; i++ )		if ( i < (imax+5) )	ratio += distri[i];

	double pourcent = 100.0 * (double)ratio /( (double)pRB->w);//*(double)pRB->h );//(double)pRB->d );
	//logf( (char*)"vmax = %d    imax = %d   pourcent = %0.2lf\%", vmax, imax, pourcent );
	
	//-----------------------------------------------------------------
	// Affichage de la courbe
	pGraphDistri->resetCourbes();

	pGraphDistri->setXmin(0);
	pGraphDistri->setXmax(255);
	pGraphDistri->setYmin(0);
	pGraphDistri->setYmax(vmax);

	pGraphDistri->setLogX();
	pGraphDistri->setLogY();
	pGraphDistri->setPtStar(PanelGraph::POINT);
	pGraphDistri->setColorStar(cBleuC);

	for( int i=0; i<256; i++ )		pGraphDistri->addStar( vec2(i, distri[i]) );

	pGraphDistri->sort_all();
	//pGraphDistri->setVisible(true);
	char str[128];
	snprintf( str, sizeof(str), "%s - (ligne :%d) %d/%d", name.c_str(), li, imax, vmax );
	pGraphDistri->setName(string(str));

	WindowsManager&     wm  = WindowsManager::getInstance();
	wm.onTop(pGraphDistri);

}
/*
//--------------------------------------------------------------------------------------------------------------------
// Algorithme de tracé de cercle d'Andres
//-----------------------------------------
// 
//--------------------------------------------------------------------------------------------------------------------
public static HashSet<int[]> cercle(int x_centre, int y_centre, int r)
{
    HashSet<int[]> pixels = new HashSet<int[]>();
    
    int x = 0;
    int y = r;
    int d = r - 1;
    
    while(y >= x)
    {
        pixels.add( new int[]{ x_centre + x, y_centre + y });
        pixels.add( new int[]{ x_centre + y, y_centre + x });
        pixels.add( new int[]{ x_centre - x, y_centre + y });
        pixels.add( new int[]{ x_centre - y, y_centre + x });
        pixels.add( new int[]{ x_centre + x, y_centre - y });
        pixels.add( new int[]{ x_centre + y, y_centre - x });
        pixels.add( new int[]{ x_centre - x, y_centre - y });
        pixels.add( new int[]{ x_centre - y, y_centre - x });
        
        if (d >= 2*x)
        {
            d -= 2*x + 1;
            x ++;
        }
        else if (d < 2 * (r-y))
        {
            d += 2*y - 1;
            y --;
        }
        else
        {
            d += 2*(y - x - 1);
            y --;
            x ++;
        }
    }
    
    return pixels;
}
*/
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FindStar::recentre_etoile(vec2& v, int& rayon)
{
	int x = 0;
	int r = rayon;
	int y = r;
	int d = r - 1;
	
	double l = 0.0;
	int t=0;
	
	while(y >= x)
	{
		l += getLum( v + vec2(+x,+y) );
		l += getLum( v + vec2(+y,+x) );
		
		l += getLum( v + vec2(-x,+y) );
		l += getLum( v + vec2(+x,-y) );
		
		l += getLum( v + vec2(-y,+x) );
		l += getLum( v + vec2(+y,-x) );
		
		l += getLum( v + vec2(-x,-y) );
		l += getLum( v + vec2(-y,-x) );
		
		if (d >= 2*x)
		{
		    d -= 2*x + 1;
		    x ++;
		    t = 1;
		}
		else if (d < 2 * (r-y))
		{
		    d += 2*y - 1;
		    y --;
		    t = 2;
		}
		else
		{
		    d += 2*(y - x - 1);
		    y --;
		    x ++;
		    t = 3;
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FindStar::add_star(vec2 v, double rayon)
{
	struct etoile_line e;

	e.centre	= v;
	e.deb		= v - vec2(0.0,0.0);
	e.fin		= v + vec2(0.0,0.0);
	e.lum		= -9999.0;
	e.li		= v.y;
	e.rayon		= 0;

	muStar.lock();
	tStar.push_back(e);
	muStar.unlock();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FindStar::get_cercle_n(vec2 v, double rayon)
{
	int xold, yold;
	int x = 0;
	int r = rayon;
	int y = r;
	int d = r - 1;
	
	double l = 0.0;
	
	xold = x, yold = y;
	while(y >= x)
	{
		l += getLum( v + vec2(+x,+y) );
		l += getLum( v + vec2(+x,-y) );

		l += getLum( v + vec2(-x,+y) );
		l += getLum( v + vec2(-x,-y) );

		l += getLum( v + vec2(+y,+x) );
		l += getLum( v + vec2(+y,-x) );

		l += getLum( v + vec2(-y,+x) );
		l += getLum( v + vec2(-y,-x) );

		xold = x, yold = y;
		
		if (d >= 2*x)
		{
		    d -= 2*x + 1;
		    x ++;
		}
		else if (d < 2 * (r-y))
		{
		    d += 2*y - 1;
		    y --;
		}
		else
		{
		    d += 2*(y - x - 1);
		    y --;
		    x ++;
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FindStar::get_centre_n(vec2 v, double rayon)
{
	int x = 0;
	int r = rayon;
	int y = r;
	int d = r - 1;
	
	double l = 0.0;
	
	while(y >= x)
	{
		add_star( v + vec2(+x,+y), rayon );
		add_star( v + vec2(+y,+x), rayon );
		add_star( v + vec2(-x,+y), rayon );
		add_star( v + vec2(+x,-y), rayon );
		add_star( v + vec2(-y,+x), rayon );
		add_star( v + vec2(+y,-x), rayon );
		add_star( v + vec2(-x,-y), rayon );
		add_star( v + vec2(-y,-x), rayon );
		
		if (d >= 2*x)
		{
		    d -= 2*x + 1;
		    x ++;
		}
		else if (d < 2 * (r-y))
		{
		    d += 2*y - 1;
		    y --;
		}
		else
		{
		    d += 2*(y - x - 1);
		    y --;
		    x ++;
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FindStar::get_max(vec2 v, vec2& vMax, double& lMax, double rayon)
{
	int x = 0;
	int r = rayon;
	int y = r;
	int d = r - 1;
	
	double l = 0.0;
	vec2 vTest;
	
	while(y >= x)
	{
		vTest = v + vec2(+x,+y);
		l = getLum(vTest);
		if ( l > lMax  )			{ vMax = vTest; lMax = l; }
		
		vTest = v + vec2(+x,-y);
		l = getLum(vTest);
		if ( l > lMax  )			{ vMax = vTest; lMax = l; }
		
		vTest = v + vec2(-x,+y);
		l = getLum(vTest);
		if ( l > lMax  )			{ vMax = vTest; lMax = l; }
		
		vTest = v + vec2(-x,-y);
		l = getLum(vTest);
		if ( l > lMax  )			{ vMax = vTest; lMax = l; }
		

		vTest = v + vec2(+y,+x);
		l = getLum(vTest);
		if ( l > lMax  )			{ vMax = vTest; lMax = l; }
		
		vTest = v + vec2(+y,-x);
		l = getLum(vTest);
		if ( l > lMax  )			{ vMax = vTest; lMax = l; }

		vTest = v + vec2(-y,+x);
		l = getLum(vTest);
		if ( l > lMax  )			{ vMax = vTest; lMax = l; }
		
		vTest = v + vec2(-y,-x);
		l = getLum(vTest);
		if ( l > lMax  )			{ vMax = vTest; lMax = l; }
		
		if (d >= 2*x)
		{
		    d -= 2*x + 1;
		    x ++;
		}
		else if (d < 2 * (r-y))
		{
		    d += 2*y - 1;
		    y --;
		}
		else
		{
		    d += 2*(y - x - 1);
		    y --;
		    x ++;
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
double FindStar::get_lum_cercle(vec2 v, double rayon)
{
	//logf( (char*)"FindStar::get_lum_cercle( " VEC2_PRINTFN(0) ", %d", VEC2_AFF(v), (int)rayon );
	log_tab(true);
	
	int x = 0;
	int y = rayon;
	int d = rayon - 1;
	double l = 0.0, ll = 0.0;
	int n = 0;
	
	vec2 V;
	
	while(y >= x)
	{
		V = v + vec2(+x,+y);
		l = getLum(V);
		ll += (l>max_lum ? l : 0.0 );
		
		V = v + vec2(+x,-y);
		l = getLum(V);
		ll += (l>max_lum ? l : 0.0 );
		
		V = v + vec2(-x,+y);
		l = getLum(V);
		ll += (l>max_lum ? l : 0.0 );
		
		V = v + vec2(-x,-y);
		l = getLum(V);
		ll += (l>max_lum ? l : 0.0 );

		V = v + vec2(+y,+x);
		l = getLum(V);
		ll += (l>max_lum ? l : 0.0 );
		
		V = v + vec2(+y,-x);
		l = getLum(V);
		ll += (l>max_lum ? l : 0.0 );

		V = v + vec2(-y,+x);
		l = getLum(V);
		ll += (l>max_lum ? l : 0.0 );
		
		V = v + vec2(-y,-x);
		l = getLum(V);
		ll += (l>max_lum ? l : 0.0 );

		n += 8;
		//logf( (char*)"x,y=" VEC2_PRINTFN(0) ", l=%0.2lf,  n=%d", VEC2_AFF (vec2(x,y)), l, n );
		
		if (d >= 2*x)
		{
		    d -= 2*x + 1;
		    x ++;
		}
		else if (d < 2 * (rayon-y))
		{
		    d += 2*y - 1;
		    y --;
		}
		else
		{
		    d += 2*(y - x - 1);
		    y --;
		    x ++;
		}
	}

	log_tab(false);
	return ll;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int FindStar::exist(vec2 v, double r)
{

	for( int i=tStar.size()-1; i>=0; i-- )
	{
		if ( v.y > (tStar[i].centre.y+80) ) 	break;
		
		vec2 v0 = vec2(tStar[i].centre) - v;
		//logf( (char*)"compare %0.1lf/%0.1lf " VEC2_PRINTFN(0) VEC2_PRINTFN(0), v0.length(), tStar[i].rayon+r, VEC2_AFF(v), VEC2_AFF(tStar[i].centre) );
		if ( v0.length() <= (tStar[i].rayon+r) )	return i; 
	}
	return -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FindStar::find_star_2(vec2 v)
{
	//logf( (char*)"FindStar::find_star_2" VEC2_PRINTFN(0), VEC2_AFF(v) );
	log_tab(true);

	//double	lum_min 	= get_li_low_lvl( (int)v.y );
	//max_lum = lum_min;
	vec2 	vMax, V;
	double	rayon, r, l, lMax;
	
	lMax = getLum(v);
	bool	bOk = false;
	
	for( r=1.0; r<20.0; r+=1.0 )
	{
		l = 0.0;
		get_max( v, V, l, r );
		
		if ( l > lMax )			{ lMax = l; vMax = V; rayon = r; bOk = true; }
		if ( l < (lMax-5.0) )	break;
	}
	
	if ( !bOk )		{ log_tab(false); return; }
	//if ( r == 20.0 and l <= 5.0 )	{ log( (char*)"Pas d'eoitle" ); log_tab(false); return; }
	//logf( (char*)"r=%.0lf  l=%0.2lf/%0.2lf " VEC2_PRINTFN(0), r, l, lMax, VEC2_AFF(vMax) );
	
	double	l1	= 0.0;
	double	l0	= 0.0;
	#define MAX_RAYON	40
	int		rr	= 0;
	l = 0.0;
	vMax = vec2( (int)vMax.x, (int)vMax.y );
	
	for ( rr=1; rr<MAX_RAYON; rr++ )
	{
		r = rr;
		double l0 = get_lum_cercle( vMax, (double)rr );

		//logf( (char*)"Etoile " VEC2_PRINTFN(0) " l0=%0.2lf  l1=%0.2lf r=%d", VEC2_AFF(vMax), l0, l1, rr );

		l += l0;
		
		if ( round(l0/(r*100.0)) == round(l1/(r*100.0)) )			break;
		l1 = l0;
		
	}
	if ( rr <= 2 )	{ log_tab(false); return; }

	//logf( (char*)"Etoile " VEC2_PRINTFN(0) " lum=%0.2lf r=%d", VEC2_AFF(vMax), l, rr );
	int idx;
	if ( idx=exist(vMax, rr) == -1 )
	{
		struct etoile_line e;

		e.centre	= vMax;
		e.deb		= vMax - vec2(0.0,0.0);
		e.fin		= vMax + vec2(0.0,0.0);
		e.lum		= l;
		e.li		= vMax.y;
		e.rayon		= rr;
		e.deb.x		= -1.0;
		muStar.lock();
		tStar.push_back(e);
		muStar.unlock();
	}
	else
	{
		if ( rr > tStar[idx].rayon  )
		{
			//logf( (char*)"Etoile " VEC2_PRINTFN(0) "  \tlum=%0.2lf r=%d  \tidx=%d", VEC2_AFF(vMax), l, rr, idx );
			tStar[idx].centre	= vMax;
			tStar[idx].deb		= vMax - vec2(0.0,0.0);
			tStar[idx].fin		= vMax + vec2(0.0,0.0);
			tStar[idx].lum		= l;
			tStar[idx].li		= vMax.y;
			tStar[idx].rayon	= rr;
			tStar[idx].deb.x	= -1.0;
		}
	}
	log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FindStar::find_stars_2( vec2 v )
{
	if ( pRB == NULL )				return;
	
	muFindStar.lock();

	//------------------------------------------------------
	for( int y=0; y<pRB->h; y+=5 )
	{
		max_lum = get_li_low_lvl( (int)y );
		
		for( int x=0; x<pRB->w; x+=5 )
		{
			//find_star_2(vec2(x, v.y));
			find_star_2(vec2(x, y));
		}
	}
	//thFindStar = NULL;
	muFindStar.unlock();
	logf_thread( (char*)"%d etoile(s)", tStar.size() );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FindStar::reset_local()
{
	local_min	= 256.0;
	local_max	= 0.0;
	local_dif_p	= 0.0;
	local_dif_m	= 0.0;
	local_dif	= 0.0;
	local_lum 	= 0.0;
	local_centre= 0.0;
	local_nb	= 0;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FindStar::print_local( int x )
{
	logf( (char*)"%04d-l=%0.1lf\t dif %0.1lf (%d, %d)  \tdebfin(%d, %d)", x, local_l, local_dif,  (int)local_dif_p, (int)local_dif_m, (int)local_deb, (int)local_fin );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FindStar::find_line( int li, bool bAff)
{
	if ( pRB == NULL )				return;
	//------------------------------------------------------
	double lum_min = get_li_low_lvl( li );
	double dif;
	double prev = 0.0;
	reset_local();
	
	for( int x=0; x<pRB->w; x++ )
	{
		int offset	= getOffset( x, li );
		double l	= getLum(offset);
		local_l		= l;
		vec2 v, vMin, vMax;
		
		if ( l > lum_min )		
		{
			if ( prev == 0.0 )		local_deb = x;
			local_fin = x;

			if ( l<local_min )		local_min = l;
			if ( l>local_max )		local_max = l;
			
			dif = l - local_min;
			if ( dif > local_dif_p )	local_dif_p = dif;

			dif = l - local_max;
			if ( dif < local_dif_m )	local_dif_m = dif;
			
			if ( prev != 0.0 )		local_dif = l - prev;			

			
			if ( bAff )
			{
				v		= vec2(x, li);
				vMin	= vec2(pView->getPosX(), pView->getPosY() );
				vMax	= vMin + vec2(pView->getPosDX(), pView->getPosDY() );

				pConvert->tex_2_screen(v);
				
				if ( vMin.x<v.x && v.x<vMax.x && vMin.y<v.y && v.y<vMax.y )
				{
					//logf( (char*)"v" VEC2_PRINTFN() " vMin" VEC2_PRINTFN() " vMax" VEC2_PRINTFN() , VEC2_AFF(v), VEC2_AFF(vMin), VEC2_AFF(vMax) );
					if ( prev == 0.0 )		log( (char*)"-----" );
					print_local(x);
				}				
			}
			prev = l;
			local_nb++;
			local_lum += l;
			local_centre += x * l;

			//if ( (x-local_deb) > 0.5*local_dif_p )	{ reset_local(); prev = 0.0; }
		}
		else
		{
			if ( (local_max-local_min) > 5.0 )
			{
				//local_fin = x+1;
				struct etoile_line e;
				//e.centre	= vec2(x-local_nb/2-1, li);
				e.centre	= vec2(local_centre/local_lum, li);
				e.deb		= vec2(local_deb, li);
				e.fin		= vec2(local_fin, li);
				e.lum		= local_lum;
				e.li		= li;
				e.rayon		= 0;

				muStar.lock();
				tStar.push_back(e);
				muStar.unlock();
				if ( bAff )
				{
					v		= vec2(x, li);
					vMin	= vec2(pView->getPosX(), pView->getPosY() );
					vMax	= vMin + vec2(pView->getPosDX(), pView->getPosDY() );

					pConvert->tex_2_screen(v);
					
					if ( vMin.x<v.x && v.x<vMax.x && vMin.y<v.y && v.y<vMax.y )
					{
						logf( (char*)"*** ETOILE *** " );
						logf( (char*)"\tlum=%0.2lf, nb=%d", local_lum, local_nb );
						logf( (char*)"\tmin=%0.2lf max=%0.2lf dif_p=%0.2lf dif_m=%0.2lf ", local_min, local_max, local_dif_p, local_dif_m );
						logf( (char*)"\tlum_min=%0.2lf vStar" VEC2_PRINTFN(1) "", lum_min, VEC2_AFF(e.centre) );
					}
				}
				/*
				*/
				 				
			}
			prev = 0.0;
			reset_local();
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FindStar::update_graph_segment()
{
	
	double ech	= pConvert->get_echelle();
	
	int xMin	= 9999;
	int	xMax	= 0;
	int yMin	= 9999;
	int yMax	= 0;
	int li;

	pGraphLum->resetCourbes();
	
	for(int j=0; j<tStar.size(); j++ )
	{
		struct etoile_line& e = tStar[j];
		li = e.li;
		
		for( int i=e.deb.x; i<=e.fin.x; i++ )
		{
			vec2 v = vec2(i, e.deb.y ) + vec2(0.5, 0.5);	// centre du pixel
			pConvert->tex_2_screen(v);
			
			vec2 vMin, vMax;
			vMin	= vec2(pView->getPosX(), pView->getPosY() );
			vMax	= vMin + vec2(pView->getPosDX(), pView->getPosDY() );

			if ( vMin.x<v.x && v.x<vMax.x && vMin.y<v.y && v.y<vMax.y )
			{
				int offset	= getOffset( i, e.deb.y );
				double l	= getLum(offset);
				//double l	= getLum(offset) - getLum(offset - pRB->d);

				//logf( (char*) "" VEC2_PRINTFN(1), VEC2_AFF( vec2(v.x, l) ) );
				v = vec2(i, e.deb.y );// + vec2(0.5, 0.5);	// centre du pixel
				pGraphLum->addStar( vec2(v.x, l) );

				if ( v.x < xMin )		xMin = v.x;
				if ( v.x > xMax )		xMax = v.x;
				if ( l < yMin )			yMin = l;
				if ( l > yMax )			yMax = l;
			}
		}
	}

	logf( (char*) " Xminmax" VEC2_PRINTFN(1) " Yminmax" VEC2_PRINTFN(1), VEC2_AFF( vec2(xMin, xMax) ), VEC2_AFF( vec2(yMin, yMax) ) );
	//-----------------------------------------------------------------
	// Affichage de la courbe
	//pGraphLum->resetCourbes();

	pGraphLum->setXmin(xMin);
	pGraphLum->setXmax(xMax);
	pGraphLum->setYmin(yMin);
	pGraphLum->setYmax(yMax);

	pGraphLum->setLinearX();
	pGraphLum->setLinearY();
	pGraphLum->setPtStar(PanelGraph::POINT);
	pGraphLum->setColorStar(cMagentaC);


	pGraphLum->sort_all();
	pGraphLum->setVisible(true);
	char str[128];
	snprintf( str, sizeof(str), "%s - etoile(s) %d", name.c_str(), li );
	pGraphLum->setName(string(str));

	WindowsManager&     wm  = WindowsManager::getInstance();
	wm.onTop(pGraphLum);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FindStar::click_find_star( vec2 v)
{
	logf( (char*)"FindStar::click_graph_distri( " VEC2_PRINTFN(0) " )", VEC2_AFF(v) );
	log_tab(true);
	
	if ( pRB == NULL )		{ log( (char*)"[ Erreur ] pRG = NULL" ); return; }
	
	if ( tStar.size() != 0 )
	{
		tStar.clear();
	}
	else
	{
		//find_stars_2(v);
        thFindStar = thread(&FindStar::find_stars_2, this, v); 
        thFindStar.detach();
        log_thread( (char*)"[thread] click_find_star() !! " );
    }
	
	
	logf( (char*)"%d etoile(s)", tStar.size() );
		
	log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FindStar::click_graph_distri( vec2 v)
{
	logf( (char*)"FindStar::click_graph_distri( " VEC2_PRINTFN(0) " )", VEC2_AFF(v) );
	log_tab(true);
	
	if ( pRB == NULL )		{ log( (char*)"[ Erreur ] pRG = NULL" ); return; }

	if ( pGraphDistri->getVisible() )
	{
		pGraphDistri->setVisible(false);
		pGraphLum->setVisible(false);
		tStar.clear();
	}	
	else
	{
		
		pGraphDistri->setVisible(true);
		get_li_distribution( (int)v.y );

		WindowsManager&     wm  = WindowsManager::getInstance();
		wm.onTop(pGraphLum);
		wm.onTop(pGraphDistri);
	}

	log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FindStar::click_graph_lum( vec2 v)
{
	logf( (char*)"FindStar::click_graph_lum( " VEC2_PRINTFN(0) " )", VEC2_AFF(v) );
	log_tab(true);

	if ( pRB == NULL )		{ log( (char*)"[ Erreur ] pRG = NULL" ); return; }

	tStar.clear();
	
	find_line( (int)v.y, true );
	
	if ( pGraphDistri->getVisible() )		WindowsManager::getInstance().onTop(pGraphDistri);
	
	update_graph_segment();

	logf( (char*)"|  %d etoiles trouvées", tStar.size() );
	log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FindStar::click_all()
{
	log( (char*)"FindStar::click_all()" );
	if ( pRB == NULL )		{ log( (char*)"[ Erreur ] pRG = NULL" ); return; }

	tStar.clear();
	for ( int y=0; y<pRB->h; y++ )		find_line( y, false );

	if ( pGraphDistri->getVisible() )		WindowsManager::getInstance().onTop(pGraphDistri);

	logf( (char*)"|  %d etoiles trouvées", tStar.size() );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FindStar::glCroix( int x,  int y,  int dx,  int dy )
{
	glBegin(GL_LINES);

	    glVertex2i(x, y-dy);         glVertex2i(x, y+dy);
	    glVertex2i(x-dx, y);         glVertex2i(x+dx, y);

    glEnd();        
    	
    //logf( (char*)"glCroix" IVEC2_PRINTF "  --- " IVEC2_PRINTF , IVEC2_AFF(ivec2(x,y)), IVEC2_AFF(ivec2(dx,dy)) );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FindStar::glCroix( int x,  int y,  int d )
{
	glCroix( x, y, d, d );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FindStar::glCercle( vec2 v, double rayon)
{
    double step = 80.0/(double)rayon;
    if ( step < 1.0 )           step = 1.0;
    
    //logf ( (char*)" rayon %d   step %0.2f", rayon, step );
    
	glBegin(GL_LINE_LOOP);

        for( double i=0; i<=360.0; i+=step )
        {
            double fx = (double)v.x+ (double)rayon*cos(DEG2RAD(i));
            double fy = (double)v.y+ (double)rayon*sin(DEG2RAD(i));
            glVertex2i(fx,fy);
        }
        
    glEnd();        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FindStar::display_segment(struct etoile_line& e )
{
	double ech = pConvert->get_echelle();

	for( int i=e.deb.x; i<=e.fin.x; i++ )
	{
		vec2 v = vec2(i, e.deb.y ) + vec2(0.5, 0.5);	// centre du pixel
		pConvert->tex_2_screen(v);
		
		vec2 vMin, vMax;
		vMin	= vec2(pView->getPosX(), pView->getPosY() );
		vMax	= vMin + vec2(pView->getPosDX(), pView->getPosDY() );

		if ( vMin.x<v.x && v.x<vMax.x && vMin.y<v.y && v.y<vMax.y )
		{
			glCercle(v, ech*0.5);
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FindStar::displayGL()
{
	if ( !bAffFindStar )				return;
	if ( tStar.size() == 0 )			return;
	if ( pConvert == NULL )				return;
	
	double ech = pConvert->get_echelle();

	muStar.lock();
	for( int i=0; i<tStar.size(); i++ )
	{
		vec2 vMin, vMax, v;
		v		= tStar[i].centre + vec2(0.5, 0.5);	// centre du pixel
		vMin	= vec2(pView->getPosX(), pView->getPosY() );
		vMax	= vMin + vec2(pView->getPosDX(), pView->getPosDY() );

		pConvert->tex_2_screen(v);
		if ( !(vMin.x<v.x && v.x<vMax.x && vMin.y<v.y && v.y<vMax.y) )		continue;

		glColor4fv( (GLfloat*)&cBleuC );
		if ( tStar[i].deb.x != -1.0 )		display_segment( tStar[i] );

		if ( tStar[i].rayon != 0.0	)
		{
			glColor4fv( (GLfloat*)&cBleuC );
			glCercle(v, ech * tStar[i].rayon );
			glCroix((int)v.x, (int)v.y, (int)(ech * tStar[i].rayon * 2.0) );
		}

		if ( tStar[i].lum<0.0 )												continue;
		if ( tStar[i].deb.x==-1.0 )											continue;

		//continue;		
		glColor4fv( (GLfloat*)&cVert );
		glCercle(v, ech * 2.0);
		glCroix((int)v.x, (int)v.y, (int)(ech*4.0), (int)(ech*4.0));
	}
	muStar.unlock();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void FindStar::idle()
{
	if ( pInfo )				pInfo->setVisible(false);
	if ( tStar.size() == 0 )	return;
	
	if ( bAffFindStar  )
	{
		for( int i=0; i<tStar.size(); i++ )
		{
			vec2 v = tStar[i].centre;
			pConvert->tex_2_screen(v);
			v -= vec2(vMouse.x, vMouse.y);
			
			if ( v.length() < 20.0 )
			{
				if ( pInfo == NULL )		create_info();
				
				pInfo->onTop();
				pInfo->reset_list();
				pInfo->setVisible(true);
				WindowsManager::getInstance().onTop(pInfo);
				
				pInfo->add_textf( (char*)"          IDX %d", i );
				pInfo->add_text(  (char*)"------------------------------" );
				pInfo->add_textf( (char*)"Centre\t: " VEC2_PRINTFN(0), VEC2_AFF(tStar[i].centre) );
				pInfo->add_textf( (char*)"Deb\t: " VEC2_PRINTFN(0), VEC2_AFF(tStar[i].deb) );
				pInfo->add_textf( (char*)"Fin\t: " VEC2_PRINTFN(0), VEC2_AFF(tStar[i].fin) );
				pInfo->add_textf( (char*)"Li   \t: %d", tStar[i].li );
				pInfo->add_textf( (char*)"Rayon\t: %d", tStar[i].rayon );
				pInfo->add_textf( (char*)"Lum\t: %0.2lf", tStar[i].lum );
				pInfo->setColorAll( VCF4_2_COLOR32(cBleuC) );
				pInfo->setPos( vMouse.x+20, vMouse.y+20 );
				break;	
			}
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------


