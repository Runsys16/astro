/*
 *  V4L2 video capture example
 *
 *  This program can be used and distributed without restrictions.
 *
 *      This program is provided with the V4L2 API
 * see http://linuxtv.org/docs.php for more information
 */


#include "v4l2.h"


long memory = 0;
bool bReadError = false;

struct v4l2_queryctrl queryctrl;
struct v4l2_querymenu querymenu;

//--------------------------------------------------------------------------------------------------------------------
//
//						Constructeurs ....
//
//--------------------------------------------------------------------------------------------------------------------
Device_cam::Device_cam()
{
    logf((char*)"Constructeur Device_cam() -------------" );
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
    bCapture        = false;
    bmp_buffer      = NULL;
    
    bHaveNew        = false;
    
    name ="";
    filenameRec = "/home/rene/Documents/astronomie/logiciel/script/images/frame";
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Device_cam::setDevName( string s )
{
    dev_name = s;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
bool Device_cam::isDevice( int u )
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
bool Device_cam::getIOCapability()
{
    if (fd == -1)
    {
        name = "";
        return false;
    }

    struct v4l2_capability cap;
    memset(&cap, 0, sizeof(v4l2_capability));
    
    if(ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1)
    {
        perror("cam_info: Can't get capabilities");
        name = "";
        return false;
    } else {
        //logf("Name:\t\t '%s'", (char*)cap.card);
        string s = (char*)cap.card;
        int n = s.find(':');
        if ( n>0 )      s[n] = 0;
        //logf("n = %d", n );
        name = string(s);
        logf( (char*)"Device_cam::getIOName() Name: '%s'", name.c_str() );
        if ( cap.device_caps&V4L2_CAP_VIDEO_CAPTURE )	{
	        logf( (char*)"  cap.capabilities 0x%08x", (uint32_t)cap.device_caps );
	    }
        else	{
	        logf( (char*)"[Error] V4L2_CAP_VIDEO_CAPTURE  : cap.capabilities 0x%08x", (uint32_t)cap.device_caps );
	        return false;
        }
    }
    return true;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Device_cam::enumerate_menu(__u32 id)
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
            logf((char*)"\t%d-%s", querymenu.index, querymenu.name);
        }
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Device_cam::capability_list( void )    {
    if (fd == -1)       return;
    
    logf((char*)"Device_cam::capability_list()");
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
void Device_cam::capability_save( string filename )    {
    if (fd == -1)       return;

    logf( (char*)"Sauvegarde des parametres de la camera dans '%s'", (char*)filename.c_str() );

    ofstream fichier;
    fichier.open(filename, ios_base::trunc);
    //fichier.open(filename);

    if ( !fichier ) 
    {
        logf( (char*)"[ERROR]impossble d'ouvrir : '%s'", (char*)filename.c_str() );
        return;
    }


    int nb = pControl.size();
    for( int i=0; i<nb; i++ )
    {
        logf( (char*)"%s %d", (char*)pControl[i]->getName().c_str(), pControl[i]->getValue() );
        fichier << pControl[i]->getName() << " : " <<  pControl[i]->getValue();
        fichier << "\n";
    }
    
    
    
    fichier.close();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Device_cam::capability_load( string filename )    {
    if (fd == -1)       return;

    logf( (char*)"Lecture des parametres de la camera dans '%s'", (char*)filename.c_str() );

    ifstream fichier;
    fichier.open(filename, std::ios_base::app);

    if ( !fichier ) 
    {
        logf( (char*)"[ERROR]impossble d'ouvrir : '%s'", (char*)filename.c_str() );
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
        logf( (char*)" %d - %s = %d", nbLigne, (char*)sCtrl.c_str(), val );
        
        Control *   p = getControl( sCtrl );
        if ( p != NULL )            p->setValue( val );
        else                        logf( (char*)"pControl NULL : line %d", __LINE__ );
        
        nbLigne++;
    }    

    logf( (char*)"Lecture de %d lignes", nbLigne );
    
    fichier.close();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Device_cam::capability_save( void )    {
    if (fd == -1)       return;

    string filename = "/home/rene/.astropilot/"+ name +".cam";
    capability_save( filename );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Device_cam::enum_format()
{
    //-------------------------------------------        
    logf((char*)"Device_cam::enum_format()");
    log_tab(true);

    logf( (char*)"ioctl =  VIDIOC_ENUM_FMT ------------------");
    struct v4l2_fmtdesc fmtdesc;
    CLEAR(fmtdesc);
    
    fmtdesc.index = 0;
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;


    while (-1 != xioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc))
    {
        logf( (char*)"description = %s" , fmtdesc.description );
        logf( (char*)"type = %08X   " , fmtdesc.type );
        logf( (char*)"pixelformat = %08X   " , fmtdesc.pixelformat );
        
        if ( fmtdesc.pixelformat == 0x47504A4D )       pixelformat = fmtdesc.pixelformat;
        
        enum_format_size( fmtdesc.pixelformat );

        fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmtdesc.index++;
    }
    log_tab(false);
    logf((char*)"Device_cam::enum_format()");
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Device_cam::enum_format_size(int pf)
{
    //-------------------------------------------        
    logf((char*)"Device_cam::enum_format_size(%d)", pf);
    log_tab(true);

    logf( (char*)"    -- VIDIOC_ENUM_FRAMESIZES --");
    struct v4l2_frmsizeenum frmsizeenum;
    
    CLEAR(frmsizeenum);
    frmsizeenum.index = 0;
    frmsizeenum.pixel_format = pf;
    nSize = 0;
    width = -1;

    while (-1 != xioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frmsizeenum))
    {
        
        logf( (char*)"Type=%08X size=%dx%d" , frmsizeenum.type, frmsizeenum.discrete.width, frmsizeenum.discrete.height );
        

        //if (pixelformat != -1 ) {
            //logf( (char*)"Format=%08X   \n" , pixelformat );
            //logf( (char*)"Format=%08X   \n" , V4L2_PIX_FMT_BGR24 );
            //logf( (char*)"Format=%08X   \n" , V4L2_PIX_FMT_RGB24 );
            if ( width == -1 )       width  = frmsizeenum.discrete.width;
            if ( height == -1 )      height = frmsizeenum.discrete.height;
            
            struct size s;
            s.width     = frmsizeenum.discrete.width;
            s.height    = frmsizeenum.discrete.height;

            if ( pf == 0x47504A4D )   
            {
                sizeChoix = 0;//tSize.size();
            }
            tSize.push_back(s);
            nSize++;
        //}
        frmsizeenum.index++;
    }
    
    
    //if (pixelformat != -1 ) {
        for( int i=0; i<nSize; i++ )  {
            logf( (char*)"%d- %dx%d",  i, tSize[i].width, tSize[i].height );
        }
    //}
    logf( (char*)"Resolution courante %dx%d",  width, height );

    log_tab(false);
    logf((char*)"Device_cam::enum_format_size(int pf");
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Device_cam::def_crop( int w, int h )
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
void Device_cam::errno_exit(const char *s)
{
    sprintf(strErr, "%s error %d, %s", s, errno, strerror(errno));
    fprintf(stderr, "%s\n", strErr);
    
    logf((char*)"%s", strErr);
    //exit(EXIT_FAILURE);
    close_device();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int Device_cam::xioctl(int fh, int request, void *arg)
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

void Device_cam::process_image(const void *p, int size)
{
    if (fd == -1)       return;
    if ( filenameRec == "" )     return;

    char filename[155];
    int nm;
    
    for( nm=0; nm<1000; nm++ )
    {
        sprintf(filename, "%s-%d.jpg", (char*)filenameRec.c_str(), nm);
        //logf( (char*)filename );

        if ( !fileExists(filename) )        break;
    }    
    //logf( (char*)filename );
    //return;
    if (nm>=1000)
    {
        logf( (char*)"[Erreur] framme-1000.jpg atteind " );
    }
    frame_number++;
    
    logf( (char*)"Enregistre : %s", filename);
    
    FILE *fp=fopen(filename,"wb");
    
    if (out_buf)
            fwrite(p, size, 1, fp);

    fflush(fp);
    fclose(fp);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int Device_cam::read_frame(void)
{
    if (fd == -1)       return -1;

	//logf((char*)"read_frame");

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

        if ( bEnregistre )      process_image(buffers[0].start, buffers[0].length);
        bEnregistre = false;
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

        decompressJpeg();

        if ( bEnregistre )      process_image(buffers[buf.index].start, buf.bytesused);
        bEnregistre = false;
        //printf( "%d %d\n", buffers[buf.index].start, buf.bytesused);

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
                log((char*)"Erreur Device_cam::read_frame() EAGAIN");
                return 0;

            case EIO:
                /* Could ignore EIO, see spec. */

                /* fall through */

            default:
                log((char*)"Erreur Device_cam::read_frame()  default");
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

        //log((char*)"Device_cam::read_frame() decompresse");

        if ( bEnregistre )      
        {
            logf( (char*)"Enregistre l'image lu" );
            process_image((void *)buf.m.userptr, buf.bytesused);
            bEnregistre = false;
        }

        //logf( (char*)"lit image" );

        decompressJpeg();
        
        
        
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
void Device_cam::mainloop(void)
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
void Device_cam::stop_capturing(void)
{
    if (fd == -1)       return;
    if ( !bCapture )            return;

    bCapture = false;

    logf( (char*)"-------------- Device_cam::stop_capturing %s------------------", dev_name.c_str() );

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
void Device_cam::start_capturing(void)
{
    if (fd == -1)       return;

    if ( bCapture ) return;
    
    logf( (char*)"-------------- Device_cam::start_capturing %s------------------", dev_name.c_str() );
    
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
void Device_cam::uninit_device(void)
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
void Device_cam::init_read(unsigned int buffer_size)
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
void Device_cam::init_mmap(void)
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
void Device_cam::init_userp(unsigned int buffer_size)
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
void Device_cam::init_device(void)
{
    if (fd == -1)       return;

    logf( (char*)"Device_cam::init_device()" );
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
    } else {
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
        fprintf(stderr, "Set MJPG\r\n");
        fmt.fmt.pix.width       = tSize[sizeChoix].width;//width; //replace
        fmt.fmt.pix.height      = tSize[sizeChoix].height;//height; //replace
        
        logf( (char*)"fmt.fmt.pix.width  = %d", fmt.fmt.pix.width );
        logf( (char*)"fmt.fmt.pix.height = %d", fmt.fmt.pix.height );
        //fmt.fmt.pix.width       = width; //replace
        //fmt.fmt.pix.height      = height; //replace

        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR24; //replace
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_H264; //replace
        fmt.fmt.pix.pixelformat = pixelformat; //replace

        fmt.fmt.pix.field       = V4L2_FIELD_ANY;

        if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))           errno_exit("VIDIOC_S_FMT");
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
    logf( (char*)"Device_cam::init_device" );
        
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Device_cam::close_device(void)
{
    if (fd==-1)     return;
    
    if (-1 == close(fd))                errno_exit("close");

    fd = -1;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int Device_cam::open_device(char * name)
{
    if ( name == NULL )       { fd = -1; return(EXIT_FAILURE); }
    logf((char*)"Device_cam::open_device(%s)", name );

    dev_name = name;
    return open_device();
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int Device_cam::open_device()
{
    struct stat st;

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
        logf((char*)"%s", strErr);
        fd = -1;
        return(EXIT_FAILURE);
    }

    if (!S_ISCHR(st.st_mode)) {
        sprintf(strErr, "%s is no device", dev_name.c_str() );
        fprintf(stderr, "%s\n", strErr);
        fd = -1;
        logf((char*)"%s", strErr);
        return(EXIT_FAILURE);
    }

    fd = open(dev_name.c_str() , O_RDWR /* required */ | O_NONBLOCK, 0);

    if (-1 == fd) {
        sprintf(strErr, "Cannot open '%s': %d, %s", dev_name.c_str(), errno, strerror(errno));
        fprintf(stderr, "%s\n", strErr);
        logf((char*)"%s", strErr);
        fd = -1;
        return(EXIT_FAILURE);
    }
    
    logf( (char*)"Device_cam::open_device() ... '%s'", dev_name.c_str());
    
    return 0;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
/*
void Device_cam::change_value_abs(int V4L2_CID, int v)
{
    if (fd == -1)       return;

    #define CONTROL V4L2_CID_EXPOSURE_ABSOLUTE

    struct v4l2_queryctrl   queryctrl;
    struct v4l2_control     control;

    memset(&queryctrl, 0, sizeof(queryctrl));
    queryctrl.id = CONTROL;

    if (-1 == ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl)) {
        if (errno != EINVAL) {
            perror("VIDIOC_QUERYCTRL");
            exit(EXIT_FAILURE);
        } else {
            printf("V4L2_CID_BRIGHTNESS is not supported\n");
        }
    } else if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED) {
        printf("V4L2_CID_BRIGHTNESS is not supported\n");
    } else {
        memset(&control, 0, sizeof (control));
        control.id = CONTROL;
        control.value = queryctrl.default_value;

        if (-1 == ioctl(fd, VIDIOC_S_CTRL, &control)) {
            perror("VIDIOC_S_CTRL");
            exit(EXIT_FAILURE);
        }
    }

    

    memset(&control, 0, sizeof(control));
    control.id = V4L2_CID;

    if (0 == ioctl(fd, VIDIOC_G_CTRL, &control)) {
        control.value = v;
        printf( "value=%d\n", control.value );
        // The driver may clamp the value or return ERANGE, ignored here 

        if (-1 == ioctl(fd, VIDIOC_S_CTRL, &control) && errno != ERANGE) {
            perror("VIDIOC_S_CTRL");
            sprintf( strErr, "change_value_abs() VIDIOC_S_CTRL" );
            close_device();
            return;
        }
    // Ignore if V4L2_CID_CONTRAST is unsupported 
    } else if (errno != EINVAL) {
        perror("VIDIOC_G_CTRL");
        sprintf( strErr, "change_value_abs() VIDIOC_G_CTRL" );
        close_device();
        return;
    }


}
*/
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
int Device_cam::change_value(int id, int value)
{
    if (fd == -1)       return -1;
	logf( (char*)"Device_cam::change_value(%d, %d)", id, value );

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
Control*  Device_cam::getControl(string s)
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
Control*  Device_cam::getControl(int id)
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
bool  Device_cam::isControl(string s)
{
    int si;
    si = pControl.size();    
    
    for(int i=0; i<si; i++ ) {
        if ( pControl[i]->getName().find(s) != string::npos )          return true;
    }
    return NULL;
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
    logf((char*)"Error Handler  %d images decode", nbJPG);
    bReadError = true;
    longjmp(myerr->setjmp_buffer, 1);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
struct jpeg_error_mgr * my_jpeg_std_error(struct jpeg_error_mgr * jerr)
{
	if ( jerr->msg_code != JTRC_EOI )	{
		logf( (char*)"Error handler my_jpeg_std_error()..code:%d", jerr->msg_code);
		logf( (char*)"  voir /usr/inlcude/jerror.h  JTRC_EOI = %d", JTRC_EOI);
		bReadError = true;
	}
    return jpeg_std_error(jerr);
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Device_cam::decompressJpeg() {
	//printf("Proc: %d--------------------------------------\n", nbJPG++);

	wBmp = -1;
	hBmp = -1;
	//bmp_buffer = NULL;

    readBgr.ptr = NULL;
    readBgr.w   = -1;
    readBgr.h   = -1;
    readBgr.d   = -1;
    readBgr.bReadError = bReadError = false;

	//logf((char*)"wBmp=%d hBmp=%d", wBmp, hBmp);

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
        return;
    }


	jpeg_create_decompress(&cinfo);
	jpeg_mem_src(&cinfo, jpg_buffer, jpg_size);

	//logf((char*)"jpg_size=%d", jpg_size);

	rc = jpeg_read_header(&cinfo, TRUE);

	if (rc != 1) {
		printf("File does not seem to be a normal JPEG\n");
		//exit(EXIT_FAILURE);
		return;
	}

	jpeg_start_decompress(&cinfo);
	
	wBmp = cinfo.output_width;
	hBmp = cinfo.output_height;

	//logf((char*)"wBmp=%d hBmp=%d", wBmp, hBmp);

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
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Device_cam::enregistre()
{
    bEnregistre = true;
    
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void Device_cam::callback(bool b, int ii, char* str)
{
    logf( (char*)"Device_cam::callback( %s, %d, \"%s\" )", BOOL2STR(b), ii, (char*)str );
    
    //if ( b && ii == 1 )         bEnregistre = true;

    if ( ii == 10 )
    {
        callback_enregistre_cam( str );
        string filename = string(str);
        if ( filename.find(".cam")==string::npos )      filename += ".cam";
        logf( (char*)"sauvegarde dans  \"%s\" )", (char*)filename.c_str() );
        
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

