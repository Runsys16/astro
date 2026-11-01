#include "asserv.h"
#include "camera_mgr.h"
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Asservissement::~Asservissement()
{
    logf_thread( (char*)"Destructeur ~Asservissement() -----------" );
    log_tab(true);

    WindowsManager&	 vm = WindowsManager::getInstance();

    log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Asservissement::Asservissement()
{
    logf_thread((char*)"Constructeur Asservissement() -------------" );
    log_tab(true);

    init();

    log_tab(false);
}
//--------------------------------------.-----------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Asservissement::init()
{
    logf_thread((char*)"Asservissement::init() -------------");

	mTexture2pap.identity();
	vStarGuide.set( 0.0, 0.0);
	ptAD[0].set( 0.0, 0.0 );
	ptAD[1].set( 0.0, 0.0 );
	ptDC[0].set( 0.0, 0.0 );
	ptDC[1].set( 0.0, 0.0 );
	vAD.set( 0.0, 0.0, 0.0);
	vDC.set( 0.0, 0.0, 0.0);
	
	fTimeCpt = 0.0;
}
//--------------------------------------.-----------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Asservissement::commande( int& ad, int& dc )
{
	vec3 v = vec3( ad, dc, 1.0 );
	vec3 w = mTexture2pap * v;
	ad = w.x * -1000.0;
	dc = w.y *  1000.0;
}
//--------------------------------------.-----------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Asservissement::commande( vec3& V )
{
	vec3 v = vec3( V );
	V = mTexture2pap * v;
	V.x *= -1000.0;
	V.y *=  1000.0;
}
//--------------------------------------.-----------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Asservissement::load_var( string & name)
{
	logf_thread((char*)"Asservissement::load_var( %s ) ", name.c_str() );

	VarManager& var = VarManager::getInstance();
	double		d;
	string		sVar;

	//---------------------------------
	
	sVar = name + "_vecAD[0].x";
	if ( var.existe(sVar) )		ptAD[0].x = var.getf( sVar );
	else						ptAD[0].x = 0.0;
	
	sVar = name + "_vecAD[0].y";
	if ( var.existe(sVar) )		ptAD[0].y = var.getf( sVar );
	else						ptAD[0].y = 0.0;

	sVar = name + "_vecAD[1].x";
	if ( var.existe(sVar) )		ptAD[1].x = var.getf( sVar );
	else						ptAD[1].x = 1.0;

	sVar = name + "_vecAD[1].y";
	if ( var.existe(sVar) )		ptAD[1].y = var.getf( sVar );
	else						ptAD[1].y = 0.0;

	//---------------------------------
	
	sVar = name + "_vecDC[0].x";
	if ( var.existe(sVar) )		ptDC[0].x = var.getf( sVar );
	else						ptDC[0].x = 0.0;
	
	sVar = name + "_vecDC[0].y";
	if ( var.existe(sVar) )		ptDC[0].y = var.getf( sVar );
	else						ptDC[0].y = 0.0;

	sVar = name + "_vecDC[1].x";
	if ( var.existe(sVar) )		ptDC[1].x = var.getf( sVar );
	else						ptDC[1].x = 0.0;

	sVar = name + "_vecDC[1].y";
	if ( var.existe(sVar) )		ptDC[1].y = var.getf( sVar );
	else						ptDC[1].y = 1.0;

	//---------------------------------
		
	vAD = vec3( ptAD[1] - ptAD[0], 0.0 );
	vDC = vec3( ptDC[1] - ptDC[0], 0.0 );

	compute_matrix();

	logf_thread( (char*)"matrix = " MAT3_PRINTF , MAT3_AFF(mTexture2pap) );
	
    sVar = name + "_RefSuivi.x";
	if ( var.existe(sVar) )		vRefSuivi.x = var.getf( sVar );
    sVar = name + "_RefSuivi.y";
	if ( var.existe(sVar) )		vRefSuivi.y = var.getf( sVar );

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Asservissement::compute_matrix()
{
	vec3 v = vec3( 0.0, 0.0, 1.0 );
	mat3 m = mat3( vAD, vDC, v );
	mat3 M = m.inverse();
	mTexture2pap = M;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Asservissement::print_ecart()
{
    static char sJoyXY[255];
    



	Camera_mgr& cams = Camera_mgr::getInstance();
	Camera* p = cams.getAsserv();
	
	if ( p != NULL )
	{
		snprintf( sJoyXY, sizeof(sJoyXY), "" VEC2_PRINTFN(0) " - %0.2lf", VEC2_AFF(vRefSuivi), dEcart );
		
	}
	else
		snprintf( sJoyXY, sizeof(sJoyXY), "" VEC2_PRINTFN(0) , VEC2_AFF(vRefSuivi) );

    pCoordSuivi->changeText((char*)sJoyXY );

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Asservissement::idleGL( float fElapsedTime, vec2& vSuivi )
{
    //------------------------------------------------------
    //------------------------------------------------------
	// CALCUL DE L' ASSERVISSEMENT
    //------------------------------------------------------
    //------------------------------------------------------
	if ( fElapsedTime != -1 )
	{
	    fTimeCpt += fElapsedTime;
        
        if ( bAsservissement )
        {
            vec3 res, v, w;  double l;

            w = vec3( vSuivi.x, vSuivi.y, 0.0);
            v = w - vec3( vRefSuivi.x, vRefSuivi.y, 0.0);
            dEcart = l = v.length();
			print_ecart();

           if ( l > fLimitCorrection0 ) {
                logf( (char*)"[WARNING] Suivi l=%0.2f/%0.2f", l, fLimitCorrection0 ); 
				//son( "/home/rene/.astropilot/sounds/alert.wav" );
                arret_urgence();
                Camera_mgr::getInstance().setAsserv( NULL );
                bAsservissement = false;
                //thread( &sound_alert).detach();
            }

	        if ( fTimeCpt > 0.0 )
	        {
                fTimeCpt -= fTimeCorrection;
                if ( fTimeCpt > 0.0 )    fTimeCpt = 0.0;//-= fTimeCorrection;
	            
	            if ( fabs(v.x) > panelCourbe->get_err() || fabs(v.y) > panelCourbe->get_err() )
	            {
                    if ( !bMouseDeplace && Serial::getInstance().getFree() )
                    {
                        res = mTexture2pap * v;
                        int ad = (int) (res.x * -1000.0);
                        int dc = (int) (res.y * 1000.0);
                        char cmd[255];
                        sprintf( cmd, "a%dp;d%dp", ad, dc );
                        Serial::getInstance().write_string(cmd, false);
                        logf( (char*)"cmd='%s' delta(%0.2f,%0.2f) l=%0.2f",  
                                      cmd, v.x, v.y, l );

                    }
                    else
                    {
                        logf( (char*)"Arduino  pas pret" ); 
                    }
	            }
            }
	    }
	}

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

