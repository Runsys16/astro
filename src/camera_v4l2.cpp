/*
 *  V4L2 video capture example
 *
 *  This program can be used and distributed without restrictions.
 *
 *      This program is provided with the V4L2 API
 * see http://linuxtv.org/docs.php for more information
 */


#include "camera_v4l2.h"
#include "exif_jpg.h"


long memory = 0;
bool bReadError = false;

struct v4l2_queryctrl queryctrl;
struct v4l2_querymenu querymenu;

//--------------------------------------------------------------------------------------------------------------------
//
//						Constructeurs ....
//
//--------------------------------------------------------------------------------------------------------------------
CameraV4L::CameraV4L()
{
    logf_thread( (char*)"Constructeur CameraV4L() -------------" );
    dev_name        = "";
    io              = IO_METHOD_USERPTR;
    fd              = -1;
    out_buf         = 1;
    force_format    = MJPG;
    frame_count     = 1;
    frame_number    = 7;
    strErr          = (char*)calloc( 200, 1 );
    memset(strErr, 0, 199);
    nControl        = 0;

    width           = -1;
    height          = -1;
    pixelformat     = -1;
    nSize           = 0;
    sizeChoix       = 0;

    bEnregistre     = false;
    bCamera			= true;
    bCapture        = false;
    bmp_buffer      = NULL;
    
    bHaveNew        = false;
    
    name ="";
    dirname			= "/home/rene/.astropilot/capture/";
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraV4L::set_filtre_name( string& s )
{
    name = "";

	for ( int i = 0; i<s.length(); i++ )
	{
		unsigned long l = (unsigned long)s[i];
		if ( l > 127 )	{
			logf_thread( (char*)"non ascii" );
			continue;
		}
		if ( l < 32 )	break;
		if ( l == 32 )		s[i] = '_';

		name = name + s[i];
	}
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool CameraV4L::isDevice( int u )
{
    char    dev_name[] = "/dev/video000";
    while( u>99)    u -= 100;

    sprintf( dev_name, "/dev/video%d", u );

    struct stat buf;
    if ( 0 == stat(dev_name, &buf)    )
    {
        return true;
    }

    return false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool CameraV4L::getIOCapability()
{
    logf_thread( (char*)"CameraV4L::getIOCapability()" );
    log_tab(true);
    if (fd == -1)
    {
        name = "";
	    log_tab(false);
        return false;
    }

    struct v4l2_capability cap;
    memset(&cap, 0, sizeof(v4l2_capability));
    
    if(ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1)
    {
        perror("cam_info: Can't get capabilities");
        name = "";
	    log_tab(false);
        return false;
    } 
    else
    {
        string s = (char*)cap.card;
        int n = s.find(':');
        if ( n>0 )      s[n] = 0;
    
        set_filtre_name( s );

        logf_thread( (char*)"Device Name: '%s'", name.c_str() );
        
        if ( cap.device_caps & V4L2_CAP_VIDEO_CAPTURE )	{
	        logf_thread( (char*)"Device capture : cap.capabilities 0x%08x", (uint32_t)cap.device_caps );
	    }
        else	{
	        logf_thread( (char*)"Device commande : cap.capabilities 0x%08x", (uint32_t)cap.device_caps );
		    log_tab(false);
	        return false;
        }
    }
    log_tab(false);
    return true;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraV4L::enumerate_menu(__u32 id)
{
    if (fd == -1)       return;
    //printf("  Menu items:\n");

    memset(&querymenu, 0, sizeof(querymenu));
    querymenu.id = id;

    for (querymenu.index = queryctrl.minimum;
         querymenu.index <= queryctrl.maximum;
         querymenu.index++) {
        if (0 == ioctl(fd, VIDIOC_QUERYMENU, &querymenu)) {
            //printf("  %d-%s\n", querymenu.index, querymenu.name);
            logf_thread((char*)"\t%d-%s", querymenu.index, querymenu.name);
        }
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraV4L::capability_list( void )    {
    if (fd == -1)       return;
    
    logf_thread((char*)"CameraV4L::capability_list()");
    log_tab(true);

    memset(&queryctrl, 0, sizeof(queryctrl));

    
    queryctrl.id = V4L2_CTRL_FLAG_NEXT_CTRL;
    while (0 == ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl)) {
        if (!(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)) {
            //Control pC = new Control(this);
            pControl.push_back(new Control(this));
            pControl[nControl]->init( &queryctrl );
            nControl++;
            
            if (queryctrl.type == V4L2_CTRL_TYPE_MENU)
                enumerate_menu(queryctrl.id);
        }

        queryctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
    }
    if (errno != EINVAL) {
        perror("VIDIOC_QUERYCTRL");
        close_device();
        //exit(EXIT_FAILURE);
    }

    log_tab(false);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraV4L::capability_save( void )    {
    if (fd == -1)       return;

    string filename = "/home/rene/.astropilot/"+ name +".cam";
    capability_save( filename );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraV4L::capability_save( string filename )    {
    if (fd == -1)       return;

    logf_thread( (char*)"Sauvegarde des parametres de la camera dans '%s'", (char*)filename.c_str() );

    ofstream fichier;
    fichier.open(filename, ios_base::trunc);
    //fichier.open(filename);

    if ( !fichier ) 
    {
        logf_thread( (char*)"[ERROR]impossble d'ouvrir : '%s'", (char*)filename.c_str() );
        return;
    }


    int nb = pControl.size();
    for( int i=0; i<nb; i++ )
    {
        logf_thread( (char*)"%s %d", (char*)pControl[i]->getName().c_str(), pControl[i]->getValue() );
        fichier << pControl[i]->getName() << " : " <<  pControl[i]->getValue();
        fichier << "\n";
    }
    
    
    
    fichier.close();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraV4L::capability_load( string filename )    {
    if (fd == -1)       return;

    logf_thread( (char*)"Lecture des parametres de la camera dans '%s'", (char*)filename.c_str() );

    ifstream fichier;
    fichier.open(filename, std::ios_base::app);

    if ( !fichier ) 
    {
        logf_thread( (char*)"[ERROR]impossble d'ouvrir : '%s'", (char*)filename.c_str() );
        return;
    }

    int nbLigne = 0;
    
    while (!fichier.eof()) 
    {
        char output[255];
        string sCtrl;
        string  s = "";
        do
        {
            sCtrl += s;
            fichier >> output;            s = string(output);
            if (fichier.eof())      break;

        } while (s.find(":") == string::npos);
        
        
        fichier >> output;          s = string(output);
        if (fichier.eof())      break;
        
        int val = (int)stof((char*)s.c_str());
        logf_thread( (char*)" %d - %s = %d", nbLigne, (char*)sCtrl.c_str(), val );
        
        Control *   p = getControl( sCtrl );
        if ( p != NULL )            p->setValue( val );
        else                        logf_thread( (char*)"pControl NULL : line %d", __LINE__ );
        
        nbLigne++;
    }    

    logf_thread( (char*)"Lecture de %d lignes", nbLigne );
    
    fichier.close();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraV4L::enum_format()
{
    //-------------------------------------------        
    logf_thread((char*)"CameraV4L::enum_format()");
    log_tab(true);

    logf_thread( (char*)"ioctl =  VIDIOC_ENUM_FMT");
    struct v4l2_fmtdesc fmtdesc;
    CLEAR(fmtdesc);
    
    fmtdesc.index = 0;
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;


    while (-1 != xioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc))
    {
        logf_thread( (char*)"description = %s" , fmtdesc.description );
        logf_thread( (char*)"type = %08X   " , fmtdesc.type );
        logf_thread( (char*)"pixelformat = %08X   " , fmtdesc.pixelformat );
        
        if ( fmtdesc.pixelformat == 0x47504A4D )       
        {
        	pixelformat = fmtdesc.pixelformat;
	        logf_thread( (char*)"Forcage format"	 );
        }
        
        enum_format_size( fmtdesc.pixelformat );

        fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmtdesc.index++;
    }
    log_tab(false);
    logf_thread((char*)"CameraV4L::enum_format() ** END **");
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraV4L::enum_format_size(int pf)
{
    //-------------------------------------------        
    logf_thread((char*)"CameraV4L::enum_format_size(%d)", pf);
    log_tab(true);

    logf_thread( (char*)"ioctl = VIDIOC_ENUM_FRAMESIZES");
    struct v4l2_frmsizeenum frmsizeenum;
    
    CLEAR(frmsizeenum);
    frmsizeenum.index = 0;
    frmsizeenum.pixel_format = pf;
    nSize = 0;
    width = -1;

    while (-1 != xioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frmsizeenum))
    {
        
        //logf_thread( (char*)"Type=%08X size=%dx%d" , frmsizeenum.type, frmsizeenum.discrete.width, frmsizeenum.discrete.height );
        

        //if (pixelformat != -1 ) {
            //logf_thread( (char*)"Format=%08X   \n" , pixelformat );
            //logf_thread( (char*)"Format=%08X   \n" , V4L2_PIX_FMT_BGR24 );
            //logf_thread( (char*)"Format=%08X   \n" , V4L2_PIX_FMT_RGB24 );
            if ( width == -1 )       width  = frmsizeenum.discrete.width;
            if ( height == -1 )      height = frmsizeenum.discrete.height;
            
            struct size s;
            s.width     = frmsizeenum.discrete.width;
            s.height    = frmsizeenum.discrete.height;

            if ( pf == 0x47504A4D )   
            {
		        logf_thread( (char*)"Forcage Size"	 );
                sizeChoix = 1;//tSize.size();
            }
            tSize.push_back(s);
            nSize++;
        //}
        frmsizeenum.index++;
    }
    
    //--------------------------------------------------------------
    // Determine le plus grand format
    int max = 0;
    for( int i=0; i<nSize; i++ )  {
        logf_thread( (char*)"%d- %dx%d",  i, tSize[i].width, tSize[i].height );
        if ( tSize[i].width > max )			max = tSize[i].width;
    }
    //--------------------------------------------------------------
    // on choisi le format le plus grand
    /*
    for( int i=0; i<nSize; i++ )  {
        if ( tSize[i].width == max )			sizeChoix = i;
    }
    */


    logf_thread( (char*)"Resolution courante %dx%d",  width, height );

    log_tab(false);
    logf_thread((char*)"CameraV4L::enum_format_size() ** END **");
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraV4L::def_crop( int w, int h )
{
    struct v4l2_crop crop;
    struct v4l2_crop cropcap;
    
    CLEAR(crop);
    CLEAR(cropcap);
    
    ioctl(fd, VIDIOC_CROPCAP, &cropcap);
    
    
    ioctl(fd, VIDIOC_G_CROP, &crop);
    
    crop.c.left    = 0;
    crop.c.top     = 0;
    crop.c.width   = w;
    crop.c.height  = h;
    
    ioctl(fd, VIDIOC_S_CROP, &crop);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraV4L::errno_exit(const char *s)
{
    sprintf(strErr, "%s error %d, %s", s, errno, strerror(errno));
    fprintf(stderr, "%s\n", strErr);
    
    logf_thread((char*)"%s", strErr);
    //exit(EXIT_FAILURE);
    close_device();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int CameraV4L::xioctl(int fh, int request, void *arg)
{
    if (fd == -1)       return -1;

    int r;

    do {
            r = ioctl(fh, request, arg);
    } while (-1 == r && EINTR == errno);

    return r;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
/**
 * Check if a file exists
 * @return true if and only if the file exists, false else
 */
bool fileExists(const std::string& file) {
    struct stat buf;
    return (stat(file.c_str(), &buf) == 0);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraV4L::write_exif( ExifJpgBuilder& exif_jpg )
{
	string	sDate = "%d";
	string	sTime = "%t";
	string	s, e;
	char	tmp[100];
	char	s0[50];
	
	// BUILD DATE
	replace_time( sTime );
	replace_date( sDate );

	for ( int i=0; i<sDate.size(); i++ )
		if ( sDate[i] == '-' )		sDate[i] = ':';

	string sDateTime = sDate + " " + sTime;

	// BUILD 	INFO ASTRO dans UserComment
	s = "\n-------------------------------------------";
	for ( int i=0; i<pControl.size(); i++ )
	{
		char tmp[128];
		snprintf( tmp, sizeof(tmp), "%s : %d", pControl[i]->getName().c_str(), pControl[i]->getValue() );
		s += "\n" + string(tmp);
	}

	deg2str_hms( pos_telescope.ad, s0, sizeof(s0) );
	snprintf( tmp, sizeof(tmp), "\nAlpha \t: %s", s0 );
	s += tmp;
	deg2str_dms( pos_telescope.dc, s0, sizeof(s0) );
	snprintf( tmp, sizeof(tmp), "\nDelta \t: %s", s0 );
	s += tmp;
	s += "\n-------------------------------------------";

	// BUILD OTHER EXIF
	exif_jpg.set_ascii_tag(		EXIF_IFD_0, EXIF_TAG_IMAGE_DESCRIPTION,		"Bellatrix 200/1000" );
	exif_jpg.set_ascii_tag(		EXIF_IFD_0, EXIF_TAG_MAKE,					"v4l" );
	exif_jpg.set_ascii_tag(		EXIF_IFD_0, EXIF_TAG_MODEL, 				name.c_str() );
	exif_jpg.set_short_tag(		EXIF_IFD_0, EXIF_TAG_ORIENTATION,			1 );
	exif_jpg.set_short_tag(		EXIF_IFD_0, EXIF_TAG_RESOLUTION_UNIT,		2 );
	exif_jpg.set_ascii_tag(		EXIF_IFD_0, EXIF_TAG_SOFTWARE,				"AstroPilot 1.0.0");
	exif_jpg.set_ascii_tag(		EXIF_IFD_0, EXIF_TAG_DATE_TIME,				sDateTime.c_str() );
	exif_jpg.set_ascii_tag( 	EXIF_IFD_0, EXIF_TAG_ARTIST,				"Runsys");
	exif_jpg.set_rational_tag(	EXIF_IFD_0, EXIF_TAG_EXPOSURE_TIME,			1, 1000000 / (unsigned int)25000);
	exif_jpg.set_rational_tag(	EXIF_IFD_0, EXIF_TAG_FOCAL_LENGTH, 			1000, 1 );
	exif_jpg.set_ascii_tag(		EXIF_IFD_0, EXIF_TAG_USER_COMMENT, 			s.c_str() );
	exif_jpg.set_rational_tag(	EXIF_IFD_0, EXIF_TAG_GAIN_CONTROL, 			(unsigned int)20, 1);

	/*	
	exif.addAscii(		EXIF_TAG_IMAGE_DESCRIPTION, "Bellatrix 200/1000");				// ImageDescription
	exif.addAscii(		EXIF_TAG_MAKE,				"ZWO");								// Make
	exif.addAscii(		EXIF_TAG_MODEL,				name);								// Model
	exif.addShort(		EXIF_TAG_ORIENTATION,		flip+5);							// Orientation (1 = Normal)
	exif.addShort(		EXIF_TAG_RESOLUTION_UNIT,	2);									// ResolutionUnit (2 = Pouces)
	exif.addAscii(		EXIF_TAG_SOFTWARE,			"AstroPilot 1.0.0");				// Software
	exif.addAscii(		EXIF_TAG_DATE_TIME,			sDateTime );						// Date Time
	exif.addAscii(		EXIF_TAG_ARTIST,			"Runsys");							// Artist
	exif.addRational(	EXIF_TAG_EXPOSURE_TIME,		1, 1000000 / (unsigned int)expo );	// temps ouverture
	exif.addRational(	EXIF_TAG_FOCAL_LENGTH,		1000, 1 );							// Focal lens
	exif.addAscii(		EXIF_TAG_USER_COMMENT,		s);		               			 	// UserCOmment
	exif.addRational(	EXIF_TAG_GAIN_CONTROL,		(unsigned int)gain, 1 );			// gain control
	*/
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraV4L::save_image( void *p, int size)
{
    if (fd == -1)       return;

    ExifJpgBuilder exif_jpg;
    vector<unsigned char> p_jpg;

    p_jpg.assign( (char*)p, (char*)p+size );

    exif_jpg.start( p_jpg );
    write_exif( exif_jpg );
    exif_jpg.stop();

    vector<unsigned char>& new_jpg = exif_jpg.get_out_buff();
    unsigned char* buff = &new_jpg[0];


	//------------------------------------------------------
	string sFile = string(filename) + ".jpg";
    new_filename( sFile );
    sFile = dirname + sFile;
    logf_thread( (char*)"Enregistre : %s", sFile.c_str());
    
    FILE *fp=fopen(sFile.c_str(),"wb");
    
    if (out_buf)
            fwrite( &new_jpg[0], new_jpg.size(), 1, fp );

    fflush(fp);
    fclose(fp);
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int CameraV4L::read_frame(void)
{
    if (fd == -1)       return -1;
	
	int err;

	//logf_thread((char*)"read_frame");

    struct v4l2_buffer buf;
    unsigned int i;

    switch (io) {
    case IO_METHOD_READ:
        if (-1 == read(fd, buffers[0].start, buffers[0].length)) {
            switch (errno) {
            case EAGAIN:
                return 0;

            case EIO:
                    /* Could ignore EIO, see spec. */

                    /* fall through */

            default:
                errno_exit("read");
                return -1;
            }
        }

        if ( bEnregistre ){
            logf_thread( (char*)"IO_METHOD_READ Enregistre l'image (jpg)" );
            save_image(buffers[0].start, buffers[0].length);
	        bEnregistre = false;
	    }
        break;

    case IO_METHOD_MMAP:
        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
            switch (errno) {
            case EAGAIN:
                return 0;

            case EIO:
                /* Could ignore EIO, see spec. */

                /* fall through */

            default:
                errno_exit("VIDIOC_DQBUF");
                return -1;
            }
        }

        assert(buf.index < n_buffers);

        jpg_buffer = (unsigned char*)buffers[buf.index].start;
        jpg_size = buf.bytesused;

        if( err=decompressJpeg() != (int)ERROR_DECODE_JPG_OK )
        {
			logf_thread( (char*)"[ ERREUR ] IO_METHOD_MMAP : %s", str_error_decode((ERROR_DECODE_JPG)err) );
			logf_thread( (char*)"[ ERREUR ] IO_METHOD_MMAP : %s", str_error_decode((ERROR_DECODE_JPG)err) );
		}

        if ( bEnregistre ){
            logf_thread( (char*)"IO_METHOD_MMAP Enregistre l'image (jpg)" );
            save_image(buffers[buf.index].start, buf.bytesused);
	        bEnregistre = false;
	    }

        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
        {
            errno_exit("VIDIOC_QBUF");
            return -1;
        }
        break;

    case IO_METHOD_USERPTR:
        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_USERPTR;

        if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
            switch (errno) {
            case EAGAIN:
                log_thread((char*)"Erreur CameraV4L::read_frame() EAGAIN");
                return 0;

            case EIO:
                /* Could ignore EIO, see spec. */

                /* fall through */

            default:
                log_thread((char*)"Erreur CameraV4L::read_frame()  default");
                errno_exit("VIDIOC_DQBUF");
                return -1;
            }
        }

        for (i = 0; i < n_buffers; ++i)
                if (buf.m.userptr == (unsigned long)buffers[i].start
                    && buf.length == buffers[i].length)
                        break;

        assert(i < n_buffers);
        
        jpg_buffer = (unsigned char*)buf.m.userptr;
        jpg_size = buf.bytesused;

        //log_thread((char*)"CameraV4L::read_frame() decompresse");

        if ( bEnregistre )      
        {
            logf_thread( (char*)"IO_METHOD_USERPTR Enregistre l'image (jpg)" );
            save_image((void *)buf.m.userptr, buf.bytesused);
            bEnregistre = false;
            bSaving = true;
        }

        //logf_thread( (char*)"lit image" );

        if( err=decompressJpeg() != (int)ERROR_DECODE_JPG_OK )	logf_thread( (char*)"[ ERREUR ] IO_METHOD_USERPTR : %s", str_error_decode((ERROR_DECODE_JPG)err) );
        
        
        
        memory += buf.bytesused;

        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))       errno_exit("VIDIOC_QBUF");
        break;
    }
    bHaveNew = true;
    return 1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraV4L::mainloop(void)
{
    if (fd == -1)       return;


    unsigned int count;

    count = frame_count;

    while (count-- > 0) {
        for (;;) {
            fd_set fds;
            struct timeval tv;
            int r;

            FD_ZERO(&fds);
            FD_SET(fd, &fds);

            /* Timeout. */
            tv.tv_sec = 2;
            tv.tv_usec = 0;

            r = select(fd + 1, &fds, NULL, NULL, &tv);

            if (-1 == r) {
                if (EINTR == errno)     continue;
                errno_exit("select");
            }

            if (0 == r) {
                sprintf(strErr, "select timeout");
                fprintf(stderr, "%s\n", strErr);
                //exit(EXIT_FAILURE);
                close_device();
                return;
            }

            if (read_frame())          break;
            /* EAGAIN - continue select loop. */
        }
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraV4L::stop_capturing(void)
{
    if (fd == -1)       return;
    if ( !bCapture )            return;

    bCapture = false;

    logf_thread( (char*)"-------------- CameraV4L::stop_capturing %s------------------", dev_name.c_str() );

    enum v4l2_buf_type type;

    switch (io) {
    case IO_METHOD_READ:
            /* Nothing to do. */
            break;

    case IO_METHOD_MMAP:
    case IO_METHOD_USERPTR:
            type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))
                    errno_exit("VIDIOC_STREAMOFF");
            break;
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraV4L::start_capturing(void)
{
    if (fd == -1)       return;

    if ( bCapture ) return;
    
    logf_thread( (char*)"-------------- CameraV4L::start_capturing %s------------------", dev_name.c_str() );
    
    unsigned int i;
    enum v4l2_buf_type type;

    switch (io) {
    case IO_METHOD_READ:
            /* Nothing to do. */
            break;

    case IO_METHOD_MMAP:
            for (i = 0; i < n_buffers; ++i) {
                    struct v4l2_buffer buf;

                    CLEAR(buf);
                    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                    buf.memory = V4L2_MEMORY_MMAP;
                    buf.index = i;

                    if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
                            errno_exit("VIDIOC_QBUF");
            }
            type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
                    errno_exit("VIDIOC_STREAMON");
            break;

    case IO_METHOD_USERPTR:
            for (i = 0; i < n_buffers; ++i) {
                    struct v4l2_buffer buf;

                    CLEAR(buf);
                    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                    buf.memory = V4L2_MEMORY_USERPTR;
                    buf.index = i;
                    buf.m.userptr = (unsigned long)buffers[i].start;
                    buf.length = buffers[i].length;

                    if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
                            errno_exit("VIDIOC_QBUF");
            }
            type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
                    errno_exit("VIDIOC_STREAMON");
            break;
    }
    bCapture = true;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraV4L::uninit_device(void)
{
    if (fd == -1)       return;


        unsigned int i;

        switch (io) {
        case IO_METHOD_READ:
                free(buffers[0].start);
                break;

        case IO_METHOD_MMAP:
                for (i = 0; i < n_buffers; ++i)
                        if (-1 == munmap(buffers[i].start, buffers[i].length))
                                errno_exit("munmap");
                break;

        case IO_METHOD_USERPTR:
                for (i = 0; i < n_buffers; ++i)
                        free(buffers[i].start);
                break;
        }

        free(buffers);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraV4L::init_read(unsigned int buffer_size)
{
    if (fd == -1)       return;


        buffers = (buffer*)calloc(1, sizeof(*buffers));

        if (!buffers) {
                sprintf(strErr, "Out of memory");
                fprintf(stderr, "%s\n", strErr);
                //exit(EXIT_FAILURE);
                close_device();
                return;
        }

        buffers[0].length = buffer_size;
        buffers[0].start = malloc(buffer_size);

        if (!buffers[0].start) {
                sprintf(strErr, "Out of memory");
                fprintf(stderr, "%s\n", strErr);
                //exit(EXIT_FAILURE);
                close_device();
                return;
        }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraV4L::init_mmap(void)
{
    if (fd == -1)       return;


        struct v4l2_requestbuffers req;

        CLEAR(req);

        req.count = 4;
        req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory = V4L2_MEMORY_MMAP;

        if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
                if (EINVAL == errno) {
                        sprintf(strErr, "%s does not support memory mapping", dev_name.c_str());
                        fprintf(stderr, "%s\n", strErr);
                        //exit(EXIT_FAILURE);
                        close_device();
                        return;
                } else {
                        errno_exit("VIDIOC_REQBUFS");
                }
        }

        if (req.count < 2) {
                fprintf(stderr, "Insufficient buffer memory on %s\n", dev_name.c_str() );
                //xit(EXIT_FAILURE);
                close_device();
                return;
        }

        buffers = (buffer *)calloc(req.count, sizeof(*buffers));

        if (!buffers) {
                fprintf(stderr, "Out of memory\n");
                //exit(EXIT_FAILURE);
                close_device();
                return;
        }

        for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
                struct v4l2_buffer buf;

                CLEAR(buf);

                buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory      = V4L2_MEMORY_MMAP;
                buf.index       = n_buffers;

                if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
                        errno_exit("VIDIOC_QUERYBUF");

                buffers[n_buffers].length = buf.length;
                buffers[n_buffers].start =
                        mmap(NULL /* start anywhere */,
                              buf.length,
                              PROT_READ | PROT_WRITE /* required */,
                              MAP_SHARED /* recommended */,
                              fd, buf.m.offset);

                if (MAP_FAILED == buffers[n_buffers].start)
                        errno_exit("mmap");
        }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraV4L::init_userp(unsigned int buffer_size)
{
    if (fd == -1)       return;
        struct v4l2_requestbuffers req;

        CLEAR(req);

        req.count  = 4;
        req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory = V4L2_MEMORY_USERPTR;

        if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
                if (EINVAL == errno) {
                        fprintf(stderr, "%s does not support "
                                 "user pointer i/o\n", dev_name.c_str() );
                        //exit(EXIT_FAILURE);
                        close_device();
                        return;
                } else {
                        errno_exit("VIDIOC_REQBUFS");
                }
        }

        buffers = (buffer*) calloc(4, sizeof(*buffers));

        if (!buffers) {
                fprintf(stderr, "Out of memory\n");
                //exit(EXIT_FAILURE);
                close_device();
                return;
        }

        for (n_buffers = 0; n_buffers < 4; ++n_buffers) {
                buffers[n_buffers].length = buffer_size;
                buffers[n_buffers].start = malloc(buffer_size);
                
                //printf( "%d - size   = %d\n", n_buffers, buffer_size );

                if (!buffers[n_buffers].start) {
                        fprintf(stderr, "Out of memory\n");
                    //exit(EXIT_FAILURE);
                    close_device();
                    return;
                }
        }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraV4L::init_device(void)
{
    if (fd == -1)       return;

    logf_thread( (char*)"CameraV4L::init_device()" );
    log_tab(true);


    struct v4l2_capability cap;
    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;
    struct v4l2_format fmt;
    
    unsigned int min;

    if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
            if (EINVAL == errno) {
                    fprintf(stderr, "%s is no V4L2 device\n", dev_name.c_str() );
                //exit(EXIT_FAILURE);
                close_device();
                goto end;
                return;
            } else {
                    errno_exit("VIDIOC_QUERYCAP");
            }
    }
    //name = ( (char*) cap.card );
    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
            fprintf(stderr, "%s is no video capture device\n", dev_name.c_str() );
            //exit(EXIT_FAILURE);
            close_device();
            goto end;
            return;
    }

    switch (io) {
    case IO_METHOD_READ:
            if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
                    fprintf(stderr, "%s does not support read i/o\n", dev_name.c_str() );
                //exit(EXIT_FAILURE);
                close_device();
                goto end;
                return;
            }
            break;

    case IO_METHOD_MMAP:
    case IO_METHOD_USERPTR:
            if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
                    fprintf(stderr, "%s does not support streaming i/o\n", dev_name.c_str() );
                //exit(EXIT_FAILURE);
                close_device();
                goto end;
                return;
            }
            break;
    }


    /* Select video input, video standard and tune here. */


    CLEAR(cropcap);

    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (0 == xioctl(fd, VIDIOC_CROPCAP, &cropcap)) {
        crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        crop.c = cropcap.defrect; /* reset to default */

        if (-1 == xioctl(fd, VIDIOC_S_CROP, &crop)) {
            switch (errno) {
	            case EINVAL:
	                    /* Cropping not supported. */
	                    break;
	            default:
	                    /* Errors ignored. */
	                    break;
            }
        }
	} else
	{
            /* Errors ignored. */
    }



    enum_format();

            /* Note VIDIOC_S_FMT may change width and height. */
    CLEAR(fmt);
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;


    switch(force_format)
    {
    case YUYV :
        fprintf(stderr, "Set YUYV\n");
        fmt.fmt.pix.width       = tSize[0].width;
        fmt.fmt.pix.height      = tSize[0].height;
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
        fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;

        if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))           errno_exit("VIDIOC_S_FMT");
        
        break;
    case H264 :
        /* Note VIDIOC_S_FMT may change width and height. */
        fprintf(stderr, "Set H264\r\n");
        fmt.fmt.pix.width       = tSize[0].width; //replace
        fmt.fmt.pix.height      = tSize[0].height; //replace
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_H264; //replace
        fmt.fmt.pix.field       = V4L2_FIELD_ANY;

        if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))           errno_exit("VIDIOC_S_FMT");
        break;

    case MJPG :
        //fprintf(stderr, "Set MJPG\r\n");
        width  = fmt.fmt.pix.width       = tSize[sizeChoix].width;//width; //replace
        height = fmt.fmt.pix.height      = tSize[sizeChoix].height;//height; //replace
        
        logf_thread( (char*)"Force format MJPG   %dx%d",fmt.fmt.pix.width, fmt.fmt.pix.height );
        //fmt.fmt.pix.width       = width; //replace
        //fmt.fmt.pix.height      = height; //replace

        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR24; //replace
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_H264; //replace
        fmt.fmt.pix.pixelformat = pixelformat; //replace

        fmt.fmt.pix.field       = V4L2_FIELD_ANY;

        if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))           errno_exit("VIDIOC_S_FMT");

        logf_thread( (char*)"Force format MJPG   OK" );
        break;

    default:
        /* Preserve original settings as set by v4l2-ctl for example */
        fprintf(stderr, "Set default\n");
        if (-1 == xioctl(fd, VIDIOC_G_FMT, &fmt))           errno_exit("VIDIOC_G_FMT");
        break;
    }


    width  = fmt.fmt.pix.width;
    height = fmt.fmt.pix.height;


    /* Buggy driver paranoia. */
    //min = fmt.fmt.pix.width * 2;
    //printf( "size   = %d\n", fmt.fmt.pix.sizeimage );
    min = fmt.fmt.pix.width;
    if (fmt.fmt.pix.bytesperline < min) fmt.fmt.pix.bytesperline = min;

    min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
    if (fmt.fmt.pix.sizeimage < min)    fmt.fmt.pix.sizeimage = min;

    switch (io) {
    case IO_METHOD_READ:
            init_read(fmt.fmt.pix.sizeimage);
            break;

    case IO_METHOD_MMAP:
            init_mmap();
            break;

    case IO_METHOD_USERPTR:
            init_userp(fmt.fmt.pix.sizeimage);
            //printf( "width  = %d\n", fmt.fmt.pix.width );
            //printf( "height = %d\n", fmt.fmt.pix.height );
            //printf( "size   = %d\n", fmt.fmt.pix.sizeimage );
            break;
    }

end:
    log_tab(false);
    logf_thread( (char*)"CameraV4L::init_device" );
        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraV4L::close_device(void)
{
    if (fd==-1)     return;
    
    if (-1 == close(fd))                errno_exit("close");

    fd = -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int CameraV4L::open_device(char * name)
{
    if ( name == NULL )       { fd = -1; return(EXIT_FAILURE); }
    logf_thread((char*)"CameraV4L::open_device(%s)", name );

    dev_name = name;
    return open_device();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int CameraV4L::open_device()
{
    struct stat st;
    logf_thread( (char*)"CameraV4L::open_device() ... '%s'", dev_name.c_str());

    /*
    if ( dev_name.find("/dev/video1")!=string::npos )
    {
        fd = -1;
        return;
    }
    */
    if (-1 == stat(dev_name.c_str(), &st)) {
        sprintf(strErr, "Cannot identify '%s': %d, %s", dev_name.c_str(), errno, strerror(errno));
        fprintf(stderr, "%s\n", strErr);
        logf_thread((char*)"%s", strErr);
        fd = -1;
        return(EXIT_FAILURE);
    }

    if (!S_ISCHR(st.st_mode)) {
        sprintf(strErr, "%s is no device", dev_name.c_str() );
        fprintf(stderr, "%s\n", strErr);
        fd = -1;
        logf_thread((char*)"%s", strErr);
        return(EXIT_FAILURE);
    }

    fd = open(dev_name.c_str() , O_RDWR /* required */ | O_NONBLOCK, 0);

    if (-1 == fd) {
        sprintf(strErr, "Cannot open '%s': %d, %s", dev_name.c_str(), errno, strerror(errno));
        fprintf(stderr, "%s\n", strErr);
        logf_thread((char*)"%s", strErr);
        fd = -1;
        return(EXIT_FAILURE);
    }
    
    
    return 0;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int CameraV4L::change_value(int id, int value)
{
    if (fd == -1)       return -1;
	logf( (char*)"CameraV4L::change_value(0x%08x, %d)", id, value );

    //struct v4l2_queryctrl   queryctrl;
    struct v4l2_control     control;


    memset(&control, 0, sizeof(control));
    control.id = id;

    if (0 == ioctl(fd, VIDIOC_G_CTRL, &control)) {
        control.value = value;
        if (-1 == ioctl(fd, VIDIOC_S_CTRL, &control) && errno != ERANGE) {
            return -1;
        }
    /* Ignore if V4L2_CID_CONTRAST is unsupported */
    } else if (errno != EINVAL) {
        return -1;
    }
    return 0;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Control*  CameraV4L::getControl(string s)
{
    int si;
    si = pControl.size();    
    
    for(int i=0; i<si; i++ ) {
        if ( pControl[i]->find(s) )          return pControl[i];
    }
    return NULL;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
Control*  CameraV4L::getControl(int id)
{
    int si;
    si = pControl.size();    
    
    for(int i=0; i<si; i++ ) {
        if ( pControl[i]->getId() == id )          return pControl[i];
    }
    return NULL;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool  CameraV4L::isControl(string s)
{
    int si;
    si = pControl.size();    
    
    for(int i=0; i<si; i++ ) {
        if ( pControl[i]->getName().find(s) != string::npos )          return true;
    }
    return false;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int nbJPG = 0;
void jpeg_saver_error_exit (j_common_ptr cinfo)
{
    /* cinfo->err really points to a jpeg_saver_error_mgr struct, so coerce pointer */
    jpeg_saver_error_mgr* myerr = (jpeg_saver_error_mgr*) cinfo->err;

    //(*cinfo->err->output_message) (cinfo);
    logf_thread((char*)"Error Handler  %d images decode", nbJPG);
    bReadError = true;
    longjmp(myerr->setjmp_buffer, 1);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
struct jpeg_error_mgr * my_jpeg_std_error(struct jpeg_error_mgr * jerr)
{
	if ( jerr->msg_code != JTRC_EOI )	{
		logf_thread( (char*)"Error handler my_jpeg_std_error()..code:%d", jerr->msg_code);
		logf_thread( (char*)"  voir /usr/inlcude/jerror.h  JTRC_EOI = %d", JTRC_EOI);
		bReadError = true;
	}
    return jpeg_std_error(jerr);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int CameraV4L::decompressJpeg() {
	//printf("Proc: %d--------------------------------------\n", nbJPG++);

	wBmp = -1;
	hBmp = -1;
	//bmp_buffer = NULL;

    readBgr.ptr = NULL;
    readBgr.w   = -1;
    readBgr.h   = -1;
    readBgr.d   = -1;
    readBgr.bReadError = bReadError = false;

	//logf_thread((char*)"wBmp=%d hBmp=%d", wBmp, hBmp);

	//pixel_size = cinfo.output_components;
    // si le buffer n'est pas vide on le libere
	if (bmp_buffer)     free(bmp_buffer);
	bmp_buffer = NULL;
    // On recreer le nouveau buffer
	bmp_size = wBmp * hBmp * pixel_size;


    int rc, i, j;
	// Variables for the source jpg
	struct stat file_info;

	// Variables for the decompressor itself
	struct jpeg_decompress_struct cinfo;
	struct jpeg_saver_error_mgr jerr;
	
	int E = 0;
    nbJPG++;

	cinfo.err = my_jpeg_std_error(&jerr.pub);	
    jerr.pub.error_exit = jpeg_saver_error_exit;

    // Point de retour de setjump
    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_decompress(&cinfo);
        //printf("Error Handler (setjump)  %d images decode\n", nbJPG);
		return ERROR_DECODE_JPG_SET_JUMP;
    }


	jpeg_create_decompress(&cinfo);
	jpeg_mem_src(&cinfo, jpg_buffer, jpg_size);

	//logf_thread((char*)"jpg_size=%d", jpg_size);

	rc = jpeg_read_header(&cinfo, TRUE);

	if (rc != 1) {
		printf("File does not seem to be a normal JPEG\n");
		//exit(EXIT_FAILURE);
		return ERROR_DECODE_JPG_READ_HEADER;
	}

	jpeg_start_decompress(&cinfo);
	
	wBmp = cinfo.output_width;
	hBmp = cinfo.output_height;

	//logf_thread((char*)"wBmp=%d hBmp=%d", wBmp, hBmp);

	pixel_size = cinfo.output_components;
    // si le buffer n'est pas vide on le libere

    unsigned char* previous = bmp_buffer;

    // On recreer le nouveau buffer
	bmp_size = wBmp * hBmp * pixel_size;
	bmp_buffer = (unsigned char*) malloc(bmp_size);
    
    if ( previous!=NULL )     free(previous);

	row_stride = wBmp * pixel_size;

    readBgr.ptr = (GLubyte*)bmp_buffer;
    readBgr.w   = wBmp;
    readBgr.h   = hBmp;
    readBgr.d   = 3;

    /*	
    if ( bmp_size<6220800 ) {
        jpeg_abort_decompress(&cinfo);
    	//printf("Proc: Abort\n");
        return;
    }
    */
    //On décode le jpg ligne par ligne
	while (cinfo.output_scanline < cinfo.output_height) {
		unsigned char *buffer_array[1];
		buffer_array[0] = bmp_buffer + (cinfo.output_scanline) * row_stride;
        //printf( "%d\n", cinfo.output_scanline);
		int n= jpeg_read_scanlines(&cinfo, buffer_array, 1);
		if ( n!= 1 )        printf( "%d\n", n );

	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

    readBgr.bReadError = bReadError;
    
    return ERROR_DECODE_JPG_OK;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraV4L::enregistre()
{
    bEnregistre = true;
    bSaving = false;
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void CameraV4L::callback(bool b, int ii, char* str)
{
    logf_thread( (char*)"CameraV4L::callback( %s, %d, \"%s\" )", BOOL2STR(b), ii, (char*)str );
    
    //if ( b && ii == 1 )         bEnregistre = true;

    if ( ii == 10 )
    {
        callback_enregistre_cam( str );
        string filename = string(str);
        if ( filename.find(".cam")==string::npos )      filename += ".cam";
        logf_thread( (char*)"sauvegarde dans  \"%s\" )", (char*)filename.c_str() );
        
        capability_save(filename);
    }
    else
    if ( ii == 11 )
    {
        string filename = string(str);
        if ( filename.find(".cam")==string::npos )      filename += ".cam";
        callback_charge_cam( (char*)filename.c_str() );
        capability_load(filename);
    }
    else
    {
        //callback_enregistre(b, extra, str);
        string filename = string(str);
        if ( filename.find(".cam")==string::npos )      filename += ".cam";
        capability_load(filename);
    }
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
/**
 * Check if a file exists
 * @return true if and only if the file exists, false else
 */
bool CameraV4L::is_file_exist(const std::string& file) {
    struct stat buf;
    return (stat(file.c_str(), &buf) == 0);
}
//--------------------------------------------------------------------------------------------------------------------
//
// Remplade le joker %n  = nom de la camera
//
//--------------------------------------------------------------------------------------------------------------------
void CameraV4L::replace_name(string& sFilenamePng)
{
	int n = sFilenamePng.find("%n");
	
	string deb = sFilenamePng.substr( 0, n );
	string fin = sFilenamePng.substr( n+2 );
	sFilenamePng = deb + name + fin;
}
//--------------------------------------------------------------------------------------------------------------------
//
// Remplade le joker %t = l'heure
//
//--------------------------------------------------------------------------------------------------------------------
void CameraV4L::replace_time(string& sFilenamePng)
{
	char 			sTime[30];
	struct timeval	tv;
	struct tm*		tm;

	gettimeofday( &tv, NULL );
	tm=localtime(&tv.tv_sec);
	
	
	long uCentieme = (long)tv.tv_usec;
	uCentieme /= 10000;
	snprintf( (char*)sTime, sizeof(sTime),"%02d:%02d:%02d.%02ld", tm->tm_hour, tm->tm_min, tm->tm_sec, uCentieme );
	
	int n = sFilenamePng.find("%t");
	
	string deb = sFilenamePng.substr( 0, n );
	string fin = sFilenamePng.substr( n+2 );
	sFilenamePng = deb + string(sTime) + fin;
}
//--------------------------------------------------------------------------------------------------------------------
//
// Remplade le joker %d  = date
//
//--------------------------------------------------------------------------------------------------------------------
void CameraV4L::replace_date(string& sFilenamePng)
{
	char 			sTime[30];
	struct timeval	tv;
	struct tm*		tm;

	gettimeofday( &tv, NULL );
	tm=localtime(&tv.tv_sec);
	
	snprintf( (char*)sTime, sizeof(sTime),"20%02d-%02d-%02d", tm->tm_year-100, tm->tm_mon+1, tm->tm_mday );
	
	int n = sFilenamePng.find("%d");
	
	string deb = sFilenamePng.substr( 0, n );
	string fin = sFilenamePng.substr( n+2 );
	sFilenamePng = deb + string(sTime) + fin;
}
//--------------------------------------------------------------------------------------------------------------------
//
//
// Genere un nouveau nom de fichier pour la sauvegarde image
//---------------------------------------------------------------
// joker autorisé :
//   %d  : date
//   %t  : fime
//   %n  : nom de la camera
//
//--------------------------------------------------------------------------------------------------------------------
void CameraV4L::new_filename_png(string& sFilenamePng)
{
   	logf_thread( (char*)"Camera::new_filename_png( %s )", sFilenamePng.c_str() );
   	
   	
	if ( sFilenamePng.find("%d") != string::npos )			replace_date( sFilenamePng );
	if ( sFilenamePng.find("%t") != string::npos )			replace_time( sFilenamePng );
	if ( sFilenamePng.find("%n") != string::npos )			replace_name( sFilenamePng );
   	
	if ( sFilenamePng.find(".png") == string::npos )		sFilenamePng = sFilenamePng + ".png";

   	if ( !is_file_exist(dirname+sFilenamePng) )
   	{
	    logf_thread( (char*)"|  Nouveau nom : %s", sFilenamePng.c_str() );
   		return;
   	}
	
	logf_thread( (char*)"|  Le fichier existe : '%s'", sFilenamePng.c_str() );
   	
	int n=(int)sFilenamePng.find(".png");
	sFilenamePng = sFilenamePng.substr(0, n);
   	logf_thread( (char*)"|  Suppression de l'extension : '%s'", sFilenamePng.c_str() );

    int no;
   	char tmp[128];
    for( no=0; no<10000; no++ )
    {
        snprintf(tmp, sizeof(tmp), "%s-%04d.png", (char*)sFilenamePng.c_str(), no );

        if ( !is_file_exist(dirname+string(tmp)) )        break;
    }    
    if (no>=10000)
    {
        logf_thread( (char*)"|  [ WARNING ] indice 10000 atteind " );
    }
    sFilenamePng = string(tmp);
    
    logf_thread( (char*)"|  Nouveau nom : %s", sFilenamePng.c_str() );
}
//--------------------------------------------------------------------------------------------------------------------
//
// Genere un nouveau nom de fichier pour la sauvegarde image
//
//--------------------------------------------------------------------------------------------------------------------
void CameraV4L::new_filename_png()
{
   	logf_thread( (char*)"Camera::new_filename_png( %s )", filename.c_str() );
   	
    int no;
    string sName = string( filename );
    
    while ( filename.find(" ") != string::npos )
    	filename.replace( filename.find(" "), 1, "_");

    string s;
    for( no=0; no<100000; no++ )
    {
    	char tmp[128];
        snprintf(tmp, sizeof(tmp), "%s-%06d.png", (char*)filename.c_str(), no );
    	s = string(tmp);

        if ( !is_file_exist(dirname+s) )        break;
    }    
	filename = s;
    if (no>=100000)
    {
        logf_thread( (char*)"[ WARNING ] indice 100000 atteind " );
    }
    
    logf_thread( (char*)"Nouveau nom : %s", filename.c_str() );
}
//--------------------------------------------------------------------------------------------------------------------
//
// Genere un nouveau nom de fichier pour la sauvegarde image
//
//--------------------------------------------------------------------------------------------------------------------
void CameraV4L::new_filename( string&  sFilename )
{
   	logf_thread( (char*)"Camera::new_filename( %s )", sFilename.c_str() );
   	
	if ( sFilename.find("%d") != string::npos )			replace_date( sFilename );
	if ( sFilename.find("%t") != string::npos )			replace_time( sFilename );
	if ( sFilename.find("%n") != string::npos )			replace_name( sFilename );
   	
    int no, n;
    string sExt = "";
    string sBasename = "";
    n=sFilename.find(".") ;
    
    if ( n != string::npos )
    {
    	sExt = sFilename.substr( n );
    	sBasename = sFilename.substr( 0, n );
    }

    logf_thread( (char*)"|  Nouveau nom : '%s'   '%s'", sBasename.c_str(), sExt.c_str() );

   	if ( !is_file_exist(dirname+sBasename+sExt) )
   	{
	   	sBasename += sExt;
   		return;
   	}
	
    string nBasename;
    for( no=0; no<100000; no++ )
    {
    	char tmp[128];
        snprintf(tmp, sizeof(tmp), "%s-%06d", (char*)sBasename.c_str(), no );
    	nBasename = string(tmp);

        if ( !is_file_exist(dirname+nBasename+sExt) )        break;
    }    

    if (no>=100000)
    {
        logf_thread( (char*)"[ WARNING ] indice 100000 atteind " );
    }
    sFilename = nBasename + sExt;
    
    logf_thread( (char*)"Nouveau nom : %s", filename.c_str() );
}
//--------------------------------------------------------------------------------------------------------------------
//
// Genere un nouveau nom de fichier pour la sauvegarde image
//
//--------------------------------------------------------------------------------------------------------------------
char * CameraV4L::str_error_decode( ERROR_DECODE_JPG error )
{
	switch( (int)error )
	{
    case ERROR_DECODE_JPG_OK:		    	return (char*)"ERROR_DECODE_JPG_OK";
    case ERROR_DECODE_JPG_READ_HEADER:		return (char*)"ERROR_DECODE_JPG_READ_HEADER";
    case ERROR_DECODE_JPG_SET_JUMP:			return (char*)"ERROR_DECODE_JPG_SET_JUMP";
	}
	return (char*)"ERROR_DECODE_inconnu";
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------

