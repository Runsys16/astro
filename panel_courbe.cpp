#ifndef PANELCOURBE_CPP
#define PANELCOURBE_CPP

#include "panel_courbe.h"
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

    taille_mini       = var.getf("taille_mini_unite");

    update_err();

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
        color       = vec4( 0.15, 0.0, 0.0, 1.0 );    
        colorLimit  = vec4( 0.8, 0.0, 0.0, 1.0 );    
        colorAxe    = vec4( 1.0, 0.0, 0.0, 1.0 );
    }
    else                            
    {
        gris = 0.15;
        color       = vec4( gris, gris, gris, 1.0 );
        colorLimit  = vec4( 1.0, 0.0, 1.0, 1.0 );
        colorAxe    = vec4( 1.0, 1.0, 1.0, 1.0 );
    }
    
    glBegin(GL_LINES);

        //
        // graduation horizontale
        //
        glColor4fv( (GLfloat*)&color );
        int pas = delta_courbe1;
        float fPas = delta_courbe1;
        while( fPas < taille_mini )     fPas += courbe1;

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
        fPas = courbe1;
        while( fPas < 10.0 )     fPas += courbe1;
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

        y = (float)(delta_courbe1*(min) + AXE);
        if ( pMax != NULL )     pMax->setPos( 5, y-7 );
        x = xStartAxe;
        xy2Screen(x, y);
        glVertex2i( x, y );

        x += getPosDX()-xStartAxe;
        glVertex2i( x, y );

        y = (float)(delta_courbe1*(max) + AXE);
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
void PanelCourbe::glCourbe( float* tab, int size, int pas, int xStart, int xdecal, int y_zero, float offset )
{
    if ( size != 0  )
    {
        glBegin(GL_LINE_STRIP);
        int deb = xdecal;
        int fin = deb + 2000.0/courbe1;
        int n = size;
        
               
        if ( fin >= size )                  fin = size;
        if ( deb <= 0 )                     deb = 0;

        for( int i=deb; i<fin; i++ )
        {
            float y = tab[(n-i-1)*pas] - offset;
            y = (float)(delta_courbe1*(y) + y_zero);
            
            int x = (i-deb)*courbe1 + xStart;
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
void PanelCourbe::glCourbes()
{
	WindowsManager&     wm  = WindowsManager::getInstance();
    VarManager& var = VarManager::getInstance();

    int DY = getY();
    int n = t_vResultat.size();
    

    float height = (float)wm.getHeight();
	int   scx  = getX();
	int   scy  = height - getDY() - getY();
	int   scdx = getDX();
	int   scdy = getDY();
    
    glScissor( scx, scy, scdx, scdy );
    glEnable( GL_SCISSOR_TEST );

    offset_x = vOrigine.x;
    offset_y = vOrigine.y;
    
    float* tabx = (float*)&t_vResultat[0];
    float* taby = tabx+1;
    
    if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 1.0 );
    else                            glColor4f( 0.0, 0.0, 1.0, 1.0 );
    
    glCourbe( tabx, t_vResultat.size(), 2, xStartAxe, decal_resultat, getDY()/2, offset_x );

    if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 1.0 );
    else                            glColor4f( 1.0, 1.0, 0.0, 1.0 );
    
    glCourbe( taby, t_vResultat.size(), 2, xStartAxe, decal_resultat, getDY()/2, offset_y );
    
    
    
    
    glDisable( GL_SCISSOR_TEST );
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

    pas  = delta_courbe1;
    fPas = delta_courbe1;
    while( fPas < taille_mini )            
        fPas += courbe1;

    int idx = 0;




    for( float i=0; i<dy; i+=fPas )
    {
        y = (-i)/delta_courbe1;
        s = "" + to_string((int)round(y)) + " px";
        y2Screen(y);

        if ( idx == unites.size() )         { unites.push_back( new PanelText((char*)" ",PanelText::NORMAL_FONT) ); add( unites[idx] ); }
        unites[idx]->changeText( (char*)s.c_str() );
        unites[idx]->setPos(25,dy+i-8);
        unites[idx]->setVisible(true);
        idx++;

        if ( i == 0 )   continue;

        y = (+i)/delta_courbe1;
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
//void PanelCourbe::displayResultat_cb(void)
void PanelCourbe::displayGL(void)
{
	if ( !visible )			return;

    VarManager& var = VarManager::getInstance();

    if ( var.getb("bNuit") )        glColor4f( 1.0, 0.0, 0.0, 1.0 );
    else                            glColor4f( 1.0, 1.0, 0.0, 1.0 );

    PanelWindow::displayGL();

    //displayGLTrace();

    glEchelle();
    glCourbes();
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

}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::motionLeft( int xm, int ym )
{
    //logf( (char*)"PanelCourbe::motionLeft( %d, %d )", xm, ym );
    
    decal_resultat += 1.0/courbe1 * (float)(xm_svg - sc2winX(xm));

    xm_svg = sc2winX(xm);
    ym_svg = sc2winY(ym);

    
    //logf( (char*)"PanelCourbe::motionLeft( %d, %0.2f )", decal_resultat, courbe1 );


    if ( decal_resultat <0 )                        decal_resultat = 0;
    if ( decal_resultat >=t_vResultat.size() )      decal_resultat = t_vResultat.size()-1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::releaseLeft( int xm, int ym )
{
    VarManager& var = VarManager::getInstance();
    logf( (char*)"PanelCourbe::releaseLeft( %d, %d )", xm, ym );

    var.set( "decal_resultat", decal_resultat );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelCourbe::fft(double* data, unsigned long nn)
{
    unsigned long n, mmax, m, j, istep, i;
    double wtemp, wr, wpr, wpi, wi, theta;
    double tempr, tempi;
 
    // reverse-binary reindexing
    n = nn<<1;
    j=1;
    for (i=1; i<n; i+=2) {
        if (j>i) {
            swap(data[j-1], data[i-1]);
            swap(data[j], data[i]);
        }
        m = nn;
        while (m>=2 && j>m) {
            j -= m;
            m >>= 1;
        }
        j += m;
    };
 
    // here begins the Danielson-Lanczos section
    mmax=2;
    while (n>mmax) {
        istep = mmax<<1;
        theta = -(2*M_PI/mmax);
        wtemp = sin(0.5*theta);
        wpr = -2.0*wtemp*wtemp;
        wpi = sin(theta);
        wr = 1.0;
        wi = 0.0;
        for (m=1; m < mmax; m += 2) {
            for (i=m; i <= n; i += istep) {
                j=i+mmax;
                tempr = wr*data[j-1] - wi*data[j];
                tempi = wr * data[j] + wi*data[j-1];
 
                data[j-1] = data[i-1] - tempr;
                data[j] = data[i] - tempi;
                data[i-1] += tempr;
                data[i] += tempi;
            }
            wtemp=wr;
            wr += wr*wpr - wi*wpi;
            wi += wi*wpr + wtemp*wpi;
        }
        mmax=istep;
    }
}
/*
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

constexpr double PI = 3.141592653589793;
 
std::complex<double> w(int nk, int N) {
    return cos(2*PI*nk/N) - (std::complex<double>(0,1))*sin(2*PI*nk/N);
}
 
unsigned int reverseNum(unsigned int num, unsigned int pos) {
    unsigned int result = 0;
 
    for (unsigned int i(0); i < pos; i++) {
        if((num & (1 << i)))
           result |= 1 << ((pos - 1) - i);
    }
    return result;
}
 
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void fft(std::vector<std::complex<double>>& signal, unsigned int end_, unsigned int start = 0) {
    if(end_ - start > 1) {
        unsigned int len = end_ - start;
 
        for(unsigned int i(0); i < len / 2; i++) {
            std::complex<double> temp(signal[start+i]);
            signal[start+i] += signal[start+i+(len/2)];
            signal[start+i+(len/2)] = (temp - signal[start+i+(len/2)])*w(i,len);
        }
 
        fft(signal,start+len/2,start);
        fft(signal,end_,start+len/2);
    }
    else if(end_ == signal.size()) {
        std::vector<std::complex<double>> temp(signal);
        unsigned int power(ceil(log(end_)/log(2)));
 
        for(unsigned int i(0); i < end_;i++) {
            signal[(i+end_/2)%end_] = temp[reverseNum(i,power)];
        }
    }
}
 
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int main() {
    std::vector<std::complex<double>> signal(128);
 
    for(unsigned int i(0); i < signal.size(); i++)
        signal[i] = sin(2*PI*2*i/(double)(signal.size())); // Calcule une sinusoide de 2 Hz
 
    std::vector<std::complex<double>> spectrum(signal);
 
    fft(spectrum,spectrum.size()); // Calcul de la FFT
 
    for(unsigned int i(0); i < spectrum.size(); i++)
        std::cout << std::fixed << std::setprecision(2) << abs(spectrum[i]) << std::endl;
 
    return 0;
}
*/
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#endif
