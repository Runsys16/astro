#ifndef PANELCOURBE_CPP
#define PANELCOURBE_CPP

#include "panel_courbe.h"
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool PanelCourbe::bDebug = false;
vec4 color_Y     = vec4(1.0,1.0,0.0,1.0);
vec4 color_X     = vec4(0.0,0.0,1.0,1.0);
vec4 color_FFT1  = vec4(0.0,1.0,0.0,0.7);
vec4 color_FFT0  = vec4(0.0,1.0,0.0,0.2);
vec4 color_FFT_Y = vec4(1.0,1.0,0.5,1.0);
vec4 color_FFT_X = vec4(0.5,0.5,1.0,1.0);
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelCourbe::PanelCourbe()
{
    logf( (char*)"PanelCourbe::PanelCourbe() Constructeur" );
    log_tab(true);
    
	WindowsManager&     wm  = WindowsManager::getInstance();
    VarManager&         var = VarManager::getInstance();

    init_var();
    //setDisplayGL(displayGLnuit_cb);
    //resizeCourbe( width, height );

    wm.add(this);
    
 	setBackground((char*)"images/background.tga");

    pXMax = new PanelText( (char*)"+err",		PanelText::NORMAL_FONT, 5, 50 );
	add( pXMax );

    pXMin = new PanelText( (char*)"-err",		PanelText::NORMAL_FONT, 5, 60 );
	add( pXMin );

    
    pYMax = new PanelText( (char*)"+err",		PanelText::NORMAL_FONT, 5, 70 );
    pYMin = new PanelText( (char*)"-err",		PanelText::NORMAL_FONT, 5, 80 );
    
    err = var.getf("err");

    courbe1         = var.getf("courbe1");
    delta_courbe1   = var.getf("delta_courbe1");
    courbe2         = var.getf("courbe2");
    delta_courbe2   = var.getf("delta_courbe2");

    vOrigine.x      = var.getf("vOrigine.x");
    vOrigine.y      = var.getf("vOrigine.y");
    vOrigine.z      = 0.0;

    if ( var.existe("bDisplayCourbeX") )        bDisplayCourbeX = var.getb("bDisplayCourbeX");
    if ( var.existe("bDisplayCourbeY") )        bDisplayCourbeY = var.getb("bDisplayCourbeY");
    if ( var.existe("bDisplayfftX") )           bDisplayfftX    = var.getb("bDisplayfftX");
    if ( var.existe("bDisplayfftY") )           bDisplayfftY    = var.getb("bDisplayfftY");
    if ( var.existe("bDisplayPt") )             bDisplayPt      = var.getb("bDisplayPt");
    else                                        { bDisplayPt=true; var.set("bDisplayPt", bDisplayPt ); }

    taille_mini     = var.getf("taille_mini_unite");

    filtre          = var.getf("filtre");



    ech_w           = 1.0;
    ech_h           = 1.0;

    bDisplayPt_old      = !bDisplayPt;
    bDisplayCourbeX_old = true;//Displayfft_old;
    bDisplayCourbeY_old = true;
    bDisplayfftX_old = true;//Displayfft_old;
    bDisplayfftY_old = true;
    filtre_old      = -1;
    
    fVal            = 256.0 / filtre;

    update_err();

    signalX.resize(256);
    signalY.resize(256);
    signal_inverseX.resize(256);
    signal_inverseY.resize(256);

    nb = 256;

    init_panel();

    string* pFile = var.gets( "FileResultat" );
    logf( (char*)"Chargement de %s", pFile->c_str() );
    if ( pFile!=NULL )
    {
        if ( pFile->find( "---" ) == std::string::npos )
        {
            charge_guidage( *pFile );
            logf( (char*)"Fichier : %s", pFile->c_str() );
        }
    }
    
    decal_x = var.geti( "decal_x" );
    decal_y = var.geti( "decal_y" );
    
    setVisible( var.getb( "bPanelCourbe" ) );

    build_unites_text();

    log_tab(false);
    logf( (char*)"PanelCourbe::PanelCourbe() Constructeur" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelText* PanelCourbe::init_text( int x, int y, char* ptext )
{
    PanelText* pT  = new PanelText( (char*)ptext,   PanelText::NORMAL_FONT, x, y ) ;
    add( pT );
    return pT;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelCheckBox* PanelCourbe::init_check_box( int x, int y )
{
    PanelCheckBox* pCB = new PanelCheckBox();
    
    pCB->setPosAndSize( x-20, y+1, 14, 14);
    pCB->setTrue(  (char*) "images/coche1.tga" );
    pCB->setFalse( (char*) "images/coche0.tga" );

    add( pCB );
    return pCB;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::init_panel()
{
	WindowsManager&     wm  = WindowsManager::getInstance();

    int x = 800;
    int y = 0;
    int dy = 16;
    char s[55];
    
    pAffCourbe    = init_text( x, y*dy, (char*)"On" );
    //pCBAffCourbe  = init_check_box( x, y++*dy );
    y++;

    sprintf( s, (char*)"Points" );
    pAffPt    = init_text( x, y*dy, (char*)s );
    pCBAffPt  = init_check_box( x, y++*dy );
    pCBAffPt->setListener( &bDisplayPt );
    if ( bDisplayPt  )              pAffPt->setColor( (unsigned long)0xffffffff );
    else                            pAffPt->setColor( (unsigned long)0xff808080 );

    if ( bDisplayfftX )         sprintf( s, (char*)"X fft On" );
    else                        sprintf( s, (char*)"X fft Off" );
    pAffFFTX    = init_text( x, y*dy, (char*)s );
    pCBAffFFTX  = init_check_box( x, y++*dy );
    pCBAffFFTX->setListener( &bDisplayfftX );

    if ( bDisplayfftX )         sprintf( s, (char*)"Y fft On" );
    else                        sprintf( s, (char*)"Y fft Off" );
    pAffFFTY    = init_text( x, y*dy, (char*)s );
    pCBAffFFTY  = init_check_box( x, y++*dy );
    pCBAffFFTY->setListener( &bDisplayfftY );

    if ( bDisplayCourbeX )      sprintf( s, (char*)"Abscisse On" );
    else                        sprintf( s, (char*)"Abscisse Off" );
    pCourbeX    = init_text( x, y*dy, (char*)s );
    pCBCourbeX  = init_check_box( x, y++*dy );
    pCBCourbeX->setListener( &bDisplayCourbeX );

    if ( bDisplayCourbeX )      sprintf( s, (char*)"Ordonnee On" );
    else                        sprintf( s, (char*)"Ordonnee Off" );
    pCourbeY    = init_text( x, y*dy, (char*)s );
    pCBCourbeY  = init_check_box( x, y++*dy );
    pCBCourbeY->setListener( &bDisplayCourbeY );
    
    pFilename       = new PanelText( (char*)"--",     PanelText::NORMAL_FONT, 200, 0 );

    add( pFilename );
    
    pFiltreVal = new PanelSpinEditText();
    pFiltreVal->setPosAndSize( x -120, 0, 80, 20 );

    sprintf( s,"%0.0f", 256.0/filtre );
    pFiltreVal->changeText( s );

    //  Valeur de l'edition dimension de la
    pFiltreVal->set( 1, 128, 1, 1 );
    pFiltreVal->set_delta( 20, 8 );
    pFiltreVal->set_pVal( &fVal );

    add( pFiltreVal );
    
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelCourbe::~PanelCourbe()
{
    // Effacement des unites
    int nb = unites.size();
    if (nb != 0)
    {
        for( int i=0; i<nb; i++ )
        {
            sup(unites[i].pText);
            delete unites[i].pText;
            //unites[i] = 0;
        }
        unites.clear();
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::init_var()
{
    VarManager&         var = VarManager::getInstance();

    if (!var.existe("err"))                         var.set("err", (float)1.0);
    if (!var.existe("courbe1"))                     var.set("courbe1", (float)1.0);
    if (!var.existe("delta_courbe1"))               var.set("delta_courbe1", (float)1.0);
    if (!var.existe("courbe2"))                     var.set("courbe2", (float)1.0);
    if (!var.existe("delta_courbe2"))               var.set("delta_courbe2", (float)1.0);

    if (!var.existe("vOrigine.x"))                  var.set("vOrigine.x", (float)0.0);
    if (!var.existe("vOrigine.y"))                  var.set("vOrigine.y", (float)0.0);

    if (!var.existe("bPanelCourbe"))                var.set("bPanelCourbe", true);
    if (!var.existe("FileResultat"))                var.set("FileResultat", "---");
    if (!var.existe("decal_x"))                     var.set("decal_x", 0 );
    if (!var.existe("decal_y"))                     var.set("decal_y", 0 );
    if (!var.existe("taille_mini_unite"))           var.set("taille_mini_unite", (float)20.0);
    
    if (!var.existe("xPanelCourbe") )               var.set( "xPanelCourbe", 10);
    if (!var.existe("yPanelCourbe") )               var.set( "yPanelCourbe", 10);
    if (!var.existe("dxPanelCourbe") )              var.set("dxPanelCourbe", 400);
    if (!var.existe("dyPanelCourbe") )              var.set("dyPanelCourbe", 400);

    if (!var.existe("filtre") )                     var.set("filtre", (float)10.0);

    pIn  = NULL;
    pOut = NULL;
    nb = 0;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::charge_guidage_1_1(ifstream& fichier)
{
    logf( (char*)"charge_guidage_1_1()" );
    log_tab(true);

    string line;
    int nbLigne = 0;
    
    
    while ( getline (fichier, line) )
    {
        float rx, ry, ox, oy, t;
        sscanf( line.c_str(), "delta(%f, %f) origine(%f, %f) time(%f)", &rx, &ry, &ox, &oy, &t );

        vOrigine.x = ox;
        vOrigine.y = oy;
        
        data d;
        d.v     = vec2(rx,ry);
        d.t     = t;
        d.type  = PanelCourbe::ABSOLU;

        t_vCourbe.push_back( d );

        nbLigne++;
    }    
    int n = t_vCourbe.size();
    logf( (char*)"Lecture de %d donnees dans %d lignes", n, nbLigne );
    
    log_tab(false);
    logf( (char*)"PanelCourbe::charge_guidage_1_1()" );
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::charge_guidage_1_0(ifstream& fichier, string line)
{
    logf( (char*)"PanelCourbe::charge_guidage_1_0()");
    log_tab(true);
    
    int nbLigne = 0;
    
    
    while ( true )
    {
        float rx, ry, ox, oy;

        sscanf( line.c_str(), "( %f , %f ) / ( %f , %f )", &rx, &ry, &ox, &oy );

        vOrigine.x = ox;
        vOrigine.y = oy;

        data d;
        d.v     = vec2(vec2(rx,ry) - vec2(ox, oy));
        d.t     = 0.0;
        d.type  = PanelCourbe::ABSOLU;

        t_vCourbe.push_back( d );

        nbLigne++;
        if ( !getline (fichier, line) )     break;
    }    
    
    
    int n = t_vCourbe.size();
    logf( (char*)"Lecture de %d donnees dans %d lignes", n, nbLigne );
    

    log_tab(false);
    logf( (char*)"PanelCourbe::charge_guidage_1_0()");
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::charge_guidage_version(ifstream& fichier)
{
    string line;
    getline (fichier, line);

    if ( line[0] == '#' && line[1] == '!' )
    {
        if ( line.substr(2).find( "version-1.1" ) != string::npos )
            charge_guidage_1_1( fichier );
        
    }
    else
        charge_guidage_1_0( fichier, line );
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::charge_guidage(string filename)
{
    logf( (char*)"PanelCourbe::charge_guidage('%s')", (char*)filename.c_str() );
    log_tab(true);
    
    std::ifstream fichier;
    fichier.open(filename, std::ios_base::app);

    if ( !fichier ) 
    {
        logf( (char*)"[ERROR]impossble d'ouvrir : '%s'", (char*)filename.c_str() );
        log_tab(false);
        logf( (char*)"charge_guidage('%s')", (char*)filename.c_str() );
        return;
    }
    
    pFilename->changeText( (char*)get_basename(filename).c_str() );
    
    t_vCourbe.clear();
    decal_x = 0;
    decal_y = 0;

    charge_guidage_version( fichier );

    fichier.close();    
    t_vSauve.clear();
    
    if ( bDisplayfftX || bDisplayfftY )           build_fft3();

    log_tab(false);
    logf( (char*)"PanelCourbe::charge_guidage('%s')", (char*)filename.c_str() );
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::create_guidage( char* version )
{
    logf( (char*)"Creation de l'entete '%s'", version );
    
    std::ofstream fichier;
 
    fichier.open(filenameSauve, std::ios_base::app);

    if ( !fichier ) 
    {
        logf( (char*)"[ERROR]impossble de creer : '%s'", (char*)filenameSauve.c_str() );
    }

    fichier << version << "\n";
    fichier.close();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::sauve_guidage_1_1()
{
    //string filename = "/home/rene/.astropilot/sauvegarde.txt";
    logf( (char*)"PanelCourbe::sauve_guidage_1_1()" );

    if ( !fexists( filenameSauve.c_str()) )       create_guidage( (char*)"#!version-1.1" );
       
    std::ofstream fichier;
    fichier.open(filenameSauve, std::ios_base::app);

    if ( !fichier ) 
    {
        logf( (char*)"[ERROR]impossble d'ouvrir : '%s'", (char*)filenameSauve.c_str() );
    }

    for(int i=0; i<t_vSauve.size(); i++)
    {
        fichier << "delta(" << t_vSauve[i].v.x << ", " <<  t_vSauve[i].v.y << ")";
        fichier << " origine" << "(" << vOrigine.x << ", " <<  vOrigine.y << ")";
        fichier << " time" << "(" << t_vSauve[i].t << ")";
        fichier << "\n";
    }

    fichier.close();
    
    t_vSauve.clear();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::sauve_guidage_1_0()
{
    std::ofstream fichier;
 
    fichier.open(filenameSauve, std::ios_base::app);

    if ( !fichier ) 
    {
        logf( (char*)"[ERROR]impossble d'ouvrir : '%s'", (char*)filenameSauve.c_str() );
    }

    for(int i=0; i<t_vSauve.size(); i++)
    {
        fichier << "( " << t_vSauve[i].v.x << " , " <<  t_vSauve[i].v.y << " ) / ";
        fichier << "( " << vOrigine.x << " , " <<  vOrigine.y << " )\n";
    }

    fichier.close();
    
    t_vSauve.clear();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::sauve_guidage()
{
    logf( (char*)"Sauvegarde des valeurs dans '%s'", (char*)filenameSauve.c_str() );
    sauve_guidage_1_1();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::idle_guidage(vec2  v)
{
    if ( t_vCourbe.size()>200000)      t_vCourbe.clear();

    vec2 o = vec2(xSuivi, ySuivi);
    ajoute( v-o );

    if ( t_vSauve.size() >= 20 ) {
        if ( bSauve )           sauve_guidage();
        logf( (char*)"Remise a zero t_vSauve %dpt", t_vCourbe.size());
        t_vSauve.clear();
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::reset_guidage()
{
    logf( (char*)"reset_guidage()" );
    
    t_vCourbe.clear();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::ajoute(vec2 v)
{
    if ( isPleiade() )      v.x /=14.0;
    v /= 4.0;
    
    data d;
    d.v     = vec2(v);
    d.t     = Timer::getInstance().getCurrentTime();
    d.type  = PanelCourbe::ABSOLU;

    t_vCourbe.push_back( d );
    t_vSauve.push_back( d );

    //if ( bDisplayfftX || bDisplayfftY )
    build_fft3();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::update_err()
{
    char s[20];
    sprintf( s, "+%0.2f px", err );
    pXMax->changeText( (char*)s );
    pYMax->changeText( (char*)s );

    sprintf( s, "-%0.2f px", err );
    pXMin->changeText( (char*)s );
    pYMin->changeText( (char*)s );
}
/*
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::screen2tex( int& x, int& y )
{
    x = (float)(x-xCam) * rw;
    y = (float)(y-yCam) * rh;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::tex2screen( int& x, int& y )
{
    x = (float)x / rw + xCam;
    y = (float)y / rh + yCam;
}
*/
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::glCarre( int x, int y, int dx,  int dy )
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
void PanelCourbe::glEchelleAxe( int AXE, int SIZE, float max, float min, PanelText* pMax, PanelText* pMin )
{
    VarManager& var = VarManager::getInstance();

    float gris = 0.3;
    vec4 color, colorLimit, colorAxe;
    
    if ( var.getb("bNuit") )
    {
        color       = vec4( gris, 0.0, 0.0, 1.0 );    
        colorLimit  = vec4( 0.8, 0.0, 0.0, 1.0 );    
        colorAxe    = vec4( 1.0, 0.0, 0.0, 1.0 );
    }
    else                            
    {
        //gris = 0.4;
        color       = vec4( gris, gris, gris, 1.0 );
        colorLimit  = vec4( 1.0, 0.0, 0.0, 1.0 );
        colorAxe    = vec4( 1.0, 1.0, 1.0, 1.0 );
    }
    
    glBegin(GL_LINES);

        //--------------------------------------------------------
        // graduation horizontale
        //--------------------------------------------------------
        glColor4fv( (GLfloat*)&color );
        
        float fPas = delta_courbe1*ech_h;
        while( fPas < taille_mini )     fPas += courbe1*ech_h;
        
        int y, y0, y1;
        int x, x0, x1;
        
        float C = AXE - decal_y;
        float deb = round( -C / fPas );
        float fin = round( (getDY()-C) / fPas );
        
        if ( deb < ( -C / fPas ) )              deb += 1.0;
        //if ( fin > ( (getDY()-C) / fPas ) )     fin += 1.0;
        
        if ( PanelCourbe::bDebug )      logf( (char*)"deb=%0.2f fin=%0.2f", deb, fin );
        
        for ( float i=deb; i<fin; i+=1.0 )
        {
            x0 = xStartAxe;
            x1 = getPosDX();

            y0 = y1 = i*fPas + C;
            if ( PanelCourbe::bDebug )      logf( (char*)"  i=%0.2f y0=%d", i, y0 );

            xy2Screen(x0, y0);
            xy2Screen(x1, y1);
            
            glVertex2i( x0, y0 );
            glVertex2i( x1, y1 );
        } 
        //--------------------------------------------------------
        // graduation verticale
        //--------------------------------------------------------
        fPas = courbe1*ech_w;
        while( fPas < 10.0 )     fPas += courbe1*ech_w;
        float M = getPosDX();
        for( float i=xStartAxe; i<M; i+=fPas )
        {
            x = i;
            y = AXE;
            xy2Screen(x,y);
            y -= SIZE/2;
            glVertex2i( x, y );
            
            y += SIZE;
            glVertex2i( x, y );
        }
        //--------------------------------------------------------
        // axe horizontale centrale
        //--------------------------------------------------------
        y = C;//AXE -decal_y;
        
        if ( (0<y) && (y<getDY()) )
        {
            glColor4fv( (GLfloat*)&colorAxe );
            int x = xStartAxe;

            xy2Screen(x, y);
            glVertex2i( x, y );

            x += getPosDX()-xStartAxe;
            glVertex2i( x, y );
        }
    glEnd();        
        //--------------------------------------------------------
        // Limite horizontale Err de suivi
        // Max
        //--------------------------------------------------------
        glColor4fv( (GLfloat*)&colorLimit );
        glLineStipple(3, 0xFF99);
        glEnable(GL_LINE_STIPPLE);    

    glBegin(GL_LINES);


        y = (float)(delta_courbe1*(min)*ech_h + AXE) - decal_y;
        if ( 0<y && y<getDY() ) {
            if ( pMax != NULL ) {
                pMax->setPos( 5, y-7 );
                pMax->setColor( get_color(colorLimit) );
                pMax->setVisible(true);
            }
            x = xStartAxe;
            xy2Screen(x, y);
            glVertex2i( x, y );

            x += getPosDX()-xStartAxe;
            glVertex2i( x, y );
        }
        else
            pMax->setVisible(false);
        //--------------------------------------------------------
        // Limite horizontale Err de suivi
        // Min
        //--------------------------------------------------------
        y = (float)(delta_courbe1*(max)*ech_h + AXE) - decal_y;

        if ( 0<y && y<getDY() ) {
            if ( pMin != NULL ) {
                pMin->setPos( 5, y-7 );
                pMin->setColor( get_color(colorLimit) );
                pMin->setVisible(true);
            }
            x = xStartAxe;
            xy2Screen(x, y);
            glVertex2i( x, y );

            x += getPosDX()-xStartAxe;
            glVertex2i( x, y );
        }
        else
            pMin->setVisible(false);
    glEnd();        
        glDisable(GL_LINE_STIPPLE);    
        glColor4fv( (GLfloat*)&colorAxe );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::glEchelle()
{
    VarManager& var = VarManager::getInstance();

    //int  AXE_X = (float)getDY()/4.0;
    //int  AXE_Y = (3.0*(float)getDY()/4.0);
    
    int  AXE_X = (float)getDY()/2.0;

    int dy_axe = (float)getDY() / 1.0;
    
    glEchelleAxe( AXE_X, dy_axe, (float)+err, (float)-err, pXMax, pXMin );
    //glEchelleAxe( AXE_Y, dy_axe, (float)+err, (float)-err, pYMax, pYMin );

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::glCourbeCube( float* tab, int size, int pas, int xStart, int xdecal, int y_zero, 
                                float offset, float a, float b )
{
    if ( size != 0  )
    {
        glBegin(GL_QUADS);
        glBegin(GL_LINE_STRIP);
        int deb = xdecal;
        int fin = deb + 2000.0/a;
        int n = size;
        int Y_zero = y_zero;
        y2Screen(Y_zero);        
               
        if ( fin >= size )                  fin = size;
        if ( deb <= 0 )                     deb = 0;

        for( int i=deb; i<fin; i++ )
        {
            float y = tab[(n-i-1)*pas] - offset;
            y = (float)(b*(y) + y_zero);
            
            int x = (i-deb)*a + xStart;
            if ( x > getPosDX() )                           break;

            int Y = y;

            xy2Screen(x, Y);

            glVertex2i( x, Y );
            glVertex2i( x+a-1, Y );
            glVertex2i( x+a-1, Y_zero );
            glVertex2i( x, Y_zero );
            
        }
        glEnd();                
    }     
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::glCourbeCube( float* tab, int size, int pas, int xStart, int xdecal, int y_zero, float offset )
{
    glCourbeCube( tab, size, pas, xStart, xdecal, y_zero, offset, courbe1, delta_courbe1 );
    glCourbeCube( tab, size, pas, xStart, xdecal, y_zero, offset, courbe1, delta_courbe1 );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::glCourbe( float* tab, int size, int pas, int xStart, int xdecal, int ydecal,
                            int y_zero, float offset, float a, float b, bool bPoints )
{
    if ( size != 0  )
    {
        glBegin(GL_LINE_STRIP);
        int deb = xdecal;
        int fin = deb + 2000.0/a;
        int n = size;
        
               
        if ( fin >= size )                  fin = size;
        if ( deb <= 0 )                     deb = 0;

        for( int i=deb; i<fin; i++ )
        {
            float y = tab[(n-i-1)*pas] - offset;
            y = (float)(b*(y) + y_zero);
            
            int x = (i-deb)*a + xStart;
            if ( x > getPosDX() )                           break;

            int Y = y - ydecal;

            x2Screen(x);
            y2Screen(Y);

            glVertex2i( x, Y );

            //glCarre( x, Y, 10, 10 );
        }
        glEnd();                

        if ( !bPoints )         return;

        for( int i=deb; i<fin; i++ )
        {
            float y = tab[(n-i-1)*pas] - offset;
            y = (float)(b*(y) + y_zero);
            
            int x = (i-deb)*a + xStart;
            if ( x > getPosDX() )                           break;

            int Y = y - ydecal;

            glCarre( x, Y, 2, 2 );
        }
    }     
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::glCourbe( float* tab, int size, int pas, int xStart, int xdecal, int ydecal, int y_zero, float offset )
{
    glCourbe( tab, size, pas, xStart, xdecal, ydecal, y_zero, offset, courbe1, delta_courbe1, false );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::glCourbes()
{
    VarManager&     var = VarManager::getInstance();

    offset_x = vOrigine.x;
    offset_y = vOrigine.y;
    
    float* tabx = (float*)&t_vCourbe[0];
    float* taby = tabx+1;
    
    if ( bDisplayCourbeX )
    {
        if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 1.0 );
        else                            glColor4fv( color_X );
        
       glCourbe(    tabx, t_vCourbe.size(), sizeof(data)/4, xStartAxe, decal_x, decal_y, getDY()/2, 0.0,
                    courbe1*ech_w, delta_courbe1*ech_h, bDisplayPt );
    }
    
    if ( bDisplayCourbeY )
    {
        if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 1.0 );
        else                            glColor4fv( color_Y );
        
            glCourbe(   taby, t_vCourbe.size(), sizeof(data)/4, xStartAxe, decal_x, decal_y, getDY()/2, 0.0,
                        courbe1*ech_w, delta_courbe1*ech_h, bDisplayPt );
    }
        
    //if ( iDisplayfft < 2 )
    
    //{
    if ( bDisplayfftX )
    {
        glLineWidth(3.0);
        if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 1.0 );
        else                            glColor4fv( color_FFT_X );
        glCourbe(   inverseX, nb, 1, xStartAxe, 0, decal_y, getDY()/2, 0.0, 
                    courbe1*ech_w, delta_courbe1*ech_h, false );
    }

    if ( bDisplayfftY )
    {
        glLineWidth(3.0);
        if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 1.0 );
        else                            glColor4fv( color_FFT_Y );
        glCourbe(   inverseY, nb, 1, xStartAxe, 0, decal_y, getDY()/2, 0.0, 
                    courbe1*ech_w, delta_courbe1*ech_h, false );
    }

    glLineWidth(1.0);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::glFft()
{
    if ( !bDisplayfftX && !bDisplayfftY )       return;

    VarManager& var = VarManager::getInstance();

    int n = nb / filtre;
    int m = nb/2;
    float coefX = (float)(getDX()-xStartAxe)/((float)nb/2.0);

    if ( var.getb("bNuit") )            glColor4f( 1.0, 0.0, 0.0, 0.35 );
    else                                glColor4fv( color_FFT1 );

    glCourbeCube(   pOut + m-n, n , 1, (int)xStartAxe, 0, (int)getDY()-20 , (float)0.0, 
                    (float)(getDX()-xStartAxe)/((float)nb/2.0), (float)delta_courbe1*ech_h );

    if ( var.getb("bNuit") )            glColor4f( 1.0, 0.0, 0.0, 0.35 );
    else                                glColor4fv( color_FFT0 );

    glCourbeCube(   pOut, m-n , 1, (int)xStartAxe+ n*coefX, 0, (int)getDY()-20 , (float)0.0, 
                    coefX, (float)delta_courbe1*ech_h );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::build_unites_text()
{
    //logf( (char*)"PanelCourbe::build_unites_text()" );
    //log_tab(true);


    float AXE = getDY()/2.0;
    float fPas = delta_courbe1*ech_h;
    while( fPas < taille_mini )     fPas += courbe1*ech_h;
    
    float C = AXE - decal_y;
    float deb = round( -C / fPas );
    float fin = round( (getDY()-C) / fPas );
      
    if ( deb < ( -C / fPas ) )              deb += 1.0;
    //if ( fin > ( (getDY()-C) / fPas ) )     fin += 1.0;
                
    int idx = 0;
    string s;
    
    for ( float i=deb; i<fin; i+=1.0 )
    {
        int y = i*fPas + C;

        s = "" + to_string((int)round(-i)) + " px";
        //y2Screen(y);

        //logf( (char*)"%d %d", (int)i, (int)y );
        unite u;

        if ( idx == unites.size() )         { 
            u.pText = new PanelText((char*)s.c_str(),PanelText::NORMAL_FONT); 
            u.x = x;
            u.y = y;
            unites.push_back( u ); 

            add( unites[idx].pText );
            unites[idx].pText->setTabSize(40);
        }
        else {
            unites[idx].pText->changeText( (char*)s.c_str(), true );
            unites[idx].pText->setTabSize(40);
        }
        unites[idx].pText->setPos(25, y-8);
        unites[idx].pText->setVisible(true);
        idx++;

    } 

    for( int i=idx; i<unites.size(); i++ )
        unites[i].pText->setVisible(false);

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int PanelCourbe::sc2winX( int n )
{
    return n - getX();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int PanelCourbe::sc2winY( int n )
{
    return n - getY();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::displayGL(void)
{
	if ( !visible )			return;

	WindowsManager& wm  = WindowsManager::getInstance();
    VarManager&     var = VarManager::getInstance();

    if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 1.0 );
    else                            glColor4f( 1.0, 1.0, 1.0, 1.0 );

    unsigned int color; 
    if ( var.getb("bNuit") )        color = 0xffff0000;
    else                            color = 0xffffffff;
    
    pFiltreVal->setColor( color );


    glEchelle();
    
    PanelWindow::displayGL();

    int DY = getY();
    

    float height = (float)wm.getHeight();
	int   scx  = getX();
	int   scy  = height - getDY() - getY();
	int   scdx = getDX();
	int   scdy = getDY();
    
    glScissor( scx, scy, scdx, scdy );
    glEnable( GL_SCISSOR_TEST );

    if ( bDisplayfftX || bDisplayfftY )
    {
        build_fft3();
        if (pOut != NULL)       glFft();
    }
    
    glCourbes();

    glDisable( GL_SCISSOR_TEST );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::updatePos()
{
    PanelSimple::updatePos();
    build_unites_text();

	WindowsManager& wm  = WindowsManager::getInstance();
    VarManager&     var = VarManager::getInstance();
    
    //      Dimension ecran
    float wSc = (float)wm.getWidth();
    float hSc = (float)wm.getHeight();
    ech_w = (float)getDX() / wSc;
    ech_h = (float)getDY() / hSc;
    
    
    pAffPt->setPos(       getDX()-100, pAffPt->getPosY() );
    pCBAffPt->setPos(     getDX()-120, pAffPt->getPosY() );

    pCourbeX->setPos(       getDX()-100, pCourbeX->getPosY() );
    pCBCourbeX->setPos(     getDX()-120, pCourbeX->getPosY() );

    pCourbeY->setPos(       getDX()-100, pCourbeY->getPosY() );
    pCBCourbeY->setPos(     getDX()-120, pCourbeY->getPosY() );

    pAffCourbe->setPos(     getDX()-100, pAffCourbe->getPosY() );
    //logf( (char*)"setPos %d, %d ", getDX()-180,  pAffCourbe->getPosY() );
    pFiltreVal->setPos(     getDX()-55,  pAffCourbe->getPosY() );
    //pCBAffCourbe->setPos(   getDX()-120, pAffCourbe->getPosY() );

    pAffFFTX->setPos(        getDX()-100, pAffFFTX->getPosY() );
    pCBAffFFTX->setPos(      getDX()-120, pAffFFTX->getPosY() );
    //logf( (char*)"updatePos()" );

    pAffFFTY->setPos(        getDX()-100, pAffFFTY->getPosY() );
    pCBAffFFTY->setPos(      getDX()-120, pAffFFTY->getPosY() );
    //logf( (char*)"updatePos()" );

    char s[255];
    //--------------------------------------------------------------    
    if (     bDisplayCourbeX_old != bDisplayCourbeX ) {
        //--------------------------------------------------------------    
        if ( bDisplayCourbeX  )     sprintf( s, (char*)"Abcisse X On" );
        else                        sprintf( s, (char*)"Abcisse X Off" );
        
        pCourbeX->changeText( s, true );
        //pCBCourbeX->setVal( bCBCourbeX );
        //--------------------------------------------------------------    
        var.set( "bDisplayCourbeX", bDisplayCourbeX );
    }

    if (     bDisplayCourbeY_old != bDisplayCourbeY ) {
        if ( bDisplayCourbeY )      sprintf( s, (char*)"Ordonnee Y On" );
        else                        sprintf( s, (char*)"Ordonnee Y Off" );
        
        pCourbeY->changeText( s, true );
        //pCBCourbeY->setVal( bCBCourbeY );
        var.set( "bDisplayCourbeY", bDisplayCourbeY );
    }
    //--------------------------------------------------------------    
    if ( bDisplayCourbeX  )                 pCourbeX->setColor( (unsigned long)0xff0808ff );
    else                                    pCourbeX->setColor( (unsigned long)0xff808080 );

    if ( bDisplayCourbeY )                  pCourbeY->setColor( (unsigned long)0xffffff00 );
    else                                    pCourbeY->setColor( (unsigned long)0xff808080 );

    //--------------------------------------------------------------    
    if ( bDisplayfftX != bDisplayfftX_old )  {
        if ( bDisplayfftX )         sprintf( s, (char*)"X fft On" );
        else                        sprintf( s, (char*)"X fft Off" );

        pAffFFTX->changeText( s, true );
        pAffFFTX->setColor( (unsigned long)0xff00ff00 );
        pCBAffFFTX->setVal( bDisplayfftX );
        var.set( "bDisplayfftX", bDisplayfftX );
    }
    //--------------------------------------------------------------    
    if ( bDisplayfftY != bDisplayfftY_old )  {
        if ( bDisplayfftY )         sprintf( s, (char*)"Y fft On" );
        else                        sprintf( s, (char*)"Y fft Off" );

        pAffFFTY->changeText( s, true );
        pAffFFTY->setColor( (unsigned long)0x8000ff00 );
        pCBAffFFTY->setVal( bDisplayfftY );
        var.set( "bDisplayfftY", bDisplayfftY );
    }
    //--------------------------------------------------------------    
    if ( bDisplayPt != bDisplayPt_old )  {
        if ( bDisplayPt  )              pAffPt->setColor( (unsigned long)0xffffffff );
        else                            pAffPt->setColor( (unsigned long)0xff808080 );
        //pCBAffFFTY->setVal( bDisplayfftY );
        var.set( "bDisplayPt", bDisplayPt );
    }
    //--------------------------------------------------------------    
    if ( bDisplayfftX  )            pAffFFTX->setColor( (unsigned long)0xff0808ff );
    else                            pAffFFTX->setColor( (unsigned long)0xff808080 );

    if ( bDisplayfftY )             pAffFFTY->setColor( (unsigned long)0xffffff00 );
    else                            pAffFFTY->setColor( (unsigned long)0xff808080 );
    //--------------------------------------------------------------    
    filtre = (float)nb / fVal;
    //logf( (char*)"PanelCourbe::updatePos() filtre %0.2f", filtre );
    
    if ( filtre_old != filtre )
    {
        sprintf( s, (char*)"Filtre :" );
        pAffCourbe->changeText( s, true );

        VarManager::getInstance().set( "filtre", filtre );
        logf( (char*)"PanelCourbe::updatePos() filtre %0.2f", filtre );
    }
    //--------------------------------------------------------------    
    //logf( (char*)"filtre %0.2f nb %d  fVal %0.2f)", filtre, nb, fVal );

    bDisplayPt_old = bDisplayPt;

    bDisplayCourbeX_old = bDisplayCourbeX;
    bDisplayCourbeY_old = bDisplayCourbeY;

    bDisplayfftX_old = bDisplayfftX;
    bDisplayfftY_old = bDisplayfftY;
    filtre_old = filtre;
    

    int l = pFilename->getTextLenght();
    pFilename->setPos( (getDX()-l)/2, 0 );

    /*
    PanelText*          pAffCourbe;
    PanelText*          pValAffCourbe;
    PanelText*          pAffFFT;
    PanelText*          pCourbeX;
    PanelText*          pCourbeY;
    */
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::clickLeft( int xm, int ym )
{
    logf( (char*)"PanelCourbe::clickLeft( %d, %d )", xm, ym );
    if ( pFiltreVal->isMouseOver(xm, ym ) != NULL )
    {
        logf( (char*)"pFiltreVal" );
    }
    xm_svg = sc2winX(xm);
    ym_svg = sc2winX(ym);
    
    decal_x_svg = decal_x;
    decal_y_svg = decal_y;

    build_unites_text();
    PanelWindow::clickLeft( xm, ym );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::motionLeft( int xm, int ym )
{
    //logf( (char*)"PanelCourbe::motionLeft( %d, %d )", xm, ym );
    
    decal_x = decal_x_svg + (float)(xm_svg - sc2winX(xm)) / courbe1;
    decal_y = decal_y_svg + (float)(ym_svg - sc2winX(ym));
    //decal_y +=  ym_svg - ym;

    //logf( (char*)"PanelCourbe::motionLeft( %d, %0.2f )", decal_x, courbe1 );

    if ( decal_x <0 )                        decal_x = 0;
    if ( decal_x >=t_vCourbe.size() )        decal_x = t_vCourbe.size()-1;

    //build_fft3();
    build_unites_text();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::releaseLeft( int xm, int ym )
{
    VarManager& var = VarManager::getInstance();
    logf( (char*)"PanelCourbe::releaseLeft( %d, %d )", xm, ym );

    //motionLeft(xm, ym);
    
    var.set( "decal_x", decal_x );
    var.set( "decal_y", decal_y );

    logf( (char*)"  decal   %d, %d )", decal_x, decal_y);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::clickMiddle( int xm, int ym )
{
    logf( (char*)"PanelCourbe::clickMiddle( %d, %d )", xm, ym );

    xm_svg = sc2winX(xm);
    ym_svg = sc2winY(ym);

    courbe1_svg = courbe1;
    courbe2_svg = courbe2;
    delta_courbe1_svg = delta_courbe1;
    delta_courbe2_svg = delta_courbe2;

    decal_y_svg = decal_y;
    
    build_unites_text();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::motionMiddle( int xm, int ym )
{
    //logf( (char*)"PanelCourbe::motionMiddle( %d, %d )", sc2winX(xm), sc2winY(ym) );

    float deltaX = sc2winX(xm) - xm_svg;
    float deltaY = sc2winY(ym) - ym_svg;
    
    deltaX = 1.0 + 2.0 * deltaX / ((float)getDX()/2.0);
    deltaY = 1.0 - 2.0 * deltaY / ((float)getDY()/2.0);

    if ( deltaX < 0.1 )         return;
    if ( deltaY < 0.1 )         return;
    
    courbe1 = deltaX * courbe1_svg;
    courbe2 = deltaX * courbe2_svg;

    delta_courbe1 = deltaY * delta_courbe1_svg;
    delta_courbe2 = deltaY * delta_courbe2_svg;
    
    decal_y = deltaY * decal_y_svg;

    build_unites_text();
    //logf( (char*)"  %0.2f, %0.2f )", courbe1, courbe2 );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::releaseMiddle( int xm, int ym )
{
    VarManager& var = VarManager::getInstance();
    logf( (char*)"PanelCourbe::releaseMiddle( %d, %d )", xm, ym );

    motionMiddle( xm, ym );

    logf( (char*)"  courbe        %0.2f, %0.2f )", courbe1, courbe2 );
    logf( (char*)"  delta_courbe  %0.2f, %0.2f )", delta_courbe1, delta_courbe2 );

    var.set("courbe1", courbe1);
    var.set("delta_courbe1", delta_courbe1);
    var.set("courbe2", courbe2);
    var.set("delta_courbe2", delta_courbe2);
    
    //courbe1 = delta_courbe1_svg;
    build_unites_text();
}
//--------------------------------------------------------------------------------------------------------------------
//
// Cooley-Tukey FFT (in-place, breadth-first, decimation-in-frequency)
// Better optimized but less intuitive
// !!! Warning : in some cases this code make result different from not optimased version above (need to fix bug)
// The bug is now fixed @2017/05/30 
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::fft3(CArray &x)
{
	// DFT
	unsigned int N = x.size(), k = N, n;
	float thetaT = 3.14159265358979323846264338328L / N;
	CComplex phiT = CComplex(cos(thetaT), -sin(thetaT)), T;
	while (k > 1)
	{
		n = k;
		k >>= 1;
		phiT = phiT * phiT;
		T = 1.0L;
		for (unsigned int l = 0; l < k; l++)
		{
			for (unsigned int a = l; a < N; a += n)
			{
				unsigned int b = a + k;
				CComplex t = x[a] - x[b];
				x[a] += x[b];
				x[b] = t * T;
			}
			T *= phiT;
		}
	}
	// Decimate
	unsigned int m = (unsigned int)log2(N);
	for (unsigned int a = 0; a < N; a++)
	{
		unsigned int b = a;
		// Reverse bits
		b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
		b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
		b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
		b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
		b = ((b >> 16) | (b << 16)) >> (32 - m);
		if (b > a)
		{
			CComplex t = x[a];
			x[a] = x[b];
			x[b] = t;
		}
	}
	//// Normalize (This section make it not working correctly)
	//Complex f = 1.0 / sqrt(N);
	//for (unsigned int i = 0; i < N; i++)
	//	x[i] *= f;
}
//--------------------------------------------------------------------------------------------------------------------
//
// inverse fft (in-place)
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::ifft3(CArray& x)
{
    // conjugate the complex numbers
    x = x.apply(std::conj);
 
    // forward fft
    fft3( x );
 
    // conjugate the complex numbers again
    x = x.apply(std::conj);
 
    // scale the numbers
    x /= x.size();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::build_fft3()
{
    //logf( (char*)"PanelCourbe::build_fft3()");
    //if (  <256 )        return;
    //if ( iDisplayfft==0 )               return;    
    if ( t_vCourbe.size() <= 256 )      return;
    
    t_fOut.clear();
    
    nb = 1;
    while (  nb *2 < t_vCourbe.size() ) nb *= 2;
    if ( nb >256 )      nb = 256;


    t_fOut.resize(nb);
    
    int nn = t_vCourbe.size();
    int idx, max;
    max = t_vCourbe.size();

    for (int i=0; i<nb; i++ )
    {
        idx = nn-i-1-decal_x;
        
        if ( idx < max && idx >= 0)   {
            signalX[i] = complex<float>((float)t_vCourbe[idx].v.x, 0.0);
            signalY[i] = complex<float>((float)t_vCourbe[idx].v.y, 0.0);
        }
        else    {
            signalX[i] = complex<float>(0.0, 0.0);
            signalY[i] = complex<float>(0.0, 0.0);
        }
    }
    fft3( signalX );
    fft3( signalY );
    
    int f = nb/2;
    float normalize = 1.0/sqrt(nb);
    
    for (int i=0, j=0; i<nb; i++ )    {
        if ( i>nb/2)        j--;
        else                j++;
        
        t_fOut[nb/2-j] = -sqrt(norm(signalX[i])) * normalize  ;
    }

    pOut = (float*)&t_fOut[0];

    signal_inverseX = signalX;
    signal_inverseY = signalY;

    sinusoide_fft3(signal_inverseX, (float*)inverseX);
    sinusoide_fft3(signal_inverseY, (float*)inverseY);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::sinusoide_fft3(CArray& x, float* t)
{
    if ( filtre == 0.0 )         filtre = 0.5;
    
    int n = round((float)nb/filtre);
    for( int i=0+n; i<nb-n; i++ )
    {
        x[i]     = CComplex(0.0,0.0);
    }
    
    ifft3( x );
    
    for( int i=0; i<nb; i++ )
    {
        t[nb-1-i] = real(x[i]);
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#endif
