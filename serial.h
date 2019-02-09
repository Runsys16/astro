#ifndef SERIAL_H
#define SERIAL_H  1

#include <stdio.h>    /* Standard input/output definitions */
#include <stdlib.h> 
#include <stdint.h>   /* Standard types */
#include <string.h>   /* String function definitions */
#include <unistd.h>   /* UNIX standard function definitions */
#include <fcntl.h>    /* File control definitions */
#include <errno.h>    /* Error number definitions */
#include <termios.h>  /* POSIX terminal control definitions */
#include <sys/ioctl.h>
#include <getopt.h>

#include <thread>

//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
class Serial 
{
protected:
    int             baudrate;
    int             fd;
public :
    Serial();

    void            serialport_init(const char* serialport, int baud);
    int             serialport_writebyte(int fd, uint8_t b);
    int             serialport_write(int fd, const char* str);
    int             serialport_read_until(int fd, char* buf, char until);


};

#endif

