#ifndef V4L2_H
#define V4L2_H  1


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <getopt.h>             /* getopt_long() */

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <linux/videodev2.h>
#include <iostream>
#include <vector> //Ne pas oublier !

#include <syslog.h>
#include <setjmp.h>
#include <jpeglib.h>

using namespace std;

//#include "file_browser.h"
#include "control.h"
#include "main.h"
#include "button_callback.h"


#define CLEAR(x) memset(&(x), 0, sizeof(x))

#ifndef V4L2_PIX_FMT_H264
#define V4L2_PIX_FMT_H264     v4l2_fourcc('H', '2', '6', '4') /* H264 with start codes */
#endif
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
enum io_method {
        IO_METHOD_READ,
        IO_METHOD_MMAP,
        IO_METHOD_USERPTR,
};
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
struct buffer {
        void   *start;
        size_t  length;
};
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
struct size {
	__u32 width;
	__u32 height;
};
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
struct jpeg_saver_error_mgr 
{
    jpeg_error_mgr pub;    /* "public" fields */
    jmp_buf setjmp_buffer;  /* for return to caller */
};
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
enum FMT 
{
    YUYV,
    H264,
    MJPG
};
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
class Control;
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
class Device_cam : public ButtonCallBack
{
private:
    string                  dev_name;
    enum io_method          io;
    int                     fd;
    struct buffer *         buffers;
    unsigned int            n_buffers;
    int                     out_buf;
    FMT                     force_format;
    int                     frame_count;
    int                     frame_number;
    char*                   strErr;
    vector<Control*>        pControl;
    int                     nControl;
    string                  name;
    
    vector<struct size>     tSize;
    int                     cSize;
    
    int                     width;
    int                     height;
    int                     pixelformat;

    int                     nSize;
    int                     sizeChoix;

	struct stat             file_info;
	unsigned long           jpg_size;
	unsigned char *         jpg_buffer;
	// Variables for the output buffer, and how long each row is
	unsigned long           bmp_size;
	unsigned char *         bmp_buffer;
	int                     row_stride, wBmp, hBmp, pixel_size;

    bool                    bEnregistre;
    bool                    bCapture;
    bool                    bHaveNew;
    string                  filenameRec;

protected:
    rb_t                    readBgr;



public :

    Device_cam();
    
    void                    getIOName();
    bool                    isDevice(int);
    void                    enumerate_menu(__u32 id);
    void                    capability_list( void );
    void                    capability_save( void );
    void                    def_crop(int, int);
    void                    enum_format_size(int pixel_format);
    void                    enum_format();
    
    void                    errno_exit(const char *s);
    int                     xioctl(int fh, int request, void *arg);
    void                    process_image(const void *p, int size);
    int                     read_frame(void);

    void                    mainloop(void);    
    void                    stop_capturing(void);
    void                    start_capturing(void);

    void                    uninit_device(void);
    void                    init_read(unsigned int buffer_size);
    void                    init_mmap(void);
    void                    init_userp(unsigned int buffer_size);
    void                    init_device(void);
    void                    close_device(void);
    int                     open_device(char *);
    int                     open_device();
    int                     change_value(int, int);
    
    Control*                getControl( string s );
    Control*                getControl( int id );

    //inline  void            enregistre()            { bEnregistre = true; }
    void                    enregistre(); //            { bEnregistre = true; }

    
    inline  char*           getErr()                { return strErr; }
    inline  bool            isErr()                 { return fd == -1; }
    
    void                    setDevName(string);
    inline  void            setDevName(char* s)     { dev_name = s; }
    inline  char*           getDevName()            { return (char*)dev_name.c_str(); }

    inline  int             getFd()                 { return fd; }

    inline  unsigned char*  getBuffer()             { return bmp_buffer; }
    inline  bool            getHaveNew()            { return bHaveNew; }
    inline  void            setHaveNew(bool b)      { bHaveNew = b; }

    inline  void            setName( char * s)      { name = s; }
    inline  char*           getName()               { return (char*)name.c_str(); }

    inline int              getWidth()              { return width; }
    inline void             setWidth(int n)         { width = n; }
    inline int              getHeight()             { return height; }
    inline void             setHeight(int n)        { height = n; }

    inline void             setSizeChoix()          {  sizeChoix = ++sizeChoix % nSize; }
    inline int              getSizeChoix()          {  return sizeChoix; }

    inline void             setFilenameRec(string s) { filenameRec = s; }

virtual void                callback(bool, char*);
virtual struct readBackground*      getRB()     {return &readBgr;}

private:
    void                    decompressJpeg();
};

#endif

