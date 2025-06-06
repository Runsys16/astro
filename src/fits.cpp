#include "fits.h"
//--------------------------------------------------------------------------------------------------------------------
//
// Lecture des fichiers FITS (Flexible Image Transport System)
//		https://www.aanda.org/articles/aa/full_html/2010/16/aa15362-10/aa15362-10.html
//		https://fits.gsfc.nasa.gov/standard30/fits_standard30.pdf
//
//--------------------------------------------------------------------------------------------------------------------
Fits::Fits(string filename, PanelCapture* p)
{
    logf((char*)"Constructeur Fits::Fits() -------------" );
    log_tab(true);
    
    readBgr.ptr = NULL;
    _filename = filename;
    pPanelCapture = p;
    
    bValid = false;
    bFlip = true;

    dCDELT1 =  1.0;
    dCDELT2 =  1.0;

    dCRPIX1 = -1.0;
    dCRPIX2 = -1.0;

    dCRVAL1 =  0.0;
    dCRVAL2 =  0.0;

    dCD1_1  = -1.0;
    dCD1_2  = -1.0;
    dCD2_1  = -1.0;
    dCD2_2  = -1.0;
    dBZERO  = -1.0;

    dBSCALE = -1.0;
    iOFFSET = -1;

    dMin = 999999999999.9;
    dMax = -999999999999.9;

	//------------------------------------------------------------------------
    VarManager& 		var	= VarManager::getInstance();
	WindowsManager&     wm  = WindowsManager::getInstance();



    pPanelCorrectionFits = new PanelCorrectionFits();
    pPanelCorrectionFits->setPos( 150, 150 );
    
    pPanelCorrectionFits->getCDELT1()->set_val( dCDELT1 );
    pPanelCorrectionFits->getCDELT1()->set_pVal( (double*)&dCDELT1 );
    pPanelCorrectionFits->getCDELT2()->set_val( dCDELT2 );
    pPanelCorrectionFits->getCDELT2()->set_pVal( (double*)&dCDELT2 );
	pPanelCorrectionFits->setVisible( var.getb("bAffFitsCorrection") );
    wm.add( pPanelCorrectionFits );

    pPanelFits = new PanelFits();
    pPanelFits->setPosAndSize( 10, 10, 695, 250 );
    pPanelFits->setVisible( false );

	//------------------------------------------------------------------------
    log_tab(false);
    logf((char*)"Constructeur Fits::Fits() -------END------" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Fits::~Fits()
{
    if ( readBgr.ptr != NULL )          free( readBgr.ptr );
    readBgr.ptr = NULL;

	delete pPanelFits;
	delete pPanelCorrectionFits;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::chargeFits()
{
	int n = 0;
	bEOF = false;
	while( !bEOF )    chargeHDU(n++);
    
    chargeTexture(n);
    
    sauveMatrice();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::chargeHDU(int n)
{
    //_filename = filename;

    logf( (char*)"Fits::chargeHDU(%d)", n );

    std::ifstream fichier;
    fichier.open(_filename, std::ios_base::app);

    if ( !fichier ) 
    {
        logf( (char*)"[ERROR]impossble d'ouvrir : '%s'", (char*)_filename.c_str() );
        return;
    }

    fichier.seekg( 0, fichier.end );
    int lenght = fichier.tellg();
    fichier.seekg( (long)n*LENGTH_HDU, fichier.beg );

    char buffer[LENGTH_HDU];
    fichier.read(buffer, LENGTH_HDU);
    
    for( int i=0; i<(LENGTH_HDU/80); i++ )
    {
        string k = "";
        string v = "";
        
		//if (  buffer[i*80+8] != '=' )			logf( (char*)"[WARNING] Mot cle non standart" );;
        
        for( int j=0; j<8; j++ )
        {
            k = k + buffer[i*80+j];
        }
        
      	/*
        if ( i==0 )
        {
            if ( k.find("SIMPLE") == std::string::npos )
            {
                logf( (char*)"[ERREUR] Ce n'est pas un fichier FITS standard 'SIMPLE' non trouve" );
                return;
            }
        }
        */
        
        for( int j=9; j<80; j++ )
        {
            v = v + buffer[i*80+j];
        }

        //logf( (char*)"Lecture de  :             '%s' : '%s'", (char*)k.c_str() ,(char*)v.c_str() );
        logf( (char*)"Fits::chargeHDU()         '%s' : '%s'", (char*)k.c_str() ,(char*)v.c_str() );
        
        
             if ( k.find("BITPIX") == 0 )               readBITPIX(v);
        else if ( k.find("NAXIS")  == 0 )               readNAXIS( k, v );
        else if ( k.find("CR")     == 0 )               readCR( k, v );
        else if ( k.find("CDELT")  == 0 )               readCDELT( k, v );
        else if ( k.find("CD")     == 0 )               readCD( k, v );
        else if ( k.find("PC")     == 0 )               readPC( k, v );
        else if ( k.find("BZERO")  == 0 )               dBZERO = getDouble( v );
        else if ( k.find("BSCALE") == 0 )               dBSCALE = getDouble( v );
        else if ( k.find("OFFSET") == 0 )               iOFFSET = getInt( v );
        //else if ( k.find("SWCREATE") == 0 )
        else if ( k.find("HISTORY") == 0 )				
        {
        	k = "";
	        for( int j=0; j<80; j++ )            k = k + buffer[i*80+j];
	        v = "";
        	//readHistory( k );
        }
        else if ( v.find("ASILive") != -1 )
    	{ 
    		log( (char*)"Image ASI" ); 
    		bFlip = false; 
    	}
        else if ( v.find("oacapture") != -1 )
    	{ 
    		log( (char*)"Image oacapture" ); 
    		bFlip = false; 
    	}
        
        row r;
        r.key = k;
        r.value = v;
        datas.push_back( r );
        
        pPanelFits->add_key_value( k, v);
        
        if ( k.find("END") == 0 )       { bEOF = true; break; }
    }

    fichier.close();
    
	//afficheDic();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool Fits::intersection( vec2&r, vec2& a, vec2& b, vec2& c, vec2& d )
{
    vec2 I = vec2(b.x - a.x, b.y - a.y);
    vec2 J = vec2(d.x - c.x, d.y - c.y);
    
    float m = 0, k = 0, diviseur = I.x * J.y - I.y * J.x;
 
    if(diviseur != 0)
    {
        m = (I.x * a.y - I.x * c.y - I.y * a.x + I.y * c.x ) / diviseur;
        k = (J.x * a.y - J.x * c.y - J.y * a.x + J.y * c.x ) / diviseur;
    }
   	
   	//vec2 rr = c + m * J;
   	vec2 rr = a + k * I;
   	r.x = rr.x;
   	r.y = rr.y;
   	
   	if ( m>=a.x && m<=b.x) 			return true;
	return false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::intersectionH( vec2&r, vec2& c, vec2& d )
{
	vec2 a = vec2( -nNAXISn[0]/2, -nNAXISn[1]/2 );
	vec2 b = vec2(  nNAXISn[0]/2, -nNAXISn[1]/2 );

	intersection( r, a, b, c, d);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::intersectionB( vec2&r, vec2& c, vec2& d )
{
	vec2 a = vec2( -nNAXISn[0]/2, nNAXISn[1]/2 );
	vec2 b = vec2(  nNAXISn[0]/2, nNAXISn[1]/2 );

	intersection( r, a, b, c, d);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::intersectionG( vec2&r, vec2& c, vec2& d )
{
	vec2 a = vec2( -nNAXISn[0]/2, -nNAXISn[1]/2 );
	vec2 b = vec2( -nNAXISn[0]/2,  nNAXISn[1]/2 );

	intersection( r, a, b, c, d);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::intersectionD( vec2&r, vec2& c, vec2& d )
{
	vec2 a = vec2( nNAXISn[0]/2, -nNAXISn[1]/2 );
	vec2 b = vec2( nNAXISn[0]/2,  nNAXISn[1]/2 );

	intersection( r, a, b, c, d);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
double Fits::computeEchelle(vec2 v)
{
	mat2 mInv = mMat.inverse();
	vec2 w = mInv * v;
	
	return  5.0 * w.length() / v.length();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::normalizeEchelleMatrice()
{
	if ( dCDELT1==1.0 && dCDELT2==1.0)	{
		dCDELT1 /= 1000.0;
		dCDELT2 /= 1000.0;

		pPanelCorrectionFits->getCDELT1()->set_val( dCDELT1 );
		pPanelCorrectionFits->getCDELT1()->set_pVal( (double*)&dCDELT1 );
		pPanelCorrectionFits->getCDELT2()->set_val( dCDELT2 );
		pPanelCorrectionFits->getCDELT2()->set_pVal( (double*)&dCDELT2 );
		
		dPC1_1 *= 1000.0;
		dPC1_2 *= 1000.0;
		dPC2_1 *= 1000.0;
		dPC2_2 *= 1000.0;
		
		mAstroEchl *= 1000.0;
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::sauveMatrice()
{	
	normalizeEchelleMatrice();
	
	mat2 mMoinsX = mat2( -1.0, 0.0, 0.0, -1.0 );
	mMat = mMoinsX * mAstroEchl * mAstroTrns;// * sym0;
	//mMat = mAstroTrns * mAstroEchl * mMoinsX ;// * sym0;
	//mMat = mAstroEchl * mAstroTrns;// * sym0;

	char STR[255];
    row r;
    r.key = "M_Trns";
    mAstroTrns.to_str(STR);
    r.value = string( STR );
    pPanelFits->add_key_value( r.key, r.value );
    r.key = "M_Echl";
    mAstroEchl.to_str(STR);
    r.value = string( STR );
    pPanelFits->add_key_value( r.key, r.value );
	
	vec2 hg = vec2(0.0, 0.0);
	vec2 bd = vec2(nNAXISn[0]/2.0, nNAXISn[1]/2.0);
	vec2 v;

    r.key = "M_Res";
    mMat.to_str(STR);
    r.value = string( STR );
    pPanelFits->add_key_value( r.key, r.value );

	v = mMat * hg;
    r.key = "HG";
    v.to_str(STR);
    r.value = string( STR );
    pPanelFits->add_key_value( r.key, r.value );
	v = mMat * bd + vec2(nNAXISn[0]/2.0, nNAXISn[1]/2.0);
    r.key = "BD";
    v.to_str(STR);
    r.value = string( STR );
    pPanelFits->add_key_value( r.key, r.value );

	//-----------------------------------------------------------------
	// Grille de coordonnee
	
	vec2 p1, pp1, p2, pp2;
	vec2 p1N, pp1N;
	vec2 p2N, pp2N;
	vec2 p1E, pp1E;
	vec2 p2E, pp2E;
	vec2 c, s;

	vec2 vHaut;
	vec2 vGauche;
	
	s = vec2( 1.0, -1.0 );
	c = vec2( nNAXISn[0]/2, nNAXISn[1]/2 );
	

	//-----------------------------------------------------------------
	p1N = vec2( 00, 00 );			p2N = vec2( 00, -50 );
	pp1N = mMat * p1N;				pp2N = mMat * p2N;
	pp1N += c;						pp2N += c;
	
	vec2 vN = pp2N - pp1N;
	vN.normalize();
	pPanelCapture->setVecteurAD( vN );
	
	intersectionH( 	vHaut,   pp1N, pp2N );
	//pPanelCapture->addP1P2( vHaut, vHaut + 2.0*(pp1N-vHaut) );

	//-----------------------------------------------------------------
	p1E = vec2( 00, 00 );			p2E = vec2( -50, 00 );
	pp1E = mMat * p1E;				pp2E = mMat * p2E;
	pp1E += c;						pp2E += c;

	vec2 vE = pp2E - pp1E;
	vE.normalize();
	pPanelCapture->setVecteurDE( vE );
	
	intersectionG( 	vGauche,   pp1E, pp2E );
	//pPanelCapture->addP1P2( vGauche, vGauche + 2.0*(pp1E-vGauche) );


	vec2 vNord = pp2N - pp1N;
	vec2 vEst  = pp2E - pp1E;
	vec2 vStart1N = pp1N;
	vec2 vStart2N = pp2N;
	vec2 vStart1E = pp1E;
	vec2 vStart2E = pp2E;
	
	double echN = computeEchelle( vec2(0.0, dCRVAL1) );
	double echE = computeEchelle( vec2(dCRVAL2, 0.0) );
	
	logf( (char*)"l-Nord = %0.4lf l-Est = %0.4lf", echN, echE );
	
	for( float f=-10.0; f <(10.0); f+=1.0 )	{
		//-----------------------------------------------------------------
		pp1N = vStart1N + f*echN *(vEst);
		pp2N = vStart2N + f*echN *(vEst);
		
		intersectionH( 	vHaut,  pp1N, pp2N );
		pPanelCapture->addP1P2( vHaut, vHaut + 2.0*(pp1N-vHaut) );

		//-----------------------------------------------------------------
		pp1E = vStart1E + f*echE *(vNord);
		pp2E = vStart2E + f*echE *(vNord);

		intersectionG( 	vGauche, pp1E, pp2E );
		
		pPanelCapture->addP1P2( vGauche, vGauche + 2.0*(pp1E-vGauche) );

	}

	/*
	//-----------------------------------------------------------------
	p1N = vec2( 00, 00 );			p2N = vec2( 00, -50 );
	pp1N = mMat * p1N;				pp2N = mMat * p2N;
	pp1N += c;						pp2N += c;
	*/

	//-----------------------------------------------------------------


	/*
	vv = v1 = vec2( 00, 00 );
	ww = v2 = vec2( 50, 00 );
	pPanelCapture->addP1P2( v1+c, v2+c );
	v1 = mMat * vv;
	v2 = mMat * ww;
	pPanelCapture->addP1P2( v1+c, v2+c );

	vv = v1 = vec2( 50, 50 );
	ww = v2 = vec2( 50, 00 );
	pPanelCapture->addP1P2( v1+c, v2+c );
	v1 = mMat * vv;
	v2 = mMat * ww;
	pPanelCapture->addP1P2( v1+c, v2+c );
	vv = v1 = vec2( 50, 00 );
	ww = v2 = vec2( 00, 00 );
	pPanelCapture->addP1P2( s*v1+c, s*v2+c );
	v1 = mMat * vv;
	v2 = mMat * ww;
	pPanelCapture->addP1P2( s*v1+c, s*v2+c );

	pPanelCapture->setAffLignes(true);
	*/

}
//--------------------------------------------------------------------------------------------------------------------
// Pour une image en 8eb par couleur
// Lit une couleur du fichier fit, et applique les corrections si nessecaire
//--------------------------------------------------------------------------------------------------------------------
void Fits::read_RGB_8( float &C, uint8_t* pBuffer )
{
	C = *pBuffer;

	if ( dBZERO!=-1 && dBSCALE!=-1 )	C = C = (C * dBSCALE) + dBZERO ;
	if ( iOFFSET != -1 )				C -= iOFFSET;

	C /= 255.0;

	if ( C>dMax )				dMax = C;
	if ( C<dMin )				dMin = C;

}
//--------------------------------------------------------------------------------------------------------------------
// Pour une image en 16eb par couleur
// Lit une couleur du fichier fit, et applique les corrections si nessecaire
//--------------------------------------------------------------------------------------------------------------------
void Fits::read_RGB_16( float &C, int16_t* pBuffer )
{	/*
	uint8_t* p = (uint8_t*)pBuffer;
	uint8_t  uLow  = *(p+0);
	uint8_t  uHigh = *(p+1);
	
	
	
	C = ((uint16_t)uHigh<<8 + (uint16_t)uLow);
	*/
	int16_t c = *pBuffer;
	
	C = (float)c;
	
	//if ( dBZERO!=-1 && dBSCALE!=-1 )	C = (C * dBSCALE);
	//if ( dBZERO!=-1 && dBSCALE!=-1 )	C = (C * dBSCALE) + dBZERO ;
	//if ( dBZERO!=-1 && dBSCALE!=-1 )	C = (C-dBZERO) * dBSCALE;
	//if ( iOFFSET != -1 )				C -= iOFFSET;
	
	/*
	if ( C < 0.0 )
	{ 
		logf( (char*)"Pixel hors valeur" );
		//C *= -1.0;
	}

	if ( C>dMax )				dMax = C;
	if ( C<dMin )				dMin = C;
	*/
	
	if ( isnan(C) )	logf( (char*)"[ERREUR] Ce n'est pas un chiffre %d", c );
	C /= 65536.0;

	if ( isnan(C) )	logf( (char*)"[ERREUR] Ce n'est pas un chiffre %d", c );

	if ( C>dMax )				dMax = C;
	if ( C<dMin )				dMin = C;

	if ( C>1.0  )		c = 1.0;
	if ( C<-1.0 )		c = -1.0;
	//C /= 21758.0;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::chargeTexture(int nHDU)
{
	//---------------------------------------------------
    //nombre d'element d'un plan
    // size_fit = size_gl = nNAXIS0 * nNAXIS1
	//---------------------------------------------------
    // Nombre total d'elements 
	// l_fit = l_gl = nNAXIS1 * nNAXIS2 * .... * nNAXISm 
	//---------------------------------------------------
    size_fit = size_gl = nNAXISn[0] * nNAXISn[1];
    
	unsigned long l_fit = 1.0;		for (int i=0; i<nNAXIS; i++)	l_fit *= nNAXISn[i];
	unsigned long l_gl  = 1.0;		for (int i=0; i<nNAXIS; i++)	l_gl  *= nNAXISn[i];
    
    logf( (char*)"Fits::chargeTexture()" ); 
    log_tab(true);
    logf( (char*)"Longueur des buffer : fits %ld, opengl  %ld", (long)l_fit*nBITPIX/8, (long)l_gl ); 
    logf( (char*)"size_fits %ld, size_gl  %ld", (long)size_fit, (long)size_gl ); 
	//---------------------------------------------------
	// Mise en place la structure readBackground
	// texture openGL
    if ( l_fit>0 && l_gl>0 )      
    {
        bValid = true;
        readBgr.w = nNAXISn[0];
        readBgr.h = nNAXISn[1];
        //readBgr.d = nNAXIS;
        readBgr.d = 3;
    }
	//---------------------------------------------------
	// Allocation des buffer
    GLubyte* 		pBuffer_gl     	= (GLubyte*)malloc( 3*size_gl );
    uint8_t* 		pBuffer_8   	= NULL;
    int16_t* 		pBuffer_16  	= NULL;
    float* 			pBuffer_fit 	= NULL;

	if 		( nBITPIX == 16 )		pBuffer_16  = (int16_t*)malloc( 2 * l_fit * sizeof(int16_t) );
	else if	( nBITPIX == 8  )		pBuffer_8   = (uint8_t*)malloc( 1 * l_fit * sizeof(uint8_t) );
    else {
        logf( (char*)"[ERROR] Nompbre de bits par pixel non pris en charge ..." );
        log_tab(false);
        return;
    }
    
	pBuffer_fit  = (float*)malloc( l_fit * sizeof(float) );
	//---------------------------------------------------
	// traitement des erreurs
    if ( pBuffer_gl == NULL )	{
        logf( (char*)"[ERROR] Impossible d\'alloue la memoire pBuffer_gl" );
        log_tab(false);
        return;
    }
    if ( pBuffer_fit == NULL )	{
        logf( (char*)"[ERROR] Impossible d\'alloue la memoire pBuffer_fit" );
        log_tab(false);
        return;
    }
    if (   nBITPIX == 8 && pBuffer_8 == NULL ) 		{
        logf( (char*)"[ERROR] Impossible d\'alloue la memoire pBuffer_8" );
        log_tab(false);
        return;
    }
    if (   nBITPIX == 16 && pBuffer_16 == NULL ) 		{
        logf( (char*)"[ERROR] Impossible d\'alloue la memoire pBuffer_16" );
        log_tab(false);
        return;
    }
	//---------------------------------------------------
	// Ouvrir le fichier
    std::ifstream fichier;
    fichier.open(_filename, std::ios_base::app);
	//---------------------------------------------------
    if ( !fichier ) 		{
        logf( (char*)"[ERROR]impossble d'ouvrir : '%s'", (char*)_filename.c_str() );
        log_tab(false);
        return;
    }
	//---------------------------------------------------
    // se placer apres l'entete HDU
    fichier.seekg( LENGTH_HDU*nHDU, fichier.beg );
	//---------------------------------------------------
    // lecture des plans memoires
	if 		( nBITPIX == 16 )		fichier.read( (char*)pBuffer_16, l_fit * sizeof(int16_t) );
	else if	( nBITPIX == 8  )		fichier.read( (char*)pBuffer_8,  l_fit * sizeof(uint8_t) );
    fichier.close();    
	//---------------------------------------------------
    // Buffer sous la forme  NAXIS = 3   (3 plans)
    //   NAXIS0     NAXIS1     NAXIS2        Buffer GL
    // RRRRRRRRRR GGGGGGGGGG BBBBBBBBBB => RGBRGBRGB

	//---------------------------------------------------
	// Pour tout les plans
	//    Pout tout le contenu d'un plan
	//
	// remplissage du pointeur pBuffer_gl alias le pointeur readBackground
	unsigned long	p=0;
    float	 		R, G, B;
	long			i_fit, i_gl;
    long			y = nNAXISn[1]-1;
    long 			x = 0;
    long 			idx;
	
    i_fit = 0;
    i_gl = 0;
    idx = 0;
    float RR, GG, BB;
	//---------------------------------------------------
	// LECTURE DU FICHIER FIT  
	//   Conversion  entier->float (0.0<Pix<1.0)
	//---------------------------------------------------
    for( long i_gl=0; i_gl<nNAXISn[0]*nNAXISn[1]; i_gl++ )    {

        if ( nBITPIX == 16 )			
        {
            	read_RGB_16( RR, pBuffer_16 + 0*size_fit + i_fit );
		    GG = BB = RR;
            if ( nNAXIS == 3 )	{
            	read_RGB_16( GG, pBuffer_16 + 1*size_fit + i_fit );
            	read_RGB_16( BB, pBuffer_16 + 2*size_fit + i_fit );
            }
		}
		else  if ( nBITPIX == 8  )	
		{
            	read_RGB_8( RR, pBuffer_8 + 0*size_fit + i_fit );
		    GG = BB = RR;
            if ( nNAXIS == 3 )	{
            	read_RGB_8( GG, pBuffer_8 + 1*size_fit + i_fit );
            	read_RGB_8( BB, pBuffer_8 + 2*size_fit + i_fit );
            }
        }

	    pBuffer_fit[idx + 0] = RR;
	    pBuffer_fit[idx + 1] = GG;
	    pBuffer_fit[idx + 2] = BB;
	    
		idx += 3;
		i_fit++;
    }
	//---------------------------------------------------
	// INIT TEXTURE OPENGL
	//---------------------------------------------------
	for( long y=0; y<nNAXISn[1]; y++ )	{
		for( long x=0; x<nNAXISn[0]; x++ )	{
			long idx_gl;
			long idx_fit;
		    if ( bFlip )
		    {
				idx_gl		= (y * nNAXISn[0]) 	              + x;
				idx_fit 	= ((nNAXISn[1]-1-y) * nNAXISn[0]) + x;
	    	}
			else
			{
				idx_gl		= (y * nNAXISn[0])  + x;
				idx_fit 	= (y * nNAXISn[0])  + x;
			}

			idx_gl  *= 3;
			idx_fit *= 3;
			
			pBuffer_gl[idx_gl + 0] = 255.0* pBuffer_fit[idx_fit + 0];
			pBuffer_gl[idx_gl + 1] = 255.0* pBuffer_fit[idx_fit + 1];
			pBuffer_gl[idx_gl + 2] = 255.0* pBuffer_fit[idx_fit + 2];
		}
    }
    //------------------------------------------------
	// Liberation de la memoire
	if 		( nBITPIX == 16 )		free( pBuffer_16 );
	else if	( nBITPIX == 8  )		free( pBuffer_8  );
		
	free( pBuffer_fit );
    //------------------------------------------------
	// Affectation de la texture du fond de fenetre 
	readBgr.ptr = pBuffer_gl;
	
	string s0 = "Min, Max";
	string s1 = "Min="+ to_string(dMin) +" Max="+ to_string(dMax) +" bFlip="+ string(BOOL2STR(bFlip)) ;
	
    pPanelFits->add_key_value( s0, s1 );

    log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int Fits::getInt(string value )
{
    return stoi( value );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
double Fits::getDouble(string value )
{
    std::string::size_type sz;
    return stod( value, NULL );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::readBITPIX( string value )
{
    nBITPIX = getInt(value);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::readNAXIS( string key, string value )
{
    if ( key.find("NAXIS ") == 0 )          nNAXIS  = getInt( value );   
    else {
       nNAXISn.push_back( getInt(value) );   
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::readCR( string key, string value )
{
            if ( key.find("CRVAL1") == 0 )          dCRVAL1  = getDouble( value );   
    else    if ( key.find("CRVAL2") == 0 )          dCRVAL2  = getDouble( value );
    else    if ( key.find("CRPIX1") == 0 )          dCRPIX1  = getDouble( value );
    else    if ( key.find("CRPIX2") == 0 )          dCRPIX2  = getDouble( value );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::readCDELT( string key, string value )
{
	bool	bOK = true;
    if ( key.find("CDELT1") == 0 )	{
            	dCDELT1  = getDouble( value );
				pPanelCorrectionFits->getCDELT1()->set_val( dCDELT1 );
				pPanelCorrectionFits->getCDELT1()->set_pVal( (double*)&dCDELT1 );
	}
    else    if ( key.find("CDELT2") == 0 )  {
    			dCDELT2  = getDouble( value );
				pPanelCorrectionFits->getCDELT2()->set_val( dCDELT2 );
				pPanelCorrectionFits->getCDELT2()->set_pVal( (double*)&dCDELT2 );
	}
    else	bOK = false;
    
    if ( bOK )	{
		//mAstroEchl = mat2( dCDELT1, dCDELT2, dCDELT1, dCDELT2 );
		char STR[255];
		mAstroEchl.to_str(STR);
		logf( (char*)"Coefficiens d\'echelle : %s", STR );
	}
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::readCD( string key, string value )
{
	bool	bOK = true;
            if ( key.find("CD1_1") == 0 )           dCD1_1  = getDouble( value );   
    else    if ( key.find("CD1_2") == 0 )           dCD1_2  = getDouble( value );
    else    if ( key.find("CD2_1") == 0 )           dCD2_1  = getDouble( value );
    else    if ( key.find("CD2_2") == 0 )           dCD2_2  = getDouble( value );
    else	bOK = false;
    if ( bOK )	{
		mAstroEchl = mat2( dCD1_1, dCD2_1, dCD1_2, dCD2_2 );
	/*    
		char STR[255];
		mAstroEchl.to_str(STR);
		logf( (char*)"Matrice d'echelle: %s", STR );
	*/
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::readPC( string key, string value )
{
	bool	bOK = true;
            if ( key.find("PC1_1") == 0 )           dPC1_1  = getDouble( value );   
    else    if ( key.find("PC1_2") == 0 )           dPC1_2  = getDouble( value );
    else    if ( key.find("PC2_1") == 0 )           dPC2_1  = getDouble( value );
    else    if ( key.find("PC2_2") == 0 )           dPC2_2  = getDouble( value );
    else	bOK = false;

    if ( bOK )	{
		mAstroTrns = mat2( dPC1_1, dPC2_1, dPC1_2, dPC2_2 );
	/*    
    	char STR[255];
		mAstroTrns.to_str(STR);
		logf( (char*)"Matrice de transformation : %s", STR );
	*/
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::getRB( struct readBackground* p )
{
    p->ptr = readBgr.ptr.load();
    p->w   = readBgr.w.load();
    p->h   = readBgr.h.load();
    p->d   = readBgr.d.load();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::afficheDic()
{
    logf( (char*)"Affichage des donnees FITS" );

    int nb = datas.size();
    for( int i=0; i<nb; i++ )
    {
        logf( (char*)" - %s\t: %s", (char*)datas[i].key.c_str(), (char*)datas[i].value.c_str() );
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool Fits::haveKey( string k )
{
    logf( (char*)"Affichage des donnees FITS" );

    int nb = datas.size();
    for( int i=0; i<nb; i++ )
    {
        if ( datas[i].key.find(k) == 0)		return true;
    }
    return false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::afficheDatas()
{
    logf( (char*)"Resultat parser" );
    logf( (char*)" - nBITPIX %d", nBITPIX );    
    logf( (char*)" - nNAXIS  %d", nNAXIS );    

    logf( (char*)" - dCRPIX1 %f", dCRPIX1 );    
    logf( (char*)" - dCRVAL1 %f", dCRVAL1 );    

    logf( (char*)" - dCRPIX2 %f", dCRPIX2 );    
    logf( (char*)" - dCRVAL2 %f", dCRVAL2 );    

    logf( (char*)" - dCD1_1 %f", dCD1_1 );    
    logf( (char*)" - dCD1_2 %f", dCD1_2 );    

    logf( (char*)" - dCD2_1 %f", dCD2_1 );    
    logf( (char*)" - dCD2_2 %f", dCD2_2 );
    logf( (char*)" - dCD2_2 %f", dCD2_2 );

    logf( (char*)" - dBZERO %f", dBZERO );
    logf( (char*)" - dBSCALE %f", dBSCALE );

    logf( (char*)" - iOFFSET %d", iOFFSET );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::afficheInfoFits()
{
	if( pPanelFits != NULL )		{
		pPanelFits->setVisible( !pPanelFits->getVisible() );
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::afficheInfoFits(bool b)
{
	logf( (char*)"Fits::afficheInfoFits(%s)", BOOL2STR(b) );
	if( pPanelFits != NULL )		
	{
		pPanelFits->setVisible( b );
		if ( b )	{
			WindowsManager::getInstance().onTop(pPanelFits);
			WindowsManager::getInstance().onTop(pPanelCorrectionFits);
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::tex_2_J2000(vec2 s, vec2& j )
{
//#define DEBUG
	#ifdef DEBUG
		char STR[255];
		r.to_str(STR);
		logf( (char*)"Fits::screen2J2000( %s, x=%d, y=%d )", STR, x, y );
		log_tab( true );
	#endif

	vec2 crpix =  vec2( dCRPIX1, dCRPIX2 );
	vec2 crval =  vec2( dCRVAL1, dCRVAL2 );
	vec2 cdelt =  vec2( dCDELT1, dCDELT2 );

	s -= crpix;
	s.x = - s.x;

	#ifdef DEBUG
		r.to_str(STR);
		logf( (char*)"s=%s", s. STR);
	#endif
	
	mat2 m = mMat;
	//m.inverse();
	j = m * s;

	#ifdef DEBUG
		r.to_st(STR);
		logf( (char*)"Coef deg/pix: %s", STR );
	#endif

	j.x = j.x * cdelt.x;
	j.y = j.y * cdelt.y;

	#ifdef DEBUG
		r.to_st(STR);
		logf( (char*)"m.inverse*v: %s", STR );
	#endif

	j += crval;

	#ifdef DEBUG
		r.to_str(STR);
		logf( (char*)"valeur de retour %s", STR );
		
		log_tab( false );
	#endif
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::J2000_2_tex(vec2 j, vec2& s )
{
//#define DEBUG
	#ifdef DEBUG
		char str1[255];
		char str2[255];
		j.to_str(str1);
		s.to_str(str2);

		logf( (char*)"Fits::J2000_2_screen( j%s, s%s )", str1, str2 );
		log_tab( true );
	#endif

	vec2 crpix =  vec2( dCRPIX1, dCRPIX2 );
	vec2 crval =  vec2( dCRVAL1, dCRVAL2 );
	vec2 cdelt =  vec2( dCDELT1, dCDELT2 );

	j -= crval;

	#ifdef DEBUG
		j.to_str(str1); crval.to_str(str2);
		logf( (char*)"j%s crval%s", str1, str2 );
	#endif

	j.x = j.x / cdelt.x;
	j.y = j.y / cdelt.y;

	#ifdef DEBUG
		j.to_str(str1); cdelt.to_str(str2);
		logf( (char*)"j%s cdelt%s", str1, str2 );
	#endif
	
	mat2 m = mMat.inverse();
	s = m * j;

	#ifdef DEBUG
		s.to_str(str1);
		logf( (char*)"s%s", str1 );
	#endif
	
	s.x = -s.x;
	s += crpix;
	
	#ifdef DEBUG
		s.to_str(str1);
		logf( (char*)"valeur de retour s%s", str1 );
		
		log_tab( false );
	#endif
#undef DEBUG
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::iconize()
{
	pPanelFits->setVisible( false );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Fits::restaure( bool bAffInfo )
{
	pPanelFits->setVisible( bAffInfo );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

