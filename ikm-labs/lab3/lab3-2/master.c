#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include <wiringPi.h>

int main ()
{
	int fd;
	char sbuf;
	unsigned char rbuf;
	struct termios stdio_config;
	struct termios savedconfig;
	
	// start the wiringPi library
	if (wiringPiSetup () == -1)
	{
		fprintf (stdout, "Unable to start wiringPi.\n") ;
		return -1;
	}
	// setup output mode for BCM22 (WiringPi 3) used to control communication direction
	// BCM22 (WiringPi 3) = HIGH: RS-485 transmit mode
	// BCM22 (WiringPi 3) = LOW: RS-485 receive mode
	pinMode(3, OUTPUT);
	  
	// set it initially to be in receive mode
	digitalWrite(3, LOW);

	// TODO: open and configure serial port with 9600 baudrate

	fd = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY); // O_NDELAY mora biti iskljucen ako hocemo da se slikaju karakteri bez entera.
		if (fd < 0)
			fprintf(stderr, "Failed to open serial port. Check if it is used by another device.\n");
				
	tcgetattr(0, &savedconfig);
	tcgetattr(0, &stdio_config);
	stdio_config.c_lflag &=  ~(ECHO | ICANON); // ovo treba ako se hoce kanonski mod iskljuciti.
	stdio_config.c_cc[VMIN] = 1; // at least 1 char threshold
	stdio_config.c_cc[VTIME] = 0; //no delay
	cfsetispeed(&stdio_config, B9600);
	cfsetospeed(&stdio_config, B9600);
	tcsetattr(0, TCSANOW, &stdio_config);
	
	while (1)
	{
		sbuf = getchar();
		if (sbuf == 'q')
		{ 
			tcsetattr(0, TCSANOW, &savedconfig);
			close(fd);
			return 0;
		}
		else
		{
			if (sbuf != '\n')
			{
				digitalWrite(3, HIGH);	// enable RS-485 transmit mode
				
				// TODO: send the entered byte
				if (sbuf != '\0')
					write(fd, &sbuf, 1);
			
		
				usleep(100000);
				
				delay(1);							
				digitalWrite(3, LOW);	// disable RS-485 transmit mode
			}
		}
		// TODO: read the received byte from serial port and display it in console
		if (read(fd, &rbuf, 1) > 0)
				{
					putchar(rbuf);
					fflush(stdout);
				}
	}
	tcsetattr(0, TCSANOW, &savedconfig);
	close(fd);	
	return 0 ;
}
