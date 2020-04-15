#ifndef PANELCOURBE_CPP
#define PANELCOURBE_CPP

#include "panel_courbe.h"
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
vec4 color_Y     = vec4(1.0,1.0,0.0,1.0);
vec4 color_X     = vec4(0.0,0.0,1.0,1.0);
vec4 color_FFT   = vec4(0.0,1.0,0.0,0.7);
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
    
    //setVisible(bPanelCourbe);
 	setBackground((char*)"images/background.tga");
    //setDisplayGL( PanelCourbe::displayResultat_cb );

    pXMax = new PanelText( (char*)"+err",		PanelText::NORMAL_FONT, 5, 50 );
	add( pXMax );

    pXMin = new PanelText( (char*)"-err",		PanelText::NORMAL_FONT, 5, 60 );
	add( pXMin );

    
    pYMax = new PanelText( (char*)"+err",		PanelText::NORMAL_FONT, 5, 70 );
	//add( pYMax );

    pYMin = new PanelText( (char*)"-err",		PanelText::NORMAL_FONT, 5, 80 );
	//add( pYMin );
    
    err = var.getf("err");

    courbe1         = var.getf("courbe1");
    delta_courbe1   = var.getf("delta_courbe1");
    courbe2         = var.getf("courbe2");
    delta_courbe2   = var.getf("delta_courbe2");

    vOrigine.x      = var.getf("vOrigine.x");
    vOrigine.y      = var.getf("vOrigine.y");
    vOrigine.z      = 0.0;

    //iDisplayfft     = var.geti("iDisplayfft");

    taille_mini     = var.getf("taille_mini_unite");

    iDisplayfft     = var.geti("iDisplayfft");
    aff_courbe      = var.geti("aff_courbe");
    filtre          = var.getf("filtre");



    ech_w           = 1.0;
    ech_h           = 1.0;
    aff_courbe_old  = -1;
    iDisplayfft_old = -1;//Displayfft_old;
    filtre_old      = -1;

    update_err();

    signalX.resize(256);
    signalY.resize(256);
    signal_inverseX.resize(256);
    signal_inverseY.resize(256);

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
    
    decal_resultat = var.geti( "decal_resultat" );
    
    setVisible( var.getb( "bPanelCourbe" ) );

    build_unites_text();

    log_tab(false);
    logf( (char*)"PanelCourbe::PanelCourbe() Constructeur" );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::init_panel()
{
    int x = 800;
    int y = 0;
    int dy = 15;
    
    pAffCourbe      = new PanelText( (char*)"--",   PanelText::NORMAL_FONT, x, y++*dy );
    pAffFFT         = new PanelText( (char*)"On",   PanelText::NORMAL_FONT, x, y++*dy );
    pCourbeX        = new PanelText( (char*)"On",   PanelText::NORMAL_FONT, x, y++*dy );
    pCourbeY        = new PanelText( (char*)"On",   PanelText::NORMAL_FONT, x, y++*dy );
    
    pFilename       = new PanelText( (char*)"",     PanelText::NORMAL_FONT, 200, 0 );
    
    add( pAffCourbe );
    add( pAffFFT );
    add( pCourbeX );
    add( pCourbeY );
    add( pFilename );
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
            sup(unites[i]);
            delete unites[i];
            unites[i] = 0;
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
    if (!var.existe("decal_resultat"))              var.set("decal_resultat", 0 );
    if (!var.existe("taille_mini_unite"))           var.set("taille_mini_unite", (float)20.0);
    
    if (!var.existe("xPanelCourbe") )               var.set( "xPanelCourbe", 10);
    if (!var.existe("yPanelCourbe") )               var.set( "yPanelCourbe", 10);
    if (!var.existe("dxPanelCourbe") )              var.set("dxPanelCourbe", 400);
    if (!var.existe("dyPanelCourbe") )              var.set("dyPanelCourbe", 400);

    if (!var.existe("iDisplayfft") )                var.set("iDisplayfft", 0);
    if (!var.existe("aff_courbe") )                 var.set("aff_courbe", 0);
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
        float rx, ry, ox, oy;
        sscanf( line.c_str(), "( %f , %f ) / ( %f , %f )", &rx, &ry, &ox, &oy );

        vOrigine.x = ox;
        vOrigine.y = oy;

        t_vResultat.push_back( vec2(rx,ry) );
        
        t_vCourbe.push_back( vec2(vec2(rx,ry) - vec2(ox, oy)) );

        nbLigne++;
    }    
    int n = t_vResultat.size();
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

        t_vResultat.push_back( vec2(rx,ry) );
        
        t_vCourbe.push_back( vec2(vec2(rx,ry) - vec2(ox, oy)) );

        nbLigne++;
        if ( !getline (fichier, line) )     break;
    }    
    
    
    int n = t_vResultat.size();
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
    
    t_vResultat.clear();
    t_vCourbe.clear();
    decal_resultat = 0;

    charge_guidage_version( fichier );

    fichier.close();
    
    t_vSauve.clear();
    
    build_fft3();

    log_tab(false);
    logf( (char*)"PanelCourbe::charge_guidage('%s')", (char*)filename.c_str() );
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::sauve_guidage_1_1()
{
    //string filename = "/home/rene/.astropilot/sauvegarde.txt";
    logf( (char*)"Sauvegarde des valeurs dans '%s'", (char*)filenameSauve.c_str() );
    
    std::ofstream fichier;
 
    fichier.open(filenameSauve, std::ios_base::app);

    if ( !fichier ) 
    {
        logf( (char*)"[ERROR]impossble d'ouvrir : '%s'", (char*)filenameSauve.c_str() );
    }

    for(int i=0; i<t_vSauve.size(); i++)
    {
        fichier << "( " << t_vSauve[i].x << " , " <<  t_vSauve[i].y << " ) / ";
        fichier << "( " << vOrigine.x << " , " <<  vOrigine.y << " )\n";
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
        fichier << "( " << t_vSauve[i].x << " , " <<  t_vSauve[i].y << " ) / ";
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
    sauve_guidage_1_0();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::idle_guidage(vec2  v)
{
    if ( t_vResultat.size()>200000)      t_vResultat.clear();

    if ( t_vResultat.size() > 20000 )
    {
        t_vResultat.erase ( t_vResultat.begin()+0);
    }

    vec2 o = vec2(xSuivi, ySuivi);
    ajoute( v-o );

    if ( t_vSauve.size() >= 200 ) {
        if ( bSauve )           sauve_guidage();
        logf( (char*)"Remise a zero t_vSauve");
        t_vSauve.clear();
    }

    t_vSauve.push_back(v);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::reset_guidage()
{
    logf( (char*)"reset_guidage()" );
    
    t_vResultat.clear();
    t_vCourbe.clear();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::ajoute(vec2 v)
{
    t_vCourbe.push_back(vec2(v));
    if ( iDisplayfft >= 1 )           build_fft3();
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
        colorLimit  = vec4( 1.0, 0.0, 1.0, 1.0 );
        colorAxe    = vec4( 1.0, 1.0, 1.0, 1.0 );
    }
    
    glBegin(GL_LINES);

        //
        // graduation horizontale
        //
        glColor4fv( (GLfloat*)&color );
        int pas = delta_courbe1*ech_h;
        float fPas = delta_courbe1*ech_h;
        while( fPas < taille_mini )     fPas += courbe1*ech_w;

        for( float i=0; i<SIZE/2; i+=fPas )
        {
            int x = xStartAxe;
            int y = AXE + i;
            xy2Screen(x, y);
            glVertex2i( x, y );

            x += getPosDX()-xStartAxe;
            glVertex2i( x, y );

            x = xStartAxe;
            y = AXE - i;
            xy2Screen(x, y);
            glVertex2i( x, y );

            x += getPosDX()-xStartAxe;
            glVertex2i( x, y );
        }
        //
        // graduation verticale
        //
        fPas = courbe1*ech_w;
        while( fPas < 10.0 )     fPas += courbe1*ech_w;
        float M = getPosDX();
        for( float i=xStartAxe; i<M; i+=fPas )
        {
            int x = i;
            int y = AXE;
            xy2Screen(x,y);
            y -= SIZE/2;
            glVertex2i( x, y );
            
            y += SIZE;
            glVertex2i( x, y );
        }
        //
        // axe horizontale centrale
        //
        glColor4fv( (GLfloat*)&colorAxe );
        int y = AXE;
        int x = xStartAxe;

        xy2Screen(x, y);
        glVertex2i( x, y );

        x += getPosDX()-xStartAxe;
        glVertex2i( x, y );
        //
        // Limite horizontale
        // Min et Max
        //
        glColor4fv( (GLfloat*)&colorLimit );

        y = (float)(delta_courbe1*(min)*ech_h + AXE);
        if ( pMax != NULL )     pMax->setPos( 5, y-7 );
        x = xStartAxe;
        xy2Screen(x, y);
        glVertex2i( x, y );

        x += getPosDX()-xStartAxe;
        glVertex2i( x, y );

        y = (float)(delta_courbe1*(max)*ech_h + AXE);
        if ( pMin != NULL )     pMin->setPos( 5, y-7 );
        x = xStartAxe;
        xy2Screen(x, y);
        glVertex2i( x, y );

        x += getPosDX()-xStartAxe;
        glVertex2i( x, y );

        glColor4fv( (GLfloat*)&colorAxe );
    glEnd();        
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
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::glCourbe( float* tab, int size, int pas, int xStart, int xdecal, 
                            int y_zero, float offset, float a, float b )
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

            int Y = y;

            x2Screen(x);
            y2Screen(Y);

            glVertex2i( x, Y );
        }
        glEnd();                
    }     
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::glCourbe( float* tab, int size, int pas, int xStart, int xdecal, int y_zero, float offset )
{
    glCourbe( tab, size, pas, xStart, xdecal, y_zero, offset, courbe1, delta_courbe1 );
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
    
    if ( iDisplayfft < 2 )
    {
        if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 1.0 );
        else                            glColor4fv( color_X );
        
        if ( (aff_courbe ==0x0) || (aff_courbe==0x02) ) {
           glCourbe(    tabx, t_vCourbe.size(), 2, xStartAxe, decal_resultat, getDY()/2, 0.0,
                        courbe1*ech_w, delta_courbe1*ech_h );
        }

        if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 1.0 );
        else                            glColor4fv( color_Y );
        
        if ( (aff_courbe==0x0) || (aff_courbe==0x03) ) {
            glCourbe( taby, t_vCourbe.size(), 2, xStartAxe, decal_resultat, getDY()/2, 0.0,
                        courbe1*ech_w, delta_courbe1*ech_h );
        }
    }
        
    if ( iDisplayfft > 0 )
    {
        glLineWidth(3.0);
        if ( (aff_courbe ==0x0) || (aff_courbe==0x02) ) {
            if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 1.0 );
            else                            glColor4fv( color_FFT_X );
            glCourbe( inverseX, nb, 1, xStartAxe, 0, getDY()/2, 0.0, courbe1*ech_w, delta_courbe1*ech_h );
        }

        if ( (aff_courbe==0x0) || (aff_courbe==0x03) ) {
            if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 1.0 );
            else                            glColor4fv( color_FFT_Y );
            glCourbe( inverseY, nb, 1, xStartAxe, 0, getDY()/2, 0.0, courbe1*ech_w, delta_courbe1*ech_h );

        }
        glLineWidth(1.0);
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::glFft()
{
    VarManager& var = VarManager::getInstance();

    if ( var.getb("bNuit") )            glColor4f( 1.0, 0.0, 0.0, 0.35 );
    else                                glColor4fv( color_FFT );

    glCourbeCube(   pOut, nb/2, 1, (int)xStartAxe, 0, (int)getDY()-20 , (float)0.0, 
                    (float)(getDX()-xStartAxe)/((float)nb/2.0), (float)delta_courbe1*ech_h );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::build_unites_text()
{
    //logf( (char*)"PanelCourbe::build_unites_text()" );
    //log_tab(true);

    float dy= (float)getDY() / 2.0;

    // Construction du tableau d'unités
    int     pas, y;
    float   fPas;
    string  s;

    //logf( (char*)"delta_courbe1 = %0.2f", delta_courbe1 ) ;

    pas  = delta_courbe1*ech_h;
    fPas = delta_courbe1*ech_h;
    while( fPas < taille_mini )            
        fPas += courbe1*ech_w;

    int idx = 0;




    for( float i=0; i<dy; i+=fPas )
    {
        y = (-i)/delta_courbe1/ech_h;
        s = "" + to_string((int)round(y)) + " px";
        y2Screen(y);

        if ( idx == unites.size() )         { unites.push_back( new PanelText((char*)" ",PanelText::NORMAL_FONT) ); add( unites[idx] ); }
        unites[idx]->changeText( (char*)s.c_str() );
        unites[idx]->setPos(25,dy+i-8);
        unites[idx]->setVisible(true);
        idx++;

        if ( i == 0 )   continue;

        y = (+i)/delta_courbe1/ech_h;
        s = "" + to_string(y) + " px";
        y2Screen(y);

        if ( idx == unites.size() )         { unites.push_back( new PanelText((char*)" ",PanelText::NORMAL_FONT) ); add( unites[idx] ); }
        unites[idx]->changeText( (char*)s.c_str() );
        unites[idx]->setPos(25,dy-i-8);
        unites[idx]->setVisible(true);
        idx++;
    }
    
    for( int i=idx; i<unites.size(); i++ )
        unites[i]->setVisible(false);


            //unites.push_back(  new PanelText( (char*)s.c_str(),		PanelText::NORMAL_FONT, 25, dy+i-8 ) );
    //log_tab(false);
    //logf( (char*)"PanelCourbe::build_unites_text()" );
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



    glEchelle();
    
    PanelWindow::displayGL();

    int DY = getY();
    //int n = t_vResultat.size();
    

    float height = (float)wm.getHeight();
	int   scx  = getX();
	int   scy  = height - getDY() - getY();
	int   scdx = getDX();
	int   scdy = getDY();
    
    glScissor( scx, scy, scdx, scdy );
    glEnable( GL_SCISSOR_TEST );

    glCourbes();

    if ( iDisplayfft > 0 )  {
        build_fft3();
        if (pOut != NULL)       glFft();
    }
    
    glDisable( GL_SCISSOR_TEST );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::updatePos()
{
    PanelSimple::updatePos();


	WindowsManager& wm = WindowsManager::getInstance();
    
    //      Dimension ecran
    float wSc = (float)wm.getWidth();
    float hSc = (float)wm.getHeight();
    ech_w = (float)getDX() / wSc;
    ech_h = (float)getDY() / hSc;
    
    
    pCourbeX->setPos(       getDX()-100, pCourbeX->getPosY() );
    pCourbeY->setPos(       getDX()-100, pCourbeY->getPosY() );
    pAffCourbe->setPos(     getDX()-100, pAffCourbe->getPosY() );
    pAffFFT->setPos(        getDX()-100, pAffFFT->getPosY() );
    //logf( (char*)"updatePos()" );

    char s[255];
    //--------------------------------------------------------------    
    if ( aff_courbe_old != aff_courbe ) {
        //--------------------------------------------------------------    
        if ( (aff_courbe ==0x0) || (aff_courbe==0x02) )    {
            sprintf( s, (char*)"Abcisse X On" );
        } else {
            sprintf( s, (char*)"Abcisse X Off" );
        }
        pCourbeX->changeText( s, true );
        //--------------------------------------------------------------    
        if ( (aff_courbe ==0x0) || (aff_courbe==0x03) )    {
            sprintf( s, (char*)"Ordonnee Y On" );
        } else {
            sprintf( s, (char*)"Ordonnee Y Off" );
        }
        pCourbeY->changeText( s, true );

        if ( (aff_courbe ==0x0) || (aff_courbe==0x02) )     pCourbeX->setColor( (unsigned long)0xff0808ff );
        else                                                pCourbeX->setColor( (unsigned long)0x800000ff );

        if ( (aff_courbe ==0x0) || (aff_courbe==0x03) )     pCourbeY->setColor( (unsigned long)0xffffff00 );
        else                                                pCourbeY->setColor( (unsigned long)0x80ffff00 );
    }
    //--------------------------------------------------------------    
    if ( iDisplayfft != iDisplayfft_old )  {
        if ( iDisplayfft>0 )  {
            sprintf( s, (char*)"FFT On" );
        } else {
            sprintf( s, (char*)"FFT Off" );
        }
        pAffFFT->changeText( s, true );

        if ( iDisplayfft>0 )            pAffFFT->setColor( (unsigned long)0xff00ff00 );
        else                            pAffFFT->setColor( (unsigned long)0x8000ff00 );
    }
    //--------------------------------------------------------------    
    if ( filtre_old != filtre )
    {
        sprintf( s, (char*)"Filtre = %0.0f", 256.0/filtre );
        pAffCourbe->changeText( s, true );
    }
    //--------------------------------------------------------------    
    aff_courbe_old = aff_courbe;
    iDisplayfft_old = iDisplayfft;
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
void PanelCourbe::clickMiddle( int xm, int ym )
{
    logf( (char*)"PanelCourbe::clickMiddle( %d, %d )", xm, ym );

    xm_svg = sc2winX(xm);
    ym_svg = sc2winY(ym);

    courbe1_svg = courbe1;
    courbe2_svg = courbe2;
    delta_courbe1_svg = delta_courbe1;
    delta_courbe2_svg = delta_courbe2;
    
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
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::clickLeft( int xm, int ym )
{
    logf( (char*)"PanelCourbe::clickLeft( %d, %d )", xm, ym );

    xm_svg = sc2winX(xm);
    ym_svg = sc2winY(ym);
    
    decal_svg = decal_resultat;

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::motionLeft( int xm, int ym )
{
    //logf( (char*)"PanelCourbe::motionLeft( %d, %d )", xm, ym );
    
    decal_resultat = decal_svg + (float)(xm_svg - sc2winX(xm)) / courbe1;

    //xm_svg = sc2winX(xm);
    //ym_svg = sc2winY(ym);

    
    //logf( (char*)"PanelCourbe::motionLeft( %d, %0.2f )", decal_resultat, courbe1 );


    if ( decal_resultat <0 )                        decal_resultat = 0;
    if ( decal_resultat >=t_vResultat.size() )      decal_resultat = t_vResultat.size()-1;

    //build_fft3();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::releaseLeft( int xm, int ym )
{
    VarManager& var = VarManager::getInstance();
    logf( (char*)"PanelCourbe::releaseLeft( %d, %d )", xm, ym );

    var.set( "decal_resultat", decal_resultat );
    //build_fft3();
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
    if ( iDisplayfft==0 )           return;    
    if ( t_vCourbe.size() < 32 )    return;
    
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
        idx = nn-i-1-decal_resultat;
        
        if ( idx < max && idx >= 0)   {
            signalX[i] = complex<float>((float)t_vCourbe[idx].x, 0.0);
            signalY[i] = complex<float>((float)t_vCourbe[idx].y, 0.0);
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
    int n = (float)nb/filtre;
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
