#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "serial_port.h"
 
void read_Serial_Port(const char* DEVICE_PORT)
{
	int file;
	struct termios options;
	char message[100];
	unsigned int nCountMax = 60;
	bool b;
 
	file = open(DEVICE_PORT, O_RDONLY | O_NOCTTY | O_NDELAY);
 
	if(file == -1){perror("Unable to open the serial port\n");}
	printf("Serial port open successful\n");
 
	tcgetattr(file, &options); 			
	cfsetispeed(&options, B9600); 					
	cfsetospeed(&options, B9600); 					
	options.c_cflag |= (CLOCAL | CREAD); 			  
	options.c_cflag |= PARENB; 						//No parity					
	options.c_cflag |= PARODD; 						
	options.c_cflag &= ~CSTOPB; 					
	options.c_cflag &= ~CSIZE; 						
	options.c_cflag |= CS8; 						//8 bits					
	options.c_iflag |= (INPCK | ISTRIP); 			
	tcsetattr(file, TCSANOW, &options); 	     
	fcntl(file, F_SETFL, FNDELAY);			
 
	printf("Reading serial port ...\n\n"); 
	b = readMessage(file, message, nCountMax);
	if (b == 0){printf("Error while reading serial port\n");}
	else printf("Serial port read successful\n");
	close(file);
	printf("Serial port closed\n");
};
 
bool readMessage(int file, unsigned int nCountMax)
{
    int i;
    size_t nbytes;
	ssize_t bytes_read;
 
    if (file != -1)
    {
	    i = 0;  
	    char message[100];
	    char data[100];
		while (i<nCountMax && data != ".")
		{
		    if (read(file, data, 1) == -1)
		    {
			    printf("reception error\n");
			    printf("code errno = %d\n", errno);
			    return false;
		    }
		    else
		    {   
				nbytes = sizeof(data);
				bytes_read = read(file, data, nbytes);
			    message[i] = *data;
			    printf("%c", message[i]);
			    i++;
		    }
		}
	    message[i] = 0;
	    return true;
    }
};
